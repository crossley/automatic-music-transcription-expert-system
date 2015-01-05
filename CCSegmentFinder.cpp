/*
 *  CCSegmentFinder.cpp
 *  CCTranscriber
 *
 *  Created by matthew crossley on 6/13/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "CCSegmentFinder.h"
#include "CCFeatures.h"
#include "CCore.h"

extern CCFeatureList gFeatureList;

#include <iostream>
using std::cout;
using std::endl;

#pragma mark -- constructors --

CCSegmentFinder::CCSegmentFinder() {
	mExperts.push_back(new CCorrelationExpert());
	mExperts.push_back(new CCDifferenceExpert());
	
	mCorrelationSegments = new float[gFeatureList.statsBufferSize()];
	mDifferenceSegments = new float[gFeatureList.statsBufferSize()];
	
	mVerdictHistory = new float[mExperts.size()];
	mConfidenceHistory = new float[mExperts.size()];
	mExpertiseHistory = new float[mExperts.size()];
}

CCorrelationExpert::CCorrelationExpert() {
	mExpertise = 0.5;
}

CCDifferenceExpert::CCDifferenceExpert() {
	mExpertise = 0.5;
}

#pragma mark -- judge --

void CCSegmentFinder::judge() 
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
	
	if(mVerdictHistory[0] == 1.0 || mVerdictHistory[1] == 1.0)
	{
		mVerdict = 1.0;
	}else
	{
		mVerdict = 0.0;
	}
	
	// If spikes cluttered together... consolidate them into one
	int mark = 0;
	list<float>::iterator localIter; 
	for(localIter = mLocalVerdictHistory.begin(); localIter != mLocalVerdictHistory.end(); localIter++, i++) 
	{
		if(*localIter == 1.0)
		{
			mark = 1;
			mVerdict = 0.0;
		}
	}
	
	// reset local history
	if(mark==1)
	{
		for(localIter = mLocalVerdictHistory.begin(); localIter != mLocalVerdictHistory.end(); localIter++, i++)
		{
			*localIter = 0.0;
		}
	}
	
	if(mLocalVerdictHistory.size() < 10)
	{
		mLocalVerdictHistory.push_back(mVerdict);
	} else
	{
		mLocalVerdictHistory.pop_front();
		mLocalVerdictHistory.push_back(mVerdict);
	}
	
//	cout << mVerdict << endl;
}

void CCorrelationExpert::judge() {

	if(gFeatureList.prevItem() != 0)
	{
		unsigned bufferSize = gFeatureList.currentItem()->sampleBufferSize();
		float* prevSpectrum = gFeatureList.prevItem()->blackmanFourierSpectrumBuffer();
		float* thisSpectrum = gFeatureList.currentItem()->blackmanFourierSpectrumBuffer();
		float prevSpectralEnergy = gFeatureList.prevItem()->fourierSpectralEnergy();
		float thisSpectralEnergy = gFeatureList.currentItem()->fourierSpectralEnergy();
		unsigned prevZeroCrossings = gFeatureList.prevItem()->zeroCrossings();
		unsigned thisZeroCrossings = gFeatureList.currentItem()->zeroCrossings();
	
		// segment: crossCorCoef = 1 during notes and significantly lower values at segment boundries (relative to neighbors)
		// Thus, search for local min in crossCorCoef to find note segments
		
		float crossCorCoef = dotProd(prevSpectrum, thisSpectrum, bufferSize) / sqrt(prevSpectralEnergy*thisSpectralEnergy);
		
		if(mCrossCorCoef.size() < 20)
		{
			mCrossCorCoef.push_back(crossCorCoef);
		} else
		{
			mCrossCorCoef.pop_front();
			mCrossCorCoef.push_back(crossCorCoef);
		}
		
		// local average
		float localAverageCor;
		list<float>::iterator cor;
		for(cor = mCrossCorCoef.begin(); cor != mCrossCorCoef.end(); cor++)
		{
			localAverageCor+= *cor;
		}
		
		localAverageCor = localAverageCor / mCrossCorCoef.size();
		
		// if crossCorCoef is substantially lower than localAverageCor
		// call current frame a note onset
		if(crossCorCoef < localAverageCor)
		{
			if(fabs(prevZeroCrossings - thisZeroCrossings) > 5.0) // arbitrary threshold
			{
				mVerdict = 1.0;
			}
		} else
		{
			mVerdict = 0.0;
		}
		
		if(mLocalVerdictHistory.size() < 10)
		{
			mLocalVerdictHistory.push_back(mVerdict);
		} else
		{
			mLocalVerdictHistory.pop_front();
			mLocalVerdictHistory.push_back(mVerdict);
		}
		
	}else
	{
		mVerdict = 0.0;
	}
	
//	cout << mVerdict << endl;
}

void CCDifferenceExpert::judge() {
	if(gFeatureList.prevItem() != 0)
	{
		unsigned bufferSize = gFeatureList.currentItem()->sampleBufferSize();
		float prevSpectralEnergy = gFeatureList.prevItem()->fourierSpectralEnergy();
		float thisSpectralEnergy = gFeatureList.currentItem()->fourierSpectralEnergy();
		unsigned prevZeroCrossings = gFeatureList.prevItem()->zeroCrossings();
		unsigned thisZeroCrossings = gFeatureList.currentItem()->zeroCrossings();
		
		// segment: crossCorCoef = 1 during notes and significantly lower values at segment boundries (relative to neighbors)
		// Thus, search for local min in crossCorCoef to find note segments
		
		float signalPowerDiff = fabs(thisSpectralEnergy - prevSpectralEnergy);
		
		if(mSignalPowerDiff.size() < 20)
		{
			mSignalPowerDiff.push_back(signalPowerDiff);
		} else
		{
			mSignalPowerDiff.pop_front();
			mSignalPowerDiff.push_back(signalPowerDiff);
		}
		
		// local average
		float localAverageDiff;
		list<float>::iterator cor;
		for(cor = mSignalPowerDiff.begin(); cor != mSignalPowerDiff.end(); cor++)
		{
			localAverageDiff+= *cor;
		}
		
		localAverageDiff = localAverageDiff / mSignalPowerDiff.size();
		
		// if crossCorCoef is substantially lower than localAverageCor
		// call current frame a note onset
		if(signalPowerDiff > localAverageDiff)
		{
			if(fabs(prevZeroCrossings - thisZeroCrossings) > 5) // arbitrary threshold
			{
				mVerdict = 1.0;
			}
		} else
		{
			mVerdict = 0.0;
		}
				
		if(mLocalVerdictHistory.size() < 10)
		{
			mLocalVerdictHistory.push_back(mVerdict);
		} else
		{
			mLocalVerdictHistory.pop_front();
			mLocalVerdictHistory.push_back(mVerdict);
		}
	} else
	{
		mVerdict = 0.0;
	}
	
//	cout << mVerdict << endl;
}

#pragma mark -- review --

void CCSegmentFinder::review() {}

void CCorrelationExpert::review() {}

void CCDifferenceExpert::review() {}