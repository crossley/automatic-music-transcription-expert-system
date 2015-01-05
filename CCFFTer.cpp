/*
 *  CCFFTer.cpp
 *  CCTranscriber
 *
 *  Created by Bebelutz on 5/12/06.
 *  Copyright 2006 Jorge Castellanos. All rights reserved.
 *
 */

#include "CCFFTer.h"

#include <string.h>
#include <math.h>


#include <iostream>
using std::cout;
using std::endl;


// Set up the FFTer
CCFFTer::CCFFTer( unsigned windowSize) : mWindowSize(windowSize)
{	
	//real input of size n
	//complex transform of size n/2 + 1
	mFFTSpectrum = (fftwf_complex*) fftwf_malloc( sizeof(fftwf_complex ) * (mWindowSize + 1));
	mFFTSampleBuffer = (float *) fftwf_malloc(sizeof(float) * 2 * mWindowSize);
	
	mFFTPlan = fftwf_plan_dft_r2c_1d(2 * mWindowSize, mFFTSampleBuffer, mFFTSpectrum, FFTW_MEASURE);
}

// Destructor
CCFFTer::~CCFFTer( ) {
	fftwf_destroy_plan(mFFTPlan);
	fftwf_free( mFFTSpectrum );
}

void CCFFTer::calculateFFT(float *inputSamples, float *outputSpectra) {
	
	memset(mFFTSampleBuffer, 0, 2 * mWindowSize * sizeof(float));
	memcpy(mFFTSampleBuffer, inputSamples, mWindowSize * sizeof(float));

	fftwf_execute( mFFTPlan );

	fftwf_complex *spectrumPtr = mFFTSpectrum;
	float normFactor = 1.0 / sqrt( static_cast<double>( mWindowSize ) );

	for (unsigned i = 0; i < mWindowSize ; i++, spectrumPtr++) {
		outputSpectra[i] = hypot(*(spectrumPtr)[0], *(spectrumPtr)[1]) * normFactor;
//		cout << outputSpectra[i] << "\t\t\t"<< i << endl;
#ifdef DEBUG_MESSAGES
//		cout << outputSpectra[i] << endl;
#endif
	}

}

