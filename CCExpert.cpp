/*
 *  CCExpert.cpp
 *  CCTranscriber
 *
 *  Created by Bebelutz on 5/7/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "CCExpert.h"

CCExpertGroup::CCExpertGroup() { };						///< empty constructor

CCExpertGroup::~CCExpertGroup() { 
	// Create constant iterator for list.
	list<CCAbstractExpert *>::iterator expert;
	// Iterate through list and delete each element.
	for (expert = mExperts.begin(); expert != mExperts.end(); expert++)
	{
		delete (*expert);
	}

};

///< loop thru my list of Experts and call judge on them.
void CCExpertGroup::judge() {
	// Create constant iterator for list.
	list<CCAbstractExpert *>::iterator expert;
	// Iterate through list and output each element.
	for (expert = mExperts.begin(); expert != mExperts.end(); expert++)
	{
		(*expert)->judge();
	}

}

void CCAbstractExpert::judge() {
	// Do nothing for now.

}

CCExpert::~CCExpert() { };				///< virtual destructor


