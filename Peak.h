/*
 *  Peaks.h
 *  ceptsrtum
 *
 *  Created by matthew crossley on 6/9/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

class Peak {
public:
	Peak(int ind, float val);
	~Peak();
	
	int mPeakIndex;
	float mPeakValue;
	
	bool operator<(Peak p);
};