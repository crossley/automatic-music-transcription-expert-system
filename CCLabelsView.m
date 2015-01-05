/*
 *	File:		CCLabelsView.m
 *	Version:	0.1
 *	Created:	May 6, 2006
 *  Copyright 2005 Jorge Castellanos. All rights reserved.
 */

#import "CCLabelsView.h"

@implementation CCLabelsView


-(void)setSongInfo: (unsigned)numSamples duration:(float)seconds aveTempo:(float)aTempo key:(float)key
{
	[[songInfoTextFields cellWithTag: 0] setStringValue:[NSString localizedStringWithFormat: @"%d", numSamples]];
	[[songInfoTextFields cellWithTag: 1] setStringValue:[NSString localizedStringWithFormat: @"%3.1f s", seconds]];
	[[songInfoTextFields cellWithTag: 2] setStringValue:[NSString localizedStringWithFormat: @"%3.0f bpm", aTempo]];
	[[songInfoTextFields cellWithTag: 3] setStringValue:[NSString localizedStringWithFormat: @" ?"]];

}

-(void)setSelectionTempo: (float)aTempo
{
	[tempoTextField setStringValue:[NSString localizedStringWithFormat: @"%3.0f bpm", aTempo]];
}

-(void)setSelectionPitch: (float)aPitch
{
	[pitchTextField setStringValue:[NSString localizedStringWithFormat: @"%3.0f Hz", aPitch]];
}

@end
