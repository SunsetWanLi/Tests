#include "JniHelper.h"
#include "MediaRecorderJni.h"
#include "../../../System/Shared/MGLog.h"
#include "../../../System/Media/Recorder/MGMediaRecorder.h"

using namespace z11;

void jniMediaRecorderStartWithFileName(const std::string &file_name)
{
    JniMethodInfo methodInfo;
    if(JniHelper::getStaticMethodInfo(methodInfo, "com/z11/mobile/framework/MGMediaRecorderManager", "start", "(Ljava/lang/String;)V"))
    {													
        MGLogD("jniMediaRecorderStartWithFileName success");
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, methodInfo.env->NewStringUTF(file_name.c_str()));
    }
    else
        MGLogD("jniMediaRecorderStartWithFileName Failed");
}
void jniMediaRecorderStop()
{
	JniMethodInfo methodInfo;
    if(JniHelper::getStaticMethodInfo(methodInfo, "com/z11/mobile/framework/MGMediaRecorderManager", "stop", "()V"))
    {													
        MGLogD("jniMediaRecorderStop success");
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
    }
    else
        MGLogD("jniMediaRecorderStop Failed");
}

void jniMediaRecorderUpdateMeters()
{
	JniMethodInfo methodInfo;
	if(JniHelper::getStaticMethodInfo(methodInfo, "com/z11/mobile/framework/MGMediaRecorderManager", "updateMeters", "()V"))
	{													
		MGLogD("jniMediaRecorderUpdateMeters success");
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
	}
	else{
		MGLogD("jniMediaRecorderUpdateMeters Failed");
	}
}


void mediaNativeUpdateMetersJNI(JNIEnv* env, jobject thiz, jfloat value)
{
	MGMediaRecorder::setChannelValue(value);
}
