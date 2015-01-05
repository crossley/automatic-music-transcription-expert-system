/* controllerAndBridge */

#import <Cocoa/Cocoa.h>

#include "sndfile.h"

#include "CCTranscriptionEngine.h"
#import "JCWaveformView.h"
#import "JCSpectrumView.h"

#include "CCFFTer.h"

@interface CControllerAndBridge : NSObject
{
	CCTranscriptionEngine *musicTranscriber;
	NSString *mFilePath;
	SNDFILE *inFile;
	SF_INFO soundFileInfo;
	IBOutlet NSWindow *myWindow;
	IBOutlet NSView *labelsView;
	NSProgressIndicator *progressBar;
	NSTextField *statusTextField;
	JCWaveformView *plotView;
	JCWaveformView *waveZoomView;	
	JCWaveformView *statsView;
	JCSpectrumView *spectrumView;
	float *songBufferPtr;
	float *spectralBufferPtr;
	
	CCFFTer *mFFTAnalyzer;
	
	BOOL shouldRunProgressBar;
}

- (void)cursorPositionChanged:(NSNotification *)notification;
- (void)runProgressBar:(id)sender;

@end
