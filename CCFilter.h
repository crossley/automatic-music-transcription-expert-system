/*
 *  CCFilter.h
 *  CCTranscriber
 *
 *  Created by matthew crossley on 5/18/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef CCFILTER_H
#define CCFILTER_H

#include <math.h>

void lowPassFilter(unsigned inputLength, float *inputBuffer, float *filteredBuffer, float prevIn[4], float prevOut[4]);
void highPassFilter(unsigned inputLength, float *inputBuffer, float *filteredBuffer, float prevIn[4], float prevOut[4]);

#endif