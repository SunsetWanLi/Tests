/*
 * =====================================================================================
 *
 *       Filename:  MediaPlayerJni.cpp
 *
 *    Description:  call Android  API -- Media Player
 *
 *        Version:  1.0
 *        Created:  04/01/2012 03:14:31 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#include "JniHelper.h"
#include "MediaPlayerJni.h"
#include "../../../System/Shared/MGLog.h"
#include "../../../System/Media/Audio/MGAudioPlayer.h"
#include "../../../System/Media/Video/MGVideoPlayer.h"
using namespace z11;

void jniAudioPlayer(const string &method_string, const string &player_id)
{
    JniMethodInfo t;
    t.classID=JniHelper::m_class_reference[JNI_CLASS_MEDIA_PLAYER];//g_web_service_class;
    bool isAttached = false;
    
    int status = JniHelper::getJavaVM()->GetEnv((void **) &t.env, JNI_VERSION_1_4);
    if(status < 0) {
        status = JniHelper::getJavaVM()->AttachCurrentThread(&t.env, NULL);
        if(status < 0) {
            return ;
        }
        isAttached = true;
    }
    
    t.methodID = t.env->GetStaticMethodID(t.classID, method_string.c_str(), "(Ljava/lang/String;)V");
    
    if(t.methodID!=NULL)
    {
        t.env->CallStaticVoidMethod(t.classID,t.methodID,t.env->NewStringUTF(player_id.c_str()));
    }
    
    std::string output = method_string+":"+player_id+"over";
    MGLogD(output.c_str());
    
    if(isAttached)
        JniHelper::getJavaVM()->DetachCurrentThread();
}

void initAudioPlayer(const string &player_id, const string &media_file_path)
{
	JniMethodInfo t;
	t.classID = JniHelper::m_class_reference[JNI_CLASS_MEDIA_PLAYER];
	bool isAttached = false;

	int status = JniHelper::getJavaVM()->GetEnv((void **) &t.env, JNI_VERSION_1_4);
	if(status < 0) {
		status = JniHelper::getJavaVM()->AttachCurrentThread(&t.env, NULL);
		if(status < 0) {
			return ;
		}
		isAttached = true;
	}

	t.methodID = t.env->GetStaticMethodID(t.classID, "initAudio", "(Ljava/lang/String;Ljava/lang/String;)V");

	if(t.methodID != NULL) 
	{
		t.env->CallStaticVoidMethod(t.classID, t.methodID, t.env->NewStringUTF(player_id.c_str()), t.env->NewStringUTF(media_file_path.c_str()));
	}

	string output = "initMediaPlayer "+player_id+"over";
	MGLogD(output.c_str());

	if(isAttached)
		JniHelper::getJavaVM()->DetachCurrentThread();
}

extern void prepareAudioPlayer(const string &player_id, float pan, float volume, float rate, int number_of_loops)
{
    JniMethodInfo t;
	t.classID = JniHelper::m_class_reference[JNI_CLASS_MEDIA_PLAYER];
	bool isAttached = false;
    
	int status = JniHelper::getJavaVM()->GetEnv((void **) &t.env, JNI_VERSION_1_4);
	if(status < 0) {
		status = JniHelper::getJavaVM()->AttachCurrentThread(&t.env, NULL);
		if(status < 0) {
			return ;
		}
		isAttached = true;
	}
    
	t.methodID = t.env->GetStaticMethodID(t.classID, "prepareAudio", "(Ljava/lang/String;FFFI)V");
    
	if(t.methodID != NULL)
	{
		t.env->CallStaticVoidMethod(t.classID, t.methodID, t.env->NewStringUTF(player_id.c_str()), pan, volume, rate, number_of_loops);
	}
    
	string output = "prepareAudio "+player_id+"over";
	MGLogD(output.c_str());
    
	if(isAttached)
		JniHelper::getJavaVM()->DetachCurrentThread();
}
void setAudioDuration(const string &player_id, const string &media_file_path)
{
	JniMethodInfo t;
	t.classID = JniHelper::m_class_reference[JNI_CLASS_MEDIA_PLAYER];
	bool isAttached = false;

	int status = JniHelper::getJavaVM()->GetEnv((void **) &t.env, JNI_VERSION_1_4);
	if(status < 0) {
		status = JniHelper::getJavaVM()->AttachCurrentThread(&t.env, NULL);
		if(status < 0) {
			return ;
		}
		isAttached = true;
	}

	t.methodID = t.env->GetStaticMethodID(t.classID, "setDuration", "(Ljava/lang/String;Ljava/lang/String;)V");

	if(t.methodID != NULL) 
	{
		t.env->CallStaticVoidMethod(t.classID, t.methodID, t.env->NewStringUTF(player_id.c_str()), t.env->NewStringUTF(media_file_path.c_str()));
	}

	//string output = "initMediaPlayer "+player_id+"over";
	//MGLogD(output.c_str());

	if(isAttached)
		JniHelper::getJavaVM()->DetachCurrentThread();
}
void playAudioPlayer(const string &player_id)
{
	jniAudioPlayer("playAudio", player_id);
}

void pauseAudioPlayer(const string &player_id)
{
	jniAudioPlayer("pauseAudio", player_id);
}

void stopAudioPlayer(const string &player_id)
{
	jniAudioPlayer("stopAudio", player_id);
}

void uninitAudioPlayer(const string &player_id)
{
	jniAudioPlayer("uninitAudio", player_id);
}

void initVideoPlayer(const string &file_path)
{
	JniMethodInfo t;
	t.classID = JniHelper::m_class_reference[JNI_CLASS_MEDIA_PLAYER];
	bool isAttached = false;

	int status = JniHelper::getJavaVM()->GetEnv((void **) &t.env, JNI_VERSION_1_4);
	if(status < 0) 
	{
		status = JniHelper::getJavaVM()->AttachCurrentThread(&t.env, NULL);
		if(status < 0)
			return;
		isAttached = true;
	}

	t.methodID = t.env->GetStaticMethodID(t.classID, "initVideo", "(Ljava/lang/String;)V");
	if(t.methodID!=NULL) 
	{
		t.env->CallStaticVoidMethod(t.classID, t.methodID, t.env->NewStringUTF(file_path.c_str()));
		MGLogD("initVideo %s: call method", file_path.c_str());
	}

	if(isAttached)
		JniHelper::getJavaVM()->DetachCurrentThread();
}

void playVideoPlayer()
{
	JniMethodInfo t;
	t.classID = JniHelper::m_class_reference[JNI_CLASS_MEDIA_PLAYER];
	bool isAttached = false;

	int status = JniHelper::getJavaVM()->GetEnv((void **) &t.env, JNI_VERSION_1_4);
	if(status < 0) 
	{
		status = JniHelper::getJavaVM()->AttachCurrentThread(&t.env, NULL);
		if(status < 0)
			return;
		isAttached = true;
	}

	t.methodID = t.env->GetStaticMethodID(t.classID, "playVideo", "()V");
	if(t.methodID!=NULL) 
	{
		t.env->CallStaticVoidMethod(t.classID, t.methodID);
		MGLogD("playVideo : call method");
	}

	if(isAttached)
		JniHelper::getJavaVM()->DetachCurrentThread();
}

void stopVideoPlayer()
{
	JniMethodInfo t;
	t.classID = JniHelper::m_class_reference[JNI_CLASS_MEDIA_PLAYER];
	bool isAttached = false;
    
	int status = JniHelper::getJavaVM()->GetEnv((void **) &t.env, JNI_VERSION_1_4);
	if(status < 0)
	{
		status = JniHelper::getJavaVM()->AttachCurrentThread(&t.env, NULL);
		if(status < 0)
			return;
		isAttached = true;
	}
    
	t.methodID = t.env->GetStaticMethodID(t.classID, "stopVideo", "()V");
	if(t.methodID!=NULL)
	{
		t.env->CallStaticVoidMethod(t.classID, t.methodID);
		MGLogD("stopVideo : call method");
	}
    
	if(isAttached)
		JniHelper::getJavaVM()->DetachCurrentThread();
}
void getPreviewImageFromFile(const string &file_path, int width, int height)
{
 	JniMethodInfo t;
 	t.classID = JniHelper::m_class_reference[JNI_CLASS_MEDIA_PLAYER];
 	bool isAttached = false;
 
 	int status = JniHelper::getJavaVM()->GetEnv((void **) &t.env, JNI_VERSION_1_4);
 	if(status < 0) 
 	{
 		status = JniHelper::getJavaVM()->AttachCurrentThread(&t.env, NULL);
 		if(status < 0)
 			return;
 		isAttached = true;
 	}
 
 	t.methodID = t.env->GetStaticMethodID(t.classID, "getPreviewImage", "(Ljava/lang/String;II)V");
 	if(t.methodID!=NULL) 
 	{
 		t.env->CallStaticVoidMethod(t.classID, t.methodID, t.env->NewStringUTF(file_path.c_str()),width,height);
 		//MGLogD("initVideo %s: call method", file_path.c_str());
 	}
 
 	if(isAttached)
 		JniHelper::getJavaVM()->DetachCurrentThread();
}
extern "C"
{
    void Java_com_z11_mobile_framework_MGMediaPlayer_audioPlayerDidFinishPlaying(JNIEnv* env, jobject thiz, jstring player_id)
    {
        MGAudioPlayer::audioPlayerDidFinishPlaying(JniHelper::jstring2string(player_id));
    }
    
    void Java_com_z11_mobile_framework_MGMediaPlayer_videoPlayerDidFinishPlaying(JNIEnv* env, jobject thiz, jstring file_full_path)
    {
        MGVideoPlayer::videoPlayerDidFinishPlaying(JniHelper::jstring2string(file_full_path));
    }
    
    void Java_com_z11_mobile_framework_MGMediaPlayer_videoPlayerDidFinishPlayingByUser(JNIEnv* env, jobject thiz, jstring file_full_path)
    {
        MGVideoPlayer::videoPlayerDidFinishPlayingByUser(JniHelper::jstring2string(file_full_path));
    }

	void Java_com_z11_mobile_framework_MGMediaPlayer_getPreviewData(JNIEnv* env, jobject thiz,  jbyteArray bytes, jint length)
	{
		jboolean isCopy = 0;
		MGData* data = NULL;
		unsigned char* _data = (unsigned char*)env->GetByteArrayElements(bytes, &isCopy);
		if (_data != NULL)
		{
			data = MGData::dataWithBytesNoCopy(_data, length, false);
		}
		MGImage *image = MGImage::imageWithData(data) ;
        delete_and_set_null(data);
		MGVideoPlayer::setPreviewImage(image);
	}

	void Java_com_z11_mobile_framework_MGMediaPlayer_setAudioDuration(JNIEnv* env, jobject thiz, jstring file_id,jint len)
	{
		auto it = MGAudioPlayer::audio_manager.find(JniHelper::jstring2string(file_id));
		if(it!=MGAudioPlayer::audio_manager.end())
		{
			MGLogD("find!!!!!!!!   %d",len);
			it->second->setDuration(len);
		}
	}
}