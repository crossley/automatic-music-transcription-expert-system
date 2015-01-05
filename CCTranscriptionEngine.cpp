/*
 *  CCTranscriptionEngine.cpp
 *  CCTranscriber
 *
 *  Created by Bebelutz on 5/7/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "CCTranscriptionEngine.h"

#include "CCTempoFinder.h"
#include "CCPitchFinder.h"
#include "CChordFinder.h"
#include "CCSegmentFinder.h"
#include "CCGlobals.h" // I'm the only one that includes this file!

#include <iostream>
using std::cout;
using std::endl;

//Constructor
CCTranscriptionEngine::CCTranscriptionEngine(unsigned numSamples, float *bufferOfSamples) : mNumSamples(numSamples), mSampleBufferPtr(bufferOfSamples) {
	
	mExperts.push_back(new CCTempoFinder());
	mExperts.push_back(new CCPitchFinder());
	//mExperts.push_back(new CChordFinder());
	//mExperts.push_back(new CCSegmentFinder());
	
	mTempoHistory = new float[numSamples / 512];
	mPitchHistory = new float[numSamples / 512];
	mChordHistory = new char[numSamples / 512];
	
}

CCTranscriptionEngine::~CCTranscriptionEngine() { 
	
	delete [] mTempoHistory;
	delete [] mPitchHistory;
	delete [] mChordHistory;

}

float CCTranscriptionEngine::tempoVerdict(){return mTempoVerdict;}
float CCTranscriptionEngine::pitchVerdict(){return mPitchVerdict;}
char CCTranscriptionEngine::chordVerdict(){return mChordVerdict;}


void CCTranscriptionEngine::judge()
{
	float *sampleBufferPtr = mSampleBufferPtr;
	int samplesLeft = (int)mNumSamples;
	unsigned increment = 512;
	
	int i = 0;
	list<CCAbstractExpert *>::iterator expert;
	list<CCAbstractExpert *>::iterator expert2;
	expert = mExperts.begin();	//refer to tempo expert
	expert2 = mExperts.end();	//refer to pitch expert
	expert2--;
	
	while (samplesLeft > 0) {
		gFeatureList.currentItem()->setSampleBuffer(sampleBufferPtr); // Set the pointer of the current feature item to look at the buffer of samples.
		gFeatureList.currentItem()->calculateBasicStats(); // Precaluclate some data.
		CCExpertGroup::judge(); // Call the superclass version of this function, so it calls judge on each of my children.
		
		mTempoHistory[i] = (*expert)->verdict();  //get final verdict of CCTempoFinder and add to mTempoHistory
		mPitchHistory[i] = (*expert2)->verdict(); //get final verdict of CCPitchFinder and add to mPitchHistory
		
//		cout << "i = " << i << "\n";
//		cout << "verdict() = " << (*expert2)->verdict() << "\n";
//		cout << "mPitchHistory = " << mPitchHistory[i] << "\n";	

		i++;		
																						
		gFeatureList.advanceCurrentItem();

		// Make sure to request only the needed samples at the end of the file.
		// TODO: zero out the buffer, so that the remaining samples don't show garbage.
		if(samplesLeft < increment) 
			increment = samplesLeft;
		
		sampleBufferPtr += increment;
		samplesLeft -= increment;
	} // end of while loop
	
	for(i = 0; i < mNumSamples / 512; i++)
	{
		mVerdict += mTempoHistory[i];
	}
	
	//we can perhaps use the pitch history in a Key estimate
		
	mVerdict = mVerdict * 512 / mNumSamples;
	
}

void CCTranscriptionEngine::judge(float *sampleBuffer)
{
	float *sampleBufferPtr = mSampleBufferPtr;
	unsigned increment = 512;
	
		gFeatureList.currentItem()->setSampleBuffer(sampleBufferPtr); // Set the pointer of the current feature item to look at the buffer of samples.
		gFeatureList.currentItem()->calculateBasicStats(); // Precaluclate some data.
		
		CCExpertGroup::judge(); // Call the superclass version of this function, so it calls judge on each of my children.

		gFeatureList.advanceCurrentItem();

		
}

float CCTranscriptionEngine::tempoAtBlock(unsigned blockNumber)
{
	return mTempoHistory[blockNumber];
}

float CCTranscriptionEngine::PitchAtBlock(unsigned blockNumber)
{
	return mPitchHistory[blockNumber];
}

char CCTranscriptionEngine::chordAtBlock(unsigned blockNumber)
{
	return mChordHistory[blockNumber];
}