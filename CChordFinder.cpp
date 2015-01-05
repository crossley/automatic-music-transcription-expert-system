/*
 *  CChordFinder.cpp
 *  CCTranscriber
 *
 *  Created by matthew crossley on 6/12/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "CChordFinder.h"
#include "CCFeatures.h"
#include "CCore.h"

extern CCFeatureList gFeatureList;

using std::cout;
using std::endl;

CChordFinder::CChordFinder()
{
	
	mExperts.push_back(new CChordNeuralNet());
	
	mVerdictHistory = new char[mExperts.size()];
	mConfidenceHistory = new float[mExperts.size()];
	mExpertiseHistory = new float[mExperts.size()];

}

CChordNeuralNet::CChordNeuralNet()
{
	mExpertise = 0.5;
	mNumPeaks = 10;
	mNeuralNetInput = new float[10];
	mNeuralNetOutput = new float[3];
	nnwork mChordTracker(10, 20, 3);
	if (!mChordTracker.load ("cfgNetFG.nnw")) {
		cerr << "File not found.\n";
	}
	
//	cout << chordTracker.get_layersize(INPUT) << endl;
//	cout << chordTracker.get_layersize(HIDDEN) << endl;
//	cout << chordTracker.get_layersize(OUTPUT) << endl;

}

#pragma mark -- CChordFinder --

char CChordFinder::charVerdict() { return mCharVerdict;}

void CChordFinder::judge() 
{

	//first loop thru experts to gather individual verdicts
	CCExpertGroup::judge();
	
}

void CChordFinder::review() {}

#pragma mark -- CCPitchExpertA judge --

void CChordNeuralNet::judge()
{
	const float *cepstrumBufferPtr = gFeatureList.currentItem()->cepstrumBuffer();
	unsigned bufferSize = gFeatureList.currentItem()->sampleBufferSize();
	
	// find cepstrum peaks
	for(int i = 1; i < bufferSize; i++){
			// is local max (peak)?
			if(cepstrumBufferPtr[i] > cepstrumBufferPtr[i-1] && cepstrumBufferPtr[i] > cepstrumBufferPtr[i+1]){
				// calculate neighborhoodAverage
				mNeighborhoodAverage = calculateNeighborhoodAverage(cepstrumBufferPtr, i, bufferSize, 50);
				// is local max substantial relative to neighborhood?
				if(cepstrumBufferPtr[i] > mNeighborhoodAverage)
				{
					//push peak onto list
					mCepstrumPeaks.push_back(Peak(i, cepstrumBufferPtr[i]));
				}
			}
		}
		
	//sort from least to greatest
	mCepstrumPeaks.sort();
	
	//put mNumPeaks highest peaks into mNeuralNetInput
	list<Peak>::iterator p;
	p = mCepstrumPeaks.end();
	p--;
	for(int i = 0; i < mNumPeaks; i++, p--)
	{
		mNeuralNetInput[i] = p->mPeakIndex;
//		cout << mNeuralNetInput[i] << endl;
	}
	
//	cout << endl;
	
	//present to neural net
	if (!mChordTracker.load ("cfgNetFG.nnw")) {
		cerr << "File not found.\n";
	}
	
	mChordTracker.run(mNeuralNetInput, mNeuralNetOutput);
	
//	cout << chordTracker.get_layersize(INPUT) << endl;
//	cout << chordTracker.get_layersize(HIDDEN) << endl;
//	cout << chordTracker.get_layersize(OUTPUT) << endl;
	
	// set mVerdict to highest probability
	if(mNeuralNetOutput[0] > mNeuralNetOutput[1] && mNeuralNetOutput[0] > mNeuralNetOutput[2]){
		mVerdict = 'C'; mConfidence = mNeuralNetOutput[0];}
	if(mNeuralNetOutput[1] > mNeuralNetOutput[0] && mNeuralNetOutput[1] > mNeuralNetOutput[2]){
		mVerdict = 'F'; mConfidence = mNeuralNetOutput[1];}
	if(mNeuralNetOutput[2] > mNeuralNetOutput[0] && mNeuralNetOutput[2] > mNeuralNetOutput[1]){
	mVerdict = 'G'; mConfidence = mNeuralNetOutput[2];}
	
//	cout << mVerdict << endl;
	
	if(mLocalVerdictHistory.size() < 10)
	{
		mLocalVerdictHistory.push_back(mVerdict);
	} else
	{
		mLocalVerdictHistory.pop_front();
		mLocalVerdictHistory.push_back(mVerdict);
	}
	
}

void CChordNeuralNet::review() {}