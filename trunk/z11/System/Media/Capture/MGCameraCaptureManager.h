//
//  MGCameraCaptureManager.h
//  System
//
//  Created by King Asdy on 12-11-29.
//  Copyright (c) 2011年 Zhejiang University. All rights reserved.
//

#pragma once

#include "../../../CommonBase/MGObject.h"
#include "../../Shared/Syncable.h"
#include "../../../CommonBase/CommonDef.h"
#include "../../../GraphicCore/MGImage.h"
#include <set>
#include <string>

using namespace std;

namespace z11 {
    
    enum {
        MGCaptureDevicePositionUnspecified         = 0,
        MGCaptureDevicePositionBack                = 1,
        MGCaptureDevicePositionFront               = 2
    };
    typedef int MGCaptureDevicePosition;
    
    class MGCaptureDevicePositionContent : public MGObject
    {
    public:
        MGCaptureDevicePositionContent(MGCaptureDevicePosition position);
        MGCaptureDevicePosition position;
        MGPoint point;
    };

    class MGCameraCaptureManagerDelegate;
    class MGCameraCaptureManager : public MGObject
    {
    public:
        MGCameraCaptureManager();
        virtual ~MGCameraCaptureManager();
    protected:
        static set<MGCameraCaptureManager*> capture_pool;
        static Syncable capture_pool_locker;
    public:
        void focusPointOfInterest(const MGPoint& point, MGCaptureDevicePosition = MGCaptureDevicePositionUnspecified);
        void startUpdatingCameraCapture(MGCaptureDevicePosition = MGCaptureDevicePositionUnspecified);
        void stopUpdatingCameraCapture();
        void setDelegate(MGCameraCaptureManagerDelegate* delegate);
    public:
        static void didCameraCaptureOutput(const MGImage* const capture_image);
    private:
        bool is_updating_capture;
        MGCameraCaptureManagerDelegate* delegate;
    };
    typedef set<MGCameraCaptureManager*>::iterator CameraCaptureManagerPoolIter;
    MIINTERFACE MGCameraCaptureManagerDelegate
    {
    public:
        virtual void cameraCaptureOutputting(const MGImage* const capture_image) = 0;
    };
#define MGCAMERACAPTUREMANAGER_DELEGAGE_DECLARE \
    virtual void cameraCaptureOutputting(const MGImage* const capture_image);
} //namespace z11
