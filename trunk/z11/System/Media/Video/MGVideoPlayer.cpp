//
//  MGVideoPlayer.cpp
//  System
//
//  Created by King Asdy on 12-10-17.
//  Copyright (c) 2012年 Zhejiang University. All rights reserved.
//

#include "MGVideoPlayer.h"
#include "MGVideoPlayerDelegate.h"
#include "../../../GUI/Events/MIEvent.h"
#include "../../../GUI/GuiContainer/GuiContainer.h"
#include "../../../System/Shared/MGLog.h"

using namespace z11;

map<string, MGVideoPlayer*> MGVideoPlayer::video_manager;
Syncable MGVideoPlayer::manager_locker;
int MGVideoPlayer::preview_width = 0;
int MGVideoPlayer::preview_height = 0;
MGImage *MGVideoPlayer::m_preview_image = NULL;
MGVideoPlayer* MGVideoPlayer::initWithContentsOfURL(const string &url)
{
    MGVideoPlayer* ret = NULL;
    manager_locker.lock();
    VideoIter iter = video_manager.find(url);
    if(iter != video_manager.end())
    {
        ret = iter->second;
    }else {
        ret = new MGVideoPlayer();
        ret->video_file_path = url;
        video_manager[url] = ret;
    }
    if(ret != NULL)
    {
        MIEvent* event = new MIEvent;
        event->_event_type = INIT_VIDEO_PLAY_EVENT;
        MGVideoPlayer* player = new MGVideoPlayer();
        *player = *ret;
        event->_content = player;
        GuiContainer::container->preserveEvent(event);
    }
    manager_locker.unlock();
    return ret;
}

void MGVideoPlayer::videoPlayerDidFinishPlaying(const string &path)
{
    MGLogD("videoPlayerDidFinishPlaying==%s", path.c_str());
    manager_locker.lock();
    VideoIter iter = video_manager.find(path);
    if(iter != video_manager.end())
    {
        if(iter->second != NULL)
        {
            iter->second->playing = false;
            if (iter->second->delegate != NULL)
                iter->second->delegate->videoPlayerDidFinishPlaying(iter->second);
        }
    }
    manager_locker.unlock();
}

void MGVideoPlayer::videoPlayerDidFinishPlayingByUser(const string &path)
{
    MGLogD("videoPlayerDidFinishPlayingByUser==%s", path.c_str());
    manager_locker.lock();
    VideoIter iter = video_manager.find(path);
    if(iter != video_manager.end())
    {
        if(iter->second != NULL)
        {
            iter->second->playing = false;
            if (iter->second->delegate != NULL)
                iter->second->delegate->videoPlayerDidFinishPlayingByUser(iter->second);
        }
    }
    manager_locker.unlock();
}
MGVideoPlayer::MGVideoPlayer()
{
    playing = false;
    video_file_path = "";
    delegate = NULL;
    scaling_mode = MGMovieScalingModeNone;
    control_mode = MGMovieControlModeFullscreen;
}

MGVideoPlayer::~MGVideoPlayer()
{
    manager_locker.lock();
	if(m_preview_image != NULL)
		delete_and_set_null(m_preview_image);
    VideoIter iter = video_manager.find(video_file_path);
    if(iter != video_manager.end())
        video_manager.erase(iter);
    if (isPlaying()) {
        stop();
    }
    delegate = NULL;
    manager_locker.unlock();
}

void MGVideoPlayer::play()
{
    MIEvent* event = new MIEvent;
    event->_event_type = PLAY_VIDEO_PLAY_EVENT;
    MGVideoPlayer* player = new MGVideoPlayer();
    *player = *this;
    event->_content = player;
    GuiContainer::container->preserveEvent(event);
    playing = true;
}

void MGVideoPlayer::stop()
{
    MIEvent* event = new MIEvent;
    event->_event_type = STOP_VIDEO_PLAY_EVENT;
    GuiContainer::container->preserveEvent(event);
    playing = false;
}


MGImage* MGVideoPlayer::getPreviewImage(const string &url, int imageWidth , int imageHeight)
{
	preview_width = imageWidth; 
	preview_height = imageHeight;
	if(m_preview_image!=NULL)
		delete_and_set_null(m_preview_image);
	if(preview_width>0&& preview_height >0){
		MIEvent* event = new MIEvent;
		event->_event_type = GET_VIDEO_PREVIEW_EVENT;
		MGTextContent *obj = new MGTextContent();
		obj->text_content = url ; 
		event->_content = obj ;
		GuiContainer::container->preserveEvent(event);

		MGImage *img = m_preview_image;
		m_preview_image=NULL;
		return img;
	}
	else{
		MGLogD("Layout image size <0 !");
		return NULL;
	}
}

void MGVideoPlayer::setPreviewImage(MGImage *img)
{
	m_preview_image = img;
}