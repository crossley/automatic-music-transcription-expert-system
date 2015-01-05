/*
 *  CCTranscriptionEngine.h
 *  CCTranscriber
 *
 *  Created by Bebelutz on 5/7/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef CC_TRANSCRIPTION_ENGINE_H			// This is in case you include this twice
#define CC_TRANSCRIPTION_ENGINE_H

#include "CCExpert.h"

class CCTranscriptionEngine : public CCExpertGroup {
public:
	CCTranscriptionEngine(unsigned numSamples = 0, float *bufferOfSamples = NULL);	///< constructor
	virtual ~CCTranscriptionEngine();				///< virtual destructor
	
	float tempoAtBlock(unsigned blockNumber);
	float PitchAtBlock(unsigned blockNumber);
	char chordAtBlock(unsigned blockNumber);
	
	/// Clients call judge() to get the features of a buffer.
	virtual void judge();	///< this will be implemented in subclasses.
	
	float tempoVerdict();
	float pitchVerdict();
	char chordVerdict();
	
	/// To be used as a real-time way of running the engine.
	void CCTranscriptionEngine::judge(float *sampleBuffer);
	
protected:
	float *mSampleBufferPtr;
	unsigned mNumSamples;
	float *mTempoHistory;
	float *mPitchHistory;
	char *mChordHistory;
	
	float mTempoVerdict;
	float mPitchVerdict;	// add confidences later...
	char mChordVerdict;
	
};



#endif
