/*
 * =====================================================================================
 *
 *       Filename:  MediaPlayerJni.h
 *
 *    Description:  call Android  API -- Media Player
 *
 *        Version:  1.0
 *        Created:  04/01/2012 03:14:58 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#ifndef __ANDROID_MEDIA_PLAYER_JNI_H__
#define __ANDROID_MEDIA_PLAYER_JNI_H__
#include <string>
using namespace std;
extern "C"
{
	extern void initAudioPlayer(const string &player_id, const string &media_file_path);
    extern void prepareAudioPlayer(const string &player_id, float pan, float volume, float rate, int number_of_loops);
	extern void playAudioPlayer(const string &player_id);
	extern void pauseAudioPlayer(const string &player_id);
	extern void stopAudioPlayer(const string &player_id);
	extern void uninitAudioPlayer(const string &player_id);
	extern void setAudioDuration(const string &player_id, const string &media_file_path);

	extern void initVideoPlayer(const string &file_path);
	extern void playVideoPlayer();
    extern void stopVideoPlayer();
	

	extern void getPreviewImageFromFile(const string &file_path, int width, int height);   
}
#endif
