 //
//  iOSAppViewController.m
//  iOSApp
//
//  Created by Wang ZhiPeng on 11-5-25.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>
#import <AVFoundation/AVFoundation.h>

#import "iOSAppViewController.h"
#import "iOSAppAppDelegate.h"
#import "EAGLView.h"
#import "MIKit.h"
#import "MGFoundation.h"

@implementation CustomMPMoviePlayerViewController
@synthesize movie_url;

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    [self moviePlayer].controlStyle = MPMovieControlStyleFullscreen;
}

@end

@implementation CustomWebView

- (BOOL)becomeFirstResponder
{
    [super becomeFirstResponder];
    return YES;
}

- (BOOL)canBecomeFirstResponder
{
    return YES;
}

@end

@implementation CustomTimer

@synthesize tag;

@end

@implementation CustomEvent

@synthesize event;

@end

@implementation iOSAppViewController

@synthesize gl_view, gui_container, movie_need_continue, movie_progress, input_text, movie_player, need_continue_movie_url, hidden_status_bar, blend_status_bar, notification_valid, launchOptions;
static int is_retina = 0;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
        self.system_version = [[[UIDevice currentDevice] systemVersion] floatValue];
        gui_container = NULL;
        web_view = nil;
        movie_player = nil;
        movie_need_continue = NO;
        self.input_text = nil;
        session = nil;
        launchOptions = nil;
        popoverController = nil;
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(moviePlayBackDidFinish:)
                                                     name:MPMoviePlayerPlaybackDidFinishNotification
                                                   object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardWasShown:) name:UIKeyboardDidShowNotification object:nil];
        
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardWasHidden:) name:UIKeyboardDidHideNotification object:nil];
        
        location_manager = nil;
        motion_manager = nil;
        need_continue_movie_url = "";
        
        //获取系统info.plist文件中的键值对
        NSDictionary *infoDict = [[NSBundle mainBundle] infoDictionary];
        notification_valid = NO;
        NSNumber* is_notify_valid = [infoDict objectForKey:@"Notification Valid"];
        if (is_notify_valid != nil)
        {
            if ([is_notify_valid intValue] == 0) {
                notification_valid = NO;
            } else {
                notification_valid = YES;
            }
        } else {
            notification_valid = NO;
        }
        
        
        NSNumber* full_screen = [infoDict objectForKey:@"Application Fullscreen"];
        NSLog(@"全屏模式=%@",full_screen);
        
        if (full_screen != nil)
        {
            if ([full_screen intValue] == 0) {
                hidden_status_bar = NO;
            } else {
                hidden_status_bar = YES;
            }
        } else {
            hidden_status_bar = NO;
        }
        
        if (self.system_version >= 9.0) {
            [self setNeedsStatusBarAppearanceUpdate];
        } else {
            [[UIApplication sharedApplication] setStatusBarHidden:self.hidden_status_bar];
        }
        
        NSNumber* blend_status = [infoDict objectForKey:@"Blending StatusBar"];
        NSLog(@"混合状态栏=%@",blend_status);
        
        if (blend_status != nil)
        {
            if ([blend_status intValue] == 0) {
                blend_status_bar = NO;
            } else {
                blend_status_bar = YES;
            }
        } else {
            blend_status_bar = NO;
        }
        
        //获取屏幕旋转支持设置
        orientation_portrait = NO;
        orientation_portraitdown = NO;
        orientation_landscapeleft = NO;
        orientation_landscaperight = NO;
        NSArray* orientation_arr = [infoDict objectForKey:@"UISupportedInterfaceOrientations"];
        for (NSString* orientation in orientation_arr) {
            if([orientation isEqualToString:  @"UIInterfaceOrientationPortrait"])
            {
                orientation_portrait = YES;
            }else if([orientation isEqualToString:  @"UIInterfaceOrientationPortraitUpsideDown"])
            {
                orientation_portraitdown = YES;
            }else if([orientation isEqualToString:  @"UIInterfaceOrientationLandscapeRight"])
            {
                orientation_landscaperight = YES;
            }else if([orientation isEqualToString:  @"UIInterfaceOrientationLandscapeLeft"])
            {
                orientation_landscapeleft = YES;
            }
        }
    }
    return self;
}

- (void)dealloc
{
    [super dealloc];
    if (location_manager != nil) {
        [location_manager release];
        location_manager = nil;
    }
    if (motion_manager != nil) {
        [motion_manager release];
        motion_manager = nil;
    }
    if(web_view != nil)
    {
        [web_view release];
        web_view = nil;
    }
    if (movie_player != nil) {
        [movie_player release];
        movie_player = nil;
    }
    if (image_picker != nil) {
        [image_picker release];
        image_picker = nil;
    }
    if (audio_recorder != nil) {
        [audio_recorder release];
        audio_recorder = nil;
    }
    if (session != nil) {
        [session release];
        session = nil;
    }
    if (self.input_text != nil) {
        self.input_text = nil;
    }
    AudioMIter audio_it;
    for (audio_it = audio_manager.begin(); audio_it != audio_manager.end(); ++audio_it) {
        [audio_it->second release];
    }
    audio_manager.clear();
    TimerMIter timer_it;
    for (timer_it = timer_manager.begin(); timer_it != timer_manager.end(); ++timer_it) {
        [timer_it->second release];
    }
    timer_manager.clear();
    MovieMIter movie_it;
    for (movie_it = movie_player_manager.begin(); movie_it != movie_player_manager.end(); ++movie_it) {
        [movie_it->second release];
    }
    movie_player_manager.clear();
    launchOptions = nil;
}

- (UIStatusBarStyle)preferredStatusBarStyle
{
    return UIStatusBarStyleBlackOpaque;
}// Defaults to UIStatusBarStyleDefault

- (BOOL)prefersStatusBarHidden
{
    return hidden_status_bar;
}// Defaults to NO

- (void)didReceiveMemoryWarning
{
    // Releases the view if it doesn't have a superview.
    // this will release EAGLView, and then call viewDidUnload->viewDidLoad->initWithCoder(EAGLView's Func)
    //[super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
}

#pragma mark - View lifecycle

- (void)handleFirstFrameDraw
{
    if (gl_view.loader_view != nil) {
        gl_view.loader_view.hidden = YES;
        [gl_view.loader_view removeFromSuperview];
        [gl_view.loader_view release];
        gl_view.loader_view = nil;
    }
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    gl_view = (EAGLView*)self.view;
    gl_view._controller = self;
    iOSAppAppDelegate *appDelegate = (iOSAppAppDelegate *) [[UIApplication sharedApplication] delegate];
    [appDelegate.window makeKeyAndVisible]; 
    // Do any additional setup after loading the view from its nib.
    
    [self.gl_view resignFirstResponder];
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

- (void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];
    if (gui_container != NULL) {
        gui_container->pauseLoop();
    }
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    if (gui_container != NULL) {
        gui_container->resumeLoop();
    }
}

//视图旋转完成之后自动调用
-(void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation {
    if (self.system_version >= 9.0) {
        [self setNeedsStatusBarAppearanceUpdate];
    } else {
        [[UIApplication sharedApplication] setStatusBarHidden:self.hidden_status_bar];
    }
    if (gui_container != NULL && gui_container->light_graphic) {
        gui_container->light_graphic->needSettingProjection();
    }
    if (movie_player != nil) {
        [movie_player.view setFrame:self.view.frame];
    }
    if (web_view != nil) {
        [web_view setFrame:self.view.frame];
    }
}

- (void) willRotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation duration:(NSTimeInterval)duration
{
    if (self.system_version >= 9.0) {
        [self setNeedsStatusBarAppearanceUpdate];
    } else {
        [[UIApplication sharedApplication] setStatusBarHidden:self.hidden_status_bar];
    }
    //UIInterfaceOrientation orientation = [UIApplication sharedApplication].statusBarOrientation;
    UIInterfaceOrientation orientation = toInterfaceOrientation;
    [UIView beginAnimations:@"iOS UIView Rotation." context:nil];
    [UIView setAnimationDuration:duration];
    
    if (self.system_version < 6.0) {
        if (movie_player != nil) {
            switch (orientation) {
                case UIInterfaceOrientationPortrait:
                    [movie_player.view setTransform:CGAffineTransformMakeRotation(0)];
                    break;
                case UIInterfaceOrientationPortraitUpsideDown:
                    [movie_player.view setTransform:CGAffineTransformMakeRotation(M_PI)];
                    break;
                case UIInterfaceOrientationLandscapeLeft:
                    [movie_player.view setTransform:CGAffineTransformMakeRotation(-M_PI_2)];
                    break;
                case UIInterfaceOrientationLandscapeRight:
                    [movie_player.view setTransform:CGAffineTransformMakeRotation(M_PI_2)];
                    break;
                default:
                    break;
            }
        }
    }
    if (web_view != nil) {
        switch (orientation) {
            case UIInterfaceOrientationPortrait:
            {
                [web_view setTransform:CGAffineTransformMake(1, 0, 0, 1, 0, 0)];
                break;
            }
            case UIInterfaceOrientationPortraitUpsideDown:
            {
                [web_view setTransform:CGAffineTransformMake(-1, 0, 0, -1, 0, 0)];
                break;
            }
            case UIInterfaceOrientationLandscapeLeft:
            {
                [web_view setTransform:CGAffineTransformMake(0, -1, 1, 0, 0, 0)];
                break;
            }
            case UIInterfaceOrientationLandscapeRight:
            {
                [web_view setTransform:CGAffineTransformMake(0, 1, -1, 0, 0, 0)];
                break;
            }
            default:
                break;
        }
    }
    
    UIImageView* loader_view = ((EAGLView*)self.view).loader_view;
    if (loader_view != nil)
    {
        CGRect screen_frame = self.view.frame;
        switch (orientation) {
            case UIInterfaceOrientationPortrait:
            {
                [loader_view setTransform:CGAffineTransformMake(1, 0, 0, 1, 0, 0)];
                break;
            }
            case UIInterfaceOrientationPortraitUpsideDown:
            {
                [loader_view setTransform:CGAffineTransformMake(-1, 0, 0, -1, 0, 0)];
                break;
            }
            case UIInterfaceOrientationLandscapeLeft:
            {
                float dx = (screen_frame.size.height - screen_frame.size.width) / 2.0;
                [loader_view setTransform:CGAffineTransformMake(0, 1, -1, 0, dx, -dx)];
                break;
            }
            case UIInterfaceOrientationLandscapeRight:
            {
                float dx = (screen_frame.size.height - screen_frame.size.width) / 2.0;
                [loader_view setTransform:CGAffineTransformMake(0, -1, 1, 0, dx, -dx)];
                break;
            }
            default:
                break;
        }
    }
    
    [UIView commitAnimations];
}

- (BOOL)shouldAutorotate
{
    return YES;
}

- (NSUInteger)supportedInterfaceOrientations
{
    NSUInteger ret = 0;
    
    if(orientation_portrait){
        ret |= UIInterfaceOrientationMaskPortrait;
    }
    if(orientation_portraitdown){
        ret |= UIInterfaceOrientationMaskPortraitUpsideDown;
    }
    if(orientation_landscapeleft){
        ret |= UIInterfaceOrientationMaskLandscapeLeft;
    }
    if(orientation_landscaperight){
        ret |= UIInterfaceOrientationMaskLandscapeRight;
    }
    return ret;
}

- (UIInterfaceOrientation)preferredInterfaceOrientationForPresentation
{
    return UIInterfaceOrientationPortrait;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    BOOL ret = NO;
    
    // Return YES for supported orientations
    if(interfaceOrientation == UIInterfaceOrientationPortrait){
        ret = orientation_portrait;
    }else if(interfaceOrientation == UIInterfaceOrientationPortraitUpsideDown){
        ret = orientation_portraitdown;
    }else if(interfaceOrientation == UIInterfaceOrientationLandscapeRight){
        ret = orientation_landscaperight;
    }else if(interfaceOrientation == UIInterfaceOrientationLandscapeLeft){
        ret = orientation_landscapeleft;
    }
	return ret;
}

#pragma mark - Touches Handle -

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (gui_container != NULL) {
        NSArray* all_touches = [touches allObjects];
        std::set<MITouch*> touches;
        for (UITouch* touch in all_touches) {
            MITouch* mi_touch = new MITouch();
            CGPoint point = [touch locationInView:gl_view];
            if (hidden_status_bar == NO && blend_status_bar == NO && (self.system_version >= 7.0 || self.system_version < 6.0)) {
                if (UIInterfaceOrientationIsLandscape([UIApplication sharedApplication].statusBarOrientation) && self.system_version < 8.0)
                {
                    point.y -= [[UIApplication sharedApplication] statusBarFrame].size.width;
                } else {
                    point.y -= [[UIApplication sharedApplication] statusBarFrame].size.height;
                }
            }
            if (is_retina == 1) {
                mi_touch->point.x = point.x * 2;
                mi_touch->point.y = point.y * 2;
            }else if (is_retina == 2) {
                mi_touch->point.x = point.x * 3;
                mi_touch->point.y = point.y * 3;
            }else{
                mi_touch->point.x = point.x;
                mi_touch->point.y = point.y;
            }
            touches.insert(mi_touch);
        }
        gui_container->handleTouchesEvent(touches, MITouchEventBegan);
    }
}
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (gui_container != NULL) {
        NSArray* all_touches = [touches allObjects];
        std::set<MITouch*> touches;
        for (UITouch* touch in all_touches) {
            MITouch* mi_touch = new MITouch();
            CGPoint point = [touch locationInView:gl_view];
            if (hidden_status_bar == NO && blend_status_bar == NO && (self.system_version >= 7.0 || self.system_version < 6.0)) {
                if (UIInterfaceOrientationIsLandscape([UIApplication sharedApplication].statusBarOrientation) && self.system_version < 8.0)
                {
                    point.y -= [[UIApplication sharedApplication] statusBarFrame].size.width;
                } else {
                    point.y -= [[UIApplication sharedApplication] statusBarFrame].size.height;
                }
            }
            if (is_retina == 1) {
                mi_touch->point.x = point.x * 2;
                mi_touch->point.y = point.y * 2;
            }else if (is_retina == 2) {
                mi_touch->point.x = point.x * 3;
                mi_touch->point.y = point.y * 3;
            }else{
                mi_touch->point.x = point.x;
                mi_touch->point.y = point.y;
            }
            mi_touch->maintained_view = NULL;
            touches.insert(mi_touch);
        }
        gui_container->handleTouchesEvent(touches, MITouchEventMoved);
    }
}
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (gui_container != NULL) {
        NSArray* all_touches = [touches allObjects];
        std::set<MITouch*> touches;
        for (UITouch* touch in all_touches) {
            MITouch* mi_touch = new MITouch();
            CGPoint point = [touch locationInView:gl_view];
            if (hidden_status_bar == NO && blend_status_bar == NO && (self.system_version >= 7.0 || self.system_version < 6.0)) {
                if (UIInterfaceOrientationIsLandscape([UIApplication sharedApplication].statusBarOrientation) && self.system_version < 8.0)
                {
                    point.y -= [[UIApplication sharedApplication] statusBarFrame].size.width;
                } else {
                    point.y -= [[UIApplication sharedApplication] statusBarFrame].size.height;
                }
            }
            if (is_retina == 1) {
                mi_touch->point.x = point.x * 2;
                mi_touch->point.y = point.y * 2;
            }else if (is_retina == 2) {
                mi_touch->point.x = point.x * 3;
                mi_touch->point.y = point.y * 3;
            }else{
                mi_touch->point.x = point.x;
                mi_touch->point.y = point.y;
            }
            mi_touch->maintained_view = NULL;
            touches.insert(mi_touch);
        }
        gui_container->handleTouchesEvent(touches, MITouchEventEnded);
    }
}
- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (gui_container != NULL) {
        NSArray* all_touches = [touches allObjects];
        std::set<MITouch*> touches;
        for (UITouch* touch in all_touches) {
            MITouch* mi_touch = new MITouch();
            CGPoint point = [touch locationInView:gl_view];
            if (hidden_status_bar == NO && blend_status_bar == NO && (self.system_version >= 7.0 || self.system_version < 6.0)) {
                if (UIInterfaceOrientationIsLandscape([UIApplication sharedApplication].statusBarOrientation) && self.system_version < 8.0)
                {
                    point.y -= [[UIApplication sharedApplication] statusBarFrame].size.width;
                } else {
                    point.y -= [[UIApplication sharedApplication] statusBarFrame].size.height;
                }
            }
            if (is_retina == 1) {
                mi_touch->point.x = point.x * 2;
                mi_touch->point.y = point.y * 2;
            }else if (is_retina == 2) {
                mi_touch->point.x = point.x * 3;
                mi_touch->point.y = point.y * 3;
            }else{
                mi_touch->point.x = point.x;
                mi_touch->point.y = point.y;
            }
            mi_touch->maintained_view = NULL;
            touches.insert(mi_touch);
        }
        gui_container->handleTouchesEvent(touches, MITouchEventCancelled);
    }
}

#pragma mark - Responder

- (BOOL)becomeFirstResponder
{
    return [super becomeFirstResponder];
}

- (BOOL)canBecomeFirstResponder
{
    return NO;
}

- (BOOL)canResignFirstResponder
{
    return YES;
}// default is YES
- (BOOL)resignFirstResponder
{
    return [super resignFirstResponder];
}

- (void) handleSysCallbackEvent
{
    if (gui_container != NULL)
        gui_container->handleEvent();
}

- (void) handleEventImmediatelly:(id)_event
{
    MIEvent* event = ((CustomEvent*)_event).event;
    if (gui_container != NULL)
        gui_container->handleEventImmediatelly(event);
    [_event release];
}

#pragma mark Keyboard methods

- (void) keyboardWasShown:(NSNotification *) notif{
    NSDictionary *info = [notif userInfo];
    NSValue *value = [info objectForKey:UIKeyboardFrameEndUserInfoKey];
    CGSize keyboardSize = [value CGRectValue].size;
    
    UIInterfaceOrientation orientation = [UIApplication sharedApplication].statusBarOrientation;
    BOOL isLandscape = UIInterfaceOrientationIsLandscape(orientation);
    float height = 0;
    if (isLandscape && self.system_version < 8.0) {
        height = keyboardSize.width;
    } else {
        height = keyboardSize.height;
    }
    
    if (is_retina == 1)
        height *= 2;
    else if (is_retina == 2)
        height *= 3;
    if (gui_container != NULL)
        gui_container->moveEntirViewForKeyboardWithHeight(height);
}

- (void) keyboardWasHidden:(NSNotification *) notif{
//    NSDictionary *info = [notif userInfo];
    
//    NSValue *value = [info objectForKey:UIKeyboardFrameBeginUserInfoKey];
//    CGSize keyboardSize = [value CGRectValue].size;
    if (gui_container != NULL)
        gui_container->moveEntirViewForKeyboardWithHeight(0);
}

- (void) setKBType:(MIKeyboardType)_kbType
{
    switch (_kbType) {
        case z11::MIKeyboardTypeDefault:
            [gl_view setKeyboardType:UIKeyboardTypeDefault];
            break;
        case z11::MIKeyboardTypeASCIICapable:
            [gl_view setKeyboardType:UIKeyboardTypeASCIICapable];
            break;
        case z11::MIKeyboardTypeNumbersAndPunctuation:
            [gl_view setKeyboardType:UIKeyboardTypeNumbersAndPunctuation];
            break;
        case z11::MIKeyboardTypeURL:
            [gl_view setKeyboardType:UIKeyboardTypeURL];
            break;
        case z11::MIKeyboardTypeNumberPad:
            [gl_view setKeyboardType:UIKeyboardTypeNumberPad];
            break;
        case z11::MIKeyboardTypePhonePad:
            [gl_view setKeyboardType:UIKeyboardTypePhonePad];
            break;
        case z11::MIKeyboardTypeNamePhonePad:
            [gl_view setKeyboardType:UIKeyboardTypeNamePhonePad];
            break;
        case z11::MIKeyboardTypeEmailAddress:
            [gl_view setKeyboardType:UIKeyboardTypeEmailAddress];
            break;
        case z11::MIKeyboardTypeDecimalPad:
            [gl_view setKeyboardType:UIKeyboardTypeDecimalPad];
            break;
        case z11::MIKeyboardTypeTwitter:
            [gl_view setKeyboardType:UIKeyboardTypeTwitter];
            break;
        default:
            [gl_view setKeyboardType:UIKeyboardTypeDefault];
            break;
    }
}

- (void) setRKType:(MIReturnKeyType)_rkType;
{
    switch (_rkType) {
        case z11::MIReturnKeyDefault:
            [gl_view setReturnKeyType:(UIReturnKeyDefault)];
            break;
        case z11::MIReturnKeyGo:
            [gl_view setReturnKeyType:(UIReturnKeyGo)];
            break;
        case z11::MIReturnKeyGoogle:
            [gl_view setReturnKeyType:(UIReturnKeyGoogle)];
            break;
        case z11::MIReturnKeyJoin:
            [gl_view setReturnKeyType:(UIReturnKeyJoin)];
            break;
        case z11::MIReturnKeyNext:
            [gl_view setReturnKeyType:(UIReturnKeyNext)];
            break;
        case z11::MIReturnKeyRoute:
            [gl_view setReturnKeyType:(UIReturnKeyRoute)];
            break;
        case z11::MIReturnKeySearch:
            [gl_view setReturnKeyType:(UIReturnKeySearch)];
            break;
        case z11::MIReturnKeySend:
            [gl_view setReturnKeyType:(UIReturnKeySend)];
            break;
        case z11::MIReturnKeyYahoo:
            [gl_view setReturnKeyType:(UIReturnKeyYahoo)];
            break;
        case z11::MIReturnKeyDone:
            [gl_view setReturnKeyType:(UIReturnKeyDone)];
            break;
        case z11::MIReturnKeyEmergencyCall:
            [gl_view setReturnKeyType:(UIReturnKeyEmergencyCall)];
            break;
        default:
            [gl_view setReturnKeyType:(UIReturnKeyDefault)];
            break;
    }
}

#pragma mard - Text Input Methods

// UITextInput required method - Insert the provided text and marks it to indicate
// that it is part of an active input session.
- (void)setMarkedText:(NSString *)markedText selectedRange:(NSRange)selectedRange
{
//    NSLog(@"=================================");
//    NSLog(@"setMarkedText");
//    NSLog(@"text is: %s",[markedText cStringUsingEncoding:(NSUTF8StringEncoding)]);
//    NSLog(@"range is: location:%d length:%d", selectedRange.location, selectedRange.length);
    if (self.gl_view.accept_keyboard_input == NO) {
        return;
    }
    
    self.input_text = markedText;
    string marked_text = string([markedText cStringUsingEncoding:(NSUTF8StringEncoding)]);
    char special_separator[4];
    special_separator[0] = 0xE2;
    special_separator[1] = 0x80;
    special_separator[2] = 0x86;
    special_separator[3] = 0x00;
    string separator = special_separator;
    while (marked_text.find_first_of(separator) != marked_text.npos) {
        marked_text.replace(marked_text.find_first_of(separator), separator.size(), " ");
    }
    MIEvent* event = new MIEvent();
    MGTextContent* content = new MGTextContent();
    content->text_content = marked_text;
    event->_event_type = KEYBOARD_PRE_INPUT;
    event->_content = content;
    event->_responder = NULL;
    if (gui_container != NULL)
        gui_container->handleSystemEvent(event);
    delete_and_set_null(event);
}

// UITextInput required method - Unmark the currently marked text.
- (void)unmarkText
{
//    NSLog(@"=================================");
//    NSLog(@"unmarkText");
    if (self.input_text == nil || self.gl_view.accept_keyboard_input == NO) {
        return;
    }
    MIEvent* event = new MIEvent();
    MGTextContent* content = new MGTextContent();
    content->text_content = string([self.input_text cStringUsingEncoding:(NSUTF8StringEncoding)]);
    event->_event_type = KEYBOARD_INPUT;
    event->_content = content;
    event->_responder = NULL;
    if (gui_container != NULL)
        gui_container->handleSystemEvent(event);
    self.input_text = nil;
    delete_and_set_null(event);
    return;
}

#pragma mark UIKeyInput methods

- (BOOL)hasText
{
//    NSLog(@"=================================");
//    NSLog(@"hasText");
    return NO;
}
- (void)insertText:(NSString *)text
{
//    NSLog(@"=================================");
//    NSLog(@"insertText");
//    NSLog(@"insertText: %s", [text cStringUsingEncoding:(NSUTF8StringEncoding)]);
    MIEvent* event = new MIEvent();
    MGTextContent* content = new MGTextContent();
    content->text_content = string([text cStringUsingEncoding:(NSUTF8StringEncoding)]);
    event->_event_type = KEYBOARD_INPUT;
    event->_content = content;
    event->_responder = NULL;
    if (gui_container != NULL)
        gui_container->handleSystemEvent(event);
    delete_and_set_null(event);
}
- (void)deleteBackward
{
//    NSLog(@"=================================");
//    NSLog(@"deleteBackward");
    MIEvent* event = new MIEvent();
    event->_event_type = KEYBOARD_DELETE_BACKWARD;
    event->_responder = NULL;
    event->_content = NULL;
    if (gui_container != NULL)
        gui_container->handleSystemEvent(event);
    delete_and_set_null(event);
}


#pragma mark - Retina

+ (int) isRetina
{
    return is_retina;
}

+ (void) setRetina:(int) retina
{
    is_retina = retina;
}

#pragma mark - location service
- (void)startLocationService
{
    if (location_manager == nil) {
        location_manager = [[CLLocationManager alloc]init];
    }
    location_manager.delegate = self;
    [location_manager startUpdatingLocation];
}

- (void)stopLocationService
{
    [location_manager stopUpdatingLocation];
}

- (void)startHeadingService
{
    if (location_manager == nil) {
        location_manager = [[CLLocationManager alloc]init];
    }
    location_manager.delegate = self;
    [location_manager startUpdatingHeading];
}

- (void)stopHeadingService
{
    [location_manager stopUpdatingHeading];
}

- (void)startMonitoringForRegion:(string)uuid
{
    if (location_manager == nil) {
        location_manager = [[CLLocationManager alloc]init];
    }
    location_manager.delegate = self;
    NSString* ns_uuid = [NSString stringWithUTF8String:uuid.c_str()];
    CLBeaconRegion *beacon = [[CLBeaconRegion alloc] initWithProximityUUID:[[NSUUID alloc] initWithUUIDString:ns_uuid] identifier:ns_uuid];
    [location_manager startMonitoringForRegion:beacon];
    [beacon release];
}

- (void)stopMonitoringForRegion:(string)uuid
{
    NSString* ns_uuid = [NSString stringWithUTF8String:uuid.c_str()];
    CLBeaconRegion *beacon = [[CLBeaconRegion alloc] initWithProximityUUID:[[NSUUID alloc] initWithUUIDString:ns_uuid] identifier:ns_uuid];
    [location_manager stopMonitoringForRegion:beacon];
    [beacon release];
}

- (void)startRangingForRegion:(string)uuid
{
    if (location_manager == nil) {
        location_manager = [[CLLocationManager alloc]init];
    }
    location_manager.delegate = self;
    NSString* ns_uuid = [NSString stringWithUTF8String:uuid.c_str()];
    CLBeaconRegion *beacon = [[CLBeaconRegion alloc] initWithProximityUUID:[[NSUUID alloc] initWithUUIDString:ns_uuid] identifier:ns_uuid];
    [location_manager startRangingBeaconsInRegion:beacon];
    [beacon release];
}

- (void)stopRangingForRegion:(string)uuid
{
    NSString* ns_uuid = [NSString stringWithUTF8String:uuid.c_str()];
    CLBeaconRegion *beacon = [[CLBeaconRegion alloc] initWithProximityUUID:[[NSUUID alloc] initWithUUIDString:ns_uuid] identifier:ns_uuid];
    [location_manager stopRangingBeaconsInRegion:beacon];
    [beacon release];
}

- (void)releaseLocationService
{
    if (location_manager != nil) {
        [location_manager release];
        location_manager = nil;
    }
}

/*
 *  locationManager:didUpdateToLocation:fromLocation:
 *  
 *  Discussion:
 *    Invoked when a new location is available. oldLocation may be nil if there is no previous location
 *    available.
 */
- (void)locationManager:(CLLocationManager *)manager
	didUpdateToLocation:(CLLocation *)newLocation
           fromLocation:(CLLocation *)oldLocation
{
    MGLocation to_location;
    MGLocation from_location;
    to_location.longitude = newLocation.coordinate.longitude;
    to_location.latitude = newLocation.coordinate.latitude;
    to_location.altitude = newLocation.altitude;
    to_location.course = newLocation.course;
    to_location.speed = newLocation.speed;
    to_location.accuracy = (newLocation.horizontalAccuracy + newLocation.verticalAccuracy) / 2;
    to_location.timestamp = MGTime::FromInternalValue(newLocation.timestamp.timeIntervalSince1970*MGTime::kMicrosecondsPerSecond);
    from_location.longitude = oldLocation.coordinate.longitude;
    from_location.latitude = oldLocation.coordinate.latitude;
    from_location.altitude = oldLocation.altitude;
    from_location.course = oldLocation.course;
    from_location.speed = oldLocation.speed;
    from_location.accuracy = (oldLocation.horizontalAccuracy + oldLocation.verticalAccuracy) / 2;
    from_location.timestamp = MGTime::FromInternalValue(oldLocation.timestamp.timeIntervalSince1970*MGTime::kMicrosecondsPerSecond);
    MGLocationManager::didUpdateLocation(to_location, from_location);
}

/*
 *  locationManager:didUpdateHeading:
 *  
 *  Discussion:
 *    Invoked when a new heading is available.
 */
- (void)locationManager:(CLLocationManager *)manager
       didUpdateHeading:(CLHeading *)newHeading __OSX_AVAILABLE_STARTING(__MAC_NA,__IPHONE_3_0)
{
    MGHeading heading;
    heading.magnetic_heading = newHeading.magneticHeading;
    heading.true_heading = newHeading.trueHeading;
    heading.heading_accuracy = newHeading.headingAccuracy;
    heading.x = newHeading.x;
    heading.y = newHeading.y;
    heading.z = newHeading.z;
    MGLocationManager::didUpdateHeading(heading);
}

/*
 *  locationManager:shouldDisplayHeadingCalibrationForDuration:
 *
 *  Discussion:
 *    Invoked when a new heading is available. Return YES to display heading calibration info. The display 
 *    will remain until heading is calibrated, unless dismissed early via dismissHeadingCalibrationDisplay.
 */
- (BOOL)locationManagerShouldDisplayHeadingCalibration:(CLLocationManager *)manager
{
    return NO;
}

/*
 *  locationManager:didEnterRegion:
 *
 *  Discussion:
 *    Invoked when the user enters a monitored region.  This callback will be invoked for every allocated
 *    CLLocationManager instance with a non-nil delegate that implements this method.
 */
- (void)locationManager:(CLLocationManager *)manager
         didEnterRegion:(CLRegion *)region
{
    if ([region isKindOfClass:[CLBeaconRegion class]] == TRUE)
    {
        MGLocationRegion r;
        r.uuid = [[[((CLBeaconRegion*)region) proximityUUID] UUIDString] UTF8String];
        MGLocationManager::didEnterRegion(r);
    }
}

/*
 *  locationManager:didExitRegion:
 *
 *  Discussion:
 *    Invoked when the user exits a monitored region.  This callback will be invoked for every allocated
 *    CLLocationManager instance with a non-nil delegate that implements this method.
 */
- (void)locationManager:(CLLocationManager *)manager
          didExitRegion:(CLRegion *)region
{
    if ([region isKindOfClass:[CLBeaconRegion class]] == TRUE)
    {
        MGLocationRegion r;
        r.uuid = [[[((CLBeaconRegion*)region) proximityUUID] UUIDString] UTF8String];
        MGLocationManager::didExitRegion(r);
    }
}

/*
 *  locationManager:didRangeBeacons:inRegion:
 *
 *  Discussion:
 *    Invoked when a new set of beacons are available in the specified region.
 *    beacons is an array of CLBeacon objects.
 *    If beacons is empty, it may be assumed no beacons that match the specified region are nearby.
 *    Similarly if a specific beacon no longer appears in beacons, it may be assumed the beacon is no longer received
 *    by the device.
 */
- (void)locationManager:(CLLocationManager *)manager
        didRangeBeacons:(NSArray<CLBeacon *> *)beacons inRegion:(CLBeaconRegion *)region __OSX_AVAILABLE_STARTING(__MAC_NA,__IPHONE_7_0) __TVOS_PROHIBITED __WATCHOS_PROHIBITED
{
    // 打印所有iBeacon的信息 //
    deque<MGBeacon> beas;
    for (CLBeacon* beacon in beacons) {
        //NSLog(@"beacon.proximity %ld, rssi is :%ld",beacon.proximity, beacon.rssi);
        MGBeacon bea;
        bea.rssi = beacon.rssi;
        bea.uuid = [[beacon.proximityUUID UUIDString] UTF8String];
        bea.distance = (BeaconDistance)beacon.proximity;
        beas.push_back(bea);
    }
    MGLocationManager::didRangeBeacons(beas);
}

/*
 *  locationManager:didFailWithError:
 *  
 *  Discussion:
 *    Invoked when an error has occurred. Error types are defined in "CLError.h".
 */
- (void)locationManager:(CLLocationManager *)manager
       didFailWithError:(NSError *)error
{
    std::string _error = [[error description] cStringUsingEncoding:NSUTF8StringEncoding];
    MGLocationManager::didUpdateLocationFailWithError(_error);
}

/*
 *  locationManager:monitoringDidFailForRegion:withError:
 *  
 *  Discussion:
 *    Invoked when a region monitoring error has occurred. Error types are defined in "CLError.h".
 */
- (void)locationManager:(CLLocationManager *)manager
monitoringDidFailForRegion:(CLRegion *)region
              withError:(NSError *)error
{
    
}

/*
 *  locationManager:didChangeAuthorizationStatus:
 *  
 *  Discussion:
 *    Invoked when the authorization status changes for this application.
 */
- (void)locationManager:(CLLocationManager *)manager didChangeAuthorizationStatus:(CLAuthorizationStatus)status
{
    if (status == kCLAuthorizationStatusNotDetermined) {
        if ([[[UIDevice currentDevice] systemVersion] floatValue] >= 8.0) {
            //获取系统info.plist文件中的键值对
            NSDictionary *infoDict = [[NSBundle mainBundle] infoDictionary];
            NSString* always_desc = [infoDict objectForKey:@"NSLocationAlwaysUsageDescription"];
            if (always_desc != nil) {
                [location_manager requestAlwaysAuthorization];
                return;
            }
            NSString* when_in_use_desc = [infoDict objectForKey:@"NSLocationWhenInUseUsageDescription"];
            if (when_in_use_desc != nil) {
                [location_manager requestWhenInUseAuthorization];
                return;
            }
        }
    }
}

/*
 *  locationManager:didStartMonitoringForRegion:
 *  
 *  Discussion:
 *    Invoked when a monitoring for a region started successfully.
 */
- (void)locationManager:(CLLocationManager *)manager
didStartMonitoringForRegion:(CLRegion *)region __OSX_AVAILABLE_STARTING(__MAC_TBD,__IPHONE_5_0)
{
    
}

#pragma mark - motion service
- (void)startAccelerationService:(float)interval
{
    if (motion_manager == nil) {
        motion_manager = [[CMMotionManager alloc]init];
    }
    if (interval > 0.0f) {
        motion_manager.accelerometerUpdateInterval = interval;
    }
    [motion_manager startAccelerometerUpdatesToQueue:[NSOperationQueue currentQueue] withHandler:^(CMAccelerometerData *accelerometerData, NSError *error) {
        MGAcceleration acceleration;
        acceleration.x = accelerometerData.acceleration.x;
        acceleration.y = accelerometerData.acceleration.y;
        acceleration.z = accelerometerData.acceleration.z;
        MGMotionManager::didUpdateAcceleration(acceleration);
    }];
}

- (void)stopAccelerationService
{
    [motion_manager stopAccelerometerUpdates];
}

- (void)startRotationService:(float)interval
{
    if (motion_manager == nil) {
        motion_manager = [[CMMotionManager alloc]init];
    }
    if (interval > 0.0f) {
        motion_manager.gyroUpdateInterval = interval;
    }
    [motion_manager startGyroUpdatesToQueue:[NSOperationQueue currentQueue] withHandler:^(CMGyroData *gyroData, NSError *error) {
        MGRotation rotation;
        rotation.x = gyroData.rotationRate.x;
        rotation.y = gyroData.rotationRate.y;
        rotation.z = gyroData.rotationRate.z;
        MGMotionManager::didUpdateRotation(rotation);
    }];
}

- (void)stopRotationService
{
    [motion_manager stopGyroUpdates];
}

- (void)releaseMotionService
{
    if (motion_manager != nil) {
        [motion_manager release];
        motion_manager = nil;
    }
}

#pragma mark - Camera Support
- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection
{
    if (self.system_version >= 5.0 && self.system_version < 7.0)
    {
#if (__IPHONE_OS_VERSION_MAX_ALLOWED < 70000 && __IPHONE_OS_VERSION_MAX_ALLOWED >= 50000)
        connection.videoMinFrameDuration = CMTimeMake(1, 15);
#endif
    }
    // Create a UIImage from the sample buffer data
    // Get a CMSampleBuffer's Core Video image buffer for the media data
    CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
    // Lock the base address of the pixel buffer
    CVPixelBufferLockBaseAddress(imageBuffer, 0);
    
    // Get the number of bytes per row for the pixel buffer
    void *baseAddress = CVPixelBufferGetBaseAddress(imageBuffer);
    
    // Get the pixel buffer width and height
    size_t width = CVPixelBufferGetWidth(imageBuffer);
    size_t height = CVPixelBufferGetHeight(imageBuffer);
    
    // Get the number of bytes per row for the pixel buffer
    size_t bytesPerRow = CVPixelBufferGetBytesPerRow(imageBuffer);
    // Create a device-dependent RGB color space
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    
    // Create a bitmap graphics context with the sample buffer data
    CGContextRef src_context = CGBitmapContextCreate(baseAddress, width, height, 8,
                                                     bytesPerRow, colorSpace, kCGBitmapByteOrder32Little | kCGImageAlphaPremultipliedFirst);
    CGImageRef src_image = CGBitmapContextCreateImage(src_context);
    
    if (img_addr == NULL) {
        img_addr = new byte_8[width * height * 4];
    }
    
    UIInterfaceOrientation orientation = [UIApplication sharedApplication].statusBarOrientation;
    BOOL isLandscape = UIInterfaceOrientationIsLandscape(orientation);
    
    size_t dst_width = 0;
    size_t dst_height = 0;
    
    if (isLandscape) {
        dst_width = width;
        dst_height = height;
    } else {
        dst_width = height;
        dst_height = width;
    }
    
    CGContextRef dst_context = CGBitmapContextCreate(img_addr, dst_width, dst_height, 8,
                                                     dst_width * 4, colorSpace, kCGBitmapByteOrder32Big | kCGImageAlphaPremultipliedLast);
    
    if(orientation == UIInterfaceOrientationPortrait){
        CGFloat angleInRadians = -90 * (M_PI / 180);
        CGContextSetAllowsAntialiasing(dst_context, FALSE);
        CGContextSetInterpolationQuality(dst_context, kCGInterpolationNone);
        CGContextTranslateCTM(dst_context, 0.0, width);
        CGContextRotateCTM(dst_context, angleInRadians);
    }else if(orientation == UIInterfaceOrientationPortraitUpsideDown){
        CGFloat angleInRadians = 90 * (M_PI / 180);
        CGContextSetAllowsAntialiasing(dst_context, FALSE);
        CGContextSetInterpolationQuality(dst_context, kCGInterpolationNone);
        CGContextTranslateCTM(dst_context, height, 0.0);
        CGContextRotateCTM(dst_context, angleInRadians);
    }else if(orientation != connection.videoOrientation){
        CGFloat angleInRadians = -180 * (M_PI / 180);
        CGContextSetAllowsAntialiasing(dst_context, FALSE);
        CGContextSetInterpolationQuality(dst_context, kCGInterpolationNone);
        CGContextTranslateCTM(dst_context, width, height);
        CGContextRotateCTM(dst_context, angleInRadians);
    }
    
    CGContextDrawImage(dst_context, CGRectMake(0, 0,width,height),src_image);

    // Unlock the pixel buffer
    CVPixelBufferUnlockBaseAddress(imageBuffer,0);
    
    // Free up the context and color space
    CGContextRelease(src_context);
    CGColorSpaceRelease(colorSpace);
    
//    UIImage *image = [UIImage imageWithCGImage:src_image scale:1.0 orientation:(UIImageOrientationRight)];
    
    MGImage * capture_image = MGImage::fromMem(32, (int)dst_width, (int)dst_height, (unsigned char*)img_addr);
    MGCameraCaptureManager::didCameraCaptureOutput(capture_image);
    delete capture_image;
    CGImageRelease(src_image);
    CGContextRelease(dst_context);
}

// Create and configure a capture session and start it running
- (void)startCameraCapture:(MGCaptureDevicePosition) position
{
    img_addr = NULL;
    if (session != nil) {
        if (session.isRunning) {
            return;
        }
        [session startRunning];
        return;
    }
    NSError *error = nil;
    
    // Create the session
    session = [[AVCaptureSession alloc] init];
    
    // Configure the session to produce lower resolution video frames, if your
    // processing algorithm can cope. We'll specify medium quality for the
    // chosen device.
    session.sessionPreset = AVCaptureSessionPresetMedium;
    
    // Find a suitable AVCaptureDevice
    AVCaptureDevice *device = nil; //[AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];//这里默认是使用后置摄像头，你可以改成前置摄像头
    
    AVCaptureDevicePosition pos;
    
    switch (position) {
        case MGCaptureDevicePositionUnspecified:
            pos = AVCaptureDevicePositionUnspecified;
            break;
        case MGCaptureDevicePositionFront:
            pos = AVCaptureDevicePositionFront;
            break;
        case MGCaptureDevicePositionBack:
            pos = AVCaptureDevicePositionBack;
            break;
        default:
            pos = AVCaptureDevicePositionBack;
            break;
    }
    
    //获取前置摄像头设备
    NSArray *cameras = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
    for (AVCaptureDevice *dev in cameras)
    {
        if (dev.position == pos)
        {
            device = dev;
            break;
        }
    }
    
    if (device == nil) {
        device = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
    }
    
    if (device == nil)
    {
        UIAlertView* alert = [[UIAlertView alloc]initWithTitle:nil message:@"打开摄像头失败!" delegate:nil cancelButtonTitle:@"关闭" otherButtonTitles:nil];
        [alert show];
        [alert release];
        [session release];
        session = nil;
        return;
    }
    
    if (self.system_version >= 7.0)
    {
#if (__IPHONE_OS_VERSION_MAX_ALLOWED >= 70000)
        [device lockForConfiguration : nil];
        device.activeVideoMinFrameDuration = CMTimeMake(1, 15);
        [device unlockForConfiguration];
#endif
    }
    
    NSError* err = [[NSError alloc] init];
    BOOL ret = [device lockForConfiguration:&err];
    if (ret) {
        if ([device isFocusModeSupported:(AVCaptureFocusModeContinuousAutoFocus)]) {
            [device setFocusMode:AVCaptureFocusModeContinuousAutoFocus];
        }
        [device unlockForConfiguration];
    } else {
        NSLog(@"%@", [err description]);
    }
    [err release];
    err = nil;
    
    // Create a device input with the device and add it to the session.
    AVCaptureDeviceInput *input = [AVCaptureDeviceInput deviceInputWithDevice:device
                                                                        error:&error];
    if (!input) {
        // Handling the error appropriately.
        UIAlertView* alert = [[UIAlertView alloc]initWithTitle:@"未获得相机权限" message:@"请前往设置->隐私->相机中打开本应用的相机权限" delegate:nil cancelButtonTitle:@"关闭" otherButtonTitles:nil];
        [alert show];
        [alert release];
        [session release];
        session = nil;
        return;
    }
    if([session canAddInput:input]){
        [session addInput:input];
    }
    else{
        UIAlertView* alert = [[UIAlertView alloc]initWithTitle:@"未获得相机权限" message:@"请前往设置->隐私->相机中打开本应用的相机权限" delegate:nil cancelButtonTitle:@"关闭" otherButtonTitles:nil];
        [alert show];
        [alert release];
        [session release];
        session = nil;
        return;
    }
    
    // Create a VideoDataOutput and add it to the session
    AVCaptureVideoDataOutput *output = [[AVCaptureVideoDataOutput alloc] init];
    // Specify the pixel format
    NSLog(@"self.view.frame is %f,%f,%f,%f", self.view.frame.origin.x, self.view.frame.origin.y, self.view.frame.size.width, self.view.frame.size.height);
    output.videoSettings = [NSDictionary dictionaryWithObjectsAndKeys:
                            [NSNumber numberWithInt: kCVPixelFormatType_32BGRA], kCVPixelBufferPixelFormatTypeKey,
                            [NSNumber numberWithInt: self.view.frame.size.width], kCVPixelBufferWidthKey,
                            [NSNumber numberWithInt: self.view.frame.size.height], kCVPixelBufferHeightKey,
                            nil];
    
    [session addOutput:output];
    
    // Configure your output.
    dispatch_queue_t queue = dispatch_queue_create("myQueue", NULL);
    [output setSampleBufferDelegate:self queue:queue];
    dispatch_release(queue);
    
//    AVCaptureVideoPreviewLayer* preLayer = [AVCaptureVideoPreviewLayer layerWithSession: session];
//    //preLayer = [AVCaptureVideoPreviewLayer layerWithSession:session];
//    NSLog(@"self.view.frame is %f,%f,%f,%f", self.view.frame.origin.x, self.view.frame.origin.y, self.view.frame.size.width, self.view.frame.size.height);
//    preLayer.frame = self.view.frame;
//    preLayer.videoGravity = AVLayerVideoGravityResizeAspectFill;
//    [self.view.layer addSublayer:preLayer];
    // If you wish to cap the frame rate to a known value, such as 15 fps, set
    // minFrameDuration.
    if (self.system_version < 5.0)
    {
#if (__IPHONE_OS_VERSION_MAX_ALLOWED < 50000)
        [output setMinFrameDuration:CMTimeMake(1, 15)];
#endif
    }
    
    // Start the session running to start the flow of data
    [session startRunning];
    [output release];
    
    // Assign session to an ivar.
    //[self setSession:session];
}

- (void)stopCameraCapture
{
    if (session == nil) {
        return;
    }
    [session stopRunning];
    [session release];
    session = nil;
    delete_array_and_set_null(img_addr);
}

- (void) focusPointOfInterest:(MGCaptureDevicePosition) position WithPoint:(CGPoint)point
{
    AVCaptureDevice *device = nil; //[AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];//这里默认是使用后置摄像头，你可以改成前置摄像头
    
    AVCaptureDevicePosition pos;
    
    switch (position) {
        case MGCaptureDevicePositionUnspecified:
            pos = AVCaptureDevicePositionUnspecified;
            break;
        case MGCaptureDevicePositionFront:
            pos = AVCaptureDevicePositionFront;
            break;
        case MGCaptureDevicePositionBack:
            pos = AVCaptureDevicePositionBack;
            break;
        default:
            pos = AVCaptureDevicePositionBack;
            break;
    }
    
    //获取前置摄像头设备
    NSArray *cameras = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
    for (AVCaptureDevice *dev in cameras)
    {
        if (dev.position == pos)
        {
            device = dev;
            break;
        }
    }
    
    if (device == nil) {
        device = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
    }
    
    if (device == nil)
        return;
    
    NSError* err = [[NSError alloc] init];
    BOOL ret = [device lockForConfiguration:&err];
    if (!ret) {
        NSLog(@"%@", [err description]);
        [err release];
        err = nil;
        return;
    }
    [err release];
    err = nil;
    if ([device isFocusModeSupported:(AVCaptureFocusModeContinuousAutoFocus)]) {
        device.focusPointOfInterest = point;
        [device setFocusMode:AVCaptureFocusModeContinuousAutoFocus];
    }
    [device unlockForConfiguration];
}

#pragma mark - imagePicker
- (void) openMedia:(MGMediaPickerType *)_pickerType
{
    switch (_pickerType->sourceType) {
        case MGMediaPickerSourceTypeImage:
            pickerType.sourceType = UIImagePickerControllerSourceTypeCamera;
            pickerType.captureMode = UIImagePickerControllerCameraCaptureModePhoto;
            break;
        case MGMediaPickerSourceTypePhotoLibrary:
            pickerType.sourceType = UIImagePickerControllerSourceTypePhotoLibrary;
            pickerType.captureMode = UIImagePickerControllerCameraCaptureModePhoto;
            break;
        case MGMediaPickerSourceTypeSavedPhotoAlbum:
            pickerType.sourceType = UIImagePickerControllerSourceTypeSavedPhotosAlbum;
            pickerType.captureMode = UIImagePickerControllerCameraCaptureModePhoto;
            break;
        case MGMediaPickerSourceTypeVideo:
            pickerType.sourceType = UIImagePickerControllerSourceTypeCamera;
            pickerType.captureMode = UIImagePickerControllerCameraCaptureModeVideo;
            break;
        case MGMediaPickerSourceTypeVideoLibrary:
            pickerType.sourceType = UIImagePickerControllerSourceTypePhotoLibrary;
            pickerType.captureMode = UIImagePickerControllerCameraCaptureModeVideo;
            break;
        default:
            break;
    }
    switch (_pickerType->mediaQuality) {
        case MGMediaPickerCameraQualityHigh:
            pickerType.mediaQuality = UIImagePickerControllerQualityTypeHigh;
            break;
        case MGMediaPickerCameraQualityMedium:
            pickerType.mediaQuality = UIImagePickerControllerQualityTypeMedium;
            break;
        case MGMediaPickerCameraQualityLow:
            pickerType.mediaQuality = UIImagePickerControllerQualityTypeLow;
            break;
        default:
            break;
    }
    
    if(_pickerType->allows_edit){
        pickerType.allows_edit = YES;
    }
    else{
        pickerType.allows_edit = NO;
    }
    
    [self performSelector:@selector(showCamera) withObject:nil afterDelay:0.3];
}

-(void) showCamera
{
    if ([UIImagePickerController isSourceTypeAvailable:pickerType.sourceType]){
        UIImagePickerController *ipc = [[UIImagePickerController alloc] init];
        ipc.sourceType = pickerType.sourceType;
        //ipc.mediaTypes = [NSArray arrayWithObjects:@"public.movie", @"public.image", nil];
        //ipc.mediaTypes = [UIImagePickerController availableMediaTypesForSourceType:ipc.sourceType];
        if (ipc.sourceType == UIImagePickerControllerSourceTypeCamera)
        {
            switch (pickerType.captureMode) {
                case UIImagePickerControllerCameraCaptureModeVideo:
                    ipc.mediaTypes = [NSArray arrayWithObjects:@"public.movie", nil];
                    break;
                case UIImagePickerControllerCameraCaptureModePhoto:
                    ipc.mediaTypes = [NSArray arrayWithObjects:@"public.image", nil];
                    break;
                default:
                    ipc.mediaTypes = [UIImagePickerController availableMediaTypesForSourceType:ipc.sourceType];
                    break;
            }
            ipc.cameraCaptureMode = pickerType.captureMode;
        }
        //ipc.videoMaximumDuration = 30.0f; // 30 seconds
        ipc.videoQuality = pickerType.mediaQuality;
        ipc.delegate = self;
        ipc.allowsEditing = pickerType.allows_edit;
        
        //[self presentModalViewController:ipc animated:YES];
        NSString* modelname = [[UIDevice currentDevice]model];
        CGSize screen_size = [[UIScreen mainScreen] currentMode].size;
        UIUserInterfaceIdiom userinterface = [[UIDevice currentDevice] userInterfaceIdiom];
        
        if (userinterface==UIUserInterfaceIdiomPhone) {
            // iPhone
            if (self.system_version < 6.0f){
                [self presentModalViewController:ipc animated:YES];
            } else {
                if (self.system_version >= 8.0) {
                    ipc.modalPresentationStyle = UIModalPresentationOverCurrentContext;
                }
                [self presentViewController:ipc animated:YES completion:nil];
            }
        } else{
            if (CGSizeEqualToSize(CGSizeMake(640, 960), screen_size) || CGSizeEqualToSize(CGSizeMake(640, 1136), screen_size) || CGSizeEqualToSize(CGSizeMake(1242, 2208), screen_size) || CGSizeEqualToSize(CGSizeMake(2208, 1242), screen_size) || CGSizeEqualToSize(CGSizeMake(1125, 2001), screen_size) || CGSizeEqualToSize(CGSizeMake(2001, 1125), screen_size) || CGSizeEqualToSize(CGSizeMake(750, 1334), screen_size) ||
                CGSizeEqualToSize(CGSizeMake(1334, 750), screen_size)) {
                // iPhone App runing on iPad
                if (self.system_version < 6.0f){
                    [self presentModalViewController:ipc animated:YES];
                } else {
                    if (self.system_version >= 8.0) {
                        ipc.modalPresentationStyle = UIModalPresentationOverCurrentContext;
                    }
                    [self presentViewController:ipc animated:YES completion:nil];
                }
            } else {
                UIInterfaceOrientation orientation = [UIApplication sharedApplication].statusBarOrientation;
                if (ipc != nil)
                {
                    if (self.system_version < 8.3f){
                        switch (orientation) {
                            case UIInterfaceOrientationPortrait:
                            {
                                [ipc setCameraViewTransform:CGAffineTransformMake(1, 0, 0, 1, 0, 0)];
                                break;
                            }
                            case UIInterfaceOrientationPortraitUpsideDown:
                            {
                                [ipc setCameraViewTransform:CGAffineTransformMake(-1, 0, 0, -1, 0, 0)];
                                break;
                            }
                            case UIInterfaceOrientationLandscapeLeft:
                            {
                                [ipc setCameraViewTransform:CGAffineTransformMake(0, 1, -1, 0, 0, 0)];
                                break;
                            }
                            case UIInterfaceOrientationLandscapeRight:
                            {
                                [ipc setCameraViewTransform:CGAffineTransformMake(0, -1, 1, 0, 0, 0)];
                                break;
                            }
                            default:
                                break;
                        }
                    }
                }
                // iPad
                CGRect rect = [[UIScreen mainScreen] bounds];
                UIPopoverController *popover = [[UIPopoverController alloc] initWithContentViewController:ipc];
                popoverController = popover;
                popoverController.popoverContentSize = CGSizeMake(rect.size.width, rect.size.height);
                [popoverController presentPopoverFromRect:CGRectMake(0, 0, 0, 0) inView:self.view permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
            }
        }
        [ipc release];
    }
    else {
        UIAlertView* alert = [[UIAlertView alloc]initWithTitle:nil message:@"请打开相机访问权限!" delegate:nil cancelButtonTitle:@"关闭" otherButtonTitles:nil];
        [alert show];
        [alert release];
    }
}
- (void) closeMedia
{
    //do something
}

- (void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info
{
    BOOL success;
    NSError* error;
    NSFileManager* fileManager = [NSFileManager defaultManager];
    NSString* mediaType = [info objectForKey:UIImagePickerControllerMediaType];
    
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [paths objectAtIndex:0];
    
    if ([mediaType isEqualToString:@"public.image"]){
        UIImage* image = nil;
        if(pickerType.allows_edit){
            image = [info objectForKey:@"UIImagePickerControllerEditedImage"];
        }
        else{
            image = [info objectForKey:@"UIImagePickerControllerOriginalImage"];
        }
        
//        NSString* imageFile = [documentsDirectory stringByAppendingPathComponent:@"temp.jpg"];
//        
//        success = [fileManager fileExistsAtPath:imageFile];
//        if(success) {
//            success = [fileManager removeItemAtPath:imageFile error:&error];
//        }
//        success = [UIImageJPEGRepresentation(image,0) writeToFile:imageFile atomically:YES];
//        std::string jpgFile = string([imageFile cStringUsingEncoding:(NSUTF8StringEncoding)]);
        
        //不压缩的图片大小大约22M，容易导致程序崩溃
        CGSize screen_size = [[UIScreen mainScreen] currentMode].size;
        float scale = image.size.width / screen_size.width;
        if (scale != 0)
        {
            screen_size.width = image.size.width / scale;
            screen_size.height = image.size.height / scale;
        } else
            screen_size = image.size;
        UIImage* temp_image = [self scaleFromImage:image toSize:screen_size];
        NSData* imageData = UIImageJPEGRepresentation(temp_image, 1);
        MGData* data = MGData::dataWithBytes([imageData bytes], [imageData length]);
        std::vector<MGObject*> objects;
        objects.push_back(data);
        std::vector<MGObject*> keys;
        MGString *key = MGString::initWithSTLString("MGMediaPickerOriginalImageData");
        keys.push_back(key);
        MGDictionary* dictionary = MGDictionary::dictionaryWithObjectsForKeys(objects, keys);
        MGMediaPicker::didFinishPickingImageWithInfo(dictionary);
        //[imageData release];
        //[temp_image release];
        delete_and_set_null(data);
        delete_and_set_null(key);
        delete_and_set_null(dictionary);
    }
    else if([mediaType isEqualToString:@"public.movie"]){
        NSURL* videoURL = [info objectForKey:UIImagePickerControllerMediaURL];
        NSData* videoData = [NSData dataWithContentsOfURL:videoURL];
        
        NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
        [dateFormatter setDateFormat:@"yyyyMMdd_HHmmss.mov"];
        NSString *currentDateStr = [dateFormatter stringFromDate:[NSDate date]];
        NSString* videoFile = [documentsDirectory stringByAppendingPathComponent:currentDateStr];
        [dateFormatter release];
        
        success = [fileManager fileExistsAtPath:videoFile];
        if(success) {
            success = [fileManager removeItemAtPath:videoFile error:&error];
        }
        success = [videoData writeToFile:videoFile atomically:YES];
        
        std::vector<MGObject*> objects;
        MGString *object = MGString::initWithSTLString([videoFile cStringUsingEncoding:NSUTF8StringEncoding]);
        objects.push_back(object);
        std::vector<MGObject*> keys;
        MGString *key = MGString::initWithSTLString("MGMediaPickerOriginalVideoFile");
        keys.push_back(key);
        MGDictionary* dictionary = MGDictionary::dictionaryWithObjectsForKeys(objects, keys);
        MGMediaPicker::didFinishPickingVideoWithInfo(dictionary);
        delete_and_set_null(object);
        delete_and_set_null(key);
        delete_and_set_null(dictionary);
    }
    CGRect rect = self.view.bounds;
    
    if (self.system_version < 6.0f){
        [picker dismissModalViewControllerAnimated:YES];
    } else {
        if (self.system_version >= 8.0) {
            picker.modalPresentationStyle = UIModalPresentationOverCurrentContext;
        }
        [picker dismissViewControllerAnimated:YES completion:nil];
    }
    
    if (self.system_version < 6.0f){
        if (self.view.bounds.size.height != rect.size.height) {
            float d = (self.view.bounds.size.height - rect.size.height) / 2;
            self.view.bounds = rect;
            [self.view setTransform:(CGAffineTransformMakeTranslation(0, d))];
        }
    }
    
    //release popoverController
    if (popoverController!=nil) {
        // iPad
        [popoverController dismissPopoverAnimated:YES];
        [popoverController release];
        popoverController = nil;
    }
}

- (void)imagePickerControllerDidCancel:(UIImagePickerController *)picker 
{
    CGRect rect = self.view.bounds;
    
    if (self.system_version < 6.0f){
        [picker dismissModalViewControllerAnimated:YES];
    } else {
        if (self.system_version >= 8.0) {
            picker.modalPresentationStyle = UIModalPresentationOverCurrentContext;
        }
        [picker dismissViewControllerAnimated:YES completion:nil];
    }
    
    if (self.system_version < 6.0f){
        if (self.view.bounds.size.height != rect.size.height) {
            float d = (self.view.bounds.size.height - rect.size.height) / 2;
            self.view.bounds = rect;
            [self.view setTransform:(CGAffineTransformMakeTranslation(0, d))];
        }
    }


    if (pickerType.captureMode == UIImagePickerControllerCameraCaptureModePhoto) {
        MGMediaPicker::didCancelPickingImageWithInfo(NULL);
    } else if (pickerType.captureMode == UIImagePickerControllerCameraCaptureModeVideo) {
        MGMediaPicker::didCancelPickingVideoWithInfo(NULL);
    }
    
    if (popoverController!=nil) {
        // iPad
        [popoverController dismissPopoverAnimated:YES];
        [popoverController release];
        popoverController = nil;
    }
}

- (unsigned char *)getimageData:(UIImage *) image {
    CGImageRef imageref = [image CGImage];
    CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceRGB();
    //创建临时上下文
    size_t width = CGImageGetWidth(imageref);
    size_t height = CGImageGetHeight(imageref);
    size_t bytesPerPixel = 4;
    size_t bytesPerRow = bytesPerPixel*width;
    int bitsPerComponent = 8;
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    unsigned char* imagedata = (unsigned char *)malloc(width * height * bytesPerPixel);
    CGContextRef cgcnt = CGBitmapContextCreate(imagedata,width,height,bitsPerComponent,bytesPerRow,colorSpace,kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
    CGRect therect = CGRectMake(0,0,width,height);
    CGContextDrawImage(cgcnt,therect,imageref);
    
    //imagedata = (unsigned char *)CGBitmapContextGetData(cgcnt);
    CGColorSpaceRelease(colorspace);
    CGContextRelease(cgcnt);
    return imagedata;
    
    
    /*        
     // 1. CGImageRef imageRef = image.CGImage;
     // 2. 获取CGImage的data provider
     CGDataProviderRef dataProvider = CGImageGetDataProvider(imageRef);
     // 3. 获取data provider中的data对象的拷贝
     CFDataRef dataRef = CGDataProviderCopyData(dataProvider); 
     // 4. 获取data对象的字节指针，该指针是一个8位字节数组，并且是只读的
     UInt8 * data = (UInt8 *) CFDataGetBytePtr(dataRef); 
     */ 
}

- (UIImage *)scaleFromImage:(UIImage *) image toSize: (CGSize) size
{
    UIGraphicsBeginImageContext(size);
    [image drawInRect:CGRectMake(0, 0, size.width, size.height)];
    UIImage* newImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return newImage;
}

#pragma mark - Audio Play And Record -
- (void) initAudio:(z11::MGAudioPlayer *)_player
{
    if(_player == NULL)
        return;
    AudioMIter iter = audio_manager.find(_player->getAudioPlayerID());
    if(iter == audio_manager.end())
    {
        NSString *file_path = [NSString stringWithCString:(_player->getAudioFilePath()).c_str() encoding:[NSString defaultCStringEncoding]];
        NSURL *fileURL = [NSURL fileURLWithPath:file_path];
        
        AVAudioPlayer* player = [[AVAudioPlayer alloc] initWithContentsOfURL:fileURL error:nil];
        [player setDelegate:self];
        audio_manager[_player->getAudioPlayerID()] = player;
    }
}
- (void) getAudioDuration:(z11::MGAudioPlayer *)_player
{
    if(_player == NULL)
        return;
    AudioMIter iter = audio_manager.find(_player->getAudioPlayerID());
    if(iter == audio_manager.end())
    {
        NSString *file_path = [NSString stringWithCString:(_player->getAudioFilePath()).c_str() encoding:[NSString defaultCStringEncoding]];
        NSURL *fileURL = [NSURL fileURLWithPath:file_path];
        
        AVAudioPlayer* player = [[AVAudioPlayer alloc] initWithContentsOfURL:fileURL error:nil];
        _player->setDuration(player.duration*1000);
        auto it = MGAudioPlayer::audio_manager.find(_player->getAudioPlayerID());
        if(it!=MGAudioPlayer::audio_manager.end())
        {
            it->second->setDuration(player.duration*1000);
        }
        [player release];
    }
    else
    {
        auto it = MGAudioPlayer::audio_manager.find(_player->getAudioPlayerID());
        if(it!=MGAudioPlayer::audio_manager.end())
        {
            it->second->setDuration(iter->second.duration*1000);
        }
    }
}
- (void) uninitAudio:(z11::MGAudioPlayer *)_player
{
    if(_player == NULL)
        return;
    AudioMIter iter = audio_manager.find(_player->getAudioPlayerID());
    if(iter != audio_manager.end())
    {
        [iter->second release];
        audio_manager.erase(iter);
    }
}

- (void) playAudio:(z11::MGAudioPlayer *)_player
{
    if(_player == NULL)
        return;
    AudioMIter iter = audio_manager.find(_player->getAudioPlayerID());
    if(iter != audio_manager.end())
    {
        [iter->second play];
    }
}

- (void) pauseAudio:(z11::MGAudioPlayer *)_player
{
    if(_player == NULL)
        return;
    AudioMIter iter = audio_manager.find(_player->getAudioPlayerID());
    if(iter != audio_manager.end())
    {
        [iter->second pause];
    }
}

- (void) stopAudio:(z11::MGAudioPlayer *)_player
{
    if(_player == NULL)
        return;
    AudioMIter iter = audio_manager.find(_player->getAudioPlayerID());
    if(iter != audio_manager.end())
    {
        [iter->second stop];
    }
}

- (void) prepareAudio:(z11::MGAudioPlayer *)_player
{
    if(_player == NULL)
        return;
    AudioMIter iter = audio_manager.find(_player->getAudioPlayerID());
    if(iter != audio_manager.end())
    {
        iter->second.pan = _player->pan;
        iter->second.volume = _player->volume;
        iter->second.rate = _player->rate;
        iter->second.numberOfLoops = _player->number_of_loops;
        [iter->second prepareToPlay];
    }
}

- (void)audioPlayerDidFinishPlaying:(AVAudioPlayer *)player successfully:(BOOL)flag
{
//    string url = string([[[player url] path] cStringUsingEncoding:(NSUTF8StringEncoding)]);
    if(player == nil)
        return;
    string player_id;
    for(AudioMIter iter = audio_manager.begin(); iter != audio_manager.end(); ++iter)
    {
        if (iter->second == player) {
            player_id = iter->first;
            break;
        }
    }
    MGAudioPlayer::audioPlayerDidFinishPlaying(player_id);
}

- (void) startAudioRecord:(const std::string &)_url
{
    if (audio_recorder != nil && [audio_recorder isRecording]) {
        return;
    }
    
    AVAudioSession *audio_session = [AVAudioSession sharedInstance];
    NSError **sessionError = nil;
//    if(audio_session == nil) {
//        NSLog(@"Error creating AVAudioSession: %@", [sessionError description]);
//    } else {
//        [audio_session setActive:YES error:&sessionError];
//        [audio_session setCategory:AVAudioSessionCategoryRecord error:&sessionError];
//    }
    
    if ([audio_session respondsToSelector:@selector(requestRecordPermission:)]) {
        [audio_session performSelector:@selector(requestRecordPermission:) withObject:^(BOOL granted) {
            if (granted) {
                [audio_session setActive:YES error:sessionError];
                [audio_session setCategory:AVAudioSessionCategoryRecord error:sessionError];
            }
            else {
                dispatch_async(dispatch_get_main_queue(), ^{
                    [[[[UIAlertView alloc] initWithTitle:@"访问麦克风失败"
                                                 message:@"本应用需要访问麦克风权限，请前往“设置/隐私/麦克风”，将本应用的麦克风权限打开"
                                                delegate:nil
                                       cancelButtonTitle:@"确定"
                                       otherButtonTitles:nil] autorelease] show];
                });
                return;
            }
        }];
    }
    
    
    NSDictionary *recordSettings =
    [[NSDictionary alloc] initWithObjectsAndKeys:
     [NSNumber numberWithFloat: 44100.0], AVSampleRateKey,
     [NSNumber numberWithInt: kAudioFormatLinearPCM], AVFormatIDKey,
     [NSNumber numberWithInt:16],AVLinearPCMBitDepthKey,
     [NSNumber numberWithInt: 2], AVNumberOfChannelsKey,
     [NSNumber numberWithBool:NO],AVLinearPCMIsBigEndianKey,
     [NSNumber numberWithBool:NO],AVLinearPCMIsFloatKey,nil];  
    
    NSString *record_path = [NSString stringWithCString: _url.c_str()   encoding:NSUTF8StringEncoding];
    NSURL *nsurl = [[NSURL alloc] initFileURLWithPath: record_path];
    audio_recorder = [[AVAudioRecorder alloc] initWithURL:nsurl settings:recordSettings error:sessionError];
    [recordSettings release];
    [nsurl release];
    [audio_recorder prepareToRecord];
    audio_recorder.meteringEnabled = YES;
    [audio_recorder record];
}
- (void) stopAudioRecord
{
    if (audio_recorder != nil && [audio_recorder isRecording]) {
        [audio_recorder stop];
        audio_recorder = nil;
        
        AVAudioSession *audio_session = [AVAudioSession sharedInstance];
        NSError **sessionError = nil;
        
        if ([audio_session respondsToSelector:@selector(requestRecordPermission:)]) {
            [audio_session performSelector:@selector(requestRecordPermission:) withObject:^(BOOL granted) {
                if (granted) {
                    [audio_session setActive:YES error:sessionError];
                    [audio_session setCategory:AVAudioSessionCategoryPlayback error:sessionError];
                }
                else {
                    dispatch_async(dispatch_get_main_queue(), ^{
                        [[[[UIAlertView alloc] initWithTitle:@"访问麦克风失败"
                                                     message:@"本应用需要访问麦克风权限，请前往“设置/隐私/麦克风”，将本应用的麦克风权限打开"
                                                    delegate:nil
                                           cancelButtonTitle:@"确定"
                                           otherButtonTitles:nil] autorelease] show];
                    });
                }
            }];
        }
    }
}

- (void) startUpdateMeters
{
    if (audio_recorder != nil && [audio_recorder isRecording]) {
        [audio_recorder updateMeters] ;
        float peakPowerforChannel = pow(10, (0.05*[audio_recorder peakPowerForChannel:0]));
        MGMediaRecorder::setChannelValue(peakPowerforChannel);
    }
}
#pragma mark - Remote Launch App

- (void) launchAppWithURL:(string)_url
{
    NSString* str = [NSString stringWithCString: _url.c_str()   encoding:NSUTF8StringEncoding];
    NSURL* nsurl = [[NSURL alloc]initWithString:str];
    BOOL ret = [[UIApplication sharedApplication] openURL:nsurl];
    if (ret == NO) {
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:nil
                                                        message:@"打开失败!"
                                                       delegate:nil
                                              cancelButtonTitle:@"关闭"
                                              otherButtonTitles:nil];
        
        [alert show];
        [alert release];
    }
}

#pragma mark - Open File

- (void) openFileWithURL:(string)_url
{
    CGRect real_rect = CGRectMake(0, 0, MGDevice::getScreenWidth(), MGDevice::getScreenHeight());
    if (is_retina == 1) {
        real_rect = CGRectMake(real_rect.origin.x, real_rect.origin.y, real_rect.size.width / 2, real_rect.size.height / 2);
    } else if (is_retina == 2)  {
        real_rect = CGRectMake(real_rect.origin.x, real_rect.origin.y, real_rect.size.width / 3, real_rect.size.height / 3);
    } else {
        real_rect = CGRectMake(real_rect.origin.x, real_rect.origin.y, real_rect.size.width, real_rect.size.height);
    }
    
    CGRect gl_view_frame = self.view.frame;
    
    real_rect.origin.x += gl_view_frame.origin.x;
    real_rect.origin.y += gl_view_frame.origin.y;
    
    if (self.hidden_status_bar == NO && self.blend_status_bar == NO && (self.system_version < 5.0 || self.system_version >= 7.0)) {
        if (UIInterfaceOrientationIsLandscape([UIApplication sharedApplication].statusBarOrientation) && self.system_version < 8.0)
        {
            real_rect.origin.y += [[UIApplication sharedApplication] statusBarFrame].size.width;
        } else {
            real_rect.origin.y += [[UIApplication sharedApplication] statusBarFrame].size.height;
        }
    }
    
    if(web_view == nil)
        web_view = [[CustomWebView alloc]initWithFrame:real_rect];
    [web_view setFrame:real_rect];
    
    if (self.system_version >= 4.0)
        [self view].window.rootViewController = self;
    [[self view] addSubview:web_view];
    
    web_view.alpha = 1;
    [web_view scrollView].bounces = YES;
    [web_view scrollView].scrollEnabled = YES;
    web_view.scalesPageToFit = YES;
    [web_view setDelegate:self];
    //    [[self view]resignFirstResponder];
    //    BOOL can = [web_view canBecomeFirstResponder];
    //    if(can)
    [self resignFirstResponder];
//    UIButton* btn = [[UIButton alloc]initWithFrame:CGRectMake(0, 0, 80, 50)];
//    [btn setTitle:@"返回" forState:UIControlStateNormal];
//    [btn setAccessibilityTraits:(UIAccessibilityTraitButton)];
    // 用+号按钮并且旋转45度来模仿关闭按钮 //
    UIButton* btn = [UIButton buttonWithType:UIButtonTypeContactAdd];
    //[btn setTitle:@"X" forState:UIControlStateNormal];
    [btn setFrame:CGRectMake([web_view frame].size.width - 36, 0, 36, 36)];
    [btn setTransform:CGAffineTransformMakeRotation(M_PI_4)];
    [btn setAutoresizingMask:(UIViewAutoresizingFlexibleLeftMargin | UIViewAutoresizingFlexibleBottomMargin)];
    [web_view setAutoresizesSubviews:YES];
    [web_view addSubview:btn];
    [btn addTarget:self action:@selector(closeWebView) forControlEvents:UIControlEventTouchUpInside];
    NSString* str = [NSString stringWithCString: _url.c_str()   encoding:NSUTF8StringEncoding];
    //NSURL* nsurl = [[NSURL alloc]initWithString:str];
    NSURL* nsurl = [NSURL fileURLWithPath:str];
    NSURLRequest* request = [[NSURLRequest alloc]initWithURL:nsurl];
    [web_view loadRequest:request];
    //[nsurl release];
    [request release];
    //[btn release];
    //    [self resignFirstResponder];
    UIInterfaceOrientation orientation = [UIApplication sharedApplication].statusBarOrientation;
    switch (orientation) {
        case UIInterfaceOrientationPortrait:
        {
            [web_view setTransform:CGAffineTransformMake(1, 0, 0, 1, 0, 0)];
            break;
        }
        case UIInterfaceOrientationPortraitUpsideDown:
        {
            [web_view setTransform:CGAffineTransformMake(-1, 0, 0, -1, 0, 0)];
            break;
        }
        case UIInterfaceOrientationLandscapeLeft:
        {
            CGSize screen_size = [[UIScreen mainScreen] currentMode].size;
            if(screen_size.width > screen_size.height)
            {
                CGFloat tmp = screen_size.width;
                screen_size.width = screen_size.height;
                screen_size.height = tmp;
            }
            CGFloat diff_x = (screen_size.width - screen_size.height) * 0.5;
            if (is_retina == 1) {
                diff_x /= 2;
            } else if (is_retina == 2)
            {
                diff_x /= 3;
            }
            CGFloat diff_y = - diff_x;
            [web_view setTransform:CGAffineTransformMake(0, -1, 1, 0, diff_x, diff_y)];
            break;
        }
        case UIInterfaceOrientationLandscapeRight:
        {
            CGSize screen_size = [[UIScreen mainScreen] currentMode].size;
            if(screen_size.width > screen_size.height)
            {
                CGFloat tmp = screen_size.width;
                screen_size.width = screen_size.height;
                screen_size.height = tmp;
            }
            CGFloat diff_x = (screen_size.width - screen_size.height) * 0.5;
            if (is_retina == 1) {
                diff_x /= 2;
            } else if (is_retina == 2)
            {
                diff_x /= 3;
            }
            CGFloat diff_y = - diff_x;
            [web_view setTransform:CGAffineTransformMake(0, 1, -1, 0, diff_x, diff_y)];
            break;
        }
        default:
            break;
    }
}

#pragma mark - Web View

- (void) closeWebView
{
    [web_view resignFirstResponder];
    [web_view removeFromSuperview];
    [web_view setDelegate:nil];
    [web_view stopLoading];
    [web_view release];
    web_view = nil;
    [self.view endEditing:true];
}

- (void) showWebView
{
    if(web_view != nil){
        web_view.alpha = 1;
        UIInterfaceOrientation orientation = [UIApplication sharedApplication].statusBarOrientation;
        switch (orientation) {
            case UIInterfaceOrientationPortrait:
            {
                [web_view setTransform:CGAffineTransformMake(1, 0, 0, 1, 0, 0)];
                break;
            }
            case UIInterfaceOrientationPortraitUpsideDown:
            {
                [web_view setTransform:CGAffineTransformMake(-1, 0, 0, -1, 0, 0)];
                break;
            }
            case UIInterfaceOrientationLandscapeLeft:
            {
                CGSize screen_size = [[UIScreen mainScreen] currentMode].size;
                if(screen_size.width > screen_size.height)
                {
                    CGFloat tmp = screen_size.width;
                    screen_size.width = screen_size.height;
                    screen_size.height = tmp;
                }
                CGFloat diff_x = (screen_size.width - screen_size.height) * 0.5;
                if (is_retina == 1) {
                    diff_x /= 2;
                } else if (is_retina == 2)
                {
                    diff_x /= 3;
                }
                CGFloat diff_y = - diff_x;
                [web_view setTransform:CGAffineTransformMake(0, -1, 1, 0, diff_x, diff_y)];
                break;
            }
            case UIInterfaceOrientationLandscapeRight:
            {
                CGSize screen_size = [[UIScreen mainScreen] currentMode].size;
                if(screen_size.width > screen_size.height)
                {
                    CGFloat tmp = screen_size.width;
                    screen_size.width = screen_size.height;
                    screen_size.height = tmp;
                }
                CGFloat diff_x = (screen_size.width - screen_size.height) * 0.5;
                if (is_retina == 1) {
                    diff_x /= 2;
                } else if (is_retina == 2)
                {
                    diff_x /= 3;
                }
                CGFloat diff_y = - diff_x;
                [web_view setTransform:CGAffineTransformMake(0, 1, -1, 0, diff_x, diff_y)];
                break;
            }
            default:
                break;
        }
    }
}

- (void) canGoBack
{
    if (web_view != nil) {
        BOOL ret = [web_view canGoBack];
        if (ret == YES) {
            MGWebViewManager::setCanGoBack(true);
        } else {
            MGWebViewManager::setCanGoBack(false);
        }
    }
}

- (void) canGoForward
{
    if (web_view != nil) {
        BOOL ret = [web_view canGoForward];
        if (ret == YES) {
            MGWebViewManager::setCanGoForward(true);
        } else {
            MGWebViewManager::setCanGoForward(false);
        }
    }
}

- (void) openURL:(string)_url withRect:(MGRect)rect zoomEnabled:(bool)zoom panEnabled:(bool)pan  isPost:(bool)is_post postBody:(string)post_body
{
    CGRect real_rect;
    if (is_retina == 1) {
        real_rect = CGRectMake(rect.x/2, rect.y/2, rect.width / 2, rect.height / 2);
    }else if (is_retina == 2) {
        real_rect = CGRectMake(rect.x/3, rect.y/3, rect.width / 3, rect.height / 3);
    }else{
        real_rect = CGRectMake(rect.x, rect.y, rect.width, rect.height);
    }
    
    CGRect gl_view_frame = self.view.frame;
    
    real_rect.origin.x += gl_view_frame.origin.x;
    if(self.system_version<6.0||self.system_version>=7.0||self.system_version!=5.0){
        real_rect.origin.y += gl_view_frame.origin.y;
    }
    if (self.hidden_status_bar == NO && self.blend_status_bar == NO && (self.system_version < 6.0 || self.system_version >= 7.0||self.system_version!=5.0)) {
        if (UIInterfaceOrientationIsLandscape([UIApplication sharedApplication].statusBarOrientation) && self.system_version < 8.0)
        {
            real_rect.origin.y += [[UIApplication sharedApplication] statusBarFrame].size.width;
        } else {
            real_rect.origin.y += [[UIApplication sharedApplication] statusBarFrame].size.height;
        }
    }

    if(web_view == nil)
        web_view = [[CustomWebView alloc]initWithFrame:real_rect];
    [web_view setFrame:real_rect];
    
    if (self.system_version >= 4.0)
        [self view].window.rootViewController = self;
    [[self view] addSubview:web_view];
    
    web_view.alpha = 0;
    [web_view scrollView].bounces = NO;
    if (zoom) {
        [web_view scrollView].multipleTouchEnabled = true;
    }else{
        [web_view scrollView].multipleTouchEnabled = false;
    }
    if (pan) {
        [web_view scrollView].scrollEnabled = YES;
    }else{
        [web_view scrollView].scrollEnabled = NO;
    }
    web_view.scalesPageToFit = YES;
//    web_view.allowsInlineMediaPlayback = YES;
//    web_view.mediaPlaybackRequiresUserAction = YES;
//    web_view.mediaPlaybackAllowsAirPlay = YES;
//    web_view.suppressesIncrementalRendering = YES;
    [web_view setDelegate:self];
//    [[self view]resignFirstResponder];
//    BOOL can = [web_view canBecomeFirstResponder];
//    if(can)
    [self resignFirstResponder];
//    UIButton* btn = [[UIButton alloc]initWithFrame:CGRectMake(0, 0, 80, 50)];
//    [btn setTitle:@"返回" forState:UIControlStateNormal];
//    [btn setAccessibilityTraits:(UIAccessibilityTraitButton)];
//    [web_view addSubview:btn];
//    [btn addTarget:self action:@selector(closeWebView:) forControlEvents:UIControlEventTouchUpInside];
    NSString* str = [NSString stringWithCString: _url.c_str()   encoding:NSUTF8StringEncoding];
    str = [str stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
    NSURL* nsurl = [[NSURL alloc]initWithString:str];
    //[[UIApplication sharedApplication] openURL:nsurl];
    NSMutableURLRequest* request = [[NSMutableURLRequest alloc]initWithURL:nsurl];
    request.cachePolicy = NSURLRequestReloadIgnoringLocalCacheData;
    if(is_post){
        if(!post_body.empty()){
            NSString* body = [NSString stringWithCString: post_body.c_str()   encoding:NSUTF8StringEncoding];
            
            [request setHTTPMethod:@"POST"];
            [request setHTTPBody:[body dataUsingEncoding:NSUTF8StringEncoding]];
        }
    }
    [web_view loadRequest:request];
    [nsurl release];
    [request release];
//    [btn release];
//    [self resignFirstResponder];
}

- (BOOL)webView:(UIWebView *)webView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType
{
    BOOL ret = NO;
    if(request){
        NSURL *url = [request URL];
        if(url){
            string req_str = string([[[request URL] absoluteString] cStringUsingEncoding:(NSUTF8StringEncoding)]);
            ret = MGWebViewManager::shouldStartLoadWithRequest(req_str);
        }
    }
    return ret;
}
- (void)webViewDidStartLoad:(UIWebView *)webView
{
    MGWebViewManager::didStartLoad();
}
- (void)webViewDidFinishLoad:(UIWebView *)webView
{
    NSString *title = [webView stringByEvaluatingJavaScriptFromString:@"document.title"];
    string title_str = string([title cStringUsingEncoding:(NSUTF8StringEncoding)]);
    MGWebViewManager::didReceivedTitle(title_str);
    MGWebViewManager::didFinishLoad();
}
- (void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error
{
    string err_str = string([[error description] cStringUsingEncoding:(NSUTF8StringEncoding)]);
    int err_code = [error code];
    MGWebViewManager::didFailLoadWithError(err_code, err_str);
}

#pragma mark - Timer

- (void) validateTimer:(z11::MGTimerObj *)_timer
{
    TimerMIter it = timer_manager.find(_timer->getTag());
    if(it != timer_manager.end())
    {
        [it->second invalidate];
        timer_manager.erase(it);
    }
    CustomTimer* c_timer = [CustomTimer alloc];
    c_timer.tag = _timer->getTag();
    NSTimer* timer = [NSTimer timerWithTimeInterval:_timer->getInterval().InSecondsF() target:self selector:@selector(handleTimer:) userInfo:c_timer repeats:_timer->getRepeatOrNot()];
    timer_manager[_timer->getTag()] = timer;
    [[NSRunLoop currentRunLoop] addTimer:timer forMode:NSDefaultRunLoopMode];
    [c_timer release];
}
- (void) invalidateTimer:(z11::MGTimerObj *)_timer
{
    TimerMIter it = timer_manager.find(_timer->getTag());
    if(it != timer_manager.end())
    {
        [it->second invalidate];
        timer_manager.erase(it);
    }
}

- (void) handleTimer: (NSTimer *) timer
{
    // 在这里进行处理 //
    CustomTimer* c_timer = (CustomTimer*)[timer userInfo];
    if(!MGTimer::fireTimerWithTag(c_timer.tag))
    {
        TimerMIter it = timer_manager.find(c_timer.tag);
        if(it != timer_manager.end())
        {
            timer_manager.erase(it);
        }
    }
}

#pragma mark - Movie Player -

- (void) playMovie:(MGVideoPlayer*)_player
{
    if (movie_player != nil) {
        // 定义视频横屏播放 //
        if (self.system_version < 6.0) {
            UIInterfaceOrientation orientation = [UIApplication sharedApplication].statusBarOrientation;
            switch (orientation) {
                case UIInterfaceOrientationPortrait:
                    [movie_player.view setTransform:CGAffineTransformMakeRotation(0)];
                    break;
                case UIInterfaceOrientationPortraitUpsideDown:
                    [movie_player.view setTransform:CGAffineTransformMakeRotation(M_PI)];
                    break;
                case UIInterfaceOrientationLandscapeLeft:
                    [movie_player.view setTransform:CGAffineTransformMakeRotation(-M_PI_2)];
                    break;
                case UIInterfaceOrientationLandscapeRight:
                    [movie_player.view setTransform:CGAffineTransformMakeRotation(M_PI_2)];
                    break;
                default:
                    break;
            }
        }
        switch (_player->scaling_mode) {
            case MGMovieScalingModeNone:
                movie_player.moviePlayer.scalingMode = MPMovieScalingModeNone;
                break;
            case MGMovieScalingModeAspectFit:
                movie_player.moviePlayer.scalingMode = MPMovieScalingModeAspectFit;
                break;
            case MGMovieScalingModeAspectFill:
                movie_player.moviePlayer.scalingMode = MPMovieScalingModeAspectFill;
                break;
            case MGMovieScalingModeFill:
                movie_player.moviePlayer.scalingMode = MPMovieScalingModeFill;
                break;
            default:
                break;
        }
        switch (_player->control_mode) {
            case MGMovieControlModeDefault:
                movie_player.moviePlayer.controlStyle = MPMovieControlStyleDefault;
                break;
            case MGMovieControlModeNone:
                movie_player.moviePlayer.controlStyle = MPMovieControlStyleNone;
                break;
            case MGMovieControlModeFullscreen:
                movie_player.moviePlayer.controlStyle = MPMovieControlStyleFullscreen;
                break;
            default:
                movie_player.moviePlayer.controlStyle = MPMovieControlStyleDefault;
                break;
        }
        [movie_player.view setFrame:self.view.frame];
        [self.view.superview addSubview:movie_player.view];
        [movie_player.moviePlayer play];
        
    }
}

- (void) stopCurrentMovie
{
    if (movie_player != nil) {
        movie_player.moviePlayer.initialPlaybackTime = -1;
        if (movie_player.moviePlayer.playbackState == MPMoviePlaybackStateStopped) {
            MovieMIter it = movie_player_manager.find(movie_player.moviePlayer);
            if (it != movie_player_manager.end()) {
                movie_player_manager.erase(it);
                [self releaseMoviePlayer:movie_player];
            } else {
                [movie_player release];
            }
            movie_player = nil;
        } else {
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= 30200 // 3.2以上的需要先暂停再停止 //
            [movie_player.moviePlayer pause];
#endif
            [movie_player.moviePlayer stop];
        }
    }
}

- (void) initMovie:(MGVideoPlayer*)_player
{
    if (movie_player != nil) {
        [self stopCurrentMovie];
    }
    
    NSString* str = [NSString stringWithCString: _player->video_file_path.c_str()   encoding:NSUTF8StringEncoding];
    NSURL* nsurl = [NSURL fileURLWithPath:str];
    
    movie_player = [[CustomMPMoviePlayerViewController alloc] initWithContentURL:nsurl];
    movie_player.movie_url = _player->video_file_path;
    movie_player_manager[movie_player.moviePlayer] = movie_player;
}

- (void) moviePlayBackDidFinish:(NSNotification *)notification
{
    MPMoviePlayerController* player = [notification object];
    MovieMIter it = movie_player_manager.find(player);
    if (it != movie_player_manager.end()) {
        CustomMPMoviePlayerViewController* customer_player = it->second;
        if (movie_player == customer_player) {
            movie_player = nil;
        }
        NSNumber* reason = [[notification userInfo] objectForKey:MPMoviePlayerPlaybackDidFinishReasonUserInfoKey];
        switch ([reason intValue]) {
            case MPMovieFinishReasonPlaybackEnded:
                NSLog(@"playbackFinished. Reason: Playback Ended");
                MGVideoPlayer::videoPlayerDidFinishPlaying(customer_player.movie_url);
                break;
            case MPMovieFinishReasonPlaybackError:
                NSLog(@"playbackFinished. Reason: Playback Error");
                MGVideoPlayer::videoPlayerDidFinishPlaying(customer_player.movie_url);
                break;
            case MPMovieFinishReasonUserExited:
                NSLog(@"playbackFinished. Reason: User Exited");
                MGVideoPlayer::videoPlayerDidFinishPlayingByUser(customer_player.movie_url);
                break;
            default:
                break;
        }
        movie_player_manager.erase(it);
        [self releaseMoviePlayer:customer_player];
    }
    if (self.system_version >= 9.0) {
        [self setNeedsStatusBarAppearanceUpdate];
    } else {
        [[UIApplication sharedApplication] setStatusBarHidden:self.hidden_status_bar];
    }
}

- (void) releaseMoviePlayer:(CustomMPMoviePlayerViewController*) player
{
    if (player != nil) {
        player.moviePlayer.controlStyle = MPMovieControlStyleDefault;
        [player.view resignFirstResponder];
        [player.view removeFromSuperview];
        [player release];
        player = nil;
    }
}

- (void) continueMovie
{
    if (self.system_version >= 9.0) {
        [self setNeedsStatusBarAppearanceUpdate];
    } else {
        [[UIApplication sharedApplication] setStatusBarHidden:self.hidden_status_bar];
    }
    if (movie_need_continue == NO || need_continue_movie_url == "") {
        return;
    }
    
    NSString* str = [NSString stringWithCString: need_continue_movie_url.c_str()   encoding:NSUTF8StringEncoding];
    need_continue_movie_url = "";
    NSURL* nsurl = [NSURL fileURLWithPath:str];
    movie_player = [[CustomMPMoviePlayerViewController alloc] initWithContentURL:nsurl];
    movie_player_manager[movie_player.moviePlayer] = movie_player;
    if (self.system_version < 6.0)
    {
        UIInterfaceOrientation orientation = [UIApplication sharedApplication].statusBarOrientation;
        switch (orientation) {
            case UIInterfaceOrientationPortrait:
                [movie_player.view setTransform:CGAffineTransformMakeRotation(0)];
                break;
            case UIInterfaceOrientationPortraitUpsideDown:
                [movie_player.view setTransform:CGAffineTransformMakeRotation(M_PI)];
                break;
            case UIInterfaceOrientationLandscapeLeft:
                [movie_player.view setTransform:CGAffineTransformMakeRotation(-M_PI_2)];
                break;
            case UIInterfaceOrientationLandscapeRight:
                [movie_player.view setTransform:CGAffineTransformMakeRotation(M_PI_2)];
                break;
            default:
                break;
        }
    }
    movie_player.moviePlayer.controlStyle = MPMovieControlStyleFullscreen;
    [movie_player.view setFrame:self.view.frame];
    [self.view.superview addSubview:movie_player.view];
    [movie_player.moviePlayer setInitialPlaybackTime:movie_progress];
    [movie_player.moviePlayer play];
    //[movie_player.moviePlayer pause];
    movie_need_continue = NO;
}
- (void) getPreviewImage:(const string &)_url w:(int)width h:(int)height
{
    AVURLAsset *asset = [[AVURLAsset alloc] initWithURL:[NSURL fileURLWithPath:[NSString stringWithCString: _url.c_str() encoding:[NSString defaultCStringEncoding]]] options:nil];
    AVAssetImageGenerator *gen = [[AVAssetImageGenerator alloc] initWithAsset:asset];
    gen.appliesPreferredTrackTransform = YES;
    CMTime time = CMTimeMakeWithSeconds(0.0, 600);
    NSError *error = nil;
    CMTime actualTime;
    CGImageRef image = [gen copyCGImageAtTime:time actualTime:&actualTime error:&error];
    UIImage *thumb = [[UIImage alloc] initWithCGImage:image];
    CGImageRelease(image);
    CGSize post_size;
    post_size.width = width;
    post_size.height = height;
    UIImage* temp_image = [self scaleFromImage:thumb toSize:post_size];
    NSData* imageData = UIImageJPEGRepresentation(temp_image, 0);
    MGData* data = MGData::dataWithBytes([imageData bytes], [imageData length]);
    MGImage *post_img = MGImage::imageWithData(data);
    delete_and_set_null(data);
    [thumb release];
    thumb = nil;
    MGVideoPlayer::setPreviewImage(post_img);
}

- (void) foreignerSDKApiCall:(string)api
{
    // Do anything you want in AppViewController, not here. //
}

- (void) foreignerSDKApiBack:(string)response
{
    GuiContainer::foreignerSDKApiBack(response);
}
- (void) registRemoteNotification
{
    // Do anything you want in z11AppViewController, not here. //
}
- (void) didGetRemoteNotificationsWithDeviceToken:(NSData*)token
{
    // Do anything you want in z11AppViewController, not here. //
}
- (void) performFetchWithCompletionHandler:(void (^)(UIBackgroundFetchResult result))completionHandler
{
    // Do anything you want in z11AppViewController, not here. //
}
- (BOOL)handleOpenURL:(NSURL *)url
{
    return NO;
}
- (BOOL)openURL:(NSURL *)url
{
    return NO;
}
- (void) performActionForShortcutItem:(UIApplicationShortcutItem *)shortcutItem completionHandler:(void (^)(BOOL))completionHandler
{
    
}
@end
