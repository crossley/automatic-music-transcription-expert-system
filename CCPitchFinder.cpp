/*
 *  CCTempoFinder.cpp
 *  CCTranscriber
 *
 *  Created by Bebelutz on 5/7/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "CCPitchFinder.h"
#include "CCFeatures.h"
#include "CCore.h"

extern CCFeatureList gFeatureList;

#include <iostream>
using std::cout;
using std::endl;

#pragma mark -- constructors --

//Constructor
CCPitchFinder::CCPitchFinder()
{
	
	mExperts.push_back(new CCMaxPeakPitchExpert());
	mExperts.push_back(new CCZeroCrossingPitchExpert());
	mExperts.push_back(new CCHarmonicProductPitchExpert());
	
	mVerdictHistory = new float[mExperts.size()];
	mConfidenceHistory = new float[mExperts.size()];
	mExpertiseHistory = new float[mExperts.size()];

}

CCMaxPeakPitchExpert::CCMaxPeakPitchExpert()
{
	mExpertise = 0.5;
}

CCZeroCrossingPitchExpert::CCZeroCrossingPitchExpert()
{
	mExpertise = 0.5;
}

CCHarmonicProductPitchExpert::CCHarmonicProductPitchExpert()
{
	mExpertise = 0.5;
}

#pragma mark -- CCPitchFinder --

void CCPitchFinder::judge() 
{

	//first loop thru experts to gather individual verdicts
	CCExpertGroup::judge();
	
	//now judge the evidence returned by experts
	int i = 0; int usefulVerdicts = mExperts.size();
	list<CCAbstractExpert *>::iterator expert;
	for (expert = mExperts.begin(); expert != mExperts.end(); expert++, i++)
	{

		mVerdictHistory[i] = (*expert)->verdict();
		mConfidenceHistory[i] = (*expert)->confidence();
		mExpertiseHistory[i] = (*expert)->expertise();
	}
	
	for(i = 0; i < mExperts.size(); i++)
	{
		if(mVerdictHistory[i] == -1)	// if block clearly didn't receive fair trial (see review())
		{
			mVerdictHistory[i] == 0;	// the verdict won't be considered
			usefulVerdicts--; 
		}
	}
	
	float highestConfidence = 0.0;			//the highest confidence returned by an expert
	float highestConfidenceVerdict = 0.0;	//the verdict of the most confident expert
	float groupConfidenceSTD = 0.0;			//the STD among expert confidence
	int numConfident = 0;					//the number of experts close to the highest confidence
	int numAgree = 0;						//number of confident experts that agree
	
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
		
		mVerdict = mVerdict / (float) numAgree;
//		cout << mVerdict<< endl;
	}
		
	//if most confident experts disagree then just take the most confident
	if(!(numAgree >= numConfident / 2))
	{
		mVerdict = highestConfidenceVerdict;
		//cout << "mVerdict = " << mVerdict << "\n";
	}
}

void CCPitchFinder::review() {}

#pragma mark -- CCPitchExpertA judge --

void CCMaxPeakPitchExpert::judge()
{
	const float *spectrumBufferPtr = gFeatureList.currentItem()->blackmanFourierSpectrumBuffer();
	unsigned bufferSize = gFeatureList.currentItem()->sampleBufferSize();
	
	// find the highest peak in the buffer
	unsigned peakIndex = findAbsPeakIndex(spectrumBufferPtr, bufferSize, 1);
//	peak = spectrumBufferPtr[index];
	
	float peakFrequency = (float) gFeatureList.mSampleRate * ((float)peakIndex - 1) * 0.5 / (float)bufferSize;
	
	mVerdict = peakFrequency;
	//	cout << "CCPitchExpertA veredict: " << mVerdict << endl;

	if(mLocalVerdictHistory.size() < 10)
	{
		mLocalVerdictHistory.push_back(mVerdict);
	} else
	{
		mLocalVerdictHistory.pop_front();
		mLocalVerdictHistory.push_back(mVerdict);
	}
	
}

#pragma mark -- CCZeroCrossingPitchExpert judge --

void CCZeroCrossingPitchExpert::judge()
{
	const float *sampleBufferPtr = gFeatureList.currentItem()->sampleBuffer();
	unsigned bufferSize = gFeatureList.currentItem()->sampleBufferSize();
	unsigned zerosCount = 0;
	float previousSample = *sampleBufferPtr++;
	float currentSample;
	for (unsigned i = 1; i < bufferSize; i++ ) {
		currentSample = *sampleBufferPtr++;
		if ((previousSample <= 0.0) && (currentSample > 0.0))
			zerosCount++;
		previousSample = currentSample;
	}
	
	float sampleRate = gFeatureList.mSampleRate;
	
	float theFrequency = zerosCount * (sampleRate / bufferSize);
	
	mVerdict = theFrequency;
	//	cout << "CCZeroCrossingPitchExpert veredict: " << mVerdict << endl;
	
	if(mLocalVerdictHistory.size() < 10)
	{
		mLocalVerdictHistory.push_back(mVerdict);
	} else
	{
		mLocalVerdictHistory.pop_front();
		mLocalVerdictHistory.push_back(mVerdict);
	}
}

#pragma mark -- CCHarmonicProductPitchExpert judge --

void CCHarmonicProductPitchExpert::judge()
{
	const float *spectrumBufferPtr = gFeatureList.currentItem()->blackmanFourierSpectrumBuffer();
	unsigned bufferSize = gFeatureList.currentItem()->sampleBufferSize();
	float harmonicProductSpectrum[bufferSize];
	unsigned index;
		
	memcpy(harmonicProductSpectrum, spectrumBufferPtr, bufferSize * sizeof(float));
	
	for (unsigned i = 2; i <= 5; i++) {
		index = 0;
		for (unsigned j = 0; index < bufferSize; j++, index+=i) {
			harmonicProductSpectrum[j] *= spectrumBufferPtr[index];
		}
	}

	// find the highest peak in the buffer
	unsigned peakIndex = findAbsPeakIndex(harmonicProductSpectrum, bufferSize, 1);
//	cout << peakIndex << endl;
	
	float sampleRate = gFeatureList.mSampleRate;
	
	// each bin spans 0.5*samplerate / fftSize hz
	float peakFrequency = (float)sampleRate * 0.5 * ((float)peakIndex) / (float)bufferSize;
	
	mVerdict = peakFrequency;
	// cout << "CCHarmonicProductPitchExpert veredict: " << mVerdict << endl;
	if(mLocalVerdictHistory.size() < 10)
	{
		mLocalVerdictHistory.push_back(mVerdict);
	} else
	{
		mLocalVerdictHistory.pop_front();
		mLocalVerdictHistory.push_back(mVerdict);
	}
}

#pragma mark -- CCPitchExpertA review --

void CCMaxPeakPitchExpert::review() 
{
	if(mVerdict > 4978.03 || mVerdict < 16.35)
	{
		mVerdict = -1;
	} else
	{	}
	
	mConfidence = 0.5;	//hack to get reasonable answer
}

#pragma mark -- CCZeroCrossingPitchExpert review --

void CCZeroCrossingPitchExpert::review() 
{
	if(mVerdict > 4978.03 || mVerdict < 16.35)
	{
		mVerdict = -1;
	} else
	{	
		// find local average
		float localAverage;
		list<float>::iterator i;
		for(i = mLocalVerdictHistory.begin(); i != mLocalVerdictHistory.end(); i++)
		{
			localAverage += *i;
		}
		
		localAverage = localAverage / mLocalVerdictHistory.size();
		
		// Zero crossings often off by an octave or a fifth
		// if off by an octave...
		if(fabs(mVerdict / localAverage - 2) < 0.5)	// probably gonna want to change this threshold
		{
			mVerdict = mVerdict / 2.0f;
		} else if(fabs(localAverage / mVerdict - 2) < 0.5)
		{
			mVerdict = mVerdict * 2.0f;
		}else if(fabs(mVerdict / localAverage - 1.3) < 0.5 /*??*/)
		{
			//what is the ratio for a fifth?
		} else if(fabs(localAverage / mVerdict - 1.3) < 0.5 /*??*/)
		{
			//...
		}
	}
	
	mConfidence = 0.5;	//hack to get reasonable answer
}

#pragma mark -- CCHarmonicProductPitchExpert review --

void CCHarmonicProductPitchExpert::review() 
{
	
	if(mVerdict > 4978.03 || mVerdict < 16.35)
	{
		mVerdict = -1;
	} else
	{	}
		
	mConfidence = 0.5;	//hack to get reasonable answer
}