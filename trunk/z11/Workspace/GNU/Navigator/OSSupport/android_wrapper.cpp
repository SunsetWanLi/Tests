//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (android_wrapper.cpp)  
/// @brief (Android 的android_wrapper)  
///  
///(android_wrapper 定义了与具体应用无关通用的jni，可以实现C++,Java之间的数据函数互相调用，可以作为其他android工程的模板)  
///  
/// @version 0.1   (版本声明)  
/// @author        (Gu RongFang)  
/// @date          (11-07-26)  
///  
///  
///    修订说明：最初版本  
////////////////////////////////////////////////////////////////////////// 
#ifndef _ANDROID_WRAPPER_CPP_
#define _ANDROID_WRAPPER_CPP_
#include <jni.h>
#include <android/log.h>
#include <stdint.h>
#include <stdexcept>
#include <string>
#include "Application/Samples/WenZhouTong/JNIClassDataStruct.h"
#include "ViewManager.h"
#include "appMain.h"
#include "System/Device/MGDevice.h"
#include "System/Shared/MGLog.h"
#include <sstream>
#include "Application/Samples/WenZhouTong/GUIRootView/GUIRootViewController.h"
#include "Application/Samples/WenZhouTong/IndexView/IndexViewController.h"
#include "Application/Samples/WenZhouTong/RoadMapView/RoadMapViewController.h"
#include "Application/Samples/WenZhouTong/TrafficSearchView/TrafficSearchViewController.h"
#include "Application/Samples/WenZhouTong/WeatherView/WeatherViewController.h"
#include "Application/Samples/WenZhouTong/WeatherView/WeatherDetailViewController.h"
using namespace std;
using namespace z11;

static int  sWindowWidth  = 480;
static int  sWindowHeight = 800;
static JNIEnv*  g_env;
static jclass ClassDemoGLSurfaceView;
static jmethodID methodID;

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////System wrapper 函数接口  begin////////////////////////////////////////////////
extern void appInit();
extern void appDeinit();
extern void appRender( int width, int height);
extern void OnMouseDown(unsigned long wParam, long lParam);
extern void OnMouseUp(unsigned long wParam, long lParam);
extern void OnMouseMove(unsigned long wParam, long lParam);
extern void OnChar(unsigned long wParam, long lParam);
extern void OnHandleEvent();
extern void OnKeyDown(int code);
extern void OnInsertText(const char *text);
static JavaVM* g_pVM=NULL;
////////////////////////////////////////////////System wrapper 函数接口  end////////////////////////////////////////////////


//////////////////////////////////////////////// JNI C++ to Java调用接口函数  begin////////////////////////////////////////////////
JNIEnv *GetJNIEnv()
{
	g_env=NULL;
	if(g_env==NULL)
	{
		if(g_pVM!=NULL)
				g_pVM->GetEnv((void**)&g_env,JNI_VERSION_1_6);
		else
			MGLogD("g_pVM==NULL");
	}
	return g_env;
}

jclass GetClass(string class_name )
{
	jclass ret=NULL;
	g_env=GetJNIEnv();
	if(g_env!=NULL)
	{
		MGLogD("try to FindClass ");
		ret=g_env->FindClass(("com/z11/mobile/framework/"+class_name).c_str());
		//ret=g_env->FindClass("com/z11/mobile/framework/DemoGLSurfaceView");
	}
	return ret;
}

jmethodID GetStaticMethod(string class_name ,string method_name ,string method_type)
{
	jmethodID ret=NULL;
	g_env=GetJNIEnv();
	jclass get_class=GetClass(class_name);
	if(g_env&&get_class)
	{
		__android_log_print(ANDROID_LOG_INFO, "z11", "FindClass success");
		ret=g_env->GetStaticMethodID(get_class,method_name.c_str(),method_type.c_str());
	}
	else
		__android_log_print(ANDROID_LOG_INFO, "z11", "FindClass Failed");

	return ret;
}

void CallStaticMethod(jclass classID,jmethodID methodID)
{
	if(methodID!=NULL)
	{
		__android_log_print(ANDROID_LOG_INFO, "z11", "GetMethodID success");
		g_env->CallStaticVoidMethod(classID,methodID);
	}
	else
		__android_log_print(ANDROID_LOG_INFO, "z11", "GetMethodID failed");
}

void CallStaticVoidMethod(string class_name ,string method_name ,string method_type)
{
    try
    {
        jmethodID ret=NULL;
        g_env=GetJNIEnv();
        jclass get_class=GetClass(class_name);
        if(g_env&&get_class)
        {
            __android_log_print(ANDROID_LOG_INFO, "z11", "FindClass success");
            ret=g_env->GetStaticMethodID(get_class,method_name.c_str(),method_type.c_str());
            g_env->CallStaticVoidMethod(get_class,ret);
        }
        else
            __android_log_print(ANDROID_LOG_INFO, "z11", "FindClass Failed");
    }
    catch (std::exception &ex)
    {
		string s=ex.what();
       __android_log_print(ANDROID_LOG_INFO, "z11","%s",s.c_str());
    }
}
/////////////////////////////////////////// JNI C++ to Java调用接口函数  end////////////////////////////////////////////////



///////////////////////////////////////////JNI Java to C++调用接口函数  begin////////////////////////////////////////////////
static bool isSetScreen=false;
static bool bFirst=true;

jint JNI_OnLoad(JavaVM*vm,void *reserved)
{
	g_pVM=vm;
	return JNI_VERSION_1_6;
}

/* Call to initialize the graphics state */
void
Java_com_z11_mobile_framework_MGGLRenderer_nativeInit( JNIEnv*  env )
{
    appInit();
}

void
Java_com_z11_mobile_framework_MGGLRenderer_nativeResize( JNIEnv*  env, jobject  thiz, jint w, jint h )
{
    if(!isSetScreen)
    {
        sWindowWidth  = w;
        sWindowHeight = h;
        z11::MGDevice::setScreenHeight(sWindowHeight);
        z11::MGDevice::setScreenWidth(sWindowWidth);
        isSetScreen=true;
    }

    bFirst=true;
    MGLogD( "resize w=%d h=%d", w, h);
}

/* Call to finalize the graphics state */
void
Java_com_z11_mobile_framework_MGGLRenderer_nativeDone( JNIEnv*  env )
{
    appDeinit();
}

/* This is called to indicate to the render loop that it should
 * stop as soon as possible.
 */
void
Java_com_z11_mobile_framework_DemoGLSurfaceView_nativePause( JNIEnv*  env )
{

}

/* Call to render the next GL frame */
void
Java_com_z11_mobile_framework_MGGLRenderer_nativeRender( JNIEnv*  env )
{
	if(isSetScreen)
	{
            if(bFirst)
            {
                bFirst=false;
                z11::ViewManager *view_manager=z11::ViewManager::getInstance();
                if (view_manager!=NULL)
                {
                    view_manager->init(NULL);
                }
                view_manager->m_app_main->initOpenGLES();

                z11::MGImage::recoveryContext();
                LightGraphic::setFps(30);
            }
		appRender(sWindowWidth, sWindowHeight);
	}
}

void
Java_com_z11_mobile_framework_DemoGLSurfaceView_nativeMouseDown( JNIEnv*  env ,jobject  thiz,jint x,jint y)
{
	OnMouseDown(0,(y<<16)|x);
}

void
Java_com_z11_mobile_framework_DemoGLSurfaceView_nativeMouseUp( JNIEnv*  env ,jobject  thiz,jint x,jint y)
{
	OnMouseUp(0,(y<<16)|x);
}

void
Java_com_z11_mobile_framework_DemoGLSurfaceView_nativeMouseMove( JNIEnv*  env ,jobject  thiz,jint x,jint y)
{
	OnMouseMove(0,(y<<16)|x);
}
/////////////////////////////////////////////////// JNI Java to C++调用接口函数  end///////////////////////////////////////////////////


/////////////////////////////////////////////////// 通用 JNI调用接口函数  begin ////////////////////////////////////////////////////////
jint
Java_com_z11_mobile_framework_MGMessageCommunication_GetNativeCommand( JNIEnv*  env ,jobject  thiz)
{
	JNIClassContainer::getInstance()->PopClass();
	return 0;
}

void NativeToJavaCommandAndData(int code,string parameter)
{
     try
    {
        string class_name="MGMessageCommunication";
        string method_name="NativeToJavaCommandAndData";
       string  method_type="(ILjava/lang/String;)V";

        jmethodID ret=NULL;
        g_env=GetJNIEnv();
        jclass get_class=GetClass(class_name);
        if(g_env&&get_class)
        {
            __android_log_print(ANDROID_LOG_INFO, "z11", "FindClass success");
            ret=g_env->GetStaticMethodID(get_class,method_name.c_str(),method_type.c_str());
            g_env->CallStaticVoidMethod(get_class,ret,code,g_env->NewStringUTF(parameter.c_str()));
        }
        else
            __android_log_print(ANDROID_LOG_INFO, "z11", "FindClass Failed");
    }
    catch (std::exception &ex)
    {
		string s=ex.what();
       __android_log_print(ANDROID_LOG_INFO, "z11","%s",s.c_str());
    }
}

void Java_com_z11_mobile_framework_MGMessageCommunication_JavaToNativeCommandAndData(JNIEnv*  env ,jobject  thiz,jint code,jstring str)
{
    JNIEventCallback::handle(code,env->GetStringUTFChars(str,NULL));
}

/////////////////////////////////////////////////// 通用 JNI调用接口函数  end ////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif


#endif //_ANDROID_WRAPPER_CPP_
