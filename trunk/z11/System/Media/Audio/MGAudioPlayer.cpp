//
//  MGAudioPlayer.cpp
//  System
//
//  Created by King Asdy on 12-3-31.
//  Copyright (c) 2012年 Zhejiang University. All rights reserved.
//
#include "MGAudioPlayer.h"
#include "MGAudioPlayerDelegate.h"
#include "../../../GUI/Events/MIEvent.h"
#include "../../../GUI/GuiContainer/GuiContainer.h"
#include "../../../System/Shared/MGLog.h"
#include <stdio.h>

using namespace z11;

map<string, MGAudioPlayer*> MGAudioPlayer::audio_manager;
Syncable MGAudioPlayer::manager_locker;

MGAudioPlayer* MGAudioPlayer::initWithContentsOfURL(const string &url)
{
    MGAudioPlayer* ret = NULL;
    manager_locker.lock();
    {
        ret = new MGAudioPlayer();
        char point[128];
        sprintf(point, "%p", ret);
        ret->audio_player_id = point;
        ret->audio_file_path = url;
        audio_manager[ret->audio_player_id] = ret;
    }
    if(ret != NULL)
    {
        MIEvent* event = new MIEvent;
        event->_event_type = INIT_AUDIO_PLAY_EVENT;
        MGAudioPlayer* player = new MGAudioPlayer();
        *player = *ret;
        event->_content = player;
        GuiContainer::container->preserveEvent(event);
    }
    manager_locker.unlock();
    return ret;
}

MGAudioPlayer* MGAudioPlayer::initWithData()
{
    MGAudioPlayer* ret = NULL;
    return ret;
}

void MGAudioPlayer::audioPlayerDidFinishPlaying(const string &id)
{
    MGLogD("1audioPlayerDidFinishPlaying==%s", id.c_str());
    manager_locker.lock();
    AudioIter iter = audio_manager.find(id);
    if(iter != audio_manager.end())
    {
        if(iter->second != NULL)
        {
			iter->second->playing = false;
            if (iter->second->delegate != NULL){
                iter->second->delegate->audioPlayerDidFinishPlaying(iter->second);
			}		
        }
    }
    manager_locker.unlock();
}

string MGAudioPlayer::getAudioFilePath()
{
    return audio_file_path;
}

string MGAudioPlayer::getAudioPlayerID()
{
    return audio_player_id;
}

MGAudioPlayer::MGAudioPlayer()
{
    delegate = NULL;
    playing = false;
    audio_file_path = "";
    volume = 1.0;
    number_of_loops = 0;
    pan = 0.0;
    rate = 1.0;
	duration = -1;
}

MGAudioPlayer::~MGAudioPlayer()
{
    manager_locker.lock();
    AudioIter iter = audio_manager.find(audio_player_id);
    if(iter != audio_manager.end() && iter->second == this)
        audio_manager.erase(iter);
    delegate = NULL;
    manager_locker.unlock();
}

void MGAudioPlayer::play()
{
    MIEvent* event = new MIEvent;
    event->_event_type = PLAY_AUDIO_PLAY_EVENT;
    MGAudioPlayer* player = new MGAudioPlayer();
    *player = *this;
    event->_content = player;
    GuiContainer::container->preserveEvent(event);
    playing = true;
}


void MGAudioPlayer::pause()
{
    MIEvent* event = new MIEvent;
    event->_event_type = PAUSE_AUDIO_PLAY_EVENT;
    MGAudioPlayer* player = new MGAudioPlayer();
    *player = *this;
    event->_content = player;
    GuiContainer::container->preserveEvent(event);
    playing = false;
}

void MGAudioPlayer::stop()
{
    MIEvent* event = new MIEvent;
    event->_event_type = STOP_AUDIO_PLAY_EVENT;
    MGAudioPlayer* player = new MGAudioPlayer();
    *player = *this;
    event->_content = player;
    GuiContainer::container->preserveEvent(event);
    playing = false;
}

void MGAudioPlayer::prepareToPlay()
{
    MIEvent* event = new MIEvent;
    event->_event_type = PREPARE_AUDIO_PLAY_EVENT;
    MGAudioPlayer* player = new MGAudioPlayer();
    *player = *this;
    event->_content = player;
    GuiContainer::container->preserveEvent(event);
}

void MGAudioPlayer::uninitPlayer()
{
    MIEvent* event = new MIEvent;
    event->_event_type = UNINIT_AUDIO_PLAY_EVENT;
    MGAudioPlayer* player = new MGAudioPlayer();
    *player = *this;
    event->_content = player;
    GuiContainer::container->preserveEvent(event);
}

void z11::MGAudioPlayer::rewind()
{
	MIEvent* event = new MIEvent;
	event->_event_type = REWIND_AUDIO_PLAY_EVENT;
	MGAudioPlayer* player = new MGAudioPlayer();
	*player = *this;
	event->_content = player;
	GuiContainer::container->preserveEvent(event);
}

int MGAudioPlayer::getDuration()
{
	MIEvent* event = new MIEvent;
	event->_event_type = GET_AUDIO_DURATION_EVENT;
	MGAudioPlayer* player = new MGAudioPlayer();
	*player = *this;
	event->_content = player;
	GuiContainer::container->preserveEvent(event);

	return duration;
}