#include "../../../Platform/Android/jni/IMEJni.h"
#include "../../../System/WebView/MGWebViewManager.h"
#include "../../../System/ClipBoard/MGClipBoard.h"
#include "../../../GUI/GuiContainer/GuiContainer.h"
#include "../../../GUI/Controllers/MIViewController.h"
#include "JniHelper.h"
#include <android/log.h>
#include <string.h>
#include <jni.h>
#include <assert.h>
#include <stdio.h>

#if 0
#define  LOG_TAG    "LocationJni"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#else
#define  LOGD(...)
#endif
using namespace z11;
extern "C"
{

    //Native IME Jni
    void Java_com_z11_mobile_framework_MGRenderer_nativeInsertText(JNIEnv* env, jobject thiz, jstring text)
    {
        jboolean isCopy = 0;
        const char* pszText = env->GetStringUTFChars(text, &isCopy);
        if (isCopy)
        {
            imeNativeInsertJNI(pszText);
            env->ReleaseStringUTFChars(text, pszText);
        }
    }


    void Java_com_z11_mobile_framework_MGRenderer_nativeDeleteBackward(JNIEnv* env, jobject thiz)
    {
        imeNativeDeleteBackward();
    }

    jstring Java_com_z11_mobile_framework_MGRenderer_nativeGetContentText()
    {
        JNIEnv * env = 0;

        if (JniHelper::getJavaVM()->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK || ! env)
        {
            return 0;
        }
        const char * pszText = imeNativeGetContentText();
        return env->NewStringUTF(pszText);
    }
    //Location service
    extern void locationNativeUpdateWithNewLocationJNI(JNIEnv* env, jobject thiz, jstring to_location, jstring from_location);
    void Java_com_z11_mobile_framework_MGLocationManager_nativeUpdateWithNewLocation(JNIEnv* env, jobject thiz, jstring to_location, jstring from_location)
    {
        locationNativeUpdateWithNewLocationJNI(env,thiz,to_location,from_location);
    }
    extern void locationNativeUpdateFailWithErrorJNI(JNIEnv* env, jobject thiz, jstring error);
    void Java_com_z11_mobile_framework_MGLocationManager_nativeUpdateFailWithError(JNIEnv* env, jobject thiz, jstring error)
    {
        locationNativeUpdateFailWithErrorJNI(env,thiz,error);
    }

    //media picker
    extern jobject mediaGetPickerType( JNIEnv*  env , jobject  thiz );
    JNIEXPORT jobject JNICALL 
    Java_com_z11_mobile_framework_MGMediaPicker_getPickerType( JNIEnv*  env , jobject  thiz )
    {
        return mediaGetPickerType(env,thiz);
    }

    extern 	void meidaDidFinishPickingImageWithInfoAndFile(JNIEnv* env, jobject thiz, jstring filepath, jbyteArray bytes, jint length);
    void Java_com_z11_mobile_framework_MGMediaPicker_didFinishPickingImageWithInfoAndFile(JNIEnv* env, jobject thiz, jstring filepath, jbyteArray bytes, jint length)
    {
        meidaDidFinishPickingImageWithInfoAndFile(env,thiz,filepath,bytes,length);
    }

    extern 	void meidaDidFinishPickingImageWithInfo(JNIEnv* env, jobject thiz, jbyteArray bytes, jint length);
    void Java_com_z11_mobile_framework_MGMediaPicker_didFinishPickingImageWithInfo(JNIEnv* env, jobject thiz, jbyteArray bytes, jint length)
    {
        meidaDidFinishPickingImageWithInfo(env,thiz,bytes,length);
    }

    extern void mediaDidFinishPickingMediaWithFile(JNIEnv* env, jobject thiz, jint type, jstring filepath);
    void Java_com_z11_mobile_framework_MGMediaPicker_didFinishPickingMediaWithFile(JNIEnv* env, jobject thiz, jint type, jstring filepath)
    {
        mediaDidFinishPickingMediaWithFile(env,thiz,type,filepath);
    }

    extern void mediaDidCancelPickingMeida(JNIEnv* env, jobject thiz, jint type);
    void Java_com_z11_mobile_framework_MGMediaPicker_didCancelPickingMedia(JNIEnv* env, jobject thiz, jint type)
    {
        mediaDidCancelPickingMeida(env,thiz,type);
    }
	//media recorder
	extern void mediaNativeUpdateMetersJNI(JNIEnv* env, jobject thiz, jfloat value);
	void Java_com_z11_mobile_framework_MGMediaRecorderManager_nativeUpdateMeters(JNIEnv*  env, jobject thiz,  jfloat value)
	{
		mediaNativeUpdateMetersJNI(env,thiz,value);
	}
    //MGRender
    extern void messageNativeRender(JNIEnv* env);
    void Java_com_z11_mobile_framework_MGRenderer_nativeRender(JNIEnv* env)
    {
        messageNativeRender(env);
    }

    extern void messageNativeResize( JNIEnv*  env , jobject  thiz,jint w, jint h);
    void Java_com_z11_mobile_framework_MGActivity_nativeResize( JNIEnv*  env , jobject  thiz,jint w, jint h)
    {
        messageNativeResize(env,thiz,w,h);
    }

    extern void messageNativeOnPause();
    void Java_com_z11_mobile_framework_MGRenderer_nativeOnPause()
    {
        messageNativeOnPause();
    }
    extern void messageNativeOnResume();
    void Java_com_z11_mobile_framework_MGGLSurfaceView_nativeOnResume()
    {
        messageNativeOnResume();
    }

    extern void messageNativeSetPaths(JNIEnv*  env, jobject thiz, jstring apkPath, jstring sdcardPath);
    void Java_com_z11_mobile_framework_MGActivity_nativeSetPaths(JNIEnv*  env, jobject thiz, jstring apkPath, jstring sdcardPath)
    {
        messageNativeSetPaths(env,thiz,apkPath, sdcardPath);
    }

    //Sensor
    extern void sensorOnSensorChanged(JNIEnv*  env, jobject thiz, jfloat x, jfloat y, jfloat z, jlong timeStamp);
    void Java_com_z11_mobile_framework_MGAccelerometer_onSensorChanged(JNIEnv*  env, jobject thiz, jfloat x, jfloat y, jfloat z, jlong timeStamp)
    {
        sensorOnSensorChanged(env,thiz,x,y,z,timeStamp);
    }

    extern void sensorNativeUpdateWithNewHeading(JNIEnv* env, jobject thiz, jdouble magnetic_heading, jdouble true_heading, jdouble heading_accuracy, jdouble x, jdouble y, jdouble z);
    void Java_com_z11_mobile_framework_MGSensorManager_nativeUpdateWithNewHeading(JNIEnv* env, jobject thiz, jdouble magnetic_heading, jdouble true_heading, jdouble heading_accuracy, jdouble x, jdouble y, jdouble z)
    {
        sensorNativeUpdateWithNewHeading(env, thiz, magnetic_heading, true_heading, heading_accuracy, x, y, z);
    }
    
    extern void sensorNativeUpdateWithNewRotation(JNIEnv* env, jobject thiz, jdouble x, jdouble y, jdouble z);
    void Java_com_z11_mobile_framework_MGSensorManager_nativeUpdateWithNewRotation(JNIEnv* env, jobject thiz, jdouble x, jdouble y, jdouble z)
    {
        sensorNativeUpdateWithNewRotation(env, thiz, x, y, z);
    }
    
    extern void sensorNativeUpdateWithNewAcceleration(JNIEnv* env, jobject thiz, jdouble x, jdouble y, jdouble z);
    void Java_com_z11_mobile_framework_MGSensorManager_nativeUpdateWithNewAcceleration(JNIEnv* env, jobject thiz, jdouble x, jdouble y, jdouble z)
    {
        sensorNativeUpdateWithNewAcceleration(env, thiz, x, y, z);
    }

	extern void sensorNativeUpdateWithNewAttitude(JNIEnv* env, jobject thiz, jdouble yaw, jdouble pitch, jdouble roll);
	void Java_com_z11_mobile_framework_MGSensorManager_nativeUpdateWithNewAttitude(JNIEnv* env, jobject thiz, jdouble yaw, jdouble pitch, jdouble roll)
	{
		sensorNativeUpdateWithNewAttitude(env, thiz, yaw, pitch, roll);
	}

    //Touches
    extern  void touchNativeTouchesBegin(JNIEnv*  env, jobject thiz, jint id, jfloat x, jfloat y);
    void Java_com_z11_mobile_framework_MGRenderer_nativeTouchesBegin(JNIEnv*  env, jobject thiz, jint id, jfloat x, jfloat y)
    {
        touchNativeTouchesBegin(env,thiz,id,x,y);
    }

    extern void touchNativeTouchesEnd(JNIEnv*  env, jobject thiz, jint id, jfloat x, jfloat y);
    void Java_com_z11_mobile_framework_MGRenderer_nativeTouchesEnd(JNIEnv*  env, jobject thiz, jint id, jfloat x, jfloat y)
    {
        touchNativeTouchesEnd(env,thiz,id,x,y);
    }

    extern void touchNativeTouchesMove(JNIEnv*  env, jobject thiz, jintArray ids, jfloatArray xs, jfloatArray ys);
    void Java_com_z11_mobile_framework_MGRenderer_nativeTouchesMove(JNIEnv*  env, jobject thiz, jintArray ids, jfloatArray xs, jfloatArray ys)
    {
        touchNativeTouchesMove(env,thiz,ids,xs,ys);
    }

    extern void touchNativeTouchesCancel(JNIEnv*  env, jobject thiz, jintArray ids, jfloatArray xs, jfloatArray ys);
    void Java_com_z11_mobile_framework_MGRenderer_nativeTouchesCancel(JNIEnv*  env, jobject thiz, jintArray ids, jfloatArray xs, jfloatArray ys)
    {
        touchNativeTouchesCancel(env,thiz,ids,xs,ys);
    }

    extern jboolean touchNativeKeyDown(JNIEnv*  env, jobject thiz, jint keyCode);
    jboolean Java_com_z11_mobile_framework_MGRenderer_nativeKeyDown(JNIEnv*  env, jobject thiz, jint keyCode)
    {
        return touchNativeKeyDown(env,thiz,keyCode);
    }

    extern void systemNativeSetDeviceParameter(JNIEnv*  env, jobject thiz, jstring os_type, jstring os_version, jstring device_id, jstring device_type, jint device_dpi);
    void Java_com_z11_mobile_framework_MGActivity_nativeSetDeviceParameter(JNIEnv*  env, jobject thiz, jstring os_type, jstring os_version, jstring device_id, jstring device_type, jint device_dpi)
    {
        systemNativeSetDeviceParameter(env, thiz, os_type, os_version, device_id, device_type, device_dpi);
    }

    extern void cameraNativeOnPreviewFrame(JNIEnv* env, jobject thiz, jbyteArray bytes, jint length,jint width,jint height,jint rotate);
    void Java_com_z11_mobile_framework_CameraPreview_nativeOnPreviewFrame(JNIEnv* env, jobject thiz, jbyteArray bytes, jint length,jint width,jint height,jint rotate)
    {
        cameraNativeOnPreviewFrame(env,thiz,bytes,length,width,height,rotate);
    }

	void Java_com_z11_mobile_framework_MGActivity_nativeApplicationWillResignActive(JNIEnv* env, jobject thiz)
	{
		if(GuiContainer::container == NULL)
			return;
		if(GuiContainer::container->gui_root == NULL)
			return;
		GuiContainer::container->gui_root->applicationWillResignActive();
	}
    
    void Java_com_z11_mobile_framework_MGActivity_nativeForeignerSDKApiBack(JNIEnv* env, jobject thiz, jstring text)
    {
        jboolean isCopy = 0;
        const char* pszText = env->GetStringUTFChars(text, &isCopy);
        if (isCopy)
        {
            GuiContainer::foreignerSDKApiBack(pszText);
            env->ReleaseStringUTFChars(text, pszText);
        }
    }

    void Java_com_z11_mobile_framework_MGActivity_nativeApplicationDidEnterBackground(JNIEnv* env, jobject thiz)
    {
        if (GuiContainer::container != NULL) {
            if (GuiContainer::container->gui_root != NULL) {
                GuiContainer::container->gui_root->applicationDidEnterBackground();
            }
        }
    }

	void Java_com_z11_mobile_framework_MGActivity_nativeApplicationWillEnterForeground(JNIEnv* env, jobject thiz)
	{
        if (GuiContainer::container != NULL) {
            if (GuiContainer::container->gui_root != NULL) {
                GuiContainer::container->gui_root->applicationWillEnterForeground();
            }
        }
	}

	void Java_com_z11_mobile_framework_MGActivity_nativeApplicationDidBecomeActive(JNIEnv* env, jobject thiz)
	{
		if(GuiContainer::container == NULL)
			return;
		if(GuiContainer::container->gui_root == NULL)
			return;
		GuiContainer::container->gui_root->applicationDidBecomeActive();
	}
    
    void Java_com_z11_mobile_framework_MGActivity_nativeHandleSystemEvent(JNIEnv* env, jobject thiz)
	{
		if(GuiContainer::container == NULL)
			return;
		GuiContainer::container->handleEvent();
	}
    
    void Java_com_z11_mobile_framework_MGActivity_nativeSetClipboardString(JNIEnv* env, jobject thiz, jstring clip_str)
    {
        const char* str;
		jboolean isCopy;
		str = env->GetStringUTFChars(clip_str, &isCopy);
        if (isCopy)
        {
            MGClipBoard::setString(str);
            env->ReleaseStringUTFChars(clip_str, str);
        }
    }
    
    jstring Java_com_z11_mobile_framework_MGActivity_nativeGetClipboardString(JNIEnv* env, jobject thiz)
    {
        string clip_str = MGClipBoard::getString();
        return env->NewStringUTF(clip_str.c_str());
    }
}