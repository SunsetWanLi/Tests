//
//  MGMotionManager.cpp
//  System
//
//  Created by King Asdy on 13-01-07.
//  Copyright (c) 2012年 Zhejiang University. All rights reserved.
//

#include "MGMotionManager.h"
#include "../../GUI/Events/MIEventDefine.h"
#include "../../GUI/Events/MIEvent.h"
#include "../../GUI/GuiContainer/GuiContainer.h"
#include "MGAcceleration.h"
#include "MGRotation.h"
#include "MGAttitude.h"
#include "../Shared/MGLog.h"

namespace z11 {
    set<MGMotionManager*> MGMotionManager::acceleration_pool;
    Syncable MGMotionManager::acceleration_pool_locker;
    set<MGMotionManager*> MGMotionManager::rotation_pool;
    Syncable MGMotionManager::rotation_pool_locker;
	set<MGMotionManager*> MGMotionManager::attitude_pool;
	Syncable MGMotionManager::attitude_pool_locker;
    MGAcceleration MGMotionManager::last_acceleration;
    MGRotation MGMotionManager::last_rotation;
	MGAttitude MGMotionManager::last_attitude;
    MGMotionManager::MGMotionManager()
    {
        is_updating_acceleration = false;
        is_updating_rotation = false;
		is_updating_attitude = false;
        delegate = NULL;
    }
    
    MGMotionManager::~MGMotionManager()
    {
        if (is_updating_acceleration) {
            stopUpdatingAcceleration();
        }
        if (is_updating_rotation) {
            stopUpdatingRotation();
        }
		if (is_updating_attitude) {
			stopUpdatingAttitude();
		}
        delegate = NULL;
    }
    void MGMotionManager::setDelegate(MGMotionManagerDelegate* dlg)
    {
        acceleration_pool_locker.lock();
        rotation_pool_locker.lock();
		attitude_pool_locker.lock();
        delegate = dlg;
		attitude_pool_locker.unlock();
        rotation_pool_locker.unlock();
        acceleration_pool_locker.unlock();
    }
    void MGMotionManager::startUpdatingAcceleration(const float interval)
    {
        acceleration_pool_locker.lock();
        if (acceleration_pool.size() == 0) {
            MGIntervalContent* content = new MGIntervalContent();
            content->interval = interval;
            MIEvent* event = new MIEvent();
            event->_event_type = START_ACCELERATION_SERVICE_EVENT;
            event->_responder = NULL;
            event->_content = content;
            GuiContainer::container->preserveEvent(event);
        }
        bool need_reupdate = false;
        if(acceleration_pool.size() > 0)
        {
            need_reupdate = true;
        }
        MotionManagerPoolIter it = acceleration_pool.find(this);
        if (it == acceleration_pool.end()) {
            acceleration_pool.insert(this);
        }
        if(need_reupdate)
        {
            didUpdateAcceleration(last_acceleration);
        }
        is_updating_acceleration = true;
        acceleration_pool_locker.unlock();
    }
    void MGMotionManager::stopUpdatingAcceleration()
    {
        acceleration_pool_locker.lock();
        MotionManagerPoolIter it = acceleration_pool.find(this);
        if (it != acceleration_pool.end()) {
            acceleration_pool.erase(this);
        }
        if (acceleration_pool.size() == 0) {
            MIEvent* event = new MIEvent;
            event->_event_type = STOP_ACCELERATION_SERVICE_EVENT;
            event->_responder = NULL;
            GuiContainer::container->preserveEvent(event);
            is_updating_acceleration = false;
        }
        rotation_pool_locker.lock();
		attitude_pool_locker.lock();
        if (acceleration_pool.size() == 0 && rotation_pool.size() == 0 && attitude_pool.size() == 0) {
            MIEvent* event = new MIEvent;
            event->_event_type = RELEASE_MOTION_SERVICE_EVENT;
            event->_responder = NULL;
            GuiContainer::container->preserveEvent(event);
        }
		attitude_pool_locker.unlock();
        rotation_pool_locker.unlock();
        acceleration_pool_locker.unlock();
    }
    void MGMotionManager::startUpdatingRotation(const float interval)
    {
        rotation_pool_locker.lock();
        if (rotation_pool.size() == 0) {
            MGIntervalContent* content = new MGIntervalContent();
            content->interval = interval;
            MIEvent* event = new MIEvent;
            event->_event_type = START_ROTATION_SERVICE_EVENT;
            event->_responder = NULL;
            event->_content = content;
            GuiContainer::container->preserveEvent(event);
        }
        bool need_reupdate = false;
        if(rotation_pool.size() > 0)
        {
            need_reupdate = true;
        }
        MotionManagerPoolIter it = rotation_pool.find(this);
        if (it == rotation_pool.end()) {
            rotation_pool.insert(this);
        }
        if(need_reupdate)
        {
            didUpdateRotation(last_rotation);
        }
        is_updating_rotation = true;
        rotation_pool_locker.unlock();
    }
    void MGMotionManager::stopUpdatingRotation()
    {
        rotation_pool_locker.lock();
        MotionManagerPoolIter it = rotation_pool.find(this);
        if (it != rotation_pool.end()) {
            rotation_pool.erase(this);
        }
        if (rotation_pool.size() == 0) {
            MIEvent* event = new MIEvent;
            event->_event_type = STOP_ROTATION_SERVICE_EVENT;
            event->_responder = NULL;
            GuiContainer::container->preserveEvent(event);
            is_updating_rotation = false;
        }
        acceleration_pool_locker.lock();
		attitude_pool_locker.lock();
        if (acceleration_pool.size() == 0 && rotation_pool.size() == 0 && attitude_pool.size() == 0) {
            MIEvent* event = new MIEvent;
            event->_event_type = RELEASE_MOTION_SERVICE_EVENT;
            event->_responder = NULL;
            GuiContainer::container->preserveEvent(event);
        }
		attitude_pool_locker.unlock();
        acceleration_pool_locker.unlock();
        rotation_pool_locker.unlock();
    }
	void MGMotionManager::startUpdatingAttitude(const float interval)
	{
		attitude_pool_locker.lock();
		if (attitude_pool.size() == 0) {
			MGIntervalContent* content = new MGIntervalContent();
			content->interval = interval;
			MIEvent* event = new MIEvent;
			event->_event_type = START_ATTITUDE_SERVICE_EVENT;
			event->_responder = NULL;
			event->_content = content;
			GuiContainer::container->preserveEvent(event);
		}
		bool need_reupdate = false;
		if(attitude_pool.size() > 0)
		{
			need_reupdate = true;
		}
		MotionManagerPoolIter it = attitude_pool.find(this);
		if (it == attitude_pool.end()) {
			attitude_pool.insert(this);
		}
		if(need_reupdate)
		{
			didUpdateAttitude(last_attitude);
		}
		is_updating_attitude = true;
		attitude_pool_locker.unlock();
	}
	void MGMotionManager::stopUpdatingAttitude()
	{
		attitude_pool_locker.lock();
		MotionManagerPoolIter it = attitude_pool.find(this);
		if (it != attitude_pool.end()) {
			attitude_pool.erase(this);
		}
		if (attitude_pool.size() == 0) {
			MIEvent* event = new MIEvent;
			event->_event_type = STOP_ATTITUDE_SERVICE_EVENT;
			event->_responder = NULL;
			GuiContainer::container->preserveEvent(event);
			is_updating_attitude = false;
		}
		acceleration_pool_locker.lock();
		rotation_pool_locker.lock();
		if (acceleration_pool.size() == 0 && rotation_pool.size() == 0 && attitude_pool.size() == 0) {
			MIEvent* event = new MIEvent;
			event->_event_type = RELEASE_MOTION_SERVICE_EVENT;
			event->_responder = NULL;
			GuiContainer::container->preserveEvent(event);
		}
		rotation_pool_locker.unlock();
		acceleration_pool_locker.unlock();
		attitude_pool_locker.unlock();
	}
    void MGMotionManager::didUpdateAcceleration(const z11::MGAcceleration &new_acceleration)
    {
        acceleration_pool_locker.lock();
        MotionManagerPoolIter it;
        for (it = acceleration_pool.begin(); it != acceleration_pool.end(); ++it) {
            if ((*it)->delegate != NULL) {
                (*it)->delegate->updatingAcceleration(new_acceleration);
            }
        }
        last_acceleration = new_acceleration;
        acceleration_pool_locker.unlock();
    }
    void MGMotionManager::didUpdateRotation(const z11::MGRotation &new_rotation)
    {
        rotation_pool_locker.lock();
        MotionManagerPoolIter it;
        for (it = rotation_pool.begin(); it != rotation_pool.end(); ++it) {
            if ((*it)->delegate != NULL) {
                (*it)->delegate->updatingRotation(new_rotation);
            }
        }
        last_rotation = new_rotation;
        rotation_pool_locker.unlock();
    }
	void MGMotionManager::didUpdateAttitude(const z11::MGAttitude &new_attitude)
	{
		attitude_pool_locker.lock();
		MotionManagerPoolIter it;
		for (it = attitude_pool.begin(); it != attitude_pool.end(); ++it) {
			if ((*it)->delegate != NULL) {
				(*it)->delegate->updatingAttitude(new_attitude);
			}
		}
		last_attitude = new_attitude;
		attitude_pool_locker.unlock();
	}
} //namespace z11
