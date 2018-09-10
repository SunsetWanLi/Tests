#include "JniHelper.h"
#include "IntentJni.h"
#include "../../../System/Shared/MGLog.h"

using namespace z11;

void jniIntentOpenFileWithPath(const std::string &path)
{
    JniMethodInfo methodInfo;
    if(JniHelper::getStaticMethodInfo(methodInfo, "com/z11/mobile/framework/MGIntent", "openFile", "(Ljava/lang/String;)V"))
    {						
		MGLogD("jniIntentOpenFileWithPath success");
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, methodInfo.env->NewStringUTF(path.c_str()));
    }
	else
        MGLogD("jniIntentOpenFileWithPath Failed");
}

void jniIntentLaunchAppWithURL(const std::string &identity, const std::string &url)
{
	JniMethodInfo methodInfo;
    if(JniHelper::getStaticMethodInfo(methodInfo, "com/z11/mobile/framework/MGIntent", "openRemoteApp", "(Ljava/lang/String;Ljava/lang/String;)V"))
    {						
		MGLogD("jniIntentLauchAppWithURL success");
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, methodInfo.env->NewStringUTF(identity.c_str()), methodInfo.env->NewStringUTF(url.c_str()));
    }
	else
        MGLogD("jniIntentLauchAppWithURL Failed");
}
