//
//  MGLocationManager.h
//  System
//
//  Created by King Asdy on 11-10-19.
//  Copyright (c) 2011年 Zhejiang University. All rights reserved.
//

#pragma once

#include "../../CommonBase/MGObject.h"
#include "../Shared/Syncable.h"
#include "../../CommonBase/CommonDef.h"
#include "MGLocationRegion.h"
#include <set>
#include <string>
#include <deque>

using namespace std;

namespace z11 {

    class MGLocation;
    class MGHeading;
    class MGLocationRegion;
    class MGBeacon;
    class MGLocationManagerDelegate;
	class MKSimulateGPSReaderThread;
    class MGLocationManager : public MGObject
    {
    public:
        MGLocationManager();
        virtual ~MGLocationManager();
    protected:
        static set<MGLocationManager*> location_pool;
        static Syncable location_pool_locker;
        static set<MGLocationManager*> heading_pool;
        static Syncable heading_pool_locker;
        static set<MGLocationManager*> monitor_pool;
        static Syncable monitor_pool_locker;
        static MGLocation last_location;
        static MGHeading last_heading;
        static set<MGLocationRegion> already_in_regions;
		static MKSimulateGPSReaderThread *reader_thread;
    public:
        void startUpdatingLocation();
		void startUpdatingLocationWithSimulateGPSData(const std::string &simulate_data, int simulate_speed=1000);
        void stopUpdatingLocation();
        void startUpdatingHeading();
        void stopUpdatingHeading();
        void startMonitoringForRegion();
        void stopMonitoringForRegion();
        void startRangingForRegion();
        void stopRangingForRegion();
        void setDelegate(MGLocationManagerDelegate* delegate);
    public:
        static void didUpdateLocation(const MGLocation& to_location, const MGLocation& from_location);
        static void didUpdateHeading(const MGHeading& new_heading);
        static void didEnterRegion(const MGLocationRegion& region);
        static void didExitRegion(const MGLocationRegion& region);
        static void didRangeBeacons(const deque<MGBeacon> beacons);
        static void didUpdateLocationFailWithError(const std::string &error);
    private:
        bool is_updating_location;
        bool is_updating_heading;
        bool is_monitoring_for_region;
        MGLocationManagerDelegate* delegate;
    public:
        string beacon_uuid;
    };
    typedef set<MGLocationManager*>::iterator ManagerPoolIter;
    MIINTERFACE MGLocationManagerDelegate
    {
    public:
        virtual void updatingLocation(const MGLocation& to_location, const MGLocation& from_location) = 0;
        virtual void updatingHeading(const MGHeading& new_heading) = 0;
        virtual void enteringRegion(const MGLocationRegion& region){};
        virtual void exitingRegion(const MGLocationRegion& region){};
        virtual void rangingBeacons(const deque<MGBeacon> beacons){};
        // 位置管理器无法确定位置，就会触发“updatingLocationFailWithError”事件 //
        virtual void updatingLocationFailWithError(const std::string &error){};
    };
#define MGLOCATIONMANAGER_DELEGAGE_DECLARE \
virtual void updatingLocation(const MGLocation& to_location, const MGLocation& from_location); \
virtual void updatingHeading(const MGHeading& new_heading);
} //namespace z11
