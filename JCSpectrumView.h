/*
 *	File:		JCSpectrumView.h
 *	Version:	0.1
 *	Created:	Sept 6, 2005
 *  Copyright 2005 Jorge Castellanos. All rights reserved.
*/

#import <Cocoa/Cocoa.h>

@interface JCSpectrumView : NSView
{	
	NSRect graphFrameRect;		// This is the frame of the drawing area of the view
	float mActiveWidth;		// The usable portion of the graph
	NSPoint mEditPoint;			// This is the current location in the drawing area that is active
	int minXValue, maxXValue, numVerticalGridLines;
	int minYValue, maxYValue, numHorizontalGridLines;
	float gridUnit;
	float zeroPoint;

	BOOL mSampleDataChanged;
	float *mSampleDataPtr;
	unsigned mSampleDataBufferSize;
	CGPoint sampledPoints[2560 * 4]; // That 4 is temp just to over sample, so it looks more clutter.

	NSPoint mSelectionLocation;
	
	NSColor *gaphColor;
	NSColor *backgroundColor;	
	NSColor *gridColor;
	
	NSImage *backgroundCache;	// An image cache of the background so that we don't have to re-draw the grid lines and labels all the time

	NSDictionary *xAxisStringAttributes;		// Text attributes used to draw the strings on the db axis
	NSDictionary *yAxisStringAttributes;	// Text attributes used to draw the strings on the frequency axis
}


-(double) locationForFrequencyValue: (double) value;	// converts a frequency value to the pixel coordinate in the graph
-(double) locationForHorizontalGridValue: (double) value;			// converts a db value to the pixel coordinate in the graph
-(void)setDataPtr: (float *)dataPtr withSize: (unsigned)bufferSize;

@end
