#include "Core/IApp.h"
#include "Core/EAppEventType.h"

#import <UIKit/UIKit.h>

@interface AppDelegate : UIApplication <UIApplicationDelegate>
{
	UIWindow *window;
	EAGLView *eaglView;
	id displayLink;
}
@end


@implementation AppDelegate

static bool running = false;

- (void)drawView:(id)sender
{
	if(running)
		return;
	running = true;
	if(Factory == NULL)
		return;
	IScheduler *scheduler = (IScheduler*)Factory->create(ClassID->SCHEDULER);
	if(scheduler == NULL)
		return;
	scheduler->step();
	running = false;
}

- (BOOL)application:(UIApplication*)application openURL:(NSURL*)url sourceApplication:(NSString*)sourceApplication annotation:(id)annotation
{
	return NO;
}

- (BOOL)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary*)launchOptions
{
	CGRect screenRect = [[UIScreen mainScreen] bounds];
	window = [[UIWindow alloc] initWithFrame:screenRect];
	window.backgroundColor = [UIColor grayColor];

	bool forceLandscapeScreenResolution = true;
	if(forceLandscapeScreenResolution)
	{
		// Change the screen's width and height to force landscape resolution for the screenRect
		screenWidth = screenRect.size.height;
		screenHeight = screenRect.size.width;

		screenRect.size.width = screenWidth;
		screenRect.size.height = screenHeight;
	}
	else
	{
		screenWidth = screenRect.size.width;
		screenHeight = screenRect.size.height;
	}

	[UIApplication sharedApplication].idleTimerDisabled = YES;

	eaglView = [[EAGLView alloc] initWithFrame:screenRect];
	eaglView.multipleTouchEnabled = YES;
	eaglView.exclusiveTouch = YES;
	CAEAGLLayer *eaglLayer = (CAEAGLLayer *)eaglView.layer;
	eaglLayer.opaque = YES;
	eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys: [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];

	inputView = eaglView;

	screenScale = [[UIScreen mainScreen] scale];
	[eaglLayer setContentsScale:screenScale];
	NSLog(@"Resolution:%f x %f @ %f!", screenRect.size.width, screenRect.size.height, screenScale);

	ViewController *viewController = [[ViewController alloc] init];
	window.rootViewController = viewController;

	window.rootViewController.view = [[UIView alloc] initWithFrame:screenRect];
	[window.rootViewController.view addSubview:eaglView];

	initMain(initLine, "");

	[window makeKeyAndVisible];

	displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(drawView:)];
	[displayLink setFrameInterval:2];
	[displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];

	// Handle notifications
	NSDictionary *remoteNotificationUserInfo = [launchOptions objectForKey:UIApplicationLaunchOptionsRemoteNotificationKey];
	if(remoteNotificationUserInfo != nil)
	{
		NSString *customData = [remoteNotificationUserInfo objectForKey:@"message"];
		const char *message = (customData != nil) ? [customData UTF8String] : nullptr;
		IApp::Get()->SendEvent(NotificationEvent(NotificationEvent::NOTIFICATION_REMOTE, message));
	}

	UILocalNotification *localNotification = [launchOptions objectForKey:UIApplicationLaunchOptionsLocalNotificationKey];
	if(localNotification != nil)
	{
		NSString *customData = [localNotification.userInfo objectForKey:@"message"];
		const char *message = (customData != nil) ? [customData UTF8String] : nullptr;
		IApp::Get()->SendEvent(NotificationEvent(NotificationEvent::NOTIFICATION_LOCAL, message));
	}

	// Register observers for keyboard events
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(KeyboardWillShow:) name:UIKeyboardWillShowNotification object:nil];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(KeyboardDidHide:) name:UIKeyboardDidHideNotification object:nil];

	return YES;
}

- (void)applicationWillResignActive:(UIApplication*)application
{
	// Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
	// Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
	IApp::Get()->SendEvent(EAppEventType::WILL_RESIGN_ACTIVE);

	// The application will be terminated if it attempts to execute OpenGL ES commands in the background (this also refers to previously submitted commands that have not yet completed)
	// We finish the rendering here to prevent this to happen
	cOpenGLESRender *render = (cOpenGLESRender*)Factory->create(ClassID->RENDER);
	render->doSwapFrameBuffer(true);
}

- (void)applicationDidBecomeActive:(UIApplication*)application
{
	// Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
	IApp::Get()->SendEvent(EAppEventType::DID_BECOME_ACTIVE);
}

- (void)applicationDidEnterBackground:(UIApplication*)application
{
	// Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
	IApp::Get()->SendEvent(EAppEventType::ENTER_BACKGROUND);
}

- (void)applicationWillEnterForeground:(UIApplication*)application
{
	// Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
	IApp::Get()->SendEvent(EAppEventType::ENTER_FOREGROUND);
}

- (void)applicationWillTerminate:(UIApplication*)application
{
	// This method may be called in situations where the application is running in the background (not suspended) and the system needs to terminate it for some reason.
	[[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)applicationDidReceiveMemoryWarning:(UIApplication*)application
{
	// Use this method to free up as much memory as possible by purging cached data objects that can be recreated (or reloaded from disk) later.
	// If the application does not release enough memory during low-memory conditions, the system may terminate it outright.
	IApp::Get()->SendEvent(EAppEventType::LOW_MEMORY);
	LOG_WARNING("---> applicationDidReceiveMemoryWarning");
}

- (void)dealloc
{
	[window release];
	[super dealloc];
}

// Notification callbacks
- (void)application:(UIApplication*)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData*)deviceToken
{
	NSLog(@"Device registered for push notifications with token: %@", deviceToken);
	NSString *deviceTokenStr = [deviceToken description];
	cIOSNotificationCenter *notificationCenter = (cIOSNotificationCenter*)Factory->create(ClassID->NOTIFICATION_CENTER);
	const char *token = [deviceTokenStr UTF8String];
	notificationCenter->setDeviceToken(token);
}

- (void)application:(UIApplication*)application didFailToRegisterForRemoteNotificationsWithError:(NSError*)error
{
	NSLog(@"Error in push notification registration: %@", [error localizedDescription]);
}

- (void)application:(UIApplication*)application didReceiveRemoteNotification:(NSDictionary*)userInfo
{
	NSString *customData = [userInfo objectForKey:@"message"];
	const char *message = (customData != nil) ? [customData UTF8String] : nullptr;
	IApp::Get()->SendEvent(NotificationEvent(NotificationEvent::NOTIFICATION_REMOTE, message));
}

- (void)application:(UIApplication*)application didReceiveLocalNotification:(UILocalNotification*)notification
{
	NSString *customData = [notification.userInfo objectForKey:@"message"];
	const char *message = (customData != nil) ? [customData UTF8String] : nullptr;
	IApp::Get()->SendEvent(NotificationEvent(NotificationEvent::NOTIFICATION_LOCAL, message));
}

-(void)KeyboardWillShow:(NSNotification*)notification
{
	NSDictionary *userInfo = [notification userInfo];
	CGSize size = [[userInfo objectForKey:UIKeyboardFrameBeginUserInfoKey] CGRectValue].size;
	if(UIInterfaceOrientationIsPortrait([UIApplication sharedApplication].statusBarOrientation))
	{
		IApp::Get()->SendEvent(KeyboardWillShowEvent(size.width, size.height));
	}
	else
	{
		IApp::Get()->SendEvent(KeyboardWillShowEvent(size.height, size.width));
	}
}

-(void)KeyboardDidHide:(NSNotification*)notification
{
	IApp::Get()->SendEvent(EAppEventType::KEYBOARD_DID_HIDE);
}

@end
