#ifndef __ANDROID_MEDIA_RECORDER_JNI_H__
#define __ANDROID_MEDIA_RECORDER_JNI_H__

extern "C"
{
	extern void jniMediaRecorderStartWithFileName(const std::string &file_name);
	extern void jniMediaRecorderStop();
	extern void jniMediaRecorderUpdateMeters();
	extern void mediaNativeUpdateMetersJNI(JNIEnv* env, jobject thiz, jfloat value);
}
#endif
