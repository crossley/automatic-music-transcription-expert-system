/*
 *	File:		JCGraphView.m
 *	Version:	0.1
 *	Created:	Sept 6, 2005
 *  Copyright 2005 Jorge Castellanos. All rights reserved.
 */

#import "JCGraphView.h"

@implementation JCGraphView
#define NSRectToCGRect(r) CGRectMake(r.origin.x, r.origin.y, r.size.width, r.size.height)

#define kDBAxisGap			35
#define kFreqAxisGap		17
#define kRightMargin		10
#define kTopMargin			5
#define kLogBase			2
#define kMaxIncrement (1<<20)
#define kTotalDataPoints 10000
#define kGutter 10


float sampledData[kTotalDataPoints];
CGPoint sampledPoints[kTotalDataPoints*2];
float maxDataValue = 0;
char text[256];

- (id)initWithFrame:(NSRect)frameRect
{
	if ((self = [super initWithFrame:frameRect]) != nil) {
	    int i;

		// initialize the sampled data using random values 
		sampledData[0] = 0;
		for (i = 1 ; i < kTotalDataPoints; i++) {
			sampledData[i] = sampledData[i-1] + random()%kMaxIncrement-kMaxIncrement/2;
			if (maxDataValue < abs(sampledData[i])) maxDataValue = abs(sampledData[i]);
		}
	
		// Initialize frame that is used for drawing the graph content
		graphFrameRect = NSMakeRect(kDBAxisGap, kFreqAxisGap, frameRect.size.width - kDBAxisGap - kRightMargin, frameRect.size.height - kFreqAxisGap - kTopMargin);
		
		// Initialize the text attributes for the db and frequency axis
		NSMutableParagraphStyle *yLabelStyle = [[NSMutableParagraphStyle alloc] init];
		[yLabelStyle setParagraphStyle: [NSParagraphStyle defaultParagraphStyle]];
		[yLabelStyle setAlignment:NSRightTextAlignment];
		
		yAxisStringAttributes = [[NSDictionary dictionaryWithObjectsAndKeys: [NSFont systemFontOfSize: 9], NSFontAttributeName,
																			   [yLabelStyle autorelease], NSParagraphStyleAttributeName,
																			   [NSColor colorWithDeviceWhite: .1 alpha: 1], NSForegroundColorAttributeName, nil] retain];
		
		NSMutableParagraphStyle *xLabelStyle = [[NSMutableParagraphStyle alloc] init];
		[xLabelStyle setParagraphStyle: [NSParagraphStyle defaultParagraphStyle]];
		[xLabelStyle setAlignment:NSCenterTextAlignment];
		
		xAxisStringAttributes = [[NSDictionary dictionaryWithObjectsAndKeys: [NSFont systemFontOfSize: 9], NSFontAttributeName,
																				[xLabelStyle autorelease], NSParagraphStyleAttributeName,
																				[NSColor colorWithDeviceWhite: .1 alpha: 1], NSForegroundColorAttributeName, nil] retain];
		gridUnit = 100;
		minXValue = 0;
		maxXValue = 500;
		minYValue = -1;
		maxYValue = 1;
		numVerticalGridLines = 11;
		numHorizontalGridLines = 6;
		
		mEditPoint = NSZeroPoint;
		mActiveWidth = [self locationForFrequencyValue: numVerticalGridLines] - graphFrameRect.origin.x - .5;
				
		[self setPostsFrameChangedNotifications: YES];	// send notifications when the frame changes
		
		gridColor = [[[NSColor greenColor] colorWithAlphaComponent: .20] retain];
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
//	double pixelIncrement = graphFrameRect.size.width / numVerticalGridLines;
//	double location = logValueForNumber(value/minXValue, kLogBase) * pixelIncrement;
//	return floor(location + graphFrameRect.origin.x) + .5;
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
	double density = 1/1;
	double value;
	
	int heightScale = (maxYValue - minYValue)/ density;
	
	[gridColor set];
	
	NSString *label = @"db";
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
		
		[self drawYAxisScale];
		[self drawMajorGridLines];
		
		[backgroundCache unlockFocus];
	}
	
	[backgroundCache drawInRect: rect fromRect: rect operation: NSCompositeSourceOver fraction: 1.0];
	
    NSGraphicsContext *nsctx = [NSGraphicsContext currentContext];
    CGContextRef context = (CGContextRef)[nsctx graphicsPort];
    CGRect r = NSRectToCGRect(rect);
    drawAsLimitedLines(context, r);
	
    CGContextFlush(context);

}

void drawAsLimitedLines(CGContextRef context, CGRect rect)
{
    int i, count;
    float sampleFrequency;
    int pixelWidth = (rect.size.width - 2 * kGutter);
    	
    // Modify the scale of the axes so that the data is visible
    CGContextSaveGState(context);
    CGContextTranslateCTM(context,kGutter, (int)(rect.size.height / 2));

    float yscale = (rect.size.height - 2 *kGutter) / 2. / maxDataValue;

    // Draw the path in red
    CGContextSetRGBStrokeColor(context,1,0,0,1);

    // Build the bulk array of points, but sub-sample the data to a limited set
    count = 0;
    sampledPoints[count].x = 0;
    sampledPoints[count].y = sampledData[0];

    // The sampling frequency is based on the total number of points and the visible number of pixels
    sampleFrequency = (float)kTotalDataPoints / pixelWidth;

    for (i = 1; i < pixelWidth; i++) {
		sampledPoints[++count].x = i;
		sampledPoints[count].y = sampledData[(int)(i*sampleFrequency)] * yscale;
		sampledPoints[++count].x = i;
		sampledPoints[count].y = sampledData[(int)(i*sampleFrequency)] * yscale;
    }

    
    ////////////////////////////////////////////////////////////////////////////////////////
    // Use the bulk line drawing function to draw only the limited set of visible lines
    ////////////////////////////////////////////////////////////////////////////////////////
    
    CGContextStrokeLineSegments(context, sampledPoints, count );
    
    
    CGContextRestoreGState(context);

}

@end
