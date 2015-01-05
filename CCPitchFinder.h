/*
 *  CCTempoFinder.h
 *  CCTranscriber
 *
 *  Created by Bebelutz on 5/7/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef CC_PITCH_FINDER_H			// In case you include this twice
#define CC_PITCH_FINDER_H

#include "CCExpert.h"

class CCPitchFinder : public CCExpertGroup {
public:
	CCPitchFinder();						///< empty constructor
	virtual ~CCPitchFinder() { };			///< virtual destructor
	
	virtual void judge();					/// Clients call judge() to get the features of a buffer.

protected:
	virtual void review();
	float* mVerdictHistory; // holds onto the verdicst of each expert in a given block
	float* mConfidenceHistory;
	float* mExpertiseHistory;

};

class CCMaxPeakPitchExpert : public CCExpert {
public:
	CCMaxPeakPitchExpert();						///< empty constructor
	virtual ~CCMaxPeakPitchExpert() { };				///< virtual destructor
										/// Clients call judge() to get the features of a buffer.
	virtual void judge();		///< makes a judgment on the tempo of a block of samples.
	
protected:
	
	virtual void review();
	
};

class CCZeroCrossingPitchExpert : public CCExpert {
public:
	CCZeroCrossingPitchExpert();						///< empty constructor
	virtual ~CCZeroCrossingPitchExpert() { };				///< virtual destructor
										/// Clients call judge() to get the features of a buffer.
	virtual void judge();		///< this will be implemented in subclasses.
	
protected:

	virtual void review();	
};

class CCHarmonicProductPitchExpert : public CCExpert {
public:
	CCHarmonicProductPitchExpert();						///< empty constructor
	virtual ~CCHarmonicProductPitchExpert() { };				///< virtual destructor
										/// Clients call judge() to get the features of a buffer.
	virtual void judge();		///< this will be implemented in subclasses.
	
protected:

	virtual void review();
	
};


#endif