#import "JCFullScreenWindow.h"

@implementation JCFullScreenWindow

// Initializer for a custom window.
- (id)initWithContentRect:(NSRect)contentRect styleMask:(unsigned int)aStyle backing:(NSBackingStoreType)bufferingType defer:(BOOL)flag 
{

    //Call NSWindow's version of this function, but pass in the NSBorderlessWindowMask so that the window doesn't have a title bar
    NSWindow* window = [super initWithContentRect:contentRect styleMask:NSBorderlessWindowMask backing:NSBackingStoreBuffered defer:NO];

//   [window setLevel: NSScreenSaverWindowLevel];
	
    [window setBackgroundColor:[NSColor blackColor]];

	[window setAlphaValue:1.0];
	
    // make sure the window has no shadow
    [window setHasShadow:NO];
	
	[NSMenu setMenuBarVisible:NO];
	
    // Resize the window to full screen   
//    NSRect screenFrame = [[NSScreen mainScreen] frame];	
//    NSRect screenFrame = [NSWindow frameRectForContentRect:[[NSScreen mainScreen] frame] styleMask:NSTitledWindowMask];
//    [mainWindow setFrame:screenFrame display:YES animate:YES];
	
    return window;
}

// Custom windows that use the NSBorderlessWindowMask can't become key by default.  Therefore, controls in such windows
// won't ever be enabled by default.  Thus, we override this method to change that.
- (BOOL) canBecomeKeyWindow
{
    return YES;
}

- (IBAction)toggleFullScreen:(id)sender
{

    // Resize the window to full screen   
    NSRect screenFrame = [[NSScreen mainScreen] frame];	
    [self setFrame:screenFrame display:YES animate:YES];

//	NSButton *aqua = [[NSButton alloc] initWithFrame:NSMakeRect(100,100,100,23)];
//   
//   [[aqua cell] setControlSize:NSRegularControlSize];
//   [aqua setButtonType:NSMomentaryPushInButton];
//   [aqua setBordered:YES];
//   [aqua setBezelStyle:NSRoundedBezelStyle];
//   [aqua setImagePosition:NSNoImage];
//   [[aqua cell] setControlTint:NSBlueControlTint];
//   [aqua setEnabled:YES];
//
//	[[self contentView] addSubview: aqua];

}

//- (void)flagsChanged:(NSEvent *)modifier
//{
//	if ([modifier modifierFlags] & NSShiftKeyMask) {
//        NSLog(@"shift key has been pressed");
//    }
//	if ([modifier modifierFlags] & NSCommandKeyMask) {
//        NSLog(@"command key has been pressed");
//    }
//	
//}

//- (void)keyDown:(NSEvent *)modifier
//{
//	if ([modifier modifierFlags] & NSShiftKeyMask) {
//        NSLog(@"shift key has been pressed");
//    }
//	if ([modifier modifierFlags] & NSCommandKeyMask) {
//        NSLog(@"command key has been pressed");
//    }
//
//}


@end
