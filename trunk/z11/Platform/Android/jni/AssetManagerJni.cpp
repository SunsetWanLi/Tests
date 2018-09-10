
#include "AssetManagerJni.h"
#include "../../MGFileUtils.h"
#include <jni.h>
#include "../../../CommonBase/MGData.h"
#include "JniHelper.h"
#include "../../../System/Shared/MGLog.h"
#include <android/api-level.h>
// for native asset manager
#if __ANDROID_API__ >= 9

#include <sys/types.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#endif
using namespace z11;

extern "C"
{
	jobject gAssetManager=NULL;
	void assetNativeSetAssetManager(JNIEnv* env, jclass clazz,
        jobject assetManager)
	{
		gAssetManager=assetManager;
	}
	MGData *assetNativeReadFromAssetManager(const char* filename)
	{
		MGData *data=MGFileUtils::sharedFileUtils()->getFileData(filename);
		return data;
	}
}

