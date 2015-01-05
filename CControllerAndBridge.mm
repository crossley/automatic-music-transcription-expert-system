#import "CControllerAndBridge.h"

#include "CCore.h"
#import "JCFullScreenWindow.h"

#include <iostream>

@implementation CControllerAndBridge

- (id)init
{
	self = [super init];
	if(self != nil) {
		// Register as an observer to changes in the array of actions.
		NSNotificationCenter *notificationCenter;
		notificationCenter = [NSNotificationCenter defaultCenter];
		[notificationCenter addObserver:self selector:@selector(cursorPositionChanged:) name:@"WaveformViewCursorPositionChanged" object:nil];

		mFFTAnalyzer = new CCFFTer;
		spectralBufferPtr = (float *)calloc(512, sizeof(float));

	}
	return self;
}

// Clean-up
- (void)dealloc
{
//	[[NSNotificationCenter defaultCenter] removeObserver:self];
	[super dealloc];
}

- (void)awakeFromNib
{
	[myWindow center];
}


- (BOOL) application:(NSApplication*)theApplication openFile:(NSString*)filename
{
	//Let's remember the file for later
	mFilePath = [filename retain];
	return YES;
}


- (void) applicationDidFinishLaunching:(NSNotification*)aNotification 
{
	
	NSOpenPanel *openPanel;
	//If no composition file was dropped on the application's icon, we need to ask the user for one
	if(mFilePath == nil) {
		openPanel = [NSOpenPanel openPanel];
		[openPanel setAllowsMultipleSelection:NO];
		[openPanel setCanChooseDirectories:NO];
		[openPanel setCanChooseFiles:YES];
		if([openPanel runModalForDirectory:nil file:nil types:[NSArray arrayWithObject:@"aif"]] != NSOKButton) {
			NSLog(@"No file specified");
			[NSApp terminate:nil];
		}
		mFilePath = [[[openPanel filenames] objectAtIndex:0] retain];
	}

	// open file
	inFile = sf_open([mFilePath cStringUsingEncoding: [NSString defaultCStringEncoding]], SFM_READ, &soundFileInfo);
	if (soundFileInfo.channels != 1) {
		NSLog(@"Error: I can only process mono files\n");
		[NSApp terminate:nil];
	}
		
	float windowWidth = [myWindow frame].size.width - 40; // find window width, minos a margin of 20 per side.

	statusTextField = [[NSTextField alloc] initWithFrame:NSMakeRect(20, 60, windowWidth, 24)];
	[statusTextField setStringValue:@"Analyzing song..."];
	[statusTextField setBordered:NO];
	[[statusTextField cell] setDrawsBackground:NO];
	[statusTextField setTextColor: [NSColor whiteColor]];
	[[myWindow contentView] addSubview: statusTextField];

	// Show a progress bar, so user knows the computer is working.
	progressBar = [[NSProgressIndicator alloc] initWithFrame:NSMakeRect(20, 20, windowWidth, 16)]; // create a progress indicator
//	[progressBar setUsesThreadedAnimation:YES];
	[progressBar setStyle:NSProgressIndicatorBarStyle];
	[progressBar sizeToFit];
	[progressBar startAnimation:self];
	[[myWindow contentView] addSubview: progressBar];

	// Allocate memory for the complete file (hopeing it's not too big)
	songBufferPtr = (float *)calloc(soundFileInfo.frames, sizeof(float));

	[NSThread detachNewThreadSelector:@selector(runProgressBar:) toTarget:self withObject: self];
	
	

	
}

- (void) applicationWillTerminate:(NSNotification*)aNotification 
{

	sf_close(inFile);

}

- (void)doneLoco
{

	// Clean up!
	[statusTextField removeFromSuperview]; // This was just a temp control. Get rid of it.
	[progressBar removeFromSuperview];

	[myWindow toggleFullScreen:self];

	NSRect windowFrame = [myWindow frame];
	// -10 'cause 10 for left margin, and 10 for right margin.
	NSRect plotViewFrame = NSMakeRect(10, 10, windowFrame.size.width - 20, (windowFrame.size.height - 10)/4);
	plotView = [[JCWaveformView alloc] initWithFrame: plotViewFrame];
	[plotView setDataPtr: songBufferPtr withSize: soundFileInfo.frames];
	[[myWindow contentView] addSubview: plotView];

	NSRect waveZoomViewFrame = NSMakeRect(10 + (windowFrame.size.width/2), 10 + (((windowFrame.size.height - 10)/4) * 3), (windowFrame.size.width/2) - 20, (windowFrame.size.height - 10)/8);
	waveZoomView = [[JCWaveformView alloc] initWithFrame: waveZoomViewFrame];
	[waveZoomView setDataPtr: songBufferPtr withSize: soundFileInfo.frames];
	[[myWindow contentView] addSubview: waveZoomView];

	NSRect statsViewFrame = NSMakeRect(10, 10 + (((windowFrame.size.height - 10)/8) * 7), windowFrame.size.width - 20, (windowFrame.size.height - 10)/8);
	statsView = [[JCWaveformView alloc] initWithFrame: statsViewFrame];
	//[statsView setDataPtr: songBufferPtr withSize: soundFileInfo.frames];
	[[myWindow contentView] addSubview: statsView];

	NSRect spectrumViewFrame = NSMakeRect(10 + (windowFrame.size.width/2), 10 + ((windowFrame.size.height - 10)/4), (windowFrame.size.width/2) - 20, (windowFrame.size.height - 10)/2);
	spectrumView = [[JCSpectrumView alloc] initWithFrame: spectrumViewFrame];
	//[spectrumView setDataPtr: songBufferPtr withSize: soundFileInfo.frames];
	[[myWindow contentView] addSubview: spectrumView];

	float averageTempo = musicTranscriber->verdict();	
	NSView *labelDisplay = [labelsView self];
	[labelDisplay setFrameOrigin: NSMakePoint(10, (windowFrame.size.height - 10)/4)];
	[labelsView setSongInfo: (unsigned)(soundFileInfo.frames) duration:(float)((float)soundFileInfo.frames/(float)soundFileInfo.samplerate) aveTempo:averageTempo key:0.0];

	[[myWindow contentView] addSubview: labelDisplay];
	
}

- (void)runProgressBar:(id)sender
{

	sf_read_float(inFile, songBufferPtr, soundFileInfo.frames);
	
	musicTranscriber = new CCTranscriptionEngine(soundFileInfo.frames, songBufferPtr);	
	musicTranscriber->judge();	
	
	[self performSelectorOnMainThread: @selector(doneLoco) withObject:nil waitUntilDone: NO];

#ifdef DEBUG_MESSAGES
	NSLog(@"Finished song analysis.\n", windowWidth);
#endif	
	// close thread and return.
	[NSThread exit];
}

- (void)cursorPositionChanged:(NSNotification *)notification
{
	unsigned position = [plotView cursorPositionInBuffer];
	float currentTempo = musicTranscriber->tempoAtBlock(position/512);
	float currentPitch = musicTranscriber->PitchAtBlock(position/512);
	[labelsView setSelectionTempo:currentTempo];
	[labelsView setSelectionPitch:currentPitch];

//	THIS CRASHES WITH A BAD ACCESS ERROR. THE PROBLEM BUFFER IS THE spectralBufferPtr, CAN'T FIND WHY.
//	mFFTAnalyzer->calculateFFT(&(songBufferPtr[position]), spectralBufferPtr);
//	WORKAROUND FOR THE ISSUE ABOVE:
	float testOut[512] = {0};
	memcpy(testOut, spectralBufferPtr, 512 * sizeof(float)); // Copy the data
	mFFTAnalyzer->calculateFFT(&(songBufferPtr[position]), testOut);
	memcpy(spectralBufferPtr, testOut, 512 * sizeof(float)); // Copy the data


	[waveZoomView setDataPtr: &(songBufferPtr[position]) withSize: 2048];
	[spectrumView setDataPtr: spectralBufferPtr withSize: 512];

}


@end
