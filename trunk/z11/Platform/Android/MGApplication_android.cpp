#include "MGApplication_android.h"
#include "MGEGLView_android.h"
//#include "MGWebServiceThread_android.h"
#include "../NetworkStatus/MGNetworkReachability.h"
#include "../../CommonBase/MGStructure.h"
#include "../../GUI/Events/MIEvent.h"
#include "../../GUI/LightGraphic/LightGraphic.h"
#include "../../System/Device/MGDevice.h"
#include "../../System/IO/MGBundle.h"
#include "../../System/Media/Audio/MGAudioPlayer.h"
#include "../../System/Media/Video/MGVideoPlayer.h"
#include "../../System/Media/Capture/MGCameraCaptureManager.h"
#include "../../System/Shared/MGLog.h"
//#include "../../System/Timer/RealTimeLoop.h"
#include "../../System/WebService/MGWebServiceThread.h"
#include "jni/CameraJni.h"
#include "jni/IntentJni.h"
#include "jni/JniHelper.h"
#include "jni/LocationJni.h"
#include "jni/MediaJni.h"
#include "jni/MediaPlayerJni.h"
#include "jni/MediaRecorderJni.h"
#include "jni/MessageJni.h"
#include "jni/SensorJni.h"
#include "jni/SystemInfoJni.h"
#include "jni/WebViewJni.h"
#include "MGTimer_android.h"

#include "../MGGL.h"

#include <math.h>

#ifndef M_PI
#define M_PI        3.14159265358979323846264338327950288
#endif

#include <android/log.h>
#include <jni.h>
#include <cstring>
#define  LOG_TAG    "MGApplication_android Debug"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

namespace z11{

// sharedApplication pointer
MGApplication * MGApplication::sm_pSharedApplication = 0;
string MGApplication::keyboard_type = "none";
string MGApplication::return_key_type = "default";
    void MGApplication::setKeyboardType(MIKeyboardType type)
    {
        switch (type) {
            case z11::MIKeyboardTypeDefault:
                keyboard_type = "textPhonetic";
                break;
            case z11::MIKeyboardTypeASCIICapable:
                keyboard_type = "text";
                break;
            case z11::MIKeyboardTypeNumbersAndPunctuation:
                keyboard_type = "numberSigned";
                break;
            case z11::MIKeyboardTypeURL:
                keyboard_type = "textUri";
                break;
            case z11::MIKeyboardTypeNumberPad:
                keyboard_type = "number";
                break;
            case z11::MIKeyboardTypePhonePad:
                keyboard_type = "phone";
                break;
            case z11::MIKeyboardTypeNamePhonePad:
                keyboard_type = "textPersonName";
                break;
            case z11::MIKeyboardTypeEmailAddress:
                keyboard_type = "textEmailAddress";
                break;
            case z11::MIKeyboardTypeDecimalPad:
                keyboard_type = "numberDecimal";
                break;
            case z11::MIKeyboardTypeTwitter:
                keyboard_type = "textShortMessage";
                break;
            default:
                keyboard_type = "none";
                break;
        }
    }
    
    void MGApplication::setReturnKeyType(MIReturnKeyType type)
    {
        switch (type) {
            case z11::MIReturnKeyDefault:
                return_key_type = "default";
                break;
            case z11::MIReturnKeyGo:
                return_key_type = "go";
                break;
            case z11::MIReturnKeyGoogle:
                return_key_type = "google";
                break;
            case z11::MIReturnKeyJoin:
                return_key_type = "join";
                break;
            case z11::MIReturnKeyNext:
                return_key_type = "next";
                break;
            case z11::MIReturnKeyRoute:
                return_key_type = "route";
                break;
            case z11::MIReturnKeySearch:
                return_key_type = "search";
                break;
            case z11::MIReturnKeySend:
                return_key_type = "send";
                break;
            case z11::MIReturnKeyYahoo:
                return_key_type = "yahoo";
                break;
            case z11::MIReturnKeyDone:
                return_key_type = "done";
                break;
            case z11::MIReturnKeyEmergencyCall:
                return_key_type = "call";
                break;
            default:
                return_key_type = "default";
                break;
        }
    }
      
	  
MGApplication::MGApplication()
{
    MGAssert(! sm_pSharedApplication);
    sm_pSharedApplication = this;
}

MGApplication::~MGApplication()
{
    MGAssert(this == sm_pSharedApplication);
    sm_pSharedApplication = NULL;
}

static bool bInit=false;
int MGApplication::run()
{
    if(bInit)//has init
        return 0;
    // Initialize instance and z11.
    if (! initInstance() ) {
        return 0;
    }
    if (! applicationDidFinishLaunching()) {
        return 0;
    }
        bInit=true;
	return -1;
}

void MGApplication::setAnimationInterval(double interval)
{
    return ;
	JniMethodInfo methodInfo;
	if (! JniHelper::getStaticMethodInfo(methodInfo, "com/z11/mobile/framework/MGRenderer", "setAnimationInterval",
		"(D)V"))
	{
		MGLogD("%s %d: error to get methodInfo", __FILE__, __LINE__);
	}
	else
	{
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, interval);
	}
}

MGApplication::Orientation MGApplication::setOrientation(Orientation orientation)
{
    return orientation;
}

void MGApplication::statusBarFrame(MGRect * rect)
{
    if (rect)
    {
        // android doesn't have status bar.
        *rect = MGRectMake(0, 0, 0, 0);
    }
}

//////////////////////////////////////////////////////////////////////////
// static member function
//////////////////////////////////////////////////////////////////////////
MGApplication& MGApplication::sharedApplication()
{
    MGAssert(sm_pSharedApplication);
    return *sm_pSharedApplication;
}

//ccLanguageType MGApplication::getCurrentLanguage()
//{
//    char* pLanguageName = getCurrentLanguageJNI();
//    ccLanguageType ret = kLanguageEnglish;
//
//    if (0 == strcmp("zh", pLanguageName))
//    {
//        ret = kLanguageChinese;
//    }
//    else if (0 == strcmp("en", pLanguageName))
//    {
//        ret = kLanguageEnglish;
//    }
//
//    return ret;
//}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void MGApplication::beforeRenderScreen()
{

}
bool MGApplication::afterRenderScreen()
{
    return true;
}
void MGApplication::preserveEvent(z11::MIEvent *event)
{
    if (event->isImmediatellyHandleEvent()) {
        GuiContainer::container->handleEventImmediatelly(event);
    }else{
        GuiContainer::preserveEvent(event);
        if (!preserveEventJNI() && GuiContainer::container != NULL)
            GuiContainer::container->handleEvent();
    }
}
bool MGApplication::initOpenGLES()
{
	setProjectionMode();
	return true;
}
//void MGApplication::initialise(WndHandle wnd)
//{
//	GuiContainer::initialise(wnd);
//	//gui_root=GUIRootViewController::getInstance();
//
//	//TODO: add your own initialise code here!
//}
void MGApplication::renderScreen()
{
}
void MGApplication::handleSysCallbackEvent(z11::MIEvent *event)
{
	//MGAssert(event != NULL,"event != NULL");
//	MGLogD("MGApplication::handleSysCallbackEvent 0x%x",event->_event_type);
	MGAssert(event != NULL);
	
	switch (event->_event_type) 
	{
		
		case BECOME_FIRST_RESPONDER_EVENT: //弹出键盘
			
		{
				
			MGLogD("BECOME_FIRST_RESPONDER_EVENT");
            
            if(event->_content != NULL)
            {
                MIKeyboardTypeObj* obj = (MIKeyboardTypeObj*)event->_content;
                MIKeyboardType type = obj->keyboard_type;
                MIReturnKeyType return_type = obj->return_key_type;
                setKeyboardType(type);
                setReturnKeyType(return_type);
            }
            showSoftInputJNI((keyboard_type+"&"+return_key_type).c_str());
				
			break;
			
		}
		
		case RESIGN_FIRST_RESPONDER_EVENT: //收回键盘
			
		{
				
			MGLogD("RESIGN_FIRST_RESPONDER_EVENT");
			
            hideSoftInputJNI();
				
			break;
			
		}
            
        case APP_LOCK_SCREEN_WHEN_IDLE_EVENT:
        {
            appLockScreenWhenIdle();
            break;
        }
        case APP_UNLOCK_SCREEN_WHEN_IDLE_EVENT:
        {
            appUnlockScreenWhenIdle();
            break;
        }
        case APP_SHOW_SYSTEM_STATUS_BAR_EVENT:
        {
            appShowSystemStatusBar();
            break;
        }
        case APP_HIDE_SYSTEM_STATUS_BAR_EVENT:
        {
            appHideSystemStatusBar();
            break;
        }
		
		case START_LOCATION_SERVICE_EVENT: //开始Location服务  
		{
			MGLogD("START_LOCATION_SERVICE_EVENT");
			startLocationService();
			break;
		}
		case STOP_LOCATION_SERVICE_EVENT:  //停止Location服务  
		{
			MGLogD("STOP_LOCATION_SERVICE_EVENT");
			stopLocationService();
			break;
		}
		case START_HEADING_SERVICE_EVENT:  //开始Heading服务  
		{
			MGLogD("START_HEADING_SERVICE_EVENT");
			startHeadingService();
			break;
		}
		case STOP_HEADING_SERVICE_EVENT:  //停止Heading服务  
		{
			MGLogD("STOP_HEADING_SERVICE_EVENT");
			stopHeadingService();
			break;
		}
		case INIT_AUDIO_PLAY_EVENT:
		{
			if(event->_content != NULL)
			{
				MGAudioPlayer* player = (MGAudioPlayer*)event->_content;
				initAudioPlayer(player->getAudioPlayerID(), player->getAudioFilePath());
			}
			break;
		}
		case GET_AUDIO_DURATION_EVENT:
		{
			if(event->_content != NULL)
			{
				MGAudioPlayer* player = (MGAudioPlayer*)event->_content;
				setAudioDuration(player->getAudioPlayerID(), player->getAudioFilePath());
			}
			break;
		}
        case PREPARE_AUDIO_PLAY_EVENT:
		{
			if(event->_content != NULL)
			{
				MGAudioPlayer* player = (MGAudioPlayer*)event->_content;
				prepareAudioPlayer(player->getAudioPlayerID(), player->pan, player->volume, player->rate, player->number_of_loops);
			}
			break;
		}
		case PAUSE_AUDIO_PLAY_EVENT:
		{
			if(event->_content != NULL)
			{
				MGAudioPlayer* player = (MGAudioPlayer*)event->_content;
				pauseAudioPlayer(player->getAudioPlayerID());
			}
			break;
		}
		case STOP_AUDIO_PLAY_EVENT:
		{
			if(event->_content != NULL)
			{
				MGAudioPlayer* player = (MGAudioPlayer*)event->_content;
				stopAudioPlayer(player->getAudioPlayerID());
			}
			break;
		}
		case PLAY_AUDIO_PLAY_EVENT:
		{
			if(event->_content != NULL)
			{
				MGAudioPlayer *player=(MGAudioPlayer*)event->_content;
				playAudioPlayer(player->getAudioPlayerID());
			}
			break;
		}
		case UNINIT_AUDIO_PLAY_EVENT:
		{
			if(event->_content != NULL)
			{
				MGAudioPlayer* player = (MGAudioPlayer*)event->_content;
				uninitAudioPlayer(player->getAudioPlayerID());
			}
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
		case MEDIA_PICK_EVENT:
			if (event->_content != NULL)
			{
				MGMediaPickerType *picker_type = (MGMediaPickerType *)(event->_content);
				g_picker_type = picker_type;
				event->_content = NULL;
				openMedia();
				break;
			}
		case MEDIA_DROP_EVENT:
			{
				closeMedia();
				break;
			}
		case UIWEBVIEW_OPEN_URL_EVENT:
			{
				MGWebViewContent* content = (MGWebViewContent*)event->_content;
				jniWebViewOpenURL(content->url_content, content->rect, content->post_body);
				break;
			}
		case UIWEBVIEW_SHOW_VIEW_EVENT:
			{
				jniWebViewShow();
				break;
			}
		case UIWEBVIEW_CLOSE_VIEW_EVENT:
			{
				jniWebViewClose();
				break;
			}
		case OPEN_FILE_WITH_URL_EVENT:
			{
				MGTextContent* content = (MGTextContent*)event->_content;
				jniIntentOpenFileWithPath(content->text_content);
				break;
			}
		case LAUNCH_APP_WITH_URL_EVENT:
			{
				MGRemoteAppContent* content = (MGRemoteAppContent*)event->_content;
				jniIntentLaunchAppWithURL(content->id_content, content->url_content);
				event->_content = NULL;
				break;
			}
		case MEDIA_RECORDER_START_EVENT:
			{
				MGTextContent* content = (MGTextContent*)event->_content;
				jniMediaRecorderStartWithFileName(content->text_content);
				event->_content = NULL;
				break;
			}
		case MEDIA_RECORDER_STOP_EVENT:
			{
				jniMediaRecorderStop();
				break;
			}
		case MEDIA_RECORDER_UPDATE_METERS_EVENT:
			{
				//MGMediaRecorderContent *content=(MGMediaRecorderContent*)event->_content;
				jniMediaRecorderUpdateMeters();
				break;
			}		
		case VALIDATE_A_TIMER_EVENT:
			{
				MGLogD("VALIDATE_A_TIMER_EVENT");
				if (event->_content!=NULL)
				{
					MGTimerObj* timer = (MGTimerObj*)event->_content;
					MGTimerAndroid::getInstance()->add(timer);
				}
			}
			break;
		case INVALIDATE_A_TIMER_EVENT:
			{
				MGLogD("INVALIDATE_A_TIMER_EVENT");
				if (event->_content!=NULL)
				{
					MGTimerObj* timer = (MGTimerObj*)event->_content;
					MGTimerAndroid::getInstance()->remove(timer);
				}
			}
			break;
		case EXIT_APPLICATION_EVENT:
			{
				jniExitApplication();
			}
			break;
		case INIT_VIDEO_PLAY_EVENT:
			if(event->_content != NULL)
			{
				MGVideoPlayer *player = (MGVideoPlayer*)event->_content;
				initVideoPlayer(player->video_file_path);
                player->video_file_path = "";
			}
			break;
		case PLAY_VIDEO_PLAY_EVENT:
            if(event->_content != NULL)
			{
				MGVideoPlayer *player = (MGVideoPlayer*)event->_content;
				playVideoPlayer();
                player->video_file_path = "";
			}
			break;
        case STOP_VIDEO_PLAY_EVENT:
            stopVideoPlayer();
			break;
		case GET_VIDEO_PREVIEW_EVENT:
			if(event->_content != NULL)
			{
				MGTextContent *obj = (MGTextContent *)event->_content;
				getPreviewImageFromFile(obj->text_content,MGVideoPlayer::preview_width,MGVideoPlayer::preview_height);
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
                cameraOpenCamera(pos);
            }
            break;
        }
        case STOP_CAMERA_CAPTURE_SERVICE_EVENT:
		{
			cameraReleaseCamera();
			break;
		}
        case START_ACCELERATION_SERVICE_EVENT:
        {
            if (event!= NULL && event->_content != NULL) {
                MGIntervalContent* content = (MGIntervalContent*)event->_content;
                startAccelerationService(content->interval);
            } else {
                startAccelerationService(0.0f);
            }
            break;
        }
        case STOP_ACCELERATION_SERVICE_EVENT:
        {
            stopAccelerationService();
            break;
        }
        case START_ROTATION_SERVICE_EVENT:
        {
            if (event!= NULL && event->_content != NULL) {
                MGIntervalContent* content = (MGIntervalContent*)event->_content;
                startRotationService(content->interval);
            } else {
                startRotationService(0.0f);
            }
            break;
        }
        case STOP_ROTATION_SERVICE_EVENT:
        {
            stopRotationService();
            break;
        }
		case START_ATTITUDE_SERVICE_EVENT:
			{
				if (event!= NULL && event->_content != NULL) {
					MGIntervalContent* content = (MGIntervalContent*)event->_content;
					startAttitudeService(content->interval);
				} else {
					startRotationService(0.0f);
				}
				break;
			}
		case STOP_ATTITUDE_SERVICE_EVENT:
			{
				stopAttitudeService();
				break;
			}
        case RELEASE_MOTION_SERVICE_EVENT:
        {
            ;
            break;
        }
		case SCAN_WIFI_STATUS_EVENT:
        {
            MGNetworkReachability::refreshWifiStatus();
            break;
        }
        case FOREIGNER_SDK_API_CALL_EVENT:
        case FOREIGNER_SDK_API_IMMEDIATELY_CALL_EVENT:
        {
            if (event!= NULL && event->_content != NULL) {
                MGTextContent* content = (MGTextContent*)event->_content;
                foreignerSDKApiCallJni(content->text_content.c_str());
            }
            break;
        }
		default:
			break;
	}

}

bool MGApplication::initInstance()
{
	MGLogD("MGApplication initInstance.");
	GuiContainer::initialise(NULL);
	return true;
}

bool MGApplication::applicationDidFinishLaunching()
{
	//light_graphic->readyToRun();
//	if (real_time)
//	{
//		real_time->run();
//	}
    LightGraphic::setFps(60);

	return true;
}
    
    void MGApplication::setProjectionMode()
    {
        z11::MGEGLView *view = &z11::MGEGLView::sharedOpenGLView();
        view->setFrameWidthAndHeight(z11::MGDevice::getScreenWidth(), z11::MGDevice::getScreenHeight());
        view->create(z11::MGDevice::getScreenWidth(), z11::MGDevice::getScreenHeight());
        
        MGSize size;
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
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    }

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
}
