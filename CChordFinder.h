/*
 *  CChordFinder.h
 *  CCTranscriber
 *
 *  Created by matthew crossley on 6/12/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */
 
#ifndef CC_CHORD_FINDER_H			// In case you include this twice
#define CC_CHORD_FINDER_H

#include "CCExpert.h"
#include <iostream>
#include <fftw3.h>	
#include <sndfile.h>
#include <math.h>
#include <list>
#include <iomanip.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include "nnwork.h"
#include "peak.h"

#pragma mark -- constructors --

class CChordFinder : public CCExpertGroup {
public:
	CChordFinder();						///< empty constructor
	virtual ~CChordFinder() { };			///< virtual destructor
	
	virtual void judge();					/// Clients call judge() to get the features of a buffer.

protected:
	virtual void review();
	char* mVerdictHistory;
	float* mConfidenceHistory;
	float* mExpertiseHistory;
	char mCharVerdict;
	char charVerdict();

};

class CChordNeuralNet : public CCExpert {
public:
	CChordNeuralNet();						///< empty constructor
	virtual ~CChordNeuralNet() { };				///< virtual destructor
										/// Clients call judge() to get the features of a buffer.
	virtual void judge();		///< this will be implemented in subclasses.
	
protected:
	
	float mNeighborhoodAverage;
	list<Peak> mCepstrumPeaks;
	float* mNeuralNetInput;
	float* mNeuralNetOutput;
	float mNumPeaks;
	nnwork mChordTracker;
	char mVerdict;

	virtual void review();
	
};

#endif