//
//  iOSAppAppDelegate.m
//  iOSApp
//
//  Created by Wang ZhiPeng on 11-5-24.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#import "iOSAppAppDelegate.h"
#import "EAGLView.h"
#import "iOSApp/z11AppViewController.h"
#import "App/AppMain.h"
#import "sys/utsname.h"

@implementation iOSAppAppDelegate

@synthesize window;
@synthesize viewController;
@synthesize gui_container;

using namespace z11;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
    {
        self.window.rootViewController = [[z11AppViewController alloc] initWithNibName:@"iOSAppViewController_iPad" bundle:[NSBundle mainBundle]];
    } else {
        self.window.rootViewController = [[z11AppViewController alloc] initWithNibName:@"iOSAppViewController_iPhone" bundle:[NSBundle mainBundle]];
    }
    
    self.viewController = (z11AppViewController*)self.window.rootViewController;
    
    {
        CGSize screen_size = [[UIScreen mainScreen] currentMode].size;
        EAGLView* view = (EAGLView*)self.window.rootViewController.view;
        CGRect screen_frame = view.frame;
        if (viewController.hidden_status_bar == NO && viewController.blend_status_bar == NO && (viewController.system_version >= 6.0 && viewController.system_version < 7.0)) {
            if (UIInterfaceOrientationIsLandscape([UIApplication sharedApplication].statusBarOrientation) && viewController.system_version < 8.0)
            {
                screen_frame.size.height += [[UIApplication sharedApplication] statusBarFrame].size.width;
                screen_frame.origin.y -= [[UIApplication sharedApplication] statusBarFrame].size.width * 2;
            } else {
                screen_frame.size.height += [[UIApplication sharedApplication] statusBarFrame].size.height;
                screen_frame.origin.y -= [[UIApplication sharedApplication] statusBarFrame].size.height * 2;
            }
        }
        UIImage* image = nil;
        // XCode6以后，只有iPad启动界面有Portrait和Landscape之分，iPhone全部统一为Portrait启动
        if (CGSizeEqualToSize(CGSizeMake(640, 1136), screen_size))
        {
            if (screen_frame.size.width > screen_frame.size.height) {
                CGFloat temp = screen_frame.size.width;
                screen_frame.size.width = screen_frame.size.height;
                screen_frame.size.height = temp;
            }
            image = [UIImage imageNamed: @"LaunchImage-568h.png"];      // iPhone5 iPhone5S iPhone5C
            if(image==nil){
                image = [UIImage imageNamed: @"LaunchImage-700-568h.png"];
            }
        } else if (CGSizeEqualToSize(CGSizeMake(750, 1334), screen_size))
        {
            if (screen_frame.size.width > screen_frame.size.height) {
                CGFloat temp = screen_frame.size.width;
                screen_frame.size.width = screen_frame.size.height;
                screen_frame.size.height = temp;
            }
            image = [UIImage imageNamed: @"LaunchImage-800-667h.png"];  // iPhone6
        }else if (CGSizeEqualToSize(CGSizeMake(1242, 2208), screen_size) || CGSizeEqualToSize(CGSizeMake(1125, 2001), screen_size) || CGSizeEqualToSize(CGSizeMake(2001, 1125), screen_size))
        {
            if (screen_frame.size.width > screen_frame.size.height) {
                CGFloat temp = screen_frame.size.width;
                screen_frame.size.width = screen_frame.size.height;
                screen_frame.size.height = temp;
            }
            image = [UIImage imageNamed: @"LaunchImage-800-Portrait-736h.png"]; // iPhone6Plus
            if(image==nil){
                image = [UIImage imageNamed: @"LaunchImage-800-667h.png"];
            }
        }else if (CGSizeEqualToSize(CGSizeMake(640, 960), screen_size) || CGSizeEqualToSize(CGSizeMake(320, 480), screen_size))
        {
            if (screen_frame.size.width > screen_frame.size.height) {
                CGFloat temp = screen_frame.size.width;
                screen_frame.size.width = screen_frame.size.height;
                screen_frame.size.height = temp;
            }
            image = [UIImage imageNamed: @"LaunchImage.png"];           // iPhone4 iPhone4S iPhone3GS
            if(image==nil){
                image = [UIImage imageNamed: @"LaunchImage-700.png"];
            }
        } else if (CGSizeEqualToSize(CGSizeMake(768, 1024), screen_frame.size) || CGSizeEqualToSize(CGSizeMake(1536, 2048), screen_frame.size))
        {
            image = [UIImage imageNamed: @"LaunchImage-Portrait.png"];           // iPad Portrait
        } else if (CGSizeEqualToSize(CGSizeMake(1024, 768), screen_frame.size) || CGSizeEqualToSize(CGSizeMake(2048, 1536), screen_frame.size))
        {
            image = [UIImage imageNamed: @"LaunchImage-Landscape.png"];           // iPad Landscape
        }
        if (image != nil) {
            view.loader_view = [[UIImageView alloc] initWithFrame:screen_frame];
            
            if (screen_size.width == 640 || screen_size.width == 320 || screen_size.width == 750 || screen_size.width == 1242) {
                UIInterfaceOrientation orientation = [UIApplication sharedApplication].statusBarOrientation;
                switch (orientation) {
                    case UIInterfaceOrientationPortrait:
                    {
                        [view.loader_view setTransform:CGAffineTransformMake(1, 0, 0, 1, 0, 0)];
                        break;
                    }
                    case UIInterfaceOrientationPortraitUpsideDown:
                    {
                        [view.loader_view setTransform:CGAffineTransformMake(-1, 0, 0, -1, 0, 0)];
                        break;
                    }
                    case UIInterfaceOrientationLandscapeLeft:
                    {
                        float dx = (screen_frame.size.height - screen_frame.size.width) / 2.0;
                        [view.loader_view setTransform:CGAffineTransformMake(0, 1, -1, 0, dx, -dx)];
                        break;
                    }
                    case UIInterfaceOrientationLandscapeRight:
                    {
                        float dx = (screen_frame.size.height - screen_frame.size.width) / 2.0;
                        [view.loader_view setTransform:CGAffineTransformMake(0, -1, 1, 0, dx, -dx)];
                        break;
                    }
                    default:
                        break;
                }
            }
            
            view.loader_view.contentMode = UIViewContentModeScaleAspectFill;
            //[loader_view setBackgroundColor: [UIColor colorWithPatternImage:image]];
            [view.loader_view setImage:image];
            [view addSubview:view.loader_view];
        }
    }
    
    self.viewController.launchOptions = launchOptions;
    
    
    // Override point for customization after application launch.
    [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
    [self performSelectorOnMainThread:@selector(checkLaunchOrientation:) withObject:launchOptions waitUntilDone:NO];
    // Be sure call this function before applicationDidBecomeActive //
    [self applicationWillEnterForeground:application];
    
    
    
    return YES;
}

- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo
{
    if(viewController.notification_valid){
        [viewController didReceiveRemoteNotification:userInfo];
    }
}

- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo fetchCompletionHandler:(void (^)(UIBackgroundFetchResult))completionHandler
{
    if(viewController.notification_valid){
        [viewController didReceiveRemoteNotification:userInfo fetchCompletionHandler:completionHandler];
    }
}


- (void)application:(UIApplication*)application
didRegisterForRemoteNotificationsWithDeviceToken:(NSData*)deviceToken
{
    NSLog(@"设备令牌: %@", deviceToken);
    [viewController didGetRemoteNotificationsWithDeviceToken:deviceToken];
}
- (void)application:(UIApplication*)application
didFailToRegisterForRemoteNotificationsWithError:(NSError*)error
{
    NSLog(@"获得令牌失败: %@", error);
}

- (void)application:(UIApplication *)application didRegisterUserNotificationSettings:(UIUserNotificationSettings *)notificationSettings
{
    [viewController didRegisterUserNotificationSettings:notificationSettings];
}

- (void)application:(UIApplication *)application performFetchWithCompletionHandler:(void (^)(UIBackgroundFetchResult result))completionHandler
{
    [viewController performFetchWithCompletionHandler:completionHandler];
}

- (BOOL)application:(UIApplication *)application handleOpenURL:(NSURL *)url
{
    return [self.viewController handleOpenURL:url];
}
- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation
{
    return [self.viewController openURL:url];
}

- (BOOL)application:(UIApplication *)app openURL:(NSURL *)url options:(NSDictionary<NSString*, id> *)options
{
    return [self.viewController openURL:url];
}
- (void)application:(UIApplication *)application performActionForShortcutItem:(UIApplicationShortcutItem *)shortcutItem completionHandler:(void (^)(BOOL))completionHandler
{
    [self.viewController performActionForShortcutItem:shortcutItem completionHandler:completionHandler];
}
-(void)checkLaunchOrientation:(id)sender{
    // Runtime Enviroments initialise.
    CGSize screen_size = [[UIScreen mainScreen] currentMode].size;
    if(CGSizeEqualToSize(CGSizeMake(640, 960), screen_size) || 
       CGSizeEqualToSize(CGSizeMake(1536, 2048), screen_size) || CGSizeEqualToSize(CGSizeMake(2048, 1536), screen_size) || CGSizeEqualToSize(CGSizeMake(640, 1136), screen_size) || CGSizeEqualToSize(CGSizeMake(750, 1334), screen_size) ||
       CGSizeEqualToSize(CGSizeMake(1334, 750), screen_size))
    {
        [z11AppViewController setRetina:1];
    } else if (CGSizeEqualToSize(CGSizeMake(1242, 2208), screen_size) || CGSizeEqualToSize(CGSizeMake(2208, 1242), screen_size) || CGSizeEqualToSize(CGSizeMake(1125, 2001), screen_size) || CGSizeEqualToSize(CGSizeMake(2001, 1125), screen_size)) {
        [z11AppViewController setRetina:2];
    }
    else {
        [z11AppViewController setRetina:0];
    }
    if(screen_size.width > screen_size.height)
    {
        CGFloat tmp = screen_size.width;
        screen_size.width = screen_size.height;
        screen_size.height = tmp;
    }
    
    UIInterfaceOrientation orientation = [UIApplication sharedApplication].statusBarOrientation;    
    BOOL isLandscape = UIInterfaceOrientationIsLandscape(self.viewController.interfaceOrientation);
    
    //
    if (UIInterfaceOrientationIsLandscape(orientation) || isLandscape) {
        //do stuff here
        MGDevice::setScreenWidth(screen_size.height);
        MGDevice::setScreenHeight(screen_size.width);
    }else {
        MGDevice::setScreenWidth(screen_size.width);
        MGDevice::setScreenHeight(screen_size.height);
    }
    
    // Main Bundle
    NSString* main_path = [[NSBundle mainBundle] bundlePath];
    MGBundle::main_bundle_path = [main_path cStringUsingEncoding:NSUTF8StringEncoding];
    
    // Data Bundle
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* data_path = [paths objectAtIndex:0];
    MGBundle::iOS_data_bundle_path = [data_path cStringUsingEncoding:NSUTF8StringEncoding];
    MGBundle::iOS_data_bundle_path = MGBundle::iOS_data_bundle_path.substr(0, MGBundle::iOS_data_bundle_path.find_last_of('/'));
    
    NSString* toPath = [ data_path stringByAppendingString:@"/../Library/Caches/documents/" ];
    BOOL isDir = NO;
    NSFileManager *fileManager = [NSFileManager defaultManager];
    BOOL existed = [fileManager fileExistsAtPath:toPath isDirectory:&isDir];
    if ( !(isDir == YES && existed == YES) )
    {
        [fileManager createDirectoryAtPath:toPath withIntermediateDirectories:YES attributes:nil error:nil];
    }
    toPath = [ data_path stringByAppendingString:@"/../Library/Caches/Resources/" ];
    existed = [fileManager fileExistsAtPath:toPath isDirectory:&isDir];
    if ( !(isDir == YES && existed == YES) )
    {
        [fileManager createDirectoryAtPath:toPath withIntermediateDirectories:YES attributes:nil error:nil];
    }
    
    //创建控件的临时文件夹 //
    toPath = [ data_path stringByAppendingString:@"/../tmp/controls" ];
    existed = [fileManager fileExistsAtPath:toPath isDirectory:&isDir];
    if ( !(isDir == YES && existed == YES) )
    {
        [fileManager createDirectoryAtPath:toPath withIntermediateDirectories:YES attributes:nil error:nil];
    }
    
    //Device Setting
    //ID
    NSString* deviceSetting = nil;
    if (self.viewController.system_version >= 6.0)
        deviceSetting = [[[UIDevice currentDevice] identifierForVendor] UUIDString];
//    else
//        deviceSetting = [[UIDevice currentDevice] uniqueIdentifier];
    
    string device_setting;
    
    if (deviceSetting != nil) {
        deviceSetting = [deviceSetting stringByReplacingOccurrencesOfString:@" " withString:@""];
        deviceSetting = [deviceSetting stringByReplacingOccurrencesOfString:@"-" withString:@""];
        //NSLog(@"device id: %@",deviceSetting); // 输出设备id
        device_setting = [deviceSetting cStringUsingEncoding:NSUTF8StringEncoding];
        MGDevice::setDeviceID(device_setting);
    }
    
    //Type
    {
        struct utsname systemInfo;
        uname(&systemInfo);
        deviceSetting = [NSString stringWithCString:systemInfo.machine encoding:NSUTF8StringEncoding];
        deviceSetting = [deviceSetting stringByReplacingOccurrencesOfString:@"iPhone" withString:@""];
        deviceSetting = [deviceSetting stringByReplacingOccurrencesOfString:@"iPad" withString:@""];
        deviceSetting = [deviceSetting stringByReplacingOccurrencesOfString:@"iPod touch" withString:@""];
    }
    deviceSetting = [[[UIDevice currentDevice] model] stringByAppendingString:deviceSetting];
    NSLog(@"device type: %@",deviceSetting); // 输出设备type
    device_setting = [deviceSetting cStringUsingEncoding:NSUTF8StringEncoding];
    MGDevice::setDeviceType(device_setting);
    if (device_setting.find("iPhone") != string::npos)
    {
        if ([z11AppViewController isRetina] > 0) {
            MGDevice::setScreenPPI(326);
        } else {
            MGDevice::setScreenPPI(163);
        }
    } else if (device_setting.find("iPad") != string::npos)
    {
        if ([z11AppViewController isRetina] > 0) {
            MGDevice::setScreenPPI(264);
        } else {
            MGDevice::setScreenPPI(132);
        }
    } else if (device_setting.find("iPhone7,1") != string::npos)
    {
        if ([z11AppViewController isRetina] > 0) {
            MGDevice::setScreenPPI(308);
        } else {
            MGDevice::setScreenPPI(154);
        }
    }
    MGDevice::setDeviceOSType("iOS");
    //MGDevice::setDeviceType("Android");
    //OS Version
    deviceSetting = [[UIDevice currentDevice] systemVersion];
    deviceSetting = [deviceSetting stringByReplacingOccurrencesOfString:@" " withString:@""];
    deviceSetting = [deviceSetting stringByReplacingOccurrencesOfString:@"-" withString:@""];
    NSLog(@"os version: %@",deviceSetting); // 输出设备os version
    device_setting = [deviceSetting cStringUsingEncoding:NSUTF8StringEncoding];
    MGDevice::setDeviceOSVersion(device_setting);
    
    //framework initialise.
    WndHandle wnd = self.viewController;
    gui_container = new AppMain();
    self.viewController.gui_container = gui_container;
    gui_container->initialise(wnd);
    
    
    if(viewController.notification_valid){
        [viewController registRemoteNotification];
        [viewController didLaunchWithOptions:sender];
    }
    
    gui_container->runLoop();
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    /*
     Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
     Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
     */
    CustomMPMoviePlayerViewController* movie_player = self.viewController.movie_player;
    if (movie_player != nil)
    {
        if (movie_player.moviePlayer.playbackState != MPMoviePlaybackStateStopped)
        {
            self.viewController.movie_need_continue = YES;
            self.viewController.movie_progress = self.viewController.movie_player.moviePlayer.currentPlaybackTime;
            self.viewController.need_continue_movie_url = self.viewController.movie_player.movie_url;
            [movie_player.moviePlayer pause];
        }
    }
    
    if(gui_container!=NULL){
        gui_container->pauseLoop();
    }
    if (gui_container != NULL && gui_container->gui_root != NULL) {
        gui_container->gui_root->applicationWillResignActive();
    }
    
    UIPasteboard *pasteboard = [UIPasteboard generalPasteboard];
    [pasteboard setString:[NSString stringWithCString:MGClipBoard::getString().c_str() encoding:NSUTF8StringEncoding]];
    
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    /*
     Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
     If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
     */
    if (gui_container != NULL) {
//        gui_container->pauseLoop();
        if (gui_container->gui_root != NULL) {
            gui_container->gui_root->applicationDidEnterBackground();
        }
    }
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    /*
     Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
     */
    UIPasteboard *pasteboard = [UIPasteboard generalPasteboard];
    if (pasteboard.string != nil) {
        MGClipBoard::setString([pasteboard.string cStringUsingEncoding:NSUTF8StringEncoding]);
    }
    
    if (gui_container != NULL) {
//        gui_container->resumeLoop();
        if (gui_container->gui_root != NULL) {
            gui_container->gui_root->applicationWillEnterForeground();
        }
    }
    if (self.viewController.movie_need_continue == YES) {
        [self.viewController continueMovie];
    }
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    /*
     Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
     */
    if (gui_container != NULL) {
        gui_container->resumeLoop();
    }
    if (gui_container != NULL && gui_container->gui_root != NULL) {
        gui_container->gui_root->applicationDidBecomeActive();
    }
}
#import <mach/mach.h>
#import <sys/sysctl.h>
- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application
{
    task_basic_info_data_t taskInfo;
    mach_msg_type_number_t infoCount = TASK_BASIC_INFO_COUNT;
    kern_return_t kernReturn = task_info(mach_task_self(),
                                         TASK_BASIC_INFO,
                                         (task_info_t)&taskInfo,
                                         &infoCount);
    
    if (kernReturn == KERN_SUCCESS) {
        double usedMemory = taskInfo.resident_size / 1024.0 / 1024.0;
        NSLog(@"=======applicationDidReceiveMemoryWarning=====usedMemory==%lf", usedMemory);
    }
    if (gui_container != NULL && gui_container->gui_root != NULL) {
        gui_container->applicationDidReceiveMemoryWarning();
        gui_container->gui_root->applicationDidReceiveMemoryWarning();
    }
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    /*
     Called when the application is about to terminate.
     Save data if appropriate.
     See also applicationDidEnterBackground:.
     */
    if (gui_container != NULL) {
        gui_container->applicationWillExit();
        delete gui_container;
        gui_container = 0;
    }
}

- (void)dealloc
{
    [self.window release];
//    [self.viewController release];
    if (gui_container != NULL) {
        delete gui_container;
        gui_container = 0;
    }
    [super dealloc];
}

@end
