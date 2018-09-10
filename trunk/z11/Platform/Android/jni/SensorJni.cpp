/****************************************************************************
Copyright (c) 2010 z11-x.org

http://www.z11-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#include "SensorJni.h"
//#include "CCGeometry.h"
//#include "CCAccelerometer.h"
//#include "platform/android/CCAccelerometer_android.h"
//#include "MGEGLView.h"
#include "../../../System/Shared/Utility.h"
#include "../../../System/LocationService/MGLocationManager.h"
#include "../../../System/LocationService/MGHeading.h"
#include "../../../System/MotionService/MGMotionManager.h"
#include "../../../System/MotionService/MGRotation.h"
#include "../../../System/MotionService/MGAcceleration.h"
#include "../../../System/MotionService/MGAttitude.h"
#include "JniHelper.h"
#include <stdio.h>
#include <string.h>
#include <android/log.h>
#include <jni.h>


#if 0
#define  LOG_TAG    "SensorJni"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#else
#define  LOGD(...)
#endif

using namespace z11;

extern "C"
{

    //////////////////////////////////////////////////////////////////////////
    // handle accelerometer changes
    //////////////////////////////////////////////////////////////////////////

    void sensorOnSensorChanged(JNIEnv*  env, jobject thiz, jfloat x, jfloat y, jfloat z, jlong timeStamp)
    {
        // We need to invert to make it compatible with iOS.
//        MGRect rcRect = MGEGLView::sharedOpenGLView().getViewPort();
//        float fScreenScaleFactor = MGEGLView::sharedOpenGLView().getScreenScaleFactor();
//        z11::CCAccelerometer::sharedAccelerometer()->update((x - rcRect.origin.x) / fScreenScaleFactor,
//            (y - rcRect.origin.y) / fScreenScaleFactor,
//            z,
//            timeStamp);
    }

	void startAccelerationService(const float interval)
	{
		JniMethodInfo t;

		if (JniHelper::getStaticMethodInfo(t,
			                         "com/z11/mobile/framework/MGActivity",
									 "startAccelerationService",
									 "(F)V"))
		{
			t.env->CallStaticVoidMethod(t.classID, t.methodID, interval);
		}
	}

	void stopAccelerationService()
	{
		JniMethodInfo t;

		if (JniHelper::getStaticMethodInfo(t,
			"com/z11/mobile/framework/MGActivity",
			"stopAccelerationService",
			"()V"))
		{
			t.env->CallStaticVoidMethod(t.classID, t.methodID);
		}
	}
    
    void startRotationService(const float interval)
	{
		JniMethodInfo t;
        
		if (JniHelper::getStaticMethodInfo(t,
                                           "com/z11/mobile/framework/MGActivity",
                                           "startRotationService",
                                           "(F)V"))
		{
			t.env->CallStaticVoidMethod(t.classID, t.methodID, interval);
		}
	}
    
	void stopRotationService()
	{
		JniMethodInfo t;
        
		if (JniHelper::getStaticMethodInfo(t,
                                           "com/z11/mobile/framework/MGActivity",
                                           "stopRotationService",
                                           "()V"))
		{
			t.env->CallStaticVoidMethod(t.classID, t.methodID);
		}
	}

	void startAttitudeService(const float interval)
	{
		JniMethodInfo t;

		if (JniHelper::getStaticMethodInfo(t,
			"com/z11/mobile/framework/MGActivity",
			"startAttitudeService",
			"(F)V"))
		{
			t.env->CallStaticVoidMethod(t.classID, t.methodID, interval);
		}
	}

	void stopAttitudeService()
	{
		JniMethodInfo t;

		if (JniHelper::getStaticMethodInfo(t,
			"com/z11/mobile/framework/MGActivity",
			"stopAttitudeService",
			"()V"))
		{
			t.env->CallStaticVoidMethod(t.classID, t.methodID);
		}
	}

	void startHeadingService()
	{
		JniMethodInfo t;

		if (JniHelper::getStaticMethodInfo(t,
			"com/z11/mobile/framework/MGActivity",
			"startHeadingService",
			"()V"))
		{
			t.env->CallStaticVoidMethod(t.classID, t.methodID);
		}
	}
	void stopHeadingService()
	{
		JniMethodInfo t;

		if (JniHelper::getStaticMethodInfo(t,
			"com/z11/mobile/framework/MGActivity",
			"stopHeadingService",
			"()V"))
		{
			t.env->CallStaticVoidMethod(t.classID, t.methodID);
		}
	}

    void sensorNativeUpdateWithNewAcceleration(JNIEnv* env, jobject thiz, jdouble x, jdouble y, jdouble z)
    {
        MGAcceleration acceleration;
        acceleration.x = x;
        acceleration.y = y;
        acceleration.z = z;
        MGMotionManager::didUpdateAcceleration(acceleration);
    }
    void sensorNativeUpdateWithNewRotation(JNIEnv* env, jobject thiz, jdouble x, jdouble y, jdouble z)
    {
        MGRotation rotation;
        rotation.x = x;
        rotation.y = y;
        rotation.z = z;
        MGMotionManager::didUpdateRotation(rotation);
    }
	void sensorNativeUpdateWithNewAttitude(JNIEnv* env, jobject thiz, jdouble yaw, jdouble pitch, jdouble roll)
    {
        MGAttitude attitude;
        attitude.yaw = yaw;
        attitude.pitch = pitch;
        attitude.roll = roll;
        MGMotionManager::didUpdateAttitude(attitude);
    }
	void buildHeadingFromString(MGHeading& new_heading, std::string str_heading);
	void sensorNativeUpdateWithNewHeading(JNIEnv* env, jobject thiz, jdouble magnetic_heading, jdouble true_heading, jdouble heading_accuracy, jdouble x, jdouble y, jdouble z)
    {
        MGHeading newHeading;
        newHeading.magnetic_heading = magnetic_heading;
        newHeading.true_heading = true_heading;
        newHeading.heading_accuracy = heading_accuracy;
        newHeading.x = x;
        newHeading.y = y;
        newHeading.z = z;
        MGLocationManager::didUpdateHeading(newHeading);
    }
}
