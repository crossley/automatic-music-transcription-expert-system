/*
 *  CCCore.h
 *  CCTranscriber
 *
 *  Created by Bebelutz on 5/7/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef CC_CORE_H			// This is in case you include this twice
#define CC_CORE_H

#include <math.h>

float caluclateRms(float *sampleBufferPtr, unsigned bufferSize)
{
	float sum = 0.0, samp;
	
	// compute the RMS value of the buffer
    for(unsigned i = 0; i < bufferSize; i++ ) {
		samp = sampleBufferPtr[i];
		sum += samp * samp;
    }
	
	return sqrt(sum / bufferSize);
}


void caluclateAutocorrelation(float *rmsBufferPtr, float *correlationBufferPtr, unsigned bufferSize)
{
	float sum;
	// AUTOCORR (original source from libtsp P. Kabal)
	for (unsigned i = 0; i < bufferSize; ++i) {
		sum = 0.0;
		for (unsigned j = 0; j < bufferSize - i; ++j) {
			sum += rmsBufferPtr[j] * rmsBufferPtr[i + j];
		}
		
		correlationBufferPtr[i] = sum;
	}

}


unsigned findAbsPeakIndex(const float *bufferIn, unsigned bufferSize, unsigned skip = 0)
{
	float absPeak = 0;
	unsigned absPeakIndex = 0;
	const float *theBuffer = bufferIn;
	for (unsigned i = skip; i < bufferSize - skip; i++) {
		// method 2: find absolute peak value
		if (theBuffer[i] > absPeak) {
			absPeak = theBuffer[i];
			absPeakIndex = i;
		}
	}
	
	return absPeakIndex;
}

bool isLocalPeak(float *bufferIn, unsigned index)
{
	unsigned i = index;
	
	if((bufferIn[i] > bufferIn[i-1]) && (bufferIn[i] > bufferIn[i+1]))
		return true;
	else
		return false;
}

float calculateMean(float *bufferIn, unsigned bufferSize) {
	
	float *buffer = bufferIn;
	unsigned N = bufferSize;
	float mean = 0.0;
	
	for(unsigned i=0; i<N; i++)
	{
		mean = mean + buffer[i];
	}
	
	mean = mean / (float) N;
	
	return mean;
}

float calculateVar(float *bufferIn, unsigned bufferSize) {
	
	float * buffer = bufferIn;
	unsigned N = bufferSize;
	
	float mean = calculateMean(buffer, N);
	
	float var = 0.0;
	float diff = 0.0;
	
	for(unsigned i=0; i<N; i++)
	{
		diff = buffer[i] - mean;
		diff = diff * diff;
		var = var + diff;
	}
	
	var = var / (float) (N-1);
	
	return var;
}

float calculateSTD(float *bufferIn, unsigned bufferSize) {
	
	float * buffer = bufferIn;
	unsigned N = bufferSize;
	
	float var = calculateVar(buffer, N);
	
	float STD = sqrt(var);
	
	return STD;
}

float * logMagnitude(float *bufferIn, float* bufferOut, unsigned bufferSize) {

	float * buffer = bufferIn;
	float * logBuffer = bufferOut;
	unsigned N = bufferSize;
	
	int i;
	for(i = 0; i < N; i++)
	{
		logBuffer[i] = log10(buffer[i]);
	}
	
	return logBuffer;

}

#pragma mark -- Filter --

//low-pass recurrence equation

/*y[n] = (  1 * x[n- 4])
     + (  4 * x[n- 3])
     + (  6 * x[n- 2])
     + (  4 * x[n- 1])
     + (  1 * x[n- 0])

     + ( -0.9282404974 * y[n- 4])
     + (  3.7820790085 * y[n- 3])
     + ( -5.7793788971 * y[n- 2])
     + (  3.9255397507 * y[n- 1])*/
	 
/* Digital filter designed by mkfilter/mkshape/gencode   A.J. Fisher
   Command line: /www/usr/fisher/helpers/mkfilter -Bu -Lp -o 4 -a 4.5351473923e-03 0.0000000000e+00 -l */

void lowPassFilter(unsigned inputLength, float *inputBuffer, float *filteredBuffer, float prevIn[4], float prevOut[4]) {
	
	unsigned length = inputLength;
	float *input = inputBuffer;
	float *output = filteredBuffer;
	float *inTemp = prevIn;
	float *outTemp = prevOut;
	float b[5] = {1.0, 4.0, 6.0, 4.0, 1.0};
	float a[5] = {1.0, -0.9282404974, 3.7820790085, -5.7793788971, 3.9255397507};
	float Gain = 2.518181782e+07;
	
	int i;
	
	//scale input to compensate for filter gain
//	for(i = 0; i < length; i++)
//	{
//		input[i] = (float) input[i] / Gain;
//	}
	
	//take care of filter history (first 4 ticks) -- crappy manual for() --
	output[0] = (b[0]*inTemp[0] + b[1]*inTemp[1] + b[2]*inTemp[2] + b[3]*inTemp[3] + b[4]*input[0]) / Gain
					+ a[1]*outTemp[0] + a[2]*outTemp[1] + a[3]*outTemp[2] + a[4]*outTemp[3];
					
	output[1] = (b[0]*inTemp[1] + b[1]*inTemp[2] + b[2]*inTemp[3] + b[3]*input[0] + b[4]*input[1]) / Gain
					+ a[1]*outTemp[1] + a[2]*outTemp[2] + a[3]*outTemp[3] + a[4]*output[0];
	
	output[2] = (b[0]*inTemp[2] + b[1]*inTemp[3] + b[2]*input[0] + b[3]*input[1] + b[4]*input[2]) / Gain
					+ a[1]*outTemp[2] + a[2]*outTemp[3] + a[3]*output[0] + a[4]*output[1];
	
	output[3] = (b[0]*inTemp[3] + b[1]*input[0] + b[2]*input[1] + b[3]*input[2] + b[4]*input[3]) / Gain
					+ a[1]*outTemp[3] + a[2]*output[0] + a[3]*output[1] + a[4]*output[2];
	
	//take care of current block
	for (i = 4; i < length; i++)
	{		
		output[i] = (b[0]*input[i-4] + b[1]*input[i-3] + b[2]*input[i-2] + b[3]*input[i-1] + b[4]*input[i]) / Gain
					+ a[1]*output[i-4] + a[2]*output[i-3] + a[3]*output[i-2] + a[4]*output[i-1];
	}
	
	//update filter history
	for(i = 0; i < 4; i++)
	{
		inTemp[i] = input[length - 4 + i];
		outTemp[i] = output[length - 4 + i];
	}
}

//Recurrence relation:
//y[n] = (  1 * x[n- 4])
//     + (  4 * x[n- 3])
//     + (  6 * x[n- 2])
//     + (  4 * x[n- 1])
//     + (  1 * x[n- 0])
//
//     + ( -0.4735064529 * y[n- 4])
//     + (  2.2460436820 * y[n- 3])
//     + ( -4.0337683927 * y[n- 2])
//     + (  3.2565693110 * y[n- 1])

void highPassFilter(unsigned inputLength, float *inputBuffer, float *filteredBuffer, float prevIn[4], float prevOut[4]) 
{
	
	unsigned length = inputLength;
	float *input = inputBuffer;
	float *output = filteredBuffer;
	float *inTemp = prevIn;
	float *outTemp = prevOut;
	float b[5] = {1.0, 4.0, 6.0, 4.0, 1.0};
	float a[5] = {1.0, -0.4735064529, 2.2460436820, -4.0337683927, 3.2565693110};
	float Gain = 3.432111891e+03;
	
	int i;
	
	//scale input to compensate for filter gain
//	for(i = 0; i < length; i++)
//	{
//		input[i] = (float) input[i] / Gain;
//	}
	
	//take care of filter history (first 4 ticks) -- crappy manual for() --
	output[0] = (b[0]*inTemp[0] + b[1]*inTemp[1] + b[2]*inTemp[2] + b[3]*inTemp[3] + b[4]*input[0]) / Gain
					+ a[1]*outTemp[0] + a[2]*outTemp[1] + a[3]*outTemp[2] + a[4]*outTemp[3];
					
	output[1] = (b[0]*inTemp[1] + b[1]*inTemp[2] + b[2]*inTemp[3] + b[3]*input[0] + b[4]*input[1]) / Gain
					+ a[1]*outTemp[1] + a[2]*outTemp[2] + a[3]*outTemp[3] + a[4]*output[0];
	
	output[2] = (b[0]*inTemp[2] + b[1]*inTemp[3] + b[2]*input[0] + b[3]*input[1] + b[4]*input[2]) / Gain
					+ a[1]*outTemp[2] + a[2]*outTemp[3] + a[3]*output[0] + a[4]*output[1];
	
	output[3] = (b[0]*inTemp[3] + b[1]*input[0] + b[2]*input[1] + b[3]*input[2] + b[4]*input[3]) / Gain
					+ a[1]*outTemp[3] + a[2]*output[0] + a[3]*output[1] + a[4]*output[2];
	
	//take care of current block
	for (i = 4; i < length; i++)
	{		
		output[i] = (b[0]*input[i-4] + b[1]*input[i-3] + b[2]*input[i-2] + b[3]*input[i-1] + b[4]*input[i]) / Gain
					+ a[1]*output[i-4] + a[2]*output[i-3] + a[3]*output[i-2] + a[4]*output[i-1];
	}
	
	//update filter history
	for(i = 0; i < 4; i++)
	{
		inTemp[i] = input[length - 4 + i];
		outTemp[i] = output[length - 4 + i];
	}
}

#pragma mark -- Window --

void hanningWindow(float* input, float* output, unsigned long inputSize, unsigned long windowSize)
{
	unsigned long i;
	for(i=0; i<windowSize; i++)
	{
		output[i] = input[i] * (0.5 - 0.5*cos(2.0*3.14159*i/windowSize));
	}
	for(i=windowSize; i<inputSize; i++)
	{
		output[i] = 0;
	}
}

void hammingWindow(float* input, float* output, unsigned long inputSize, unsigned long windowSize)
{
	unsigned long i;
	for(i=0; i<windowSize; i++)
	{
		output[i] = input[i] * (0.54 - 0.46*cos(2*3.14159*i/windowSize));
	}
	for(i=windowSize; i<inputSize; i++)
	{
		output[i] = 0;
	}
}

void BlackmanWindow(float* input, float* output, unsigned long inputSize, unsigned long windowSize)
{
	unsigned long i;
	for(i=0; i<windowSize; i++)
	{
		output[i] = input[i] * (0.42 - 0.5*cos(2*3.14159*i/windowSize) + 0.08*cos(4*3.14159*i/windowSize));
	}
	for(i=windowSize; i<inputSize; i++)
	{
		output[i] = 0;
	}
}

float calculateNeighborhoodAverage(const float* input, unsigned peakIndex, unsigned bufferSize, unsigned numAveragePoints)
{
	// how close is peak to buffer boundries
	int roomToSpareLow = peakIndex;
	int roomToSpareHigh = bufferSize - peakIndex;
	
	float ave = input[peakIndex];
	// if peak is away from boundries
	if(peakIndex >= numAveragePoints / 2)
	{
		for(int i = 1; i < numAveragePoints / 2; i++)
		{
			ave += input[i+peakIndex] + input[peakIndex-i];
		}
		
		return ave / numAveragePoints;
	} else if(roomToSpareLow < roomToSpareHigh)
	{
		for(int i = 1; i < roomToSpareLow; i++)
		{
			ave += input[i+peakIndex] + input[peakIndex-i];
		}
		return ave / 2*roomToSpareLow;
	} else
	{
		for(int i = 1; i < roomToSpareHigh; i++)
		{
			ave += input[i+peakIndex] + input[peakIndex-i];
		}
		return ave / 2*roomToSpareHigh;
	}
	
}

float dotProd(float * vec1, float * vec2, int N)
{
	float * v1 = vec1;
	float * v2 = vec2;
	float result = 0.0;
	
	int i;
	for(i = 0; i < N; i++)
	{
		result += v1[i]*v2[i];
	}
	
	return result;
}

#endif
