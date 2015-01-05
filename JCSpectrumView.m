/*
 *	File:		JCGraphView.m
 *	Version:	0.1
 *	Created:	Sept 6, 2005
 *  Copyright 2005 Jorge Castellanos. All rights reserved.
 */

#import "JCSpectrumView.h"

#include <float.h>
#include <math.h>

@implementation JCSpectrumView
#define NSRectToCGRect(r) CGRectMake(r.origin.x, r.origin.y, r.size.width, r.size.height)

#define kDBAxisGap			25
#define kFreqAxisGap		17
#define kRightMargin		10
#define kTopMargin			5
#define kLogBase			2
#define kMaxIncrement (1<<20)
#define kTotalDataPoints 10000
#define kGutter 10


float maxDataValue = 2;
char text[256];

- (id)initWithFrame:(NSRect)frameRect
{
	if ((self = [super initWithFrame:frameRect]) != nil) {

		mSampleDataPtr = 0x0;
		mSampleDataBufferSize = 0;
		mSampleDataChanged = YES;
		
		mSelectionLocation.x = mSelectionLocation.y = kDBAxisGap;
	
		// Initialize frame that is used for drawing the graph content
		graphFrameRect = NSMakeRect(kDBAxisGap, kFreqAxisGap, frameRect.size.width - kDBAxisGap - kRightMargin, frameRect.size.height - kFreqAxisGap - kTopMargin);
		
		// Initialize the text attributes for the db and frequency axis
		NSMutableParagraphStyle *yLabelStyle = [[NSMutableParagraphStyle alloc] init];
		[yLabelStyle setParagraphStyle: [NSParagraphStyle defaultParagraphStyle]];
		[yLabelStyle setAlignment:NSRightTextAlignment];
		
		yAxisStringAttributes = [[NSDictionary dictionaryWithObjectsAndKeys: [NSFont systemFontOfSize: 9], NSFontAttributeName,
																			   [yLabelStyle autorelease], NSParagraphStyleAttributeName,
																			   [NSColor colorWithDeviceWhite: .3 alpha: 1], NSForegroundColorAttributeName, nil] retain];
		
		NSMutableParagraphStyle *xLabelStyle = [[NSMutableParagraphStyle alloc] init];
		[xLabelStyle setParagraphStyle: [NSParagraphStyle defaultParagraphStyle]];
		[xLabelStyle setAlignment:NSCenterTextAlignment];
		
		xAxisStringAttributes = [[NSDictionary dictionaryWithObjectsAndKeys: [NSFont systemFontOfSize: 9], NSFontAttributeName,
																				[xLabelStyle autorelease], NSParagraphStyleAttributeName,
																				[NSColor colorWithDeviceWhite: .3 alpha: 1], NSForegroundColorAttributeName, nil] retain];
		gridUnit = 100;
		minXValue = 43;
		maxXValue = 22050;
		minYValue = -96;
		maxYValue = 0;
		numVerticalGridLines = 16;
		numHorizontalGridLines = 3;
		
		mEditPoint = NSZeroPoint;
		mActiveWidth = [self locationForFrequencyValue: numVerticalGridLines] - graphFrameRect.origin.x - .5;
				
		[self setPostsFrameChangedNotifications: YES];	// send notifications when the frame changes
		
		gridColor = [[[NSColor grayColor] colorWithAlphaComponent: .20] retain];
	}
	return self;
}

//- (void)awakeFromNib {
//    [[self window] setBackgroundColor:[[NSColor windowBackgroundColor] blendedColorWithFraction: 0.5 ofColor:[NSColor blackColor]]];
//    return;
//}

-(void) dealloc {
	[yAxisStringAttributes release];
	[xAxisStringAttributes release];
	
	[backgroundCache release];
	
	[super dealloc];
}

/* Compute the pixel location on the y axis within the graph frame for the grid index argument */
- (double) locationForHorizontalGridValue: (double) idx {
	return (graphFrameRect.size.height / (maxYValue - minYValue)) * idx + graphFrameRect.origin.y;
}

/* Compute the pixel location on the x axis within the graph frame for the frequency value argument */
- (double) locationForFrequencyValue: (double) value {	
	// how many pixels are in one base power increment?
	return (graphFrameRect.size.width / numVerticalGridLines) * value + graphFrameRect.origin.x + .5;
}

/* Compute the logarithm of a number with an arbitrary base */
inline double logValueForNumber(double number, double base) {
	return log(number) / log(base);
}


/* Compute the decibel value at a specific y coordinate in the graph */
- (double) dbValueForLocation: (float) location {
	double step	= graphFrameRect.size.height / (maxYValue * 2);// number of pixels per db
	return ((location - graphFrameRect.origin.y)/ step) - maxYValue;
}

/* Compute the pixel value of a specific grid line */
- (double) valueAtGridIndex: (double) index {
	return minXValue * pow(kLogBase, index);
}

/* Compute the frequency value of a specific pixel location in the graph */
- (double) freqValueForLocation: (float) location {
	double pixelIncrement = graphFrameRect.size.width / numVerticalGridLines;
	
	return [self valueAtGridIndex:(location - graphFrameRect.origin.x - .5)/pixelIncrement];
}

/* returns a string for a specific double value (for displaying axis labels) */
- (NSString *) stringForValue:(double) value {		
	NSString * theString;
	double temp = value;
	
	if (value >= 1000)
		temp = temp / 1000;
	
	temp = (floor(temp *100))/100;	// chop everything after 2 decimal places
									// we don't want trailing 0's
	
	//if we do not have trailing zeros
	if (floor(temp) == temp)
		theString = [NSString localizedStringWithFormat: @"%.0f", temp];
	else 	// if we have only one digit
		theString = [NSString localizedStringWithFormat: @"%.1f", temp];
	
	return theString;
}

/* draws the grid lines and labels for the Y axis */
- (void)drawYAxisScale {
	NSPoint startPoint, endPoint;
	int index;
	double density = 1/0.0625;
	double value;
	
	int heightScale = (maxYValue - minYValue)/ density;
	
	[gridColor set];
	
	NSString *label = @"Amplitude";
	[[NSString localizedStringWithFormat: @"%d %@", minYValue, label] drawInRect: NSMakeRect(0, graphFrameRect.origin.y - 3.5, graphFrameRect.origin.x - 4, 11) withAttributes: yAxisStringAttributes];
	
	// figure out how many grid divisions to use for the gain axis
	for (index = 1; index <= heightScale; index ++) {
		value = index * density;
		startPoint = NSMakePoint(graphFrameRect.origin.x, floor([self locationForHorizontalGridValue: (double)index * density]) + .5);
		endPoint   = NSMakePoint(graphFrameRect.origin.x + mActiveWidth, startPoint.y);
		
		if (index > minYValue && index <= heightScale) {
			if ((minYValue + value) == 0) {
				[[[NSColor whiteColor] colorWithAlphaComponent: 0.2] set];
				[NSBezierPath strokeLineFromPoint: startPoint toPoint: endPoint];
				[gridColor set];
			} else
				[NSBezierPath strokeLineFromPoint: startPoint toPoint: endPoint];
		}
		[[NSString localizedStringWithFormat: @"%.0f %@", (minYValue + value), label] drawInRect: NSMakeRect(0, startPoint.y - 4, graphFrameRect.origin.x - 4, 11) withAttributes: yAxisStringAttributes];
	}
	
//	[[NSString localizedStringWithFormat: @"%d %@", maxYValue, label] drawInRect: NSMakeRect(0, graphFrameRect.origin.y + graphFrameRect.size.height - 4.5, graphFrameRect.origin.x - 4, 11) withAttributes: yAxisStringAttributes];
	
}

/* draws the grid lines and labels for the X axis (Logarithmic Scale) */
- (void) drawMajorGridLines {
	int index;
	double location, value;
	float labelWidth = graphFrameRect.origin.x - 2;
	int widthScale = ceil((maxXValue - minXValue)/ numVerticalGridLines);
	
	BOOL firstK = YES;	// we only want a 'K' label the first time a value is over 1000

	for (index = 0; index <= numVerticalGridLines; index++) {
		value = minXValue + index * widthScale;//valueAtGridIndex(index);
		location = [self locationForFrequencyValue: index];
		
		
		if (index > 0 && index < numVerticalGridLines) {	
			[gridColor set];
			[NSBezierPath strokeLineFromPoint: NSMakePoint(location, graphFrameRect.origin.y)
									  toPoint: NSMakePoint(location, floor(graphFrameRect.origin.y + graphFrameRect.size.height - 2) +.5)];
			
			NSString *s = [self stringForValue: value];
			if (value >= 1000 && firstK) {
				s = [s stringByAppendingString: @"K"];
				firstK = NO;
			}
			[s drawInRect: NSMakeRect(location - 3 - labelWidth/2, 0, labelWidth, 12) withAttributes: xAxisStringAttributes];
		} else if (index == 0) {	// append hertz label to first frequency
			[[[self stringForValue: value] stringByAppendingString: @"Hz"] drawInRect: NSMakeRect(location - labelWidth/2, 0, labelWidth, 12) withAttributes: xAxisStringAttributes];
		} else {	// always label the last grid marker the maximum hertz value
			[[[self stringForValue: maxXValue] stringByAppendingString: @"K"] drawInRect: NSMakeRect(location - labelWidth/2 - 12, 0, labelWidth + kRightMargin, 12) withAttributes: xAxisStringAttributes];
		}
	}
}

- (void)drawRect:(NSRect)rect
{

		
	if (!backgroundCache) {
		backgroundCache = [[NSImage alloc] initWithSize: [self frame].size];

		[backgroundCache lockFocus];
		// fill the graph area
		[[[NSColor blackColor] colorWithAlphaComponent: 0.1] set];
		[NSBezierPath fillRect:[self bounds]];

		[[NSColor blackColor] set];
		NSRectFill(NSIntersectionRect(rect, NSMakeRect(graphFrameRect.origin.x, graphFrameRect.origin.y, mActiveWidth, graphFrameRect.size.height)));
//		// draw the graph border
		[[NSColor colorWithDeviceWhite: .5 alpha: 1] set];
		NSRect lineRect = NSMakeRect(graphFrameRect.origin.x, graphFrameRect.origin.y-1, mActiveWidth, 1);
		NSRectFill(NSIntersectionRect(rect, lineRect));

		lineRect.origin.y = graphFrameRect.origin.y + graphFrameRect.size.height -1;
		NSRectFill(NSIntersectionRect(rect, lineRect));

		lineRect = NSMakeRect(graphFrameRect.origin.x + mActiveWidth, graphFrameRect.origin.y, 1, graphFrameRect.size.height);
		NSRectFill(NSIntersectionRect(rect, lineRect));
		
		[self drawYAxisScale];
		[self drawMajorGridLines];
		
		[backgroundCache unlockFocus];
	}
	
	[backgroundCache drawInRect: rect fromRect: rect operation: NSCompositeSourceOver fraction: 1.0];
	
    NSGraphicsContext *nsctx = [NSGraphicsContext currentContext];
    CGContextRef context = (CGContextRef)[nsctx graphicsPort];
    CGRect r = NSRectToCGRect(rect);

	if(mSampleDataChanged && mSampleDataPtr) {

	//    drawAsLimitedLines(context, r);
		int i, count;
		float sampleFrequency;
		int pixelWidth = (r.size.width - (kRightMargin + kDBAxisGap));
			
		// Modify the scale of the axes so that the data is visible
		CGContextSaveGState(context);
		CGContextTranslateCTM(context, kDBAxisGap, 17);

//		float yscale = (r.size.height - kFreqAxisGap - kTopMargin) / 2. / maxDataValue;
		float yscale = (r.size.height - kFreqAxisGap - kTopMargin) / 96;//maxYValue - minYValue;

		// Draw the path in red
		CGContextSetRGBStrokeColor(context,0,0,1,1);
		CGContextSetRGBFillColor(context,1,0.8,0.1,0.5);

		// Build the bulk array of points, but sub-sample the data to a limited set
		count = 0;
		sampledPoints[count].x = 0;
		sampledPoints[count].y = mSampleDataPtr[0];

		// The sampling frequency is based on the total number of points and the visible number of pixels
		sampleFrequency = (float)mSampleDataBufferSize / (pixelWidth *4);

		float result, fraction;
		unsigned temp;
//		for (i = 1; i < pixelWidth *4; i++) {
//			temp = floor(i*sampleFrequency);
//			fraction = i*sampleFrequency - temp;
//			result = (mSampleDataPtr[temp] * (1-fraction)) + mSampleDataPtr[temp + 1] * fraction; 
//			sampledPoints[++count].x = (float)i/4.;
//			sampledPoints[count].y = result * yscale;
//			sampledPoints[++count].x = (float)i/4.;
//			sampledPoints[count].y = result * yscale;
//		}

		float ref = 12;
		CGContextBeginPath(context);
		CGContextMoveToPoint(context, 0., 0.);

		for (i = 1; i < pixelWidth *4; i++) {
			temp = floor(i*sampleFrequency);
			fraction = i*sampleFrequency - temp;
			result = (mSampleDataPtr[temp] * (1-fraction)) + mSampleDataPtr[temp + 1] * fraction; 
			result = 20 * log10(result/ref);
			CGContextAddLineToPoint(context, (float)i/4., (result + 96) * yscale);
		}
		
		CGContextAddLineToPoint(context, (float)i/4., 0);
		CGContextClosePath(context);
//		CGContextStrokePath(context);
		CGContextFillPath(context);
		mSampleDataChanged = NO;

    // Use the bulk line drawing function to draw only the limited set of visible lines    
//    CGContextStrokeLineSegments(context, sampledPoints, count );
    CGContextRestoreGState(context);

	}

    CGContextFlush(context);

}

-(void)setDataPtr: (float *)dataPtr withSize: (unsigned)bufferSize
{
	mSampleDataPtr = dataPtr;
	mSampleDataBufferSize = bufferSize;
	mSampleDataChanged = YES;
	[self setNeedsDisplay:YES];
	
}

-(void)mouseDown: (NSEvent *)theEvent
{
	mSelectionLocation = [self convertPoint:[theEvent locationInWindow] fromView:nil];
	[self setNeedsDisplay:YES];
	mSampleDataChanged = YES;
}

@end
