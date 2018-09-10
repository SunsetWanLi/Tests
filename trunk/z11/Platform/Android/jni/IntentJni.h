#ifndef __ANDROID_INTENT_JNI_H__
#define __ANDROID_INTENT_JNI_H__

extern "C"
{
	extern void jniIntentOpenFileWithPath(const std::string &path);
	extern void jniIntentLaunchAppWithURL(const std::string &identity, const std::string &url);
}
#endif
