//
//  iOSAppViewController.h
//  iOSApp
//
//  Created by Wang ZhiPeng on 11-5-25.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//
#import <UIKit/UIKit.h>

#import <OpenGLES/EAGL.h>

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import "MIKit.h"
#import "MGFoundation.h"
#import <map>

#import <CoreLocation/CoreLocation.h>
#import <CoreMotion/CoreMotion.h>
#import <AVFoundation/AVAudioPlayer.h>
#import <MediaPlayer/MediaPlayer.h>
#import <AVFoundation/AVFoundation.h>

#import "EAGLView.h"

using namespace z11;
namespace z11 {
    class MGMediaPickerType;
}
typedef struct _PickerType
{
    UIImagePickerControllerSourceType sourceType;
    UIImagePickerControllerCameraCaptureMode captureMode;
    UIImagePickerControllerQualityType mediaQuality;
    int pickerLimit;
    BOOL allows_edit;
}PickerType;

@interface CustomMPMoviePlayerViewController : MPMoviePlayerViewController{
    string movie_url;
}
@property (nonatomic) string movie_url;
@end

@interface CustomWebView : UIWebView {
    
}

@end

@interface CustomEvent : NSObject {
    MIEvent* event;
}

@property (nonatomic) MIEvent* event;

@end

@interface CustomTimer : NSObject {
}
@property (nonatomic) MGTimerTag tag;
@end

@interface iOSAppViewController : UIViewController <CLLocationManagerDelegate,
UINavigationControllerDelegate, UIImagePickerControllerDelegate, UIWebViewDelegate, AVAudioPlayerDelegate, AVCaptureVideoDataOutputSampleBufferDelegate> {
    IBOutlet EAGLView 				*gl_view;
    CLLocationManager				*location_manager;
    CMMotionManager                 *motion_manager;
    UIPopoverController             *popoverController;
    UIImagePickerController         *image_picker;
    PickerType                      pickerType;
    AVAudioRecorder                 *audio_recorder;
    map<string, AVAudioPlayer*>     audio_manager;
    map<MGTimerTag, NSTimer*>       timer_manager;
    CustomMPMoviePlayerViewController     *movie_player;
    map<MPMoviePlayerController*, CustomMPMoviePlayerViewController*> movie_player_manager;
    string                          need_continue_movie_url;
    BOOL                            movie_need_continue;
    BOOL                            hidden_status_bar;
    BOOL                            blend_status_bar;
    BOOL                            notification_valid;
    NSTimeInterval                  movie_progress;
    NSString                        *input_text;
    AVCaptureSession                *session;
    byte_8* img_addr; // capture img
    CustomWebView	*web_view;
    BOOL 	orientation_portrait;
    BOOL 	orientation_portraitdown;
    BOOL 	orientation_landscapeleft;
    BOOL 	orientation_landscaperight;
    
}
typedef map<string, AVAudioPlayer*>::iterator AudioMIter;
typedef map<MGTimerTag, NSTimer*>::iterator TimerMIter;
typedef map<MPMoviePlayerController*, CustomMPMoviePlayerViewController*>::iterator MovieMIter;
@property (nonatomic) GuiContainer* gui_container;
@property (nonatomic, assign) EAGLView *gl_view;
@property (nonatomic, retain) NSString* input_text;
@property (nonatomic, assign) CustomMPMoviePlayerViewController* movie_player;
@property (nonatomic) BOOL movie_need_continue;
@property (nonatomic) BOOL hidden_status_bar;
@property (nonatomic) BOOL blend_status_bar;
@property (nonatomic) BOOL notification_valid;
@property (nonatomic) NSTimeInterval movie_progress;
@property (nonatomic) string need_continue_movie_url;
@property (nonatomic) float system_version;
@property (nonatomic, assign) NSDictionary *launchOptions;

- (UIImage *)scaleFromImage:(UIImage *) image toSize: (CGSize) size;

+ (int) isRetina;
+ (void) setRetina:(int) retina;
- (void) handleSysCallbackEvent;
- (void) handleEventImmediatelly:(id)_event;
- (void) startLocationService;
- (void) stopLocationService;
- (void) startHeadingService;
- (void) stopHeadingService;
- (void) startMonitoringForRegion:(string)uuid;
- (void) stopMonitoringForRegion:(string)uuid;
- (void) startRangingForRegion:(string)uuid;
- (void) stopRangingForRegion:(string)uuid;
- (void) releaseLocationService;
- (void) startAccelerationService:(float)interval;
- (void) stopAccelerationService;
- (void) startRotationService:(float)interval;
- (void) stopRotationService;
- (void) releaseMotionService;
- (void) openMedia:(MGMediaPickerType *)_pickerType;
- (void) closeMedia;
- (void) initAudio:(MGAudioPlayer*)_player;
- (void) playAudio:(MGAudioPlayer*)_player;
- (void) pauseAudio:(MGAudioPlayer*)_player;
- (void) stopAudio:(MGAudioPlayer*)_player;
- (void) uninitAudio:(MGAudioPlayer*)_player;
- (void) prepareAudio:(MGAudioPlayer*)_player;
- (void) startAudioRecord:(const std::string &)_url;
- (void) stopAudioRecord;
- (void) startUpdateMeters;
- (void) openURL:(string)_url withRect:(MGRect)rect zoomEnabled:(bool)zoom panEnabled:(bool)pan isPost:(bool)is_post postBody:(string)post_body;
- (void) closeWebView;
- (void) showWebView;
- (void) launchAppWithURL:(string)_url;
- (void) openFileWithURL:(string)_url;
- (void) validateTimer:(MGTimerObj*)_timer;
- (void) invalidateTimer:(MGTimerObj*)_timer;
- (void) handleTimer:(NSTimer*)timer;
- (void) keyboardWasShown:(NSNotification*)notif;
- (void) keyboardWasHidden:(NSNotification*)notif;
- (void) setKBType:(MIKeyboardType)_kbType;
- (void) setRKType:(MIReturnKeyType)_rkType;
- (void) initMovie:(MGVideoPlayer*)_player;
- (void) playMovie:(MGVideoPlayer*)_player;
- (void) getPreviewImage:(const string &)_url w:(int)width h:(int)height ;
- (void) moviePlayBackDidFinish:(NSNotification *)notification;
- (void) releaseMoviePlayer:(CustomMPMoviePlayerViewController*) player;
- (void) stopCurrentMovie;
- (void) continueMovie;
- (void) startCameraCapture:(MGCaptureDevicePosition) position;
- (void) stopCameraCapture;
- (void) focusPointOfInterest:(MGCaptureDevicePosition) position WithPoint:(CGPoint)point;
- (void) foreignerSDKApiCall:(string)api;
- (void) foreignerSDKApiBack:(string)response;
- (void) registRemoteNotification;
- (void) didGetRemoteNotificationsWithDeviceToken:(NSData*)token;
- (void) didLaunchWithOptions:(NSDictionary *)launchOptions;
- (void) didReceiveRemoteNotification:(NSDictionary *)userInfo;
- (void) didReceiveRemoteNotification:(NSDictionary *)userInfo fetchCompletionHandler:(void (^)(UIBackgroundFetchResult))completionHandler;
- (void) didRegisterUserNotificationSettings:(UIUserNotificationSettings *)notificationSettings;
- (void) performFetchWithCompletionHandler:(void (^)(UIBackgroundFetchResult result))completionHandler;
- (void) performActionForShortcutItem:(UIApplicationShortcutItem *)shortcutItem completionHandler:(void (^)(BOOL))completionHandler;
- (unsigned char *)getimageData:(UIImage *)image;

- (BOOL)handleOpenURL:(NSURL *)url;
- (BOOL)openURL:(NSURL *)url;

@end
