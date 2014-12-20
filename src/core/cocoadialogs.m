#import <Cocoa/Cocoa.h>

NSWindow *getMainWindow() {
	NSWindow *window = [NSApp mainWindow];
	if (window)
		return window;
	NSArray *windows = [NSApp windows];
	if ([windows count] > 0)
		return [windows objectAtIndex: 0];
	return nil;
}

@interface AlertController : NSObject {
@public
  int retcode;
}
- (void) alertDidEnd: (NSAlert*) alert returnCode: (int) returnCode contextInfo: (void*) contextInfo;
@end

@implementation AlertController
- (void) alertDidEnd: (NSAlert*) alert returnCode: (int) returnCode contextInfo: (void*) contextInfo {
  retcode = returnCode;
	[NSApp stopModal];
}
@end


int showWarning(const char *str) {
	NSWindow *sdlWindow = getMainWindow();
	AlertController *controller = [[AlertController alloc] init];
	NSAlert *alert = [[NSAlert alloc] init];
	[alert setAlertStyle: NSWarningAlertStyle];
	[alert setMessageText: [NSString stringWithCString: str]];
	[alert addButtonWithTitle: @"Cancel"];
	[alert addButtonWithTitle: @"OK"];
	[alert beginSheetModalForWindow: sdlWindow modalDelegate: controller didEndSelector: @selector(alertDidEnd:returnCode:contextInfo:) contextInfo: NULL];


	[NSApp runModalForWindow: [alert window]];
	[alert release];

  int retcode = controller->retcode != NSAlertFirstButtonReturn;

	[controller release];
	return retcode;
}

@interface SaveController : NSObject {
@public
	int choice;
}
- (void) savePanelDidEnd: (NSSavePanel*) sheet returnCode: (int) returnCode contextInfo: (void*) contextInfo;
@end

@implementation SaveController
- (void) savePanelDidEnd: (NSSavePanel*) sheet returnCode: (int) returnCode contextInfo: (void*) contextInfo {
	choice = returnCode;
	[NSApp stopModal];
}
@end

@interface OpenController : NSObject {
@public
	int choice;
}
- (void) openPanelDidEnd: (NSOpenPanel*) sheet returnCode: (int) returnCode contextInfo: (void*) contextInfo;
@end

@implementation OpenController
- (void) openPanelDidEnd: (NSOpenPanel*) sheet returnCode: (int) returnCode contextInfo: (void*) contextInfo {
	choice = returnCode;
	[NSApp stopModal];
}
@end

int doSaveDialog(char * buf, const char *title) {
	NSSavePanel* panel = [NSSavePanel savePanel];
	[panel setTitle: [NSString stringWithCString: title]];
	[panel setRequiredFileType: @"atanua"];
	[panel setExtensionHidden: false];
	[panel setPrompt: @"Save"];
	if ([panel runModal] == NSFileHandlingPanelOKButton) {
		NSString *filename = [panel filename];
		const char *cname = [[NSFileManager defaultManager] fileSystemRepresentationWithPath: filename];
		strcpy(buf, cname);
	}
	else
	{
	  return 0;
	}
	return 1;
}

int doSaveSheet(char * buf, const char *title) {
	NSWindow *sdlWindow = getMainWindow();
	NSSavePanel* panel = [NSSavePanel savePanel];
	[panel setTitle: [NSString stringWithCString: title]];
	[panel setPrompt: @"Save"];
	[panel setNameFieldLabel: @"NameFieldLabel"];
	SaveController* controller = [[[SaveController alloc] init] autorelease];
	[panel beginSheetForDirectory: nil file: nil modalForWindow: sdlWindow modalDelegate: controller didEndSelector: @selector(savePanelDidEnd:returnCode:contextInfo:) contextInfo: nil];

	[NSApp runModalForWindow: panel];
	if (controller->choice == NSFileHandlingPanelOKButton) {
		NSString *filename = [panel filename];
		const char *cname = [[NSFileManager defaultManager] fileSystemRepresentationWithPath: filename];
		strcpy(buf, cname);
	}
	else
	{
	  return 0;
	}
	return 1;
}

int doOpenSheet(char * buf) {
	NSWindow *sdlWindow = getMainWindow();
	NSOpenPanel* panel = [NSOpenPanel openPanel];
	[panel setTitle: @"Title"];
	[panel setPrompt: @"Prompt"];
	[panel setMessage: @"Message"];
	[panel setCanChooseDirectories: FALSE];
	OpenController* controller = [[[OpenController alloc] init] autorelease];
	[panel beginSheetForDirectory: nil file: nil types: nil modalForWindow: sdlWindow modalDelegate: controller didEndSelector: @selector(openPanelDidEnd:returnCode:contextInfo:) contextInfo: nil];

	[NSApp runModalForWindow: panel];
	if (controller->choice == NSOKButton) {
		NSString *filename = [panel filename];
		const char *cname = [[NSFileManager defaultManager] fileSystemRepresentationWithPath: filename];
		strcpy(buf, cname);
	}
	else
	{
	  return 0;
	}
	return 1;
}


int doOpenDialog(char *buf, const char *title) {
	NSOpenPanel* panel = [NSOpenPanel openPanel];
	[panel setTitle: [NSString stringWithCString: title]];
	[panel setPrompt: @"Open"];
	[panel setCanChooseDirectories: FALSE];

	if ([panel runModalForTypes: nil] == NSOKButton) {
		NSString *filename = [panel filename];
		const char *cname = [[NSFileManager defaultManager] fileSystemRepresentationWithPath: filename];
		strcpy(buf, cname);
	}
	else
	{
	  return 0;
	}
	return 1;
}


