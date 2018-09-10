//
//  MGCaptureManager.cpp
//  System
//
//  Created by King Asdy on 12-11-29.
//  Copyright (c) 2011年 Zhejiang University. All rights reserved.
//

#include "MGCameraCaptureManager.h"
#include "../../../GUI/Events/MIEventDefine.h"
#include "../../../GUI/Events/MIEvent.h"
#include "../../../GUI/GuiContainer/GuiContainer.h"
#include "../../Shared/MGLog.h"
#include <math.h>

namespace z11 {
    set<MGCameraCaptureManager*> MGCameraCaptureManager::capture_pool;
    Syncable MGCameraCaptureManager::capture_pool_locker;
    
    MGCaptureDevicePositionContent::MGCaptureDevicePositionContent(MGCaptureDevicePosition pos)
    {
        position = pos;
    }
    
    MGCameraCaptureManager::MGCameraCaptureManager()
    {
        is_updating_capture = false;
        delegate = NULL;
    }
    
    MGCameraCaptureManager::~MGCameraCaptureManager()
    {
        if (is_updating_capture) {
            stopUpdatingCameraCapture();
        }
        delegate = NULL;
    }
    void MGCameraCaptureManager::setDelegate(MGCameraCaptureManagerDelegate* dlg)
    {
        capture_pool_locker.lock();
        delegate = dlg;
        capture_pool_locker.unlock();
    }
    void MGCameraCaptureManager::focusPointOfInterest(const MGPoint& point, MGCaptureDevicePosition pos)
    {
        capture_pool_locker.lock();
        if (capture_pool.size() > 0 && is_updating_capture == true) {
            MIEvent* event = new MIEvent;
            event->_event_type = ADJUST_CAMERA_CAPTURE_FOCUS_EVENT;
            MGCaptureDevicePositionContent* content = new MGCaptureDevicePositionContent(pos);
            content->point = point;
            event->_content = content;
            event->_responder = NULL;
            GuiContainer::container->preserveEvent(event);
        }
        capture_pool_locker.unlock();
    }
    void MGCameraCaptureManager::startUpdatingCameraCapture(MGCaptureDevicePosition position)
    {
        capture_pool_locker.lock();
        if (capture_pool.size() == 0) {
            MIEvent* event = new MIEvent;
            event->_event_type = START_CAMERA_CAPTURE_SERVICE_EVENT;
            event->_responder = NULL;
            event->_content = new MGCaptureDevicePositionContent(position);
            GuiContainer::container->preserveEvent(event);
        }
        CameraCaptureManagerPoolIter it = capture_pool.find(this);
        if (it == capture_pool.end()) {
            capture_pool.insert(this);
        }
        is_updating_capture = true;
        capture_pool_locker.unlock();
    }
    void MGCameraCaptureManager::stopUpdatingCameraCapture()
    {
        capture_pool_locker.lock();
        CameraCaptureManagerPoolIter it = capture_pool.find(this);
        if (it != capture_pool.end()) {
            capture_pool.erase(this);
        }
        if (capture_pool.size() == 0) {
            MIEvent* event = new MIEvent;
            event->_event_type = STOP_CAMERA_CAPTURE_SERVICE_EVENT;
            event->_responder = NULL;
            GuiContainer::container->preserveEvent(event);
            is_updating_capture = false;
        }
        capture_pool_locker.unlock();
    }
    
    void MGCameraCaptureManager::didCameraCaptureOutput(const MGImage* const capture_image)
    {
        capture_pool_locker.lock();
        CameraCaptureManagerPoolIter it;
        for (it = capture_pool.begin(); it != capture_pool.end(); ++it) {
            if ((*it)->delegate != NULL) {
                (*it)->delegate->cameraCaptureOutputting(capture_image);
            }
        }
        capture_pool_locker.unlock();
    }

} //namespace z11
