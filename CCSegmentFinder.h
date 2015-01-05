/*
 *  CCSegmentFinder.h
 *  CCTranscriber
 *
 *  Created by matthew crossley on 6/13/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef CC_SEGMENT_FINDER_H			// In case you include this twice
#define CC_SEGMENT_FINDER_H

#include "CCExpert.h"

class CCSegmentFinder : public CCExpertGroup {
public:
	CCSegmentFinder();						///< empty constructor
	virtual ~CCSegmentFinder() { };			///< virtual destructor
	
	virtual void judge();					/// Clients call judge() to get the features of a buffer.

protected:
	virtual void review();
	float* mVerdictHistory;
	float* mConfidenceHistory;
	float* mExpertiseHistory;
	
	float* mCorrelationSegments;	// the buffers of spikes to be displayed
	float* mDifferenceSegments;
};

class CCorrelationExpert : public CCExpert {
public:
	CCorrelationExpert();
	virtual ~CCorrelationExpert() { };
										
	virtual void judge();
	
protected:
	
	virtual void review();
	list<float> mCrossCorCoef;
	
};

class CCDifferenceExpert : public CCExpert {
public:
	CCDifferenceExpert();
	virtual ~CCDifferenceExpert() { };
										
	virtual void judge();
	
protected:
	
	virtual void review();
	list<float> mSignalPowerDiff;
	
};


#endif