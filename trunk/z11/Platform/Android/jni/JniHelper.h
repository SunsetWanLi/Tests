/****************************************************************************
Copyright (c) 2010-2011 cocos2d-x.org

http://www.cocos2d-x.org

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
#ifndef __ANDROID_JNI_HELPER_H__
#define __ANDROID_JNI_HELPER_H__

#include <jni.h>
#include <string>
#include <vector>
//#include "CCPlatformMacros.h"
//  javap -s  com.z11.mobile.framework.DemoActivity get Signature~

namespace z11 {

	typedef struct JniMethodInfo_
	{
		JNIEnv *    env;
		jclass      classID;
		jmethodID   methodID;
	} JniMethodInfo;
	

	enum JNIClassRef
	{
		JNI_CLASS_ACTIVITY,
		JNI_CLASS_MEDIA_PLAYER,
		JNI_CLASS_TIMER,
		JNI_CLASS_NETWORKSTATUS,
		JNI_CLASS_COUNT
	};

	class JniHelper
	{
	public:
		static JavaVM* getJavaVM();
		static void setJavaVM(JavaVM *javaVM);
		static jclass getClassID(const char *className, JNIEnv *env=0);
		static bool getStaticMethodInfo(JniMethodInfo &methodinfo, const char *className, const char *methodName, const char *paramCode);
		static bool getMethodInfo(JniMethodInfo &methodinfo, const char *className, const char *methodName, const char *paramCode);
		static std::string jstring2string(jstring str);
		static int init();
        static std::vector<jclass> m_class_reference;
	private:
		static JavaVM *m_psJavaVM;
	};
	
	class AutoJni
	{
	public:
		AutoJni();
		JNIEnv *    env;
		~AutoJni();
	private:
		bool isAttached ;
	};
}

#endif // __ANDROID_JNI_HELPER_H__
