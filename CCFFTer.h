/*
 *  CCFFTer.h
 *  CCTranscriber
 *
 *  Created by Bebelutz on 5/12/06.
 *  Copyright 2006 Jorge Castellanos. All rights reserved.
 *
 */

#ifndef CCFFTER_H
#define CCFFTER_H

#include <fftw3.h>				// FFTW include file


// The FFT analyzer class
class CCFFTer {
public:								// Constructor
	CCFFTer(unsigned windowSize = 512);
	~CCFFTer();

	void calculateFFT(float *inputSamples, float *outputSpectra);
	
protected:
	unsigned mWindowSize;			// analysis window size
	float * mFFTSampleBuffer;			// Buffer to store samples
	fftwf_complex * mFFTSpectrum;		// Buffer to store the spectrum

	fftwf_plan mFFTPlan;

};


#endif
