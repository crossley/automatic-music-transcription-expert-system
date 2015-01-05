/*
 *  CCExpert.h
 *  CCTranscriber
 *
 *  Created by Bebelutz on 5/7/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef CC_EXPERT_H			// This is in case you include this twice
#define CC_EXPERT_H

using namespace std;

#include <list>

// Composite Pattern. Expert is an abstract class that keeps a list of experts.
// the default behavior of judge is to loop thru its children and call judge on each one.
class CCExpertGroup;

class CCAbstractExpert {
public:
	CCAbstractExpert() : mConfidence(0.0f), mExpertise(0.0f), mVerdict(0.0f) { };						///< empty constructor
	virtual ~CCAbstractExpert() { };				///< virtual destructor
	
	float confidence() { return mConfidence; }
	float verdict() { return mVerdict; }
	float expertise() { return mExpertise; }
	/// Clients call judge() to get the features of a buffer.
	virtual void judge() = 0;		///< this will be implemented in subclasses.
	
protected:
	virtual void review() = 0;
	CCExpertGroup *parentExpert;
	float mConfidence;
	float mExpertise;
	float mVerdict;
	list<float> mLocalVerdictHistory;
};

class CCExpertGroup : public CCAbstractExpert {
public:
	CCExpertGroup();
	virtual ~CCExpertGroup();

	virtual void judge();		///< I loop thru my list of Experts and call judge on them.

protected:
	virtual void review() {};
	list<CCAbstractExpert *> mExperts;		// the list of experts (children)
};

class CCExpert : public CCAbstractExpert {
public:
	CCExpert() { };						///< empty constructor
	virtual ~CCExpert();				///< virtual destructor
										/// Clients call judge() to get the features of a buffer.
//	virtual void judge();		///< this will be implemented in subclasses.

protected:
	virtual void review() {};
};

#endif
