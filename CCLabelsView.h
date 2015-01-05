/*
 *	File:		CCLabelsView.h
 *	Version:	0.1
 *	Created:	May 6, 2006
 *  Copyright 2005 Jorge Castellanos. All rights reserved.
*/

#import <Cocoa/Cocoa.h>

@interface CCLabelsView : NSView
{
	IBOutlet NSTextField *tempoTextField;
	IBOutlet NSTextField *pitchTextField;
	IBOutlet NSMatrix *songInfoTextFields;
}

-(void)setSongInfo: (unsigned)numSamples duration:(float)seconds aveTempo:(float)aTempo key:(float)key;
-(void)setSelectionTempo: (float)aTempo;
-(void)setSelectionPitch: (float)aPitch;

@end
