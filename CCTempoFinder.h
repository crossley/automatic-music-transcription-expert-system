/*
 *  CCTempoFinder.h
 *  CCTranscriber
 *
 *  Created by Bebelutz on 5/7/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef CC_TEMPO_FINDER_H			// This is in case you include this twice
#define CC_TEMPO_FINDER_H

#include "CCExpert.h"

class CCTempoFinder : public CCExpertGroup {
public:
	CCTempoFinder();							///< empty constructor
	virtual ~CCTempoFinder() { };				///< virtual destructor
										/// Clients call judge() to get the features of a buffer.
	
	virtual void judge();		///< makes a judgment on the tempo of a block of samples.

protected:
	virtual void review();
	float* mVerdictHistory; // holds onto the verdicst of each expert in a given block
	float* mConfidenceHistory;
	float* mExpertiseHistory;
										
};

class CCTempoExpertA : public CCExpert {
public:
	CCTempoExpertA();							///< empty constructor
	virtual ~CCTempoExpertA() { };				///< virtual destructor
										/// Clients call judge() to get the features of a buffer.
	virtual void judge();		///< makes a judgment on the tempo of a block of samples.
	
protected:
	virtual void review();
};

class CCLowPassTempoExpertA : public CCExpert {
public:
	CCLowPassTempoExpertA();					///< empty constructor
	virtual ~CCLowPassTempoExpertA() { };		///< virtual destructor
										/// Clients call judge() to get the features of a buffer.
	virtual void judge();		///< makes a judgment on the tempo of a block of samples.
	
protected:
	virtual void review();
	
};

class CCHighPassTempoExpertA : public CCExpert {
public:
	CCHighPassTempoExpertA();					///< empty constructor
	virtual ~CCHighPassTempoExpertA() { };		///< virtual destructor
										/// Clients call judge() to get the features of a buffer.
	virtual void judge();		///< makes a judgment on the tempo of a block of samples.
	
protected:
	virtual void review();
	
};

class CCTempoExpertB : public CCExpert {
public:
	CCTempoExpertB();							///< empty constructor
	virtual ~CCTempoExpertB() { };				///< virtual destructor
										/// Clients call judge() to get the features of a buffer.
	virtual void judge();		///< this will be implemented in subclasses.	
	
protected:
	virtual void review();
	
};

class CCLowPassTempoExpertB : public CCExpert {
public:
	CCLowPassTempoExpertB();					///< empty constructor
	virtual ~CCLowPassTempoExpertB() { };		///< virtual destructor
										/// Clients call judge() to get the features of a buffer.
	virtual void judge();		///< this will be implemented in subclasses.	
	
protected:
	virtual void review();
	
};

class CCHighPassTempoExpertB : public CCExpert {
public:
	CCHighPassTempoExpertB();				///< empty constructor
	virtual ~CCHighPassTempoExpertB() { };		///< virtual destructor
										/// Clients call judge() to get the features of a buffer.
	virtual void judge();		///< this will be implemented in subclasses.	
	
protected:
	virtual void review();
	
};

#endif
