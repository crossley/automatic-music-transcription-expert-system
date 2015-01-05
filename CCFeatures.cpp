/*
 *  CCFeatureItem.cpp
 *  CCTranscriber
 *
 *  Created by Bebelutz on 5/7/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
  */

#include "CCFeatures.h"

#include "CCore.h"
#include "CCFilter.h"

#include <iostream>
using std::cout;
using std::endl;

#define TEMP_SIZE 512

#pragma mark CCFeatureItem Implementation

CCFeatureItem::CCFeatureItem() : mSampleBufferSize(TEMP_SIZE) {
	
	mHanningSampleBuffer = new float[TEMP_SIZE];
	mHammingSampleBuffer = new float[TEMP_SIZE];
	mBlackmanSampleBuffer = new float[TEMP_SIZE];
	mLowPassSampleBuffer = new float[TEMP_SIZE];
	mLowPassHanningSampleBuffer = new float[TEMP_SIZE];
	mLowPassHammingSampleBuffer = new float[TEMP_SIZE];
	mLowPassBlackmanSampleBuffer = new float[TEMP_SIZE];
	mHighPassSampleBuffer = new float[TEMP_SIZE];
	mHighPassHanningSampleBuffer = new float[TEMP_SIZE];
	mHighPassHammingSampleBuffer = new float[TEMP_SIZE];
	mHighPassBlackmanSampleBuffer = new float[TEMP_SIZE];	
	mFourierSpectrumBuffer = new float[TEMP_SIZE];
	mHanningFourierSpectrumBuffer = new float[TEMP_SIZE];
	mHammingFourierSpectrumBuffer = new float[TEMP_SIZE];
	mBlackmanFourierSpectrumBuffer = new float[TEMP_SIZE];
	mLogFourierSpectrumBuffer = new float[TEMP_SIZE];
	mCepstrumBuffer = new float[TEMP_SIZE];
	
	mFFTAnalyzer = new CCFFTer(TEMP_SIZE);

}

CCFeatureItem::~CCFeatureItem() {

	delete mFFTAnalyzer;
	delete mCepstrumAnalyzer;

}

// NOTE: ALL THE METHODS BELOW SHOULD BE AWARE IF THEY WERE CALLED, SO DO NOT 
// PERFORM THE CALCULATIONS AGAIN. FOR NOW, THE METHOD BELOW PRECALCULATES EVERYTHING,
// SO THE OTHER METHODS ASSUME IT'S ALREADY DONE. THIS SHOULD BE FIXED IN THE FUTURE :-) Jorge C. 
void CCFeatureItem::calculateBasicStats() {
	
	zeroCrossings();
	
	hanningSampleBuffer();
	hammingSampleBuffer();
	blackmanSampleBuffer();
	
	lowPassSampleBuffer();
	lowPassHanningSampleBuffer();
	lowPassHammingSampleBuffer();
	lowPassBlackmanSampleBuffer();
	highPassSampleBuffer();
	highPassHanningSampleBuffer();
	highPassHammingSampleBuffer();
	highPassBlackmanSampleBuffer();
	
	sampleBufferRMS();
	lowPassSampleBufferRMS();
	lowPassHanningSampleBufferRMS();
	lowPassHammingSampleBufferRMS();
	lowPassBlackmanSampleBufferRMS();
	highPassSampleBufferRMS();
	highPassHanningSampleBufferRMS();
	highPassHammingSampleBufferRMS();
	highPassBlackmanSampleBufferRMS();
	
	mFFTAnalyzer->calculateFFT(mSampleBuffer, mFourierSpectrumBuffer);
	mFFTAnalyzer->calculateFFT(mHanningSampleBuffer, mHanningFourierSpectrumBuffer);
	mFFTAnalyzer->calculateFFT(mHammingSampleBuffer, mHammingFourierSpectrumBuffer);
	mFFTAnalyzer->calculateFFT(mBlackmanSampleBuffer, mBlackmanFourierSpectrumBuffer);
	logFourierSpectrumBuffer();
	mFFTAnalyzer->calculateFFT(mLogFourierSpectrumBuffer, mCepstrumBuffer);
	
	fourierSpectralEnergy();
	
}

unsigned CCFeatureItem::sampleBufferSize() {
	return mSampleBufferSize;
}

void CCFeatureItem::setSampleBuffer(float *sampleBuffer) { 
	mSampleBuffer = sampleBuffer;
}

float *CCFeatureItem::sampleBuffer() {
	return mSampleBuffer;
}

unsigned CCFeatureItem::zeroCrossings() {
	float* sampleBufferPtr = sampleBuffer();
	mZeroCrossings = 0;
	for(int i = 0; i < sampleBufferSize() - 1; i++)
	{
		if(sampleBufferPtr[i] < 0.0 && sampleBufferPtr[i+1] > 0.0)
		{
			mZeroCrossings++;
		}
	}
	return mZeroCrossings;
}

float* CCFeatureItem::hanningSampleBuffer() {
	hanningWindow(mSampleBuffer, mHanningSampleBuffer, TEMP_SIZE, TEMP_SIZE);
	return mHanningSampleBuffer;
}

float* CCFeatureItem::hammingSampleBuffer() {
	hammingWindow(mSampleBuffer, mHammingSampleBuffer, TEMP_SIZE, TEMP_SIZE);
	return mHammingSampleBuffer;
}

float* CCFeatureItem::blackmanSampleBuffer() {
	BlackmanWindow(mSampleBuffer, mBlackmanSampleBuffer, TEMP_SIZE, TEMP_SIZE);
	return mBlackmanSampleBuffer;
}

float* CCFeatureItem::lowPassSampleBuffer() {
	float* inputBuffer = mSampleBuffer;
	float* filteredBuffer = mLowPassSampleBuffer;
	float* prevIn = mLowPassPrevIn;
	float* prevOut = mLowPassPrevOut;
	lowPassFilter(TEMP_SIZE, inputBuffer, filteredBuffer, prevIn, prevOut);
	
	return mLowPassSampleBuffer;
}

float* CCFeatureItem::highPassSampleBuffer() {
	float* inputBuffer = mSampleBuffer;
	float* filteredBuffer = mHighPassSampleBuffer;
	float* prevIn = mHighPassPrevIn;
	float* prevOut = mHighPassPrevOut;
	highPassFilter(TEMP_SIZE, inputBuffer, filteredBuffer, prevIn, prevOut);
	
	return mHighPassSampleBuffer;
}

float* CCFeatureItem::lowPassHanningSampleBuffer() {
	float* inputBuffer = mHanningSampleBuffer;
	float* filteredBuffer = mLowPassHanningSampleBuffer;
	float* prevIn = mLowPassHanningPrevIn;
	float* prevOut = mLowPassHanningPrevOut;
	lowPassFilter(TEMP_SIZE, inputBuffer, filteredBuffer, prevIn, prevOut);
	
	return mLowPassHanningSampleBuffer;
}

float* CCFeatureItem::lowPassHammingSampleBuffer() {
	float* inputBuffer = mHammingSampleBuffer;
	float* filteredBuffer = mLowPassHammingSampleBuffer;
	float* prevIn = mLowPassHammingPrevIn;
	float* prevOut = mLowPassHammingPrevOut;
	lowPassFilter(TEMP_SIZE, inputBuffer, filteredBuffer, prevIn, prevOut);
	
	return mLowPassHammingSampleBuffer;
}

float* CCFeatureItem::lowPassBlackmanSampleBuffer() {
	float* inputBuffer = mBlackmanSampleBuffer;
	float* filteredBuffer = mLowPassBlackmanSampleBuffer;
	float* prevIn = mLowPassBlackmanPrevIn;
	float* prevOut = mLowPassBlackmanPrevOut;
	lowPassFilter(TEMP_SIZE, inputBuffer, filteredBuffer, prevIn, prevOut);
	
	return mLowPassBlackmanSampleBuffer;
}

float* CCFeatureItem::highPassHanningSampleBuffer() {
	float* inputBuffer = mHanningSampleBuffer;
	float* filteredBuffer = mHighPassHanningSampleBuffer;
	float* prevIn = mHighPassHanningPrevIn;
	float* prevOut = mHighPassHanningPrevOut;
	lowPassFilter(TEMP_SIZE, inputBuffer, filteredBuffer, prevIn, prevOut);
	
	return mHighPassHanningSampleBuffer;
}

float* CCFeatureItem::highPassHammingSampleBuffer() {
	float* inputBuffer = mHammingSampleBuffer;
	float* filteredBuffer = mHighPassHammingSampleBuffer;
	float* prevIn = mHighPassHammingPrevIn;
	float* prevOut = mHighPassHammingPrevOut;
	lowPassFilter(TEMP_SIZE, inputBuffer, filteredBuffer, prevIn, prevOut);
	
	return mHighPassHammingSampleBuffer;
}

float* CCFeatureItem::highPassBlackmanSampleBuffer() {
	float* inputBuffer = mBlackmanSampleBuffer;
	float* filteredBuffer = mLowPassBlackmanSampleBuffer;
	float* prevIn = mHighPassBlackmanPrevIn;
	float* prevOut = mHighPassBlackmanPrevOut;
	lowPassFilter(TEMP_SIZE, inputBuffer, filteredBuffer, prevIn, prevOut);
	
	return mLowPassBlackmanSampleBuffer;
}

float* CCFeatureItem::fourierSpectrumBuffer() {	
	return mFourierSpectrumBuffer;
}

float* CCFeatureItem::hanningFourierSpectrumBuffer() {
	return mHanningFourierSpectrumBuffer;
}

float* CCFeatureItem::hammingFourierSpectrumBuffer() {
	return mHammingFourierSpectrumBuffer;
}

float* CCFeatureItem::blackmanFourierSpectrumBuffer() {
	return mBlackmanFourierSpectrumBuffer;
}

float* CCFeatureItem::logFourierSpectrumBuffer() {
	return logMagnitude(mFourierSpectrumBuffer, mLogFourierSpectrumBuffer, TEMP_SIZE);
}

float* CCFeatureItem::logHanningFourierSpectrumBuffer() {
	return logMagnitude(mHanningFourierSpectrumBuffer, mLogHanningFourierSpectrumBuffer, TEMP_SIZE);
}

float* CCFeatureItem::logHammingFourierSpectrumBuffer() {
	return logMagnitude(mHammingFourierSpectrumBuffer, mLogHammingFourierSpectrumBuffer, TEMP_SIZE);

}

float* CCFeatureItem::logBlackmanFourierSpectrumBuffer() {
	return logMagnitude(mBlackmanFourierSpectrumBuffer, mLogBlackmanFourierSpectrumBuffer, TEMP_SIZE);
}

float* CCFeatureItem::cepstrumBuffer() {
	return mCepstrumBuffer;
}

float CCFeatureItem::sampleBufferRMS() {
	mSampleBufferRMS = caluclateRms(mSampleBuffer, TEMP_SIZE);
	return mSampleBufferRMS;
}

float CCFeatureItem::lowPassSampleBufferRMS() {

	mLowPassSampleBufferRMS = caluclateRms(mLowPassSampleBuffer, TEMP_SIZE);
	return mLowPassSampleBufferRMS;
}

float CCFeatureItem::lowPassHanningSampleBufferRMS() {
	mLowPassHanningSampleBufferRMS = caluclateRms(mLowPassHanningSampleBuffer, TEMP_SIZE);
	return mLowPassHanningSampleBufferRMS;
}

float CCFeatureItem::lowPassHammingSampleBufferRMS() {
	mLowPassHammingSampleBufferRMS = caluclateRms(mLowPassHammingSampleBuffer, TEMP_SIZE);
	return mLowPassHammingSampleBufferRMS;
}

float CCFeatureItem::lowPassBlackmanSampleBufferRMS() {
	mLowPassBlackmanSampleBufferRMS = caluclateRms(mLowPassBlackmanSampleBuffer, TEMP_SIZE);
	return mLowPassBlackmanSampleBufferRMS;
}

float CCFeatureItem::highPassSampleBufferRMS() {
	mHighPassSampleBufferRMS = caluclateRms(mHighPassSampleBuffer, TEMP_SIZE);
	return mHighPassSampleBufferRMS;
}

float CCFeatureItem::highPassHanningSampleBufferRMS() {
	mHighPassHanningSampleBufferRMS = caluclateRms(mHighPassHanningSampleBuffer, TEMP_SIZE);
	return mHighPassHanningSampleBufferRMS;
}

float CCFeatureItem::highPassHammingSampleBufferRMS() {
	mHighPassHammingSampleBufferRMS = caluclateRms(mHighPassHammingSampleBuffer, TEMP_SIZE);
	return mHighPassHammingSampleBufferRMS;
}

float CCFeatureItem::highPassBlackmanSampleBufferRMS() {
	mHighPassBlackmanSampleBufferRMS = caluclateRms(mHighPassBlackmanSampleBuffer, TEMP_SIZE);
	return mHighPassBlackmanSampleBufferRMS;
}

float CCFeatureItem::spectrumPeak() {

	return 0;
}

unsigned CCFeatureItem::autocorBufferSize() {

	return TEMP_SIZE;
}

float CCFeatureItem::fourierSpectralEnergy() {
	
	mFourierSpectralEnergy = calculateMean(mFourierSpectrumBuffer, TEMP_SIZE);
	
}


#pragma mark CCFeatureList Implementation

// ******* FeatureList Implementation starts here! **************

CCFeatureList::CCFeatureList(unsigned listLength) : mNumCachedFeatureItems(listLength), mCurrentItemIndex(0), mSampleRate(44100) {

	mFeatureItems = new CCFeatureItem[mNumCachedFeatureItems];
	mTempOppBuffer = new float[mNumCachedFeatureItems];
	mRMSAutocor = new float[mNumCachedFeatureItems];
	mLowPassRMSAutocor = new float[mNumCachedFeatureItems];
	mHighPassRMSAutocor = new float[mNumCachedFeatureItems];
	mCrossCorCoef = new float[mNumCachedFeatureItems-1];
	mSignalPowerDiff = new float[mNumCachedFeatureItems-1];
}

CCFeatureList::~CCFeatureList() {

	delete [] mFeatureItems;
	delete [] mTempOppBuffer;
	delete [] mRMSAutocor;
	delete [] mLowPassRMSAutocor;
	delete [] mHighPassRMSAutocor;

};

void CCFeatureList::calculateBasicStats(){}

CCFeatureItem *CCFeatureList::currentItem() {
	return &mFeatureItems[mCurrentItemIndex];
}

CCFeatureItem *CCFeatureList::prevItem() {
	if(mCurrentItemIndex)
		return &mFeatureItems[mCurrentItemIndex-1];
	return 0;
}


void CCFeatureList::advanceCurrentItem() {

	++mCurrentItemIndex;	
	if(mCurrentItemIndex >= mNumCachedFeatureItems) {
		// TODO: save the contents of the temp feature items into a file.
		mCurrentItemIndex = 0;
	}
	
}

// *** All stats calculations on the list start here! *** //

float *CCFeatureList::rmsAutocor() {
	float *sampleRMSBufferPtr = mTempOppBuffer;
	unsigned index = mCurrentItemIndex;
	// go thru the list and get the RMS values
	for(unsigned i = 0; i < mNumCachedFeatureItems; ++i, --index) {
		sampleRMSBufferPtr[i] = mFeatureItems[index].mSampleBufferRMS;
		if(index == 0)
			index = mNumCachedFeatureItems;
	}
	
	// call the autocorrelation function and return a pointer to the autocorr buffer.
	caluclateAutocorrelation(sampleRMSBufferPtr, mRMSAutocor, mNumCachedFeatureItems);
	
	return mRMSAutocor;

}

float *CCFeatureList::hanningRMSAutocor() {
	float *sampleRMSBufferPtr = mTempOppBuffer;
	unsigned index = mCurrentItemIndex;
	// go thru the list and get the RMS values
	for(unsigned i = 0; i < mNumCachedFeatureItems; ++i, --index) {
		sampleRMSBufferPtr[i] = mFeatureItems[index].mHanningSampleBufferRMS;
		if(index == 0)
			index = mNumCachedFeatureItems;
	}
	
	// call the autocorrelation function and return a pointer to the autocorr buffer.
	caluclateAutocorrelation(sampleRMSBufferPtr, mHanningRMSAutocor, mNumCachedFeatureItems);
	
	return mHanningRMSAutocor;
}

float *CCFeatureList::hammingRMSAutocor() {
	float *sampleRMSBufferPtr = mTempOppBuffer;
	unsigned index = mCurrentItemIndex;
	// go thru the list and get the RMS values
	for(unsigned i = 0; i < mNumCachedFeatureItems; ++i, --index) {
		sampleRMSBufferPtr[i] = mFeatureItems[index].mHammingSampleBufferRMS;
		if(index == 0)
			index = mNumCachedFeatureItems;
	}
	
	// call the autocorrelation function and return a pointer to the autocorr buffer.
	caluclateAutocorrelation(sampleRMSBufferPtr, mHammingRMSAutocor, mNumCachedFeatureItems);
	
	return mHammingRMSAutocor;
}

float *CCFeatureList::blackmanRMSAutocor() {
	float *sampleRMSBufferPtr = mTempOppBuffer;
	unsigned index = mCurrentItemIndex;
	// go thru the list and get the RMS values
	for(unsigned i = 0; i < mNumCachedFeatureItems; ++i, --index) {
		sampleRMSBufferPtr[i] = mFeatureItems[index].mBlackmanSampleBufferRMS;
		if(index == 0)
			index = mNumCachedFeatureItems;
	}
	
	// call the autocorrelation function and return a pointer to the autocorr buffer.
	caluclateAutocorrelation(sampleRMSBufferPtr, mBlackmanRMSAutocor, mNumCachedFeatureItems);
	
	return mBlackmanRMSAutocor;
}

float *CCFeatureList::lowPassRMSAutocor() {
	float *lowPassSampleRMSBufferPtr = mTempOppBuffer;
	unsigned index = mCurrentItemIndex;

	// go thru the list and get the RMS values
	for(unsigned i = 0; i < mNumCachedFeatureItems; ++i, --index) {
		lowPassSampleRMSBufferPtr[i] = mFeatureItems[index].mLowPassSampleBufferRMS;
		if(index == 0)
			index = mNumCachedFeatureItems;
	}
	// call the autocorrelation function and return a pointer to the autocorr buffer.
	caluclateAutocorrelation(lowPassSampleRMSBufferPtr, mLowPassRMSAutocor, mNumCachedFeatureItems);
	
	return mLowPassRMSAutocor;
}

float *CCFeatureList::highPassRMSAutocor() {
	float *highPassSampleRMSBufferPtr = mTempOppBuffer;
	unsigned index = mCurrentItemIndex;
	
	// go thru the list and get the RMS values
	for(unsigned i = 0; i < mNumCachedFeatureItems; ++i, --index) {
		highPassSampleRMSBufferPtr[i] = mFeatureItems[index].mHighPassSampleBufferRMS;
		if(index == 0)
			index = mNumCachedFeatureItems;
	}
	// call the autocorrelation function and return a pointer to the autocorr buffer.
	caluclateAutocorrelation(highPassSampleRMSBufferPtr, mHighPassRMSAutocor, mNumCachedFeatureItems);
	
	return mHighPassRMSAutocor;
}

float CCFeatureList::rmsAutocorPeak() {
		
	// call the autocorrelation function and return a pointer to the autocorr buffer.
	unsigned index = findAbsPeakIndex(mRMSAutocor, mNumCachedFeatureItems, 5);
	mRMSAutocorPeak = mRMSAutocor[index];
	
	return mRMSAutocorPeak;

}

float CCFeatureList::hanningRMSAutocorPeak() {
	// call the autocorrelation function and return a pointer to the autocorr buffer.
	unsigned index = findAbsPeakIndex(mHanningRMSAutocor, mNumCachedFeatureItems, 5);
	mHanningRMSAutocorPeak = mHanningRMSAutocor[index];
	
	return mHanningRMSAutocorPeak;	
}

float CCFeatureList::hammingRMSAutocorPeak() {
	// call the autocorrelation function and return a pointer to the autocorr buffer.
	unsigned index = findAbsPeakIndex(mHammingRMSAutocor, mNumCachedFeatureItems, 5);
	mHammingRMSAutocorPeak = mHammingRMSAutocor[index];
	
	return mHammingRMSAutocorPeak;
}

float CCFeatureList::blackmanRMSAutocorPeak() {
	// call the autocorrelation function and return a pointer to the autocorr buffer.
	unsigned index = findAbsPeakIndex(mBlackmanRMSAutocor, mNumCachedFeatureItems, 5);
	mBlackmanRMSAutocorPeak = mBlackmanRMSAutocor[index];
	
	return mBlackmanRMSAutocorPeak;
}

float CCFeatureList::lowPassRMSAutocorPeak() {
	
	// call the autocorrelation function and return a pointer to the autocorr buffer.
	unsigned index = findAbsPeakIndex(mLowPassRMSAutocor, mNumCachedFeatureItems, 5);
	mLowPassRMSAutocorPeak = mLowPassRMSAutocor[index];
	
	return mLowPassRMSAutocorPeak;
	
}

float CCFeatureList::highPassRMSAutocorPeak() {

	// call the autocorrelation function and return a pointer to the autocorr buffer.
	unsigned index = findAbsPeakIndex(mHighPassRMSAutocor, mNumCachedFeatureItems, 5);
	mHighPassRMSAutocorPeak = mHighPassRMSAutocor[index];
	
	return mLowPassRMSAutocorPeak;
	
}

float CCFeatureList::rmsAutocorPeakThreshold() {

	float STD = calculateSTD(mRMSAutocor, mNumCachedFeatureItems);
	mRMSAutocorPeak = rmsAutocorPeak();		// Think about putting this kind of stuff in calculateBasicStats...
	float threshold = mRMSAutocorPeak - 2.0 * STD;
	mRMSAutocorPeakThreshold = threshold;
	
//	cout << "\n" << "Autocor STD = " << STD << "\n";
//	cout << "mRMSAutocorPeak = " << mRMSAutocorPeak << "\n";
//	cout << "threshold = " << threshold << "\n";
	
	return mRMSAutocorPeakThreshold;

}

float CCFeatureList::hanningRMSAutocorThreshold() {
	float STD = calculateSTD(mHanningRMSAutocor, mNumCachedFeatureItems);
	mHanningRMSAutocorPeak = hanningRMSAutocorPeak();		// Think about putting this kind of stuff in calculateBasicStats...
	float threshold = mHanningRMSAutocorPeak - 2.0 * STD;
	mHanningRMSAutocorPeakThreshold = threshold;
	
//	cout << "\n" << "Autocor STD = " << STD << "\n";
//	cout << "mRMSAutocorPeak = " << mRMSAutocorPeak << "\n";
//	cout << "threshold = " << threshold << "\n";
	
	return mHanningRMSAutocorPeakThreshold;
}

float CCFeatureList::hammingRMSAutocorThreshold() {
	float STD = calculateSTD(mHammingRMSAutocor, mNumCachedFeatureItems);
	mHammingRMSAutocorPeak = hammingRMSAutocorPeak();		// Think about putting this kind of stuff in calculateBasicStats...
	float threshold = mHammingRMSAutocorPeak - 2.0 * STD;
	mHammingRMSAutocorPeakThreshold = threshold;
	
//	cout << "\n" << "Autocor STD = " << STD << "\n";
//	cout << "mRMSAutocorPeak = " << mRMSAutocorPeak << "\n";
//	cout << "threshold = " << threshold << "\n";
	
	return mHammingRMSAutocorPeakThreshold;
}

float CCFeatureList::blackmanRMSAutocorThreshold() {
	float STD = calculateSTD(mBlackmanRMSAutocor, mNumCachedFeatureItems);
	mBlackmanRMSAutocorPeak = blackmanRMSAutocorPeak();		// Think about putting this kind of stuff in calculateBasicStats...
	float threshold = mBlackmanRMSAutocorPeak - 2.0 * STD;
	mBlackmanRMSAutocorPeakThreshold = threshold;
	
//	cout << "\n" << "Autocor STD = " << STD << "\n";
//	cout << "mRMSAutocorPeak = " << mRMSAutocorPeak << "\n";
//	cout << "threshold = " << threshold << "\n";
	
	return mBlackmanRMSAutocorPeakThreshold;
}

float CCFeatureList::lowPassRMSAutocorPeakThreshold() {

	float STD = calculateSTD(mLowPassRMSAutocor, mNumCachedFeatureItems);
	mLowPassRMSAutocorPeak = lowPassRMSAutocorPeak();	// Think about putting this kind of stuff in calculateBasicStats...
	float threshold = mLowPassRMSAutocorPeak - 2.0 * STD;
	mLowPassRMSAutocorPeakThreshold = threshold;
	
//	cout << "\n" << "lowPassAutocor STD = " << STD << "\n";
//	cout << "mLowPassRMSAutocorPeak = " << mLowPassRMSAutocorPeak << "\n";
//	cout << "threshold = " << threshold << "\n";
	
	return mLowPassRMSAutocorPeakThreshold;

}

float CCFeatureList::highPassRMSAutocorPeakThreshold() {

	float STD = calculateSTD(mHighPassRMSAutocor, mNumCachedFeatureItems);
	mHighPassRMSAutocorPeak = highPassRMSAutocorPeak();	// Think about putting this kind of stuff in calculateBasicStats...
	float threshold = mHighPassRMSAutocorPeak - 2.0 * STD;
	mHighPassRMSAutocorPeakThreshold = threshold;
	
//	cout << "\n" << "highPassAutocor STD = " << STD << "\n";
//	cout << "mHighPassRMSAutocorPeak = " << mLowPassRMSAutocorPeak << "\n";
//	cout << "threshold = " << threshold << "\n";
	
	return mHighPassRMSAutocorPeakThreshold;

}

float* CCFeatureList::spectralCrossCorrelation() {
	
	for(int i = 0; i < statsBufferSize()-1; i++)
	{
		float* thisBuffer = mFeatureItems[i].mFourierSpectrumBuffer;
		float* nextBuffer = mFeatureItems[i+1].mFourierSpectrumBuffer;
		float thisSpectralEnergy = mFeatureItems[i].mFourierSpectralEnergy;
		float nextSpectralEnergy = mFeatureItems[i+1].mFourierSpectralEnergy;
		mCrossCorCoef[i] = dotProd(thisBuffer, nextBuffer, TEMP_SIZE) / sqrt(thisSpectralEnergy*nextSpectralEnergy);
	}
}
float* CCFeatureList::spectralCrossDifference() {
	for(int i = 0; i < statsBufferSize()-1; i++)
	{
		float thisSpectralEnergy = mFeatureItems[i].mFourierSpectralEnergy;
		float nextSpectralEnergy = mFeatureItems[i+1].mFourierSpectralEnergy;
		mSignalPowerDiff[i] = fabs(thisSpectralEnergy - nextSpectralEnergy);
	}
}
