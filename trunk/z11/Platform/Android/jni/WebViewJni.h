#ifndef __ANDROID_WEB_VIEW_JNI_H__
#define __ANDROID_WEB_VIEW_JNI_H__

#include "../../../CommonBase/MGStructure.h"

extern "C"
{
	extern void jniWebViewOpenURL(const std::string &url, z11::MGRect rect, const std::string &post_body);
	extern void jniWebViewShow();
	extern void jniWebViewClose();
}
#endif
