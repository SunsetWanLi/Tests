#pragma once
#include <string>
#include <map>
#include "../../../../System/Media/Audio/MGAudioPlayer.h"
#include "../../../../System/Shared/Syncable.h"
namespace z11
{
	class MciPlayer;
	class MciPlayerManager
	{
	public:
		typedef std::map<std::string ,MciPlayer *>::iterator AudioThreadMapIterator;
		static std::map<std::string ,MciPlayer *> m_audio_manager;
		static Syncable audio_lock;

		static void init(MGAudioPlayer * player);
		static void play(MGAudioPlayer * player, int Times=1);
		static void pause(MGAudioPlayer * player);
		static void stop(MGAudioPlayer * player);
		static void rewind(MGAudioPlayer * player);
		static void uninit(MGAudioPlayer * player);
		static void AudioDidFinish(MciPlayer *);
	};
}