#include "MciPlayerManager.h"
#include "MciPlayer.h"
#include "../../../../System/Media/Audio/MGAudioPlayer.h"
namespace z11
{
	std::map<std::string ,MciPlayer *> MciPlayerManager::m_audio_manager;
	Syncable MciPlayerManager::audio_lock;

	void MciPlayerManager::init(MGAudioPlayer * mg_player)
	{
		if (mg_player == NULL)
			return;
		audio_lock.lock();
		AudioThreadMapIterator it=m_audio_manager.find(mg_player->getAudioPlayerID());
		if (it==m_audio_manager.end())
		{
			MciPlayer *player=new MciPlayer();
			player->Open(mg_player->getAudioFilePath().c_str(),mg_player->getAudioPlayerID().c_str(),0);
			m_audio_manager[mg_player->getAudioPlayerID()]=player;
		}
		audio_lock.unlock();
	}

	void MciPlayerManager::play( MGAudioPlayer * mg_player,INT Times/*=1*/ )
	{
		if (mg_player == NULL)
			return;
		audio_lock.lock();
		AudioThreadMapIterator it=m_audio_manager.find(mg_player->getAudioPlayerID());
		if (it!=m_audio_manager.end())
		{
			if (it->second->IsPlaying())
			{
				it->second->Resume();
			}
			else
			{
				it->second->Play(Times);
			}
		}
		audio_lock.unlock();
	}

	void MciPlayerManager::pause( MGAudioPlayer * mg_player)
	{
		if (mg_player == NULL)
			return;
		audio_lock.lock();
		AudioThreadMapIterator it=m_audio_manager.find(mg_player->getAudioPlayerID());
		if (it!=m_audio_manager.end())
		{
			it->second->Pause();
		}
		audio_lock.unlock();
	}

	void MciPlayerManager::stop( MGAudioPlayer * mg_player)
	{
		if (mg_player == NULL)
			return;
		audio_lock.lock();
		AudioThreadMapIterator it=m_audio_manager.find(mg_player->getAudioPlayerID());
		if (it!=m_audio_manager.end())
		{
			it->second->Stop();
		}
		audio_lock.unlock();
	}

	void MciPlayerManager::rewind( MGAudioPlayer * mg_player )
	{
		if (mg_player == NULL)
			return;
		audio_lock.lock();
		AudioThreadMapIterator it=m_audio_manager.find(mg_player->getAudioPlayerID());
		if (it!=m_audio_manager.end())
		{
			it->second->Rewind();
		}
		audio_lock.unlock();
	}

	void MciPlayerManager::uninit( MGAudioPlayer * mg_player)
	{
		if (mg_player == NULL)
			return;
		audio_lock.lock();
		AudioThreadMapIterator it=m_audio_manager.find(mg_player->getAudioPlayerID());
		if (it!=m_audio_manager.end())
		{
			delete(it->second);
			it->second=NULL;
			m_audio_manager.erase(it);
		}
		audio_lock.unlock();
	}

	void MciPlayerManager::AudioDidFinish(MciPlayer * mg_player)
	{
		string id;
		for (AudioThreadMapIterator it=m_audio_manager.begin(); it != m_audio_manager.end(); ++it)
		{
			if (it->second == mg_player)
			{
				id = it->first;
				break;
			}
		}
		MGAudioPlayer::audioPlayerDidFinishPlaying(id);
	}
}