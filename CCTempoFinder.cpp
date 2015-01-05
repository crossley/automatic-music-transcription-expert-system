/*
 *  CCTempoFinder.cpp
 *  CCTranscriber
 *
 *  Created by Bebelutz on 5/7/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "CCTempoFinder.h"
#include "CCFeatures.h"
#include "CCore.h"

extern CCFeatureList gFeatureList;

#include <iostream>
using std::cout;
using std::endl;

#pragma mark -- constructors --

//Constructor
CCTempoFinder::CCTempoFinder()
{	
	// Add all experts. The Base class takes care of freeing them.
	mExperts.push_back(new CCTempoExpertA());
	mExperts.push_back(new CCLowPassTempoExpertA());
	mExperts.push_back(new CCHighPassTempoExpertA());
//	mExperts.push_back(new CCTempoExpertB());
//	mExperts.push_back(new CCLowPassTempoExpertB());
//	mExperts.push_back(new CCHighPassTempoExpertB());
	
	mVerdictHistory = new float[mExperts.size()];
	mConfidenceHistory = new float[mExperts.size()];
	mExpertiseHistory = new float[mExperts.size()];

}

CCTempoExpertA::CCTempoExpertA() 
{
	mExpertise = 0.2;
}
CCLowPassTempoExpertA::CCLowPassTempoExpertA() 
{
	mExpertise = 0.3;
}
CCHighPassTempoExpertA::CCHighPassTempoExpertA()
{
	mExpertise = 0.3;
}
CCTempoExpertB::CCTempoExpertB() 
{
	mExpertise = 0.4;
}
CCLowPassTempoExpertB::CCLowPassTempoExpertB() 
{
	mExpertise = 0.5;
}
CCHighPassTempoExpertB::CCHighPassTempoExpertB() 
{
	mExpertise = 0.5;
}

#pragma mark -- CCTempoFinder --

void CCTempoFinder::judge()
{	

	//first loop thru experts to gather individual verdicts
	CCExpertGroup::judge();
	
	//now judge the evidence returned by experts
	int i = 0; 
	int usefulVerdicts = mExperts.size();
	list<CCAbstractExpert *>::iterator expert;
	
	for (expert = mExperts.begin(); expert != mExperts.end(); expert++, i++)
	{

		mVerdictHistory[i] = (*expert)->verdict();
		mConfidenceHistory[i] = (*expert)->confidence();
		mExpertiseHistory[i] = (*expert)->expertise();
	}
	
//	for(i = 0; i < mExperts.size(); i++)
//	{
//		cout << mVerdictHistory[i] << "\n";
//	}
	
	for(i = 0; i < mExperts.size(); i++)
	{
		if(mVerdictHistory[i] == -1)	// if block clearly didn't receive fair trial (see review())
		{
			mVerdictHistory[i] == 0.0;	// the verdict won't be considered
			usefulVerdicts--; 
		}
	}
	
// WHY ARE THERE STILL -1's IN THE VERDICT HISTORY??? 

//	for(i = 0; i < mExperts.size(); i++)
//	{
//		cout << mVerdictHistory[i] << "\n";
//	}
	
//	cout << "usefulVerdicts = " << usefulVerdicts << "\n";
	
	float highestConfidence = 0.0;			//the highest confidence returned by an expert
	float highestConfidenceVerdict = 0.0;	//the verdict of the most confident expert
	float groupConfidenceSTD = 0.0;			//the STD among expert confidence
	int numConfident = 0.0;					//the number of experts close to the highest confidence
	int numAgree = 0.0;						//number of confident experts that agree
	
	// find the highest confidence
	for(i = 0; i < mExperts.size() - 1; i++)
	{
		if(mConfidenceHistory[i] < mConfidenceHistory[i+1])
		{
			highestConfidence = mConfidenceHistory[i+1];
			highestConfidenceVerdict = mVerdictHistory[i+1];
		}
	}
//	cout << "highestConfidenceVerdict = " << highestConfidenceVerdict << "\n";
	
	//find the group STD
	groupConfidenceSTD = calculateSTD(mConfidenceHistory, mExperts.size());
	
	//find numConfident
	for(i = 0; i < mExperts.size(); i++)
	{
		if(mConfidenceHistory[i] >= highestConfidence - 2 * groupConfidenceSTD)
		{
			numConfident++;
		}
	}
	
//	cout << "numConfident = " << numConfident << "\n";
	
	//how many confident experts agree?
	for(i = 0; i < mExperts.size(); i++)
	{
		if(mConfidenceHistory[i] >= highestConfidence - 2 * groupConfidenceSTD)
		{
			//if confident then check if value agrees with highest confidence
			//TODO -- use absolute value in math.h
			if(mVerdictHistory[i] - highestConfidenceVerdict <= 0.1 || highestConfidenceVerdict - mVerdictHistory[i] <= 0.1)
			{
				numAgree++;
			}
		}
	}
	
//	cout << "numAgree = " << numAgree << "\n";
//	cout << "mVerdict = " << mVerdict << "\n";
	
	mVerdict = 0.0;
	
	//if most of the confident experts agree then average them
	if(numAgree >= numConfident / 2)
	{
		for(i = 0; i < mExperts.size(); i++)
		{
			//if confident
			if(mConfidenceHistory[i] >= highestConfidence - 2 * groupConfidenceSTD)
			{
				//if value agrees with highest confidence
				// TODO -- abs()
				if(mVerdictHistory[i] - highestConfidenceVerdict <= 0.1 || highestConfidenceVerdict - mVerdictHistory[i] <= 0.1)
				{	
					//cout << "mVerdict = " << mVerdict << "\n";
					mVerdict += (float) mVerdictHistory[i];
				}
			}
		}
		
		//cout << "mVerdict = " << mVerdict << "\n";
		//cout << "numAgree = " << numAgree << "\n";
		mVerdict = mVerdict / (float) numAgree;
	}
		
	//if most confident experts disagree then just take the most confident
	if(!(numAgree >= numConfident / 2))
	{
		mVerdict = highestConfidenceVerdict;
//		cout << "mVerdict = " << mVerdict << "\n";
	}
}

void CCTempoFinder::review() {}

#pragma mark -- CCTempoExpertA judge --

void CCTempoExpertA::judge()
{
	float *aCorrBufferIn = gFeatureList.rmsAutocor();
	gFeatureList.rmsAutocorPeak(); 	// find absPeakInd
	float threshold = gFeatureList.rmsAutocorPeakThreshold();
	unsigned bufferSize = gFeatureList.statsBufferSize();	//num cached feature items
	float peak = 0.0;
	unsigned peakIndex = 0;
	
	// Threshold to pick peak out of noise
	for(unsigned i = 5; i < bufferSize - 5; i++){
		if (isLocalPeak(aCorrBufferIn, i) && aCorrBufferIn[i] >= threshold) {
			peak = aCorrBufferIn[i];
			peakIndex = i;
			break;
		}
	}
	
	//assume first block counts as first beat
	float sampleRate = gFeatureList.mSampleRate;
	float tempo;
	if(peakIndex != 0)
	{
		tempo = ((float) sampleRate / (bufferSize * peakIndex)) * 60;
	}	else tempo = 0;
	
	mVerdict = tempo;
	
	review();
	
//	cout << "threshold = " << threshold << "\n";
//	cout << "peakIndex = " << peakIndex << "\n";
//	cout << "bufferSize = "<< bufferSize << "\n";
//	cout << "\n" <<"tempoA = " << mVerdict << "\n \n";
}

void CCLowPassTempoExpertA::judge()
{
	float *aCorrBufferIn = gFeatureList.lowPassRMSAutocor();
	gFeatureList.lowPassRMSAutocorPeak(); 	// find absPeakInd
	float threshold = gFeatureList.lowPassRMSAutocorPeakThreshold();
	unsigned bufferSize = gFeatureList.statsBufferSize();	//num cached feature items
	float peak = 0.0;
	unsigned peakIndex= 0;
	
	// Threshold to pick peak out of noise
	for(unsigned i = 5; i < bufferSize - 5; i++){
		if (isLocalPeak(aCorrBufferIn, i) && aCorrBufferIn[i] >= threshold) {
			peak = aCorrBufferIn[i];
			peakIndex = i;
			break;
		}
	}
	
	//assume first block counts as first beat
	float sampleRate = gFeatureList.mSampleRate;
	float tempo;
	if(peakIndex != 0)
	{
		tempo = ((float) sampleRate / (bufferSize * peakIndex)) * 60;
	}	else tempo = 0;	
	mVerdict = tempo;
	
	review();
	
//	cout << "threshold = " << threshold << "\n";
//	cout << "peakIndex = " << peakIndex << "\n";
//	cout << "bufferSize = "<< bufferSize << "\n";
//	cout << "\n" << "lowPassTempoA = " << mVerdict << "\n \n";
}

void CCHighPassTempoExpertA::judge()
{
	float *aCorrBufferIn = gFeatureList.highPassRMSAutocor();
	gFeatureList.highPassRMSAutocorPeak(); 	// find absPeakInd
	float threshold = gFeatureList.highPassRMSAutocorPeakThreshold();
	unsigned bufferSize = gFeatureList.statsBufferSize();	//num cached feature items
	float peak = 0.0;
	unsigned peakIndex = 0;
	
	// Threshold to pick peak out of noise
	for(unsigned i = 5; i < bufferSize - 5; i++){
		if (isLocalPeak(aCorrBufferIn, i) && aCorrBufferIn[i] >= threshold) {
			peak = aCorrBufferIn[i];
			peakIndex = i;
			break;
		}
	}
	
	//assume first block counts as first beat
	float sampleRate = gFeatureList.mSampleRate;
	float tempo;
	if(peakIndex != 0)
	{
		tempo = ((float) sampleRate / (bufferSize * peakIndex)) * 60;
	}	else tempo = 0;	
	mVerdict = tempo;
	
	review();
	
//	cout << "absPeak = " << absPeak << "\n";
//	cout << "threshold = " << threshold << "\n";
//	cout << "diff = " << diff << "\n";
//	cout << "peakIndex = " << peakIndex << "\n";
//	cout << "bufferSize = "<< bufferSize << "\n";
//	cout << "\n" << "highPassTempoA = " << mVerdict << "\n \n";

}

#pragma mark -- CCTempoExpertB judge --

void CCTempoExpertB::judge()
{
	float *aCorrBufferIn = gFeatureList.rmsAutocor();
	float absPeak = gFeatureList.rmsAutocorPeak(); 	// find absPeakInd
	float threshold = gFeatureList.rmsAutocorPeakThreshold();
	unsigned bufferSize = gFeatureList.statsBufferSize();	//num cached feature items
	float peak = 0.0;
	unsigned peakIndex1, peakIndex2;
	peakIndex1 = 0;
	peakIndex2 = 0;
	
	// Threshold to pick first peak out of noise
	for(unsigned i = 5; i < bufferSize - 5; i++){
		if (isLocalPeak(aCorrBufferIn, i) && aCorrBufferIn[i] >= threshold) {
			peak = aCorrBufferIn[i];
			peakIndex1 = i;
			break;
		}
	}
	
	// Threshold to pick second peak out of noise
	for(unsigned i = peakIndex1; i < bufferSize - peakIndex1; i++){
		if (isLocalPeak(aCorrBufferIn, i) && aCorrBufferIn[i] >= threshold) {
			peak = aCorrBufferIn[i];
			peakIndex2 = i;
			break;
		}
	}
	
	//calculate tempo from two peaks
	float sampleRate = gFeatureList.mSampleRate;
	float diff = peakIndex2 - peakIndex1;
	float tempo;
	if(diff != 0)
	{
		tempo = ((float) sampleRate / (bufferSize * diff)) * 60;
	}else tempo = 0;
	
	mVerdict = tempo;
	
	review();
	
//	cout << "absPeak = " << absPeak << "\n";
//	cout << "threshold = " << threshold << "\n";
//	cout << "diff = " << diff << "\n";
//	cout << "peakIndex1 = " << peakIndex1 << "\n";
//	cout << "peakIndex2 = " << peakIndex2 << "\n";
//	cout << "bufferSize = "<< bufferSize << "\n";
//	cout << "tempoB = " << mVerdict << "\n \n";
}

void CCLowPassTempoExpertB::judge()
{
	float *aCorrBufferIn = gFeatureList.lowPassRMSAutocor();
	gFeatureList.lowPassRMSAutocorPeak(); 	// find absPeakInd
	float threshold = gFeatureList.lowPassRMSAutocorPeakThreshold();
	unsigned bufferSize = gFeatureList.statsBufferSize();	//num cached feature items
	float peak = 0.0;
	unsigned peakIndex1, peakIndex2;
	peakIndex1 = 0;
	peakIndex2 = 0;
	
	// Threshold to pick first peak out of noise
	for(unsigned i = 5; i < bufferSize - 5; i++){
		if (isLocalPeak(aCorrBufferIn, i) && aCorrBufferIn[i] >= threshold) {
			peak = aCorrBufferIn[i];
			peakIndex1 = i;
			break;
		}
	}
	
	// Threshold to pick second peak out of noise
	for(unsigned i = peakIndex1; i < bufferSize - peakIndex1; i++){
		if (isLocalPeak(aCorrBufferIn, i) && aCorrBufferIn[i] >= threshold) {
			peak = aCorrBufferIn[i];
			peakIndex2 = i;
			break;
		}
	}
	
	//calculate tempo from two peaks
	float sampleRate = gFeatureList.mSampleRate;
	float diff = peakIndex2 - peakIndex1;
	float tempo;
	if(diff != 0)
	{
		tempo = ((float) sampleRate / (bufferSize * diff)) * 60;
	}else tempo = 0;
	
	mVerdict = tempo;
	
	review();
	
//	cout << "absPeak = " << absPeak << "\n";
//	cout << "threshold = " << threshold << "\n";
//	cout << "diff = " << diff << "\n";
//	cout << "peakIndex1 = " << peakIndex1 << "\n";
//	cout << "peakIndex2 = " << peakIndex2 << "\n";
//	cout << "bufferSize = "<< bufferSize << "\n";
//	cout << "lowPassTempoB = " << mVerdict << "\n \n";
}

void CCHighPassTempoExpertB::judge()
{
	float *aCorrBufferIn = gFeatureList.highPassRMSAutocor();
	gFeatureList.highPassRMSAutocorPeak(); 	// find absPeakInd
	float threshold = gFeatureList.highPassRMSAutocorPeakThreshold();
	unsigned bufferSize = gFeatureList.statsBufferSize();	//num cached feature items
	float peak = 0.0;
	unsigned peakIndex1, peakIndex2;
	peakIndex1 = 0;
	peakIndex2 = 0;	
	
	// Threshold to pick first peak out of noise
	for(unsigned i = 5; i < bufferSize - 5; i++){
		if (isLocalPeak(aCorrBufferIn, i) && aCorrBufferIn[i] >= threshold) {
			peak = aCorrBufferIn[i];
			peakIndex1 = i;
			break;
		}
	}
	
	// Threshold to pick second peak out of noise
	for(unsigned i = peakIndex1; i < bufferSize - peakIndex1; i++){
		if (isLocalPeak(aCorrBufferIn, i) && aCorrBufferIn[i] >= threshold) {
			peak = aCorrBufferIn[i];
			peakIndex2 = i;
			break;
		}
	}
	
	//calculate tempo from two peaks
	float sampleRate = gFeatureList.mSampleRate;
	float diff = peakIndex2 - peakIndex1;
	float tempo;
	if(diff != 0)
	{
		tempo = ((float) sampleRate / (bufferSize * diff)) * 60;
	}else tempo = 0;
		
	mVerdict = tempo;
	
	review();
	
//	cout << "absPeak = " << absPeak << "\n";
//	cout << "threshold = " << threshold << "\n";
//	cout << "diff = " << diff << "\n";
//	cout << "peakIndex1 = " << peakIndex1 << "\n";
//	cout << "peakIndex2 = " << peakIndex2 << "\n";
//	cout << "bufferSize = "<< bufferSize << "\n";
//	cout << "highpPassTempoB = " << mVerdict << "\n \n";
}

#pragma mark -- CCTempoExpertA review --

void CCTempoExpertA::review() 
{

	//if verdict > 200 or < 2 then assume its crap
	
	if(mVerdict > 2000 || mVerdict < 2)
	{
		mVerdict = -1;
	} else
	{
		//if verdict is not in the range 60..220 divide/multiply by half until it is.
		
		if(mVerdict < 60)
		{
			while(mVerdict < 60)
			{
				mVerdict = 2.0 * mVerdict;
			}
		}
		
		if(mVerdict > 220)
		{	
			while(mVerdict > 220)
			{
				mVerdict = mVerdict / 2.0;
			}
		}
	}
	
	mConfidence = 0.5;	//hack to get reasonable answer
}

void CCLowPassTempoExpertA::review() 
{
	//if verdict > 200 or < 2 then assume its crap
	
	if(mVerdict > 2000 || mVerdict < 2)
	{
		mVerdict = -1;
	} else
	{
		//if verdict is not in the range 60..220 divide/multiply by half until it is.
		
		if(mVerdict < 60)
		{
			while(mVerdict < 60)
			{
				mVerdict = 2.0 * mVerdict;
			}
		}
		
		if(mVerdict > 220)
		{	
			while(mVerdict > 220)
			{
				mVerdict = mVerdict / 2.0;
			}
		}
	}
	
	mConfidence = 0.6;
}

void CCHighPassTempoExpertA::review() 
{
	//if verdict > 200 or < 2 then assume its crap
	
	if(mVerdict > 2000 || mVerdict < 2)
	{
		mVerdict = -1;
	} else
	{
		//if verdict is not in the range 60..220 divide/multiply by half until it is.
		
		if(mVerdict < 60)
		{
			while(mVerdict < 60)
			{
				mVerdict = 2.0 * mVerdict;
			}
		}
		
		if(mVerdict > 220)
		{	
			while(mVerdict > 220)
			{
				mVerdict = mVerdict / 2.0;
			}
		}
	}
	
	mConfidence = 0.55;
}

#pragma mark -- CCTempoExpertB review --

void CCTempoExpertB::review() 
{
	//if verdict > 200 or < 2 then assume its crap
	
	if(mVerdict > 2000 || mVerdict < 2)
	{
		mVerdict = -1;
	} else
	{
		//if verdict is not in the range 60..220 divide/multiply by half until it is.
		
		if(mVerdict < 60)
		{
			while(mVerdict < 60)
			{
				mVerdict = 2.0 * mVerdict;
			}
		}
		
		if(mVerdict > 220)
		{	
			while(mVerdict > 220)
			{
				mVerdict = mVerdict / 2.0;
			}
		}
	}
	
	mConfidence = 0.5;
}

void CCLowPassTempoExpertB::review() 
{
	//if verdict > 200 or < 2 then assume its crap
	
	if(mVerdict > 2000 || mVerdict < 2)
	{
		mVerdict = -1;
	} else
	{
		//if verdict is not in the range 60..220 divide/multiply by half until it is.
		
		if(mVerdict < 60)
		{
			while(mVerdict < 60)
			{
				mVerdict = 2.0 * mVerdict;
			}
		}
		
		if(mVerdict > 220)
		{	
			while(mVerdict > 220)
			{
				mVerdict = mVerdict / 2.0;
			}
		}
	}
	
	mConfidence = 0.6;
}

void CCHighPassTempoExpertB::review() 
{
	//if verdict > 200 or < 2 then assume its crap
	
	if(mVerdict > 2000 || mVerdict < 2)
	{
		mVerdict = -1;
	} else
	{
		//if verdict is not in the range 60..220 divide/multiply by half until it is.
		
		if(mVerdict < 60)
		{
			while(mVerdict < 60)
			{
				mVerdict = 2.0 * mVerdict;
			}
		}
		
		if(mVerdict > 220)
		{	
			while(mVerdict > 220)
			{
				mVerdict = mVerdict / 2.0;
			}
		}
	}
	
	mConfidence = 0.55;
}
