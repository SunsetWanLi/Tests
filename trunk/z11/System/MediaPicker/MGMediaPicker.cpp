//
//  MGMediaPicker.cpp
//  System
//
//  Created by DanYuan Hu on 11-12-29.
//  Copyright (c) 2011Äê Zhejiang University. All rights reserved.
//

#include "MGMediaPicker.h"
#include "../../CommonBase/MGDictionary.h"
#include "../../GUI/Events/MIEventDefine.h"
#include "../../GUI/Events/MIEvent.h"
#include "../../GUI/GuiContainer/GuiContainer.h"
#include "../../GraphicCore/MGImage.h"

namespace z11 {
	bool MGMediaPicker::m_is_media_opened = false;
    Syncable MGMediaPicker::media_locker;
	MGMediaPickerType MGMediaPicker::mediaPickerType;
	set<MGMediaPicker*> MGMediaPicker::image_pool;
	set<MGMediaPicker*> MGMediaPicker::sound_pool;
	set<MGMediaPicker*> MGMediaPicker::video_pool;
	Syncable MGMediaPicker::image_pool_locker;
	Syncable MGMediaPicker::sound_pool_locker;
	Syncable MGMediaPicker::video_pool_locker;
	
	MGMediaPicker::MGMediaPicker()
	{
		m_is_image_picking = false;
		m_is_sound_picking = false;
		m_is_video_picking = false;
		delegate = NULL;
	}

	MGMediaPicker::~MGMediaPicker()
	{
		if (m_is_image_picking) {
			stopPickingImage();
		}
		if (m_is_sound_picking) {
			stopPickingSound();
		}
		if (m_is_video_picking) {
			stopPickingVideo();
		}
		setDelegate(NULL);
	}
	void MGMediaPicker::setDelegate(MGMediaPickerDelegate* dlg)
	{
		image_pool_locker.lock();
		sound_pool_locker.lock();
		video_pool_locker.lock();
		delegate = dlg;
		video_pool_locker.unlock();
		sound_pool_locker.unlock();
		image_pool_locker.unlock();
	}
	void MGMediaPicker::openMedia()
	{
        media_locker.lock();
//		if (!m_is_media_opened) {
			MGMediaPickerType *type = new MGMediaPickerType;
			*type = mediaPickerType;
			MIEvent* event = new MIEvent;
			event->_event_type = MEDIA_PICK_EVENT;
			event->_responder = NULL;
			event->_content = type;
			GuiContainer::container->preserveEvent(event);
//			m_is_media_opened = true;
//		}
        media_locker.unlock();
	}
	void MGMediaPicker::closeMedia()
	{
//        if (m_is_media_opened) {
			MGMediaPickerType *type = new MGMediaPickerType;
			*type = mediaPickerType;
			MIEvent* event = new MIEvent;
			event->_event_type = MEDIA_DROP_EVENT;
			event->_responder = NULL;
			event->_content = type;
			GuiContainer::container->preserveEvent(event);
//            m_is_media_opened = false;
//        }
	}

	void MGMediaPicker::startPickingImage()
	{
		image_pool_locker.lock();
		ManagerPool::iterator it = image_pool.find(this);
		if (it == image_pool.end()) {
			image_pool.insert(this);
		}
		m_is_image_picking = true;
		image_pool_locker.unlock();
	}
	void MGMediaPicker::stopPickingImage()
	{
		image_pool_locker.lock();
		ManagerPool::iterator it = image_pool.find(this);
		if (it != image_pool.end()) {
			image_pool.erase(it);
		}
		m_is_image_picking = false;
		image_pool_locker.unlock();
	}
	void MGMediaPicker::startPickingSound()
	{
		sound_pool_locker.lock();
		ManagerPool::iterator it = sound_pool.find(this);
		if (it == sound_pool.end()) {
			sound_pool.insert(this);
		}
		m_is_sound_picking = true;
		sound_pool_locker.unlock();
	}
	void MGMediaPicker::stopPickingSound()
	{
		sound_pool_locker.lock();
		ManagerPool::iterator it = sound_pool.find(this);
		if (it != sound_pool.end()) {
			sound_pool.erase(it);
		}
		m_is_sound_picking = false;
		sound_pool_locker.unlock();
	}
	void MGMediaPicker::startPickingVideo()
	{
		video_pool_locker.lock();
		ManagerPool::iterator it = video_pool.find(this);
		if (it == video_pool.end()) {
			video_pool.insert(this);
		}
		m_is_video_picking = true;
		video_pool_locker.unlock();
	}
	void MGMediaPicker::stopPickingVideo()
	{
		video_pool_locker.lock();
		ManagerPool::iterator it = video_pool.find(this);
		if (it != video_pool.end()) {
			video_pool.erase(it);
		}
		m_is_video_picking = false;
		video_pool_locker.unlock();
	}

	void MGMediaPicker::didFinishPickingImageWithInfo(MGDictionary *info)
	{
		image_pool_locker.lock();
		ManagerPool::iterator it;
		for (it = image_pool.begin(); it != image_pool.end(); ++it) {
			if ((*it)->delegate != NULL) {
				(*it)->delegate->didFinishPickingMediaWithInfo(info);
			}
		}
		image_pool_locker.unlock();
        media_locker.lock();
        m_is_media_opened = false;
        media_locker.unlock();
	}
	void MGMediaPicker::didCancelPickingImageWithInfo(MGDictionary *info)
	{
		image_pool_locker.lock();
		ManagerPool::iterator it;
		for (it = image_pool.begin(); it != image_pool.end(); ++it) {
			if ((*it)->delegate != NULL) {
				(*it)->delegate->didCancelPickingMediaWithInfo(info);
			}
		}
		image_pool_locker.unlock();
        media_locker.lock();
        m_is_media_opened = false;
        media_locker.unlock();
	}

	void MGMediaPicker::didFinishPickingSoundWithInfo(MGDictionary *info)
	{
		sound_pool_locker.lock();
		ManagerPool::iterator it;
		for (it = sound_pool.begin(); it != sound_pool.end(); ++it) {
			if ((*it)->delegate != NULL) {
				(*it)->delegate->didFinishPickingMediaWithInfo(info);
			}
		}
		sound_pool_locker.unlock();
        media_locker.lock();
        m_is_media_opened = false;
        media_locker.unlock();
	}
	void MGMediaPicker::didCancelPickingSoundWithInfo(MGDictionary *info)
	{
		sound_pool_locker.lock();
		ManagerPool::iterator it;
		for (it = sound_pool.begin(); it != sound_pool.end(); ++it) {
			if ((*it)->delegate != NULL) {
				(*it)->delegate->didCancelPickingMediaWithInfo(info);
			}
		}
		sound_pool_locker.unlock();
        media_locker.lock();
        m_is_media_opened = false;
        media_locker.unlock();
	}

	void MGMediaPicker::didFinishPickingVideoWithInfo(MGDictionary *info)
	{
		video_pool_locker.lock();
		ManagerPool::iterator it;
		for (it = video_pool.begin(); it != video_pool.end(); ++it) {
			if ((*it)->delegate != NULL) {
				(*it)->delegate->didFinishPickingMediaWithInfo(info);
			}
		}
		video_pool_locker.unlock();
        media_locker.lock();
        m_is_media_opened = false;
        media_locker.unlock();
	}
	void MGMediaPicker::didCancelPickingVideoWithInfo(MGDictionary *info)
	{
		video_pool_locker.lock();
		ManagerPool::iterator it;
		for (it = video_pool.begin(); it != video_pool.end(); ++it) {
			if ((*it)->delegate != NULL) {
				(*it)->delegate->didCancelPickingMediaWithInfo(info);
			}
		}
		video_pool_locker.unlock();
        media_locker.lock();
        m_is_media_opened = false;
        media_locker.unlock();
	}
} //namespace z11
