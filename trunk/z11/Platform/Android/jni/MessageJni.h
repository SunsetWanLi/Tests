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
#ifndef __ANDROID_MESSAGE_JNI_H__
#define __ANDROID_MESSAGE_JNI_H__
#include <string>
extern "C"
{
	extern bool preserveEventJNI();
	extern void showMessageBoxJNI(const char * pszMsg, const char * pszTitle);
    extern void showSoftInputJNI(const char* keyboard_type);
    extern void hideSoftInputJNI();
	extern void terminateProcessJNI();
    extern void appLockScreenWhenIdle();
    extern void appUnlockScreenWhenIdle();
    extern void appShowSystemStatusBar();
    extern void appHideSystemStatusBar();
    extern void NativeToJavaCommandAndData(int code,std::string parameter);
    extern void foreignerSDKApiCallJni(const char *api);
}
#endif // __ANDROID_MESSAGE_JNI_H__
