//
//  MGMotionManager.h
//  System
//
//  Created by King Asdy on 13-01-07.
//  Copyright (c) 2012年 Zhejiang University. All rights reserved.
//

#pragma once

#include "../../CommonBase/MGObject.h"
#include "../Shared/Syncable.h"
#include "../../CommonBase/CommonDef.h"
#include <set>
#include <string>

using namespace std;

namespace z11 {

    class MGAcceleration;
    class MGRotation;
	class MGAttitude;
    class MGMotionManagerDelegate;
    class MGMotionManager : public MGObject
    {
    public:
        MGMotionManager();
        virtual ~MGMotionManager();
    protected:
        static set<MGMotionManager*> acceleration_pool;
        static Syncable acceleration_pool_locker;
        static set<MGMotionManager*> rotation_pool;
        static Syncable rotation_pool_locker;
		static set<MGMotionManager*> attitude_pool;
		static Syncable attitude_pool_locker;
        static MGAcceleration last_acceleration;
        static MGRotation last_rotation;
		static MGAttitude last_attitude;
    public:
        void startUpdatingAcceleration(const float interval = 0.0f);
        void stopUpdatingAcceleration();
        void startUpdatingRotation(const float interval = 0.0f);
        void stopUpdatingRotation();
		void startUpdatingAttitude(const float interval = 0.0f);
		void stopUpdatingAttitude();
        void setDelegate(MGMotionManagerDelegate* delegate);
    public:
        static void didUpdateAcceleration(const MGAcceleration& new_acceleration);
        static void didUpdateRotation(const MGRotation& new_rotation);
		static void didUpdateAttitude(const MGAttitude& new_attitude);
    private:
        bool is_updating_acceleration;
        bool is_updating_rotation;
		bool is_updating_attitude;
        MGMotionManagerDelegate* delegate;
    };
    typedef set<MGMotionManager*>::iterator MotionManagerPoolIter;
    MIINTERFACE MGMotionManagerDelegate
    {
    public:
        virtual void updatingAcceleration(const MGAcceleration& new_acceleration) = 0;
        virtual void updatingRotation(const MGRotation& new_rotation) = 0;
		virtual void updatingAttitude(const MGAttitude& new_attitude) = 0;
    };
#define MGMOTIONMANAGER_DELEGAGE_DECLARE \
virtual void updatingAcceleration(const MGAcceleration& new_acceleration); \
virtual void updatingRotation(const MGRotation& new_rotation); \
virtual void updatingAttitude(const MGAttitude& new_attitude);
} //namespace z11
