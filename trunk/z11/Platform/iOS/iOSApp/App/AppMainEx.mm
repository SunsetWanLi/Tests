//
//  AppMainEx.mm
//  iOSApp
//
//  Created by Wang ZhiPeng on 11-6-2.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//
#include "AppMain.h"

#include <UIKit/UIKit.h>

#include "MIKit.h"
#include "iOSApp/z11AppViewController.h"
#include "MGFoundation.h"
#include "System/WebService/MGWebService.h"
//#include <assert.h>

#ifndef USE_DEPTH_BUFFER
#define USE_DEPTH_BUFFER 1
#endif
#ifndef DEGREES_TO_RADIANS
#define DEGREES_TO_RADIANS(__ANGLE) ((__ANGLE) / 180.0 * M_PI)
#endif

//OpenGL/ES//
EAGLContext* CreateBestEAGLContext()
{
    EAGLContext *context;
#ifdef SUPPORT_OPEN_GLES_2
    context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
#else
    context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
#endif
    if (context == nil)
    {
        context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
    }
    return context;
}
void checkGLError(bool visibleCheck)
{
    GLenum error = glGetError();
    switch (error) {
        case GL_INVALID_ENUM:
            NSLog(@"GL Error: Enum argument is out of range");
            break;
        case GL_INVALID_VALUE:
            NSLog(@"GL Error: Numeric value is out of range");
            break;
        case GL_INVALID_OPERATION:
            NSLog(@"GL Error: Operation illegal in current state");
            break;
        case GL_STACK_OVERFLOW:
            NSLog(@"GL Error: Command would cause a stack overflow");
            break;
        case GL_STACK_UNDERFLOW:
            NSLog(@"GL Error: Command would cause a stack underflow");
            break;
        case GL_OUT_OF_MEMORY:
            NSLog(@"GL Error: Not enough memory to execute command");
            break;
        case GL_NO_ERROR:
            if (visibleCheck) {
                NSLog(@"No GL Error");
            }
            break;
        default:
            NSLog(@"Unknown GL Error");
            break;
    }
}

//void gluPerspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar)
//{	
//	GLfloat xmin, xmax, ymin, ymax;
//    
//	ymax = zNear * (GLfloat)tanf(fovy * (float)M_PI / 360);
//	ymin = -ymax;
//	xmin = ymin * aspect;
//	xmax = ymax * aspect;
//    
//	glFrustumf(xmin, xmax,
//               ymin, ymax,
//               zNear, zFar);	
//}
//
//void gluLookAt(GLfloat eyex, GLfloat eyey, GLfloat eyez,
//               GLfloat centerx, GLfloat centery, GLfloat centerz,
//               GLfloat upx, GLfloat upy, GLfloat upz)
//{
//    GLfloat m[16];
////    GLfloat x[3], y[3], z[3];
////    GLfloat mag;
////	
////    /* Make rotation matrix */
////	
////    /* Z vector */
////    z[0] = eyex - centerx;
////    z[1] = eyey - centery;
////    z[2] = eyez - centerz;
////    mag = (float)sqrtf(z[0] * z[0] + z[1] * z[1] + z[2] * z[2]);
////    if (mag) {
////        z[0] /= mag;
////        z[1] /= mag;
////        z[2] /= mag;
////    }
////	
////    /* Y vector */
////    y[0] = upx;
////    y[1] = upy;
////    y[2] = upz;
////	
////    /* X vector = Y cross Z */
////    x[0] = y[1] * z[2] - y[2] * z[1];
////    x[1] = -y[0] * z[2] + y[2] * z[0];
////    x[2] = y[0] * z[1] - y[1] * z[0];
////	
////    /* Recompute Y = Z cross X */
////    y[0] = z[1] * x[2] - z[2] * x[1];
////    y[1] = -z[0] * x[2] + z[2] * x[0];
////    y[2] = z[0] * x[1] - z[1] * x[0];
////	
////    /* cross product gives area of parallelogram, which is < 1.0 for
////     * non-perpendicular unit-length vectors; so normalize x, y here
////     */
////	
////    mag = (float)sqrtf(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
////    if (mag) {
////        x[0] /= mag;
////        x[1] /= mag;
////        x[2] /= mag;
////    }
////	
////    mag = (float)sqrtf(y[0] * y[0] + y[1] * y[1] + y[2] * y[2]);
////    if (mag) {
////        y[0] /= mag;
////        y[1] /= mag;
////        y[2] /= mag;
////    }
//	
//#define M(row,col)  m[col*4+row]
//    M(0, 0) = 1.0f;//x[0];
//    M(0, 1) = 0.0f;//x[1];
//    M(0, 2) = 0.0f;//x[2];
//    M(0, 3) = 0.0f;
//    M(1, 0) = 0.0f;//y[0];
//    M(1, 1) = -1.0f;//y[1];
//    M(1, 2) = 0.0f;//y[2];
//    M(1, 3) = 0.0f;
//    M(2, 0) = 0.0f;//z[0];
//    M(2, 1) = 0.0f;//z[1];
//    M(2, 2) = 1.0f;//z[2];
//    M(2, 3) = 0.0f;
//    M(3, 0) = 0.0f;
//    M(3, 1) = 0.0f;
//    M(3, 2) = 0.0f;
//    M(3, 3) = 1.0f;
//#undef M
//	
//	glMultMatrixf(m);
//    
//	
//    /* Translate Eye to Origin */
//    glTranslatef(-eyex, -eyey, -eyez);
//}

void AppMain::setProjectionMode()
{
    //glEnable(GL_DEPTH_TEST);
    
    // This give us the size of the iPhone display
    // Can not using device size here, because when running iPhone App on an iPad, it's device size is according to iPad not iPhone. //
    EAGLView * view = ((z11AppViewController*)wnd_handle).gl_view;
    CGSize size = view.bounds.size;
    uint_32 status_bar_height = 0;
    
    if (UIInterfaceOrientationIsLandscape([UIApplication sharedApplication].statusBarOrientation) && ((z11AppViewController*)wnd_handle).system_version < 8.0)
    {
        status_bar_height = [[UIApplication sharedApplication] statusBarFrame].size.width;
    } else {
        status_bar_height = [[UIApplication sharedApplication] statusBarFrame].size.height;
    }
    
    if (((z11AppViewController*)wnd_handle).hidden_status_bar == NO && ((z11AppViewController*)wnd_handle).blend_status_bar == NO && (((z11AppViewController*)wnd_handle).system_version >= 7.0 || (((z11AppViewController*)wnd_handle).system_version < 6.0 && ((z11AppViewController*)wnd_handle).system_version != 5.0))) {
        size.height -= status_bar_height;
    }
    
    if ([z11AppViewController isRetina] == 1)
    {
        size.width *= 2;
        size.height *= 2;
        status_bar_height *= 2;
    } else if ([z11AppViewController isRetina] == 2) {
        size.width *= 3;
        size.height *= 3;
        status_bar_height *= 3;
    }
   
    MGDevice::setScreenWidth(size.width);
    MGDevice::setScreenHeight(size.height);
    if (((z11AppViewController*)wnd_handle).blend_status_bar == YES) {
        MGDevice::setStatusBarHeight(status_bar_height);
    }
    
    size.width=MGDevice::getScreenWidth();
    size.height=MGDevice::getScreenHeight();
    MGLogD("*************************************");
    MGLogD("new size is %f, %f.", size.width, size.height);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (GLfloat)size.width/size.height, 0.0001f, 2500.0f);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt( size.width/2, size.height/2, size.height / (2 * tan(M_PI / 6)),
              size.width/2, size.height/2, 0,
              0.0f, 1.0f, 0.0f);
    glViewport(0, 0, size.width, size.height);
    glScissor(0, 0, size.width, size.height);
    //glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void AppMain::createFrameBuffer()
{
    glGenFramebuffersOES(1, &defaultFramebuffer);
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, defaultFramebuffer);
}
void AppMain::createRenderBuffer()
{
    //////////////
    glGenRenderbuffersOES(1, &colorRenderbuffer);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
    EAGLView * view = ((z11AppViewController*)wnd_handle).gl_view;
    CAEAGLLayer *eaglLayer = (CAEAGLLayer*)view.layer;
    [maintain_context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:eaglLayer];
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, colorRenderbuffer);
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &framebufferWidth);
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &framebufferHeight);
}

void AppMain::createTexture()
{
//    glEnable(GL_TEXTURE_2D);
//    textureBuffer = 0;
//	glGenTextures(1,&textureBuffer);
//	glBindTexture(GL_TEXTURE_2D,textureBuffer);
//    MGBundle* bundle = MGBundle::mainBundle();
//    string file_path = bundle->pathWithFileName("desk.jpg");
//    MGImage* image = MGImage::loadImageFromFile(file_path);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->content_info.width_in_pixel, image->content_info.height_in_pixel, 0, GL_RGB, GL_UNSIGNED_BYTE, image->content_data);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}
//Create a depth or depth/stencil renderbuffer, allocate storage for it, and attach it to the framebuffer’s depth attachment point.
void AppMain::allocateAndAttachDepthBuffer()
{
    if (USE_DEPTH_BUFFER) {
        glGenRenderbuffersOES(1, &depthRenderbuffer);
        glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthRenderbuffer);
        glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, framebufferWidth, framebufferHeight);
        glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthRenderbuffer);
    }
}
//Test the framebuffer for completeness. This test only needs to be performed when the framebuffer’s configuration changes.
void AppMain::testBufferCompleteness()
{
    if(glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES) {
        NSLog(@"failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
    }
}
//--//

bool AppMain::initOpenGLES()
{
    bool ret = false;
    //建立Context//
    maintain_context = CreateBestEAGLContext();
    if (!maintain_context || ![EAGLContext setCurrentContext:maintain_context]) {
        return ret;
    }
    //0 set projection
    setProjectionMode();
    //1 create frame buffer.
    createFrameBuffer();
    //2 create render buffer.
    createRenderBuffer();
    //3 create the texture.
    createTexture();
    //4 allocate and attach depth buffer.
    allocateAndAttachDepthBuffer();
    //5 test buffer completeness.
    testBufferCompleteness();
 
    ret = true;
    return ret;
}

void AppMain::beforeRenderScreen()
{
    [EAGLContext setCurrentContext:maintain_context];
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, defaultFramebuffer);
    
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void AppMain::renderScreen()
{
    
}

bool AppMain::afterRenderScreen()
{
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
    return [maintain_context presentRenderbuffer:GL_RENDERBUFFER_OES];
}

void AppMain::handleSysCallbackEvent(z11::MIEvent *event)
{
    //assert(event != NULL);
    switch (event->_event_type) {
        case BECOME_FIRST_RESPONDER_EVENT:
        {
            z11AppViewController* controller = (z11AppViewController*)wnd_handle;
            if(event->_content != NULL)
            {
                MIKeyboardTypeObj* obj = (MIKeyboardTypeObj*)event->_content;
                MIKeyboardType type = obj->keyboard_type;
                MIReturnKeyType return_type = obj->return_key_type;
                [controller setKBType:type];
                [controller setRKType:return_type];
                [controller.gl_view resignFirstResponder];
            }
            [controller.gl_view becomeFirstResponder];
            break;
        }
        case RESIGN_FIRST_RESPONDER_EVENT:
        {
            z11AppViewController* controller = (z11AppViewController*)wnd_handle;
            [controller.gl_view resignFirstResponder];
            break;
        }
        case SCAN_WIFI_STATUS_EVENT:
        {
            MGNetworkReachability::refreshWifiStatus();
            string ret;
            MGNetworkReachability::receiveWifiStatus(ret);
            break;
        }
        case LIGHT_GRAPHIC_FIRST_FRAME_DRAWED_EVENT:
        {
            z11AppViewController* controller = (z11AppViewController*)wnd_handle;
            if (controller != nil) {
                [controller handleFirstFrameDraw];
            }
            break;
        }
        case APP_LOCK_SCREEN_WHEN_IDLE_EVENT:
        {
            [UIApplication sharedApplication].idleTimerDisabled = NO; //let iphone go to sleep
            break;
        }
        case APP_UNLOCK_SCREEN_WHEN_IDLE_EVENT:
        {
            [UIApplication sharedApplication].idleTimerDisabled = YES; //not let iphone go to sleep
            break;
        }
        case APP_SHOW_SYSTEM_STATUS_BAR_EVENT:
        {
            z11AppViewController* controller = (z11AppViewController*)wnd_handle;
            controller.hidden_status_bar = NO;
            if (controller.system_version >= 9.0) {
                [controller setNeedsStatusBarAppearanceUpdate];
            } else {
                [[UIApplication sharedApplication] setStatusBarHidden:controller.hidden_status_bar];
            }
            break;
        }
        case APP_HIDE_SYSTEM_STATUS_BAR_EVENT:
        {
            z11AppViewController* controller = (z11AppViewController*)wnd_handle;
            controller.hidden_status_bar = YES;
            if (controller.system_version >= 9.0) {
                [controller setNeedsStatusBarAppearanceUpdate];
            } else {
                [[UIApplication sharedApplication] setStatusBarHidden:controller.hidden_status_bar];
            }
            break;
        }
        case START_LOCATION_SERVICE_EVENT:
        {
            z11AppViewController* controller = (z11AppViewController*)wnd_handle;
            [controller startLocationService];
            break;
        }
        case STOP_LOCATION_SERVICE_EVENT:
        {
            z11AppViewController* controller = (z11AppViewController*)wnd_handle;
            [controller stopLocationService];
            break;
        }
        case START_HEADING_SERVICE_EVENT:
        {
            z11AppViewController* controller = (z11AppViewController*)wnd_handle;
            [controller startHeadingService];
            break;
        }
        case STOP_HEADING_SERVICE_EVENT:
        {
            z11AppViewController* controller = (z11AppViewController*)wnd_handle;
            [controller stopHeadingService];
            break;
        }
        case START_MONITORING_FOR_REGION_EVENT:
        {
            if (event->_content != NULL) {
                MGTextContent* content = (MGTextContent*)event->_content;
                string uuid = content->text_content;
                z11AppViewController* controller = (z11AppViewController*)wnd_handle;
                [controller startMonitoringForRegion:uuid];
            }
            break;
        }
        case STOP_MONITORING_FOR_REGION_EVENT:
        {
            if (event->_content != NULL) {
                MGTextContent* content = (MGTextContent*)event->_content;
                string uuid = content->text_content;
                z11AppViewController* controller = (z11AppViewController*)wnd_handle;
                [controller stopMonitoringForRegion:uuid];
            }
            break;
        }
        case START_RANGING_FOR_REGION_EVENT:
        {
            if (event->_content != NULL) {
                MGTextContent* content = (MGTextContent*)event->_content;
                string uuid = content->text_content;
                z11AppViewController* controller = (z11AppViewController*)wnd_handle;
                [controller startRangingForRegion:uuid];
            }
            break;
        }
        case STOP_RANGING_FOR_REGION_EVENT:
        {
            if (event->_content != NULL) {
                MGTextContent* content = (MGTextContent*)event->_content;
                string uuid = content->text_content;
                z11AppViewController* controller = (z11AppViewController*)wnd_handle;
                [controller stopRangingForRegion:uuid];
            }
            break;
        }
        case RELEASE_LOCATION_SERVICE_EVENT:
        {
            z11AppViewController* controller = (z11AppViewController*)wnd_handle;
            [controller releaseLocationService];
            break;
        }
        case START_ACCELERATION_SERVICE_EVENT:
        {
            if (event->_content != NULL) {
                MGIntervalContent* content = (MGIntervalContent*)event->_content;
                z11AppViewController* controller = (z11AppViewController*)wnd_handle;
                [controller startAccelerationService:content->interval];
            } else {
                z11AppViewController* controller = (z11AppViewController*)wnd_handle;
                [controller startAccelerationService:0.0f];
            }
            break;
        }
        case STOP_ACCELERATION_SERVICE_EVENT:
        {
            z11AppViewController* controller = (z11AppViewController*)wnd_handle;
            [controller stopAccelerationService];
            break;
        }
        case START_ROTATION_SERVICE_EVENT:
        {
            if (event->_content != NULL) {
                MGIntervalContent* content = (MGIntervalContent*)event->_content;
                z11AppViewController* controller = (z11AppViewController*)wnd_handle;
                [controller startRotationService:content->interval];
            } else {
                z11AppViewController* controller = (z11AppViewController*)wnd_handle;
                [controller startRotationService:0.0f];
            }
            break;
        }
        case STOP_ROTATION_SERVICE_EVENT:
        {
            z11AppViewController* controller = (z11AppViewController*)wnd_handle;
            [controller stopRotationService];
            break;
        }
        case RELEASE_MOTION_SERVICE_EVENT:
        {
            z11AppViewController* controller = (z11AppViewController*)wnd_handle;
            [controller releaseMotionService];
            break;
        }
        case MEDIA_PICK_EVENT:
        {
            if (event->_content != NULL)
            {
                MGMediaPickerType* type = (MGMediaPickerType*)event->_content;
                z11AppViewController* controller = (z11AppViewController*)wnd_handle;
                [controller openMedia:type];
            }
            break;
        }
        case MEDIA_DROP_EVENT:
        {
            z11AppViewController* controller = (z11AppViewController*)wnd_handle;
            [controller closeMedia];
            break;
        }
        case WEB_SERVICE_BEGIN_EVENT:
        {
            if(event->_content!=NULL)
			{
				MGWebServiceThread *thread=new MGWebServiceThread(static_cast<MGRequestSoapObject *>(event->_content));
				event->_content=NULL; //it's important;
				thread->run();
			}
            break;
        }
        case WEB_SERVICE_END_EVENT:
        {
            if (event->_content != NULL)
            {
                MGWebServiceThread *thread = dynamic_cast<MGWebServiceThread *>(event->_content);
                if (thread != NULL)
                {
                    thread->requestExitAndWait();
                    delete_and_set_null(thread);
                }
                event->_content=NULL;
            }
            break;
        }
        case PLAY_AUDIO_PLAY_EVENT:
        {
            if(event->_content != NULL)
            {
                MGAudioPlayer* player = (MGAudioPlayer*)event->_content;
                z11AppViewController* controller = (z11AppViewController*)wnd_handle;
                [controller playAudio:player];
            }
            break;
        }
        case GET_AUDIO_DURATION_EVENT:
        {
            if(event->_content != NULL)
            {
                MGAudioPlayer* player = (MGAudioPlayer*)event->_content;
                z11AppViewController* controller = (z11AppViewController*)wnd_handle;
                [controller getAudioDuration:player];
            }
            break;
        }
        case PAUSE_AUDIO_PLAY_EVENT:
        {
            if(event->_content != NULL)
            {
                MGAudioPlayer* player = (MGAudioPlayer*)event->_content;
                z11AppViewController* controller = (z11AppViewController*)wnd_handle;
                [controller pauseAudio:player];
            }
            break;
        }
        case STOP_AUDIO_PLAY_EVENT:
        {
            if(event->_content != NULL)
            {
                MGAudioPlayer* player = (MGAudioPlayer*)event->_content;
                z11AppViewController* controller = (z11AppViewController*)wnd_handle;
                [controller stopAudio:player];
            }
            break;
        }
        case PREPARE_AUDIO_PLAY_EVENT:
        {
            if(event->_content != NULL)
            {
                MGAudioPlayer* player = (MGAudioPlayer*)event->_content;
                z11AppViewController* controller = (z11AppViewController*)wnd_handle;
                [controller prepareAudio:player];
            }
            break;
        }
        case INIT_AUDIO_PLAY_EVENT:
        {
            if(event->_content != NULL)
            {
                MGAudioPlayer* player = (MGAudioPlayer*)event->_content;
                z11AppViewController* controller = (z11AppViewController*)wnd_handle;
                [controller initAudio:player];
            }
            break;
        }
        case UNINIT_AUDIO_PLAY_EVENT:
        {
            if(event->_content != NULL)
            {
                MGAudioPlayer* player = (MGAudioPlayer*)event->_content;
                z11AppViewController* controller = (z11AppViewController*)wnd_handle;
                [controller uninitAudio:player];
            }
            break;
        }
        case VALIDATE_A_TIMER_EVENT:
        {
            if(event->_content != NULL)
            {
                MGTimerObj* obj = (MGTimerObj*)event->_content;
                z11AppViewController* controller = (z11AppViewController*)wnd_handle;
                [controller validateTimer:obj];
            }
            break;
        }
        case INVALIDATE_A_TIMER_EVENT:
        {
            if(event->_content != NULL)
            {
                MGTimerObj* obj = (MGTimerObj*)event->_content;
                z11AppViewController* controller = (z11AppViewController*)wnd_handle;
                [controller invalidateTimer:obj];
            }
            break;
        }
        case UIWEBVIEW_OPEN_URL_EVENT:
        {
            if(event->_content != NULL)
            {
                MGWebViewContent* content = (MGWebViewContent*)event->_content;
                string url = content->url_content;
                bool is_post = content->is_post;
                string post_body = content->post_body;
                z11AppViewController* controller = (z11AppViewController*)wnd_handle;
                [controller openURL:(url) withRect:(content->rect) zoomEnabled:(content->zoom_enabled) panEnabled:(content->pan_enabled) isPost:is_post postBody:post_body];
            }
            break;
        }
        case UIWEBVIEW_CAN_BACK_EVENT:
        {
            z11AppViewController* controller = (z11AppViewController*)wnd_handle;
            [controller canGoBack];
            break;
        }
        case UIWEBVIEW_CAN_FORWARD_EVENT:
        {
            z11AppViewController* controller = (z11AppViewController*)wnd_handle;
            [controller canGoForward];
            break;
        }
        case UIWEBVIEW_SHOW_VIEW_EVENT:
        {
            if(event != NULL)
            {
                z11AppViewController* controller = (z11AppViewController*)wnd_handle;
                [controller showWebView];
            }
            break;
        }
        case UIWEBVIEW_CLOSE_VIEW_EVENT:
        {
            if(event != NULL)
            {
                z11AppViewController* controller = (z11AppViewController*)wnd_handle;
                [controller closeWebView];
            }
            break;
        }
        case LAUNCH_APP_WITH_URL_EVENT:
        {
            if(event != NULL)
            {
                MGTextContent* content = (MGTextContent*)event->_content;
                string url = content->text_content;
                z11AppViewController* controller = (z11AppViewController*)wnd_handle;
                [controller launchAppWithURL:url];
            }
            break;
        }
        case OPEN_FILE_WITH_URL_EVENT:
        {
            if(event != NULL)
            {
                MGTextContent* content = (MGTextContent*)event->_content;
                string url = content->text_content;
                z11AppViewController* controller = (z11AppViewController*)wnd_handle;
                [controller openFileWithURL:url];
            }
            break;
        }
        case MEDIA_RECORDER_START_EVENT:
        {
            if (event != NULL) {
                MGTextContent* content = (MGTextContent*)event->_content;
                z11AppViewController* controller = (z11AppViewController*)wnd_handle;
                [controller startAudioRecord:content->text_content];
            }
            break;
        }
		case MEDIA_RECORDER_STOP_EVENT:
        {
            z11AppViewController* controller = (z11AppViewController*)wnd_handle;
            [controller stopAudioRecord];
            break;
        }
        case MEDIA_RECORDER_UPDATE_METERS_EVENT:
        {
            z11AppViewController* controller = (z11AppViewController*)wnd_handle;
            [controller startUpdateMeters];
            break;
        }
        case INIT_VIDEO_PLAY_EVENT:
        {
            if(event != NULL)
            {
                MGVideoPlayer* player = (MGVideoPlayer*)event->_content;
                z11AppViewController* controller = (z11AppViewController*)wnd_handle;
                [controller initMovie:player];
                player->video_file_path = "";
            }
            break;
        }
        case PLAY_VIDEO_PLAY_EVENT:
        {
            if(event != NULL)
            {
                MGVideoPlayer* player = (MGVideoPlayer*)event->_content;
                z11AppViewController* controller = (z11AppViewController*)wnd_handle;
                [controller playMovie:player];
                player->video_file_path = "";
            }
            break;
        }
        case STOP_VIDEO_PLAY_EVENT:
        {
            if(event != NULL)
            {
                z11AppViewController* controller = (z11AppViewController*)wnd_handle;
                [controller stopCurrentMovie];
            }
            break;
        }
        case GET_VIDEO_PREVIEW_EVENT:
            if(event != NULL)
            {
                z11AppViewController* controller = (z11AppViewController*)wnd_handle;
                MGTextContent *obj = (MGTextContent*)event->_content;
                [controller getPreviewImage:(obj->text_content) w:(MGVideoPlayer::preview_width) h:(MGVideoPlayer::preview_height)];
            }
            break;
        case START_CAMERA_CAPTURE_SERVICE_EVENT:
        {
            if(event != NULL)
            {
                MGCaptureDevicePosition pos = MGCaptureDevicePositionUnspecified;
                if (event->_content != NULL)
                {
                    MGCaptureDevicePositionContent* content = dynamic_cast<MGCaptureDevicePositionContent*>(event->_content);
                    if (content != NULL) {
                        pos = content->position;
                    }
                }
                z11AppViewController* controller = (z11AppViewController*)wnd_handle;
                [controller startCameraCapture:pos];
            }
            break;
        }
        case STOP_CAMERA_CAPTURE_SERVICE_EVENT:
        {
            if(event != NULL)
            {
                z11AppViewController* controller = (z11AppViewController*)wnd_handle;
                [controller stopCameraCapture];
            }
            break;
        }
        case ADJUST_CAMERA_CAPTURE_FOCUS_EVENT:
        {
            if(event != NULL)
            {
                CGPoint point;
                MGCaptureDevicePosition pos = MGCaptureDevicePositionUnspecified;
                if (event->_content != NULL)
                {
                    MGCaptureDevicePositionContent* content = dynamic_cast<MGCaptureDevicePositionContent*>(event->_content);
                    if (content != NULL) {
                        pos = content->position;
                        point = CGPointMake(content->point.x, content->point.y);
                    }
                }
                z11AppViewController* controller = (z11AppViewController*)wnd_handle;
                [controller focusPointOfInterest:pos WithPoint:point];
            }
            break;
        }
        case FOREIGNER_SDK_API_CALL_EVENT:
        case FOREIGNER_SDK_API_IMMEDIATELY_CALL_EVENT:
        {
            if (event != NULL) {
                MGTextContent* content = (MGTextContent*)event->_content;
                z11AppViewController* controller = (z11AppViewController*)wnd_handle;
                [controller foreignerSDKApiCall:content->text_content];
            }
            break;
        }
        case EXIT_APPLICATION_EVENT:
        {
            exit(0);
            break;
        }
        default:
            break;
    }
}

void AppMain::preserveEvent(z11::MIEvent *event)
{
    z11AppViewController* controller = (z11AppViewController*)wnd_handle;
    if (event->isImmediatellyHandleEvent()) {
//        [controller handleEventImmediatelly:event];
        CustomEvent* c_event = [CustomEvent alloc];
        c_event.event = event;
        [controller performSelectorOnMainThread:@selector(handleEventImmediatelly:) withObject:c_event waitUntilDone:YES];
    }else{
        GuiContainer::preserveEvent(event);
        [controller performSelectorOnMainThread:@selector(handleSysCallbackEvent) withObject:nil waitUntilDone:NO];
    }
}
