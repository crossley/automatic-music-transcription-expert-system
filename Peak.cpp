/*
 *  Peaks.cpp
 *  ceptsrtum
 *
 *  Created by matthew crossley on 6/9/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "Peak.h"

Peak::Peak(int ind, float val)
{
	mPeakIndex = ind;
	mPeakValue = val;
}
Peak::~Peak(){}
bool Peak::operator<(Peak p)
{
	if(mPeakValue < p.mPeakValue) 
	{
		return true;
	} else 
	{
		return false;
	}
}