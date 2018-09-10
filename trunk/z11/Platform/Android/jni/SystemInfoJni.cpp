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
#include "SystemInfoJni.h"
#include "JniHelper.h"
#include "../../../System/Device/MGDevice.h"
#include <android/log.h>
#include <jni.h>


#if 0
#define  LOG_TAG    "SystemInfoJni"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#else
#define  LOGD(...)
#endif

using namespace z11;

extern "C"
{
	std::string getPackageNameJNI()
	{
		JniMethodInfo t;
		std::string  ret = 0;

		if (JniHelper::getStaticMethodInfo(t,
						"com/z11/mobile/framework/MGActivity",
						"getZ11PackageName",
						"()Ljava/lang/String;"))
		{
			jstring str = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID);
			ret = JniHelper::jstring2string(str);

			LOGD("package name %s", ret.c_str());
		}

		return ret;
	}

	//////////////////////////////////////////////////////////////////////////
    // handle get current language
    //////////////////////////////////////////////////////////////////////////
    char* getCurrentLanguageJNI()
    {
        JniMethodInfo t;
        char* ret = 0;

        if (JniHelper::getStaticMethodInfo(t
            , "com/z11/mobile/framework/MGActivity"
            , "getCurrentLanguage"
            , "()Ljava/lang/String;"))
        {
            jstring str = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID);
			ret = (char*)JniHelper::jstring2string(str).c_str();

			LOGD("language name %s", ret);
        }

        return ret;
    }
	
	static string jstring2string_(JNIEnv *env ,jstring jstr)
	{

		jboolean isCopy;

		const char* chars = env->GetStringUTFChars(jstr, &isCopy);
		string ret(chars);
		if (isCopy)
		{
			env->ReleaseStringUTFChars(jstr, chars);
		}

		return ret;
	}
	
	void systemNativeSetDeviceParameter(JNIEnv*  env, jobject thiz, jstring os_type, jstring os_version, jstring device_id, jstring device_type, jint device_dpi)
	{
        MGDevice::setDeviceOSType(jstring2string_(env,os_type));
		MGDevice::setDeviceOSVersion(jstring2string_(env,os_version));
		MGDevice::setDeviceType(jstring2string_(env,device_type));
		MGDevice::setDeviceID(jstring2string_(env,device_id));
        MGDevice::setScreenPPI(device_dpi);
	}
	
	void jniExitApplication()
	{
		JniMethodInfo methodInfo;
		if(JniHelper::getStaticMethodInfo(methodInfo, "com/z11/mobile/framework/MGActivity", "exitApplication", "()V"))
		{						
			//MGLogD("jniIntentLauchAppWithURL success");
			methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
		}
		else
		{
			//MGLogD("jniIntentLauchAppWithURL Failed");
		}
	}
}
