/*
 *  CCFilter.cpp
 *  CCTranscriber
 *
 *  Created by matthew crossley on 5/18/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "CCFilter.h"

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
	output[0] = b[0]*inTemp[0] + b[1]*inTemp[1] + b[2]*inTemp[2] + b[3]*inTemp[3] + b[4]*input[0] / Gain
					+ a[1]*outTemp[0] + a[2]*outTemp[1] + a[3]*outTemp[2] + a[4]*outTemp[3];
					
	output[1] = b[0]*inTemp[1] + b[1]*inTemp[2] + b[2]*inTemp[3] + (b[3]*input[0] + b[4]*input[1]) / Gain
					+ a[1]*outTemp[1] + a[2]*outTemp[2] + a[3]*outTemp[3] + a[4]*output[0];
	
	output[2] = b[0]*inTemp[2] + b[1]*inTemp[3] + (b[2]*input[0] + b[3]*input[1] + b[4]*input[2]) / Gain
					+ a[1]*outTemp[2] + a[2]*outTemp[3] + a[3]*output[0] + a[4]*output[1];
	
	output[3] = b[0]*inTemp[3] + (b[1]*input[0] + b[2]*input[1] + b[3]*input[2] + b[4]*input[3]) / Gain
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
	output[0] = b[0]*inTemp[0] + b[1]*inTemp[1] + b[2]*inTemp[2] + b[3]*inTemp[3] + b[4]*input[0] / Gain
					+ a[1]*outTemp[0] + a[2]*outTemp[1] + a[3]*outTemp[2] + a[4]*outTemp[3];
					
	output[1] = b[0]*inTemp[1] + b[1]*inTemp[2] + b[2]*inTemp[3] + (b[3]*input[0] + b[4]*input[1]) / Gain
					+ a[1]*outTemp[1] + a[2]*outTemp[2] + a[3]*outTemp[3] + a[4]*output[0];
	
	output[2] = b[0]*inTemp[2] + b[1]*inTemp[3] + (b[2]*input[0] + b[3]*input[1] + b[4]*input[2]) / Gain
					+ a[1]*outTemp[2] + a[2]*outTemp[3] + a[3]*output[0] + a[4]*output[1];
	
	output[3] = b[0]*inTemp[3] + (b[1]*input[0] + b[2]*input[1] + b[3]*input[2] + b[4]*input[3]) / Gain
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
