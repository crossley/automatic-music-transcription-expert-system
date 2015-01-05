/*
 *  CCFeatureItem.h
 *  CCTranscriber
 *
 *  Created by Bebelutz on 5/7/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef CC_FEATURES_H			// This is in case you include this twice
#define CC_FEATURES_H

#include "CCFFTer.h"

// The feature item holds useful stats from a block of samples (judgment-free information).

class CCFeatureItem {
friend class CCFeatureList;
public:
	CCFeatureItem();
	~CCFeatureItem();

	void calculateBasicStats();

	//hack-like methods  ???
	unsigned sampleBufferSize();
	unsigned autocorBufferSize(); // This returns the size of the list (the number of feature items it has).
	void setSampleBuffer(float *sampleBuffer);

	float* sampleBuffer();
	float* hanningSampleBuffer();
	float* hammingSampleBuffer();
	float* blackmanSampleBuffer();
	float* lowPassSampleBuffer();
	float* lowPassHanningSampleBuffer();
	float* lowPassHammingSampleBuffer();
	float* lowPassBlackmanSampleBuffer();
	float* highPassSampleBuffer();
	float* highPassHanningSampleBuffer();
	float* highPassHammingSampleBuffer();
	float* highPassBlackmanSampleBuffer();
	float* fourierSpectrumBuffer();
	float* hanningFourierSpectrumBuffer();
	float* hammingFourierSpectrumBuffer();
	float* blackmanFourierSpectrumBuffer();
	float* logFourierSpectrumBuffer();
	float* logHanningFourierSpectrumBuffer();
	float* logHammingFourierSpectrumBuffer();
	float* logBlackmanFourierSpectrumBuffer();
	float* cepstrumBuffer();
	float sampleBufferRMS();
	float lowPassSampleBufferRMS();
	float lowPassHanningSampleBufferRMS();
	float lowPassHammingSampleBufferRMS();
	float lowPassBlackmanSampleBufferRMS();
	float highPassSampleBufferRMS();
	float highPassHanningSampleBufferRMS();
	float highPassHammingSampleBufferRMS();
	float highPassBlackmanSampleBufferRMS();
	float spectrumPeak();
	float fourierSpectralEnergy();
	unsigned zeroCrossings();

private:

	CCFFTer *mFFTAnalyzer;
	CCFFTer *mCepstrumAnalyzer;

	//Time-Domain stats
	float *mSampleBuffer; //< I only point to a buffer of samples. I don't own my memory.
	unsigned mSampleBufferSize; //< Unsure how to implement this...
	float mSampleMean;
	float mSampleBufferRMS; //< RMS of the buffer of samples
	float mTimePeak;
	unsigned mTimePeakIndex;
	unsigned mZeroCrossings;
//	float mTimeVar;
//	float mTimeSTD;
//	float mTimePosZeroCross;
//	float mTimeCentroid;

	//window-stats
	float* mHanningSampleBuffer;
	float* mHammingSampleBuffer;
	float* mBlackmanSampleBuffer;
	float mHanningSampleBufferRMS;
	float mHammingSampleBufferRMS;
	float mBlackmanSampleBufferRMS;
	
	//filter-stats
	float *mLowPassSampleBuffer; //< We own our own memory (allocated in featureItem constructor).
	float *mLowPassHanningSampleBuffer;
	float *mLowPassHammingSampleBuffer;
	float *mLowPassBlackmanSampleBuffer;
	float *mHighPassSampleBuffer;
	float *mHighPassHanningSampleBuffer;
	float *mHighPassHammingSampleBuffer;
	float *mHighPassBlackmanSampleBuffer;
	float mLowPassPrevIn[4];	//< Note: Can proably do with only one set of these...
	float mLowPassPrevOut[4];	//< just reuse the memory... yes?
	float mLowPassHanningPrevIn[4];
	float mLowPassHanningPrevOut[4];
	float mLowPassHammingPrevIn[4];
	float mLowPassHammingPrevOut[4];
	float mLowPassBlackmanPrevIn[4];
	float mLowPassBlackmanPrevOut[4];
	float mHighPassPrevIn[4];
	float mHighPassPrevOut[4];
	float mHighPassHanningPrevIn[4];
	float mHighPassHanningPrevOut[4];
	float mHighPassHammingPrevIn[4];
	float mHighPassHammingPrevOut[4];
	float mHighPassBlackmanPrevIn[4];
	float mHighPassBlackmanPrevOut[4];
	float mLowPassSampleBufferRMS;
	float mLowPassHanningSampleBufferRMS;
	float mLowPassHammingSampleBufferRMS;
	float mLowPassBlackmanSampleBufferRMS;
	float mHighPassSampleBufferRMS;
	float mHighPassHanningSampleBufferRMS;
	float mHighPassHammingSampleBufferRMS;
	float mHighPassBlackmanSampleBufferRMS;
	
//	//Autocor-Domain stats
//	float *mAutocorBuffer;
//	float mAutocorMean;
//	float mAutocorRMS;
//	float mAutocorPeak;
//	unsigned mAutocorPeakIndex;
//	float mAutocorVar;
//	float mAutocorSTD;
//	float mAutocorPosZeroCross;
//	float mAutocorCentroid;
	
//	//Autocor(Autocor)-Domain stats
//	float *mAutocorAutocorBuffer;
//	float mmAutocorAutocorMean;
//	float mAutocorAutocorRMS;
//	float mAutocorAutocorPeak;
//	unsigned mAutocorAutocorPeakIndex;
//	float mAutocorAutocorVar;
//	float mAutocorAutocorSTD;
//	float mAutocorAutocorPosZeroCross;
//	float mAutocorAutocorCentroid;
	
	//Fourier-Domain stats
	float *mFourierSpectrumBuffer;
	float *mHanningFourierSpectrumBuffer;
	float *mHammingFourierSpectrumBuffer;
	float *mBlackmanFourierSpectrumBuffer;
	float *mLogFourierSpectrumBuffer;
	float *mLogHanningFourierSpectrumBuffer;
	float *mLogHammingFourierSpectrumBuffer;
	float *mLogBlackmanFourierSpectrumBuffer;
	float mFourierSpectralEnergy;
//	float mFourierSpectrumMean;
//	float mFourierSpectrumRMS;
//	float mFourierSpectrumPeak;
//	unsigned mFourierSpectrumPeakIndex;
//	float mFourierSpectrumVar;
//	float mFourierSpectrumSTD;
//	float mFourierSpectrumPosZeroCross;
//	float mFourierSpectrumCentroid;
//	
	//Fourier(Fourier)-domain stats
	float *mCepstrumBuffer;
//	float mCepstrumMean;
//	float mCepstrumRMS;
//	float mCepstrumPeak;
//	unsigned mCepstrumPeakIndex;
//	float mCepstrumVar;
//	float mCepstrumSTD;
//	float mCepstrumPosZeroCross;
//	float mCepstrumCentroid;
//	
	
};

// holds a list of FeatureItems and stats about that list.

class CCFeatureList {
public:
	CCFeatureList(unsigned listLength = 192);
	~CCFeatureList();
	
	void calculateBasicStats();		// this approach isn't used yet... should it be?
	
	void advanceCurrentItem();
	CCFeatureItem *currentItem();
	CCFeatureItem *prevItem();
	float mSampleRate;
	unsigned statsBufferSize() { return mNumCachedFeatureItems; };
	
	float *rmsAutocor();
	float rmsAutocorPeak();
	float rmsAutocorPeakThreshold();
	float *lowPassRMSAutocor();
	float lowPassRMSAutocorPeak();
	float lowPassRMSAutocorPeakThreshold();
	float *highPassRMSAutocor();
	float highPassRMSAutocorPeak();
	float highPassRMSAutocorPeakThreshold();
	float *hanningRMSAutocor();
	float hanningRMSAutocorPeak();
	float hanningRMSAutocorThreshold();
	float *hammingRMSAutocor();
	float hammingRMSAutocorPeak();
	float hammingRMSAutocorThreshold();
	float *blackmanRMSAutocor();
	float blackmanRMSAutocorPeak();
	float blackmanRMSAutocorThreshold();
	float* spectralCrossCorrelation();
	float* spectralCrossDifference();
	float* crossCorCoef(){return mCrossCorCoef;}
	float* signalPowerDiff(){return mSignalPowerDiff;}
//	float* correlationSegments();
//	float* differenceSegments();
	
	
protected:
	CCFeatureItem *mFeatureItems;	// not a vector, because it is allocated at construction we don't need dynamic allocation.
	
	float *mRMSAutocor; //< The autocorrelation of the RMS values in the feature item list
	float *mLowPassRMSAutocor; //< The autocorrelation of the RMS values of the lowpassed samples in the feature item list
	float *mHighPassRMSAutocor; //< The autocorrelation of the RMS values of the highpassed samples in the feature item list
	float *mHanningRMSAutocor;
	float *mHammingRMSAutocor;
	float *mBlackmanRMSAutocor;
	
	float mRMSAutocorPeak; //< The highest value of the mRMSAutocor values in the feature item list
	float mLowPassRMSAutocorPeak; //< The highest value of the mLowPassRMSAutocor values in the feature item list 
	float mHighPassRMSAutocorPeak; //< The highest value of the mHighPassRMSAutocor values in the feature item list	
	float mHanningRMSAutocorPeak;
	float mHammingRMSAutocorPeak;
	float mBlackmanRMSAutocorPeak;
	
	float mRMSAutocorPeakThreshold; //< The minimum value a local max in mRMSAutocor must be to be called a peak -- should be a slider --
	float mLowPassRMSAutocorPeakThreshold;//< The minimum value a local max in mHighPassRMSAutocor must be to be called a peak
	float mHighPassRMSAutocorPeakThreshold;//< The minimum value a local max in mHighPassRMSAutocor must be to be called a peak
	float mHanningRMSAutocorPeakThreshold;
	float mHammingRMSAutocorPeakThreshold;
	float mBlackmanRMSAutocorPeakThreshold;
	
	float** magSpectralSequence; //this is already being stored... just gotta access it via the list
	float* mCrossCorCoef;
	float* mSignalPowerDiff;
	
	float mListMean;
	float mListRMS;	
	float mListPeak;
	float mListPeakInd;
	float mListVar;
	float mListSTD;
	float mListCentroid;
	
	float *mTempOppBuffer;
	
	unsigned mCurrentItemIndex; //< holds the index to the FeatureItem being analyzed.
	unsigned mNumCachedFeatureItems; //< the number of FeatureItems held in memory.
		
};


#endif
