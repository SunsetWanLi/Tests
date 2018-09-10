#ifndef __ANDROID_ASSET_MANAGER_JNI_H__
#define __ANDROID_ASSET_MANAGER_JNI_H__
#include "../../../CommonBase/MGData.h"
extern "C"

{
	extern z11::MGData *assetNativeReadFromAssetManager(const char* filename);
}

#endif // __ANDROID_ASSET_MANAGER_JNI_H__
