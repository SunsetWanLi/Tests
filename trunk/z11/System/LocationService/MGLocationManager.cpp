//
//  MGLocationManager.cpp
//  System
//
//  Created by King Asdy on 11-10-19.
//  Copyright (c) 2011年 Zhejiang University. All rights reserved.
//

#include "MGLocationManager.h"
#include "../../GUI/Events/MIEventDefine.h"
#include "../../GUI/Events/MIEvent.h"
#include "../../GUI/GuiContainer/GuiContainer.h"
#include "MGLocation.h"
#include "MGHeading.h"
#include "../Shared/MGLog.h"
#include "MKSimulateGPSReader.h"
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288 
#endif
namespace z11 {
    set<MGLocationManager*> MGLocationManager::location_pool;
    Syncable MGLocationManager::location_pool_locker;
    set<MGLocationManager*> MGLocationManager::heading_pool;
    Syncable MGLocationManager::heading_pool_locker;
    set<MGLocationManager*> MGLocationManager::monitor_pool;
    Syncable MGLocationManager::monitor_pool_locker;
    MGLocation MGLocationManager::last_location;
    MGHeading MGLocationManager::last_heading;
    set<MGLocationRegion> MGLocationManager::already_in_regions;
	MKSimulateGPSReaderThread* MGLocationManager::reader_thread=NULL;
    MGLocationManager::MGLocationManager()
    {
        is_updating_location = false;
        is_updating_heading = false;
        delegate = NULL;
        beacon_uuid = "";
    }
    
    MGLocationManager::~MGLocationManager()
    {
        if (is_updating_location) {
            stopUpdatingLocation();
        }
        if (is_updating_heading) {
            stopUpdatingHeading();
        }
        delegate = NULL;
    }
    void MGLocationManager::setDelegate(MGLocationManagerDelegate* dlg)
    {
        location_pool_locker.lock();
        heading_pool_locker.lock();
        monitor_pool_locker.lock();
        delegate = dlg;
        monitor_pool_locker.unlock();
        heading_pool_locker.unlock();
        location_pool_locker.unlock();
    }
    void MGLocationManager::startUpdatingLocation()
    {
        location_pool_locker.lock();
        if (location_pool.size() == 0) {
            MIEvent* event = new MIEvent;
            event->_event_type = START_LOCATION_SERVICE_EVENT;
            event->_responder = NULL;
            GuiContainer::container->preserveEvent(event);
        }
        bool need_reupdate = false;
        if(location_pool.size() > 0)
        {
            need_reupdate = true;
        }
        ManagerPoolIter it = location_pool.find(this);
        if (it == location_pool.end()) {
            location_pool.insert(this);
        }
        if(need_reupdate)
        {
            didUpdateLocation(last_location, last_location);
        }
        is_updating_location = true;
        location_pool_locker.unlock();
    }
    void MGLocationManager::stopUpdatingLocation()
    {
        location_pool_locker.lock();
        ManagerPoolIter it = location_pool.find(this);
        if (it != location_pool.end()) {
            location_pool.erase(this);
        }
        if (location_pool.size() == 0) {

			if (reader_thread!=NULL)
			{
                reader_thread->requestExitAndWait();
				delete_and_set_null(reader_thread);
			}
			else
			{
				MIEvent* event = new MIEvent;
				event->_event_type = STOP_LOCATION_SERVICE_EVENT;
				event->_responder = NULL;
				GuiContainer::container->preserveEvent(event);
			}
            is_updating_location = false;
        }
        heading_pool_locker.lock();
        monitor_pool_locker.lock();
        if (location_pool.size() == 0 && heading_pool.size() == 0 && monitor_pool.size() == 0) {
            MIEvent* event = new MIEvent;
            event->_event_type = RELEASE_LOCATION_SERVICE_EVENT;
            event->_responder = NULL;
            GuiContainer::container->preserveEvent(event);
        }
        monitor_pool_locker.unlock();
        heading_pool_locker.unlock();
        location_pool_locker.unlock();
    }
    void MGLocationManager::startUpdatingHeading()
    {
        heading_pool_locker.lock();
        if (heading_pool.size() == 0) {
            MIEvent* event = new MIEvent;
            event->_event_type = START_HEADING_SERVICE_EVENT;
            event->_responder = NULL;
            GuiContainer::container->preserveEvent(event);
        }
        bool need_reupdate = false;
        if(heading_pool.size() > 0)
        {
            need_reupdate = true;
        }
        ManagerPoolIter it = heading_pool.find(this);
        if (it == heading_pool.end()) {
            heading_pool.insert(this);
        }
        if(need_reupdate)
        {
            didUpdateHeading(last_heading);
        }
        is_updating_heading = true;
        heading_pool_locker.unlock();
    }
    void MGLocationManager::stopUpdatingHeading()
    {
        heading_pool_locker.lock();
        ManagerPoolIter it = heading_pool.find(this);
        if (it != heading_pool.end()) {
            heading_pool.erase(this);
        }
        if (heading_pool.size() == 0) {
            MIEvent* event = new MIEvent;
            event->_event_type = STOP_HEADING_SERVICE_EVENT;
            event->_responder = NULL;
            GuiContainer::container->preserveEvent(event);
            is_updating_heading = false;
        }
        location_pool_locker.lock();
        monitor_pool_locker.lock();
        if (location_pool.size() == 0 && heading_pool.size() == 0 && monitor_pool.size() == 0) {
            MIEvent* event = new MIEvent;
            event->_event_type = RELEASE_LOCATION_SERVICE_EVENT;
            event->_responder = NULL;
            GuiContainer::container->preserveEvent(event);
        }
        monitor_pool_locker.unlock();
        location_pool_locker.unlock();
        heading_pool_locker.unlock();
    }
    void MGLocationManager::startMonitoringForRegion()
    {
        monitor_pool_locker.lock();
        if (monitor_pool.size() == 0) {
            MIEvent* event = new MIEvent;
            event->_event_type = START_MONITORING_FOR_REGION_EVENT;
            event->_responder = NULL;
            MGTextContent* context = new MGTextContent();
            context->text_content = beacon_uuid;
            event->_content = context;
            GuiContainer::container->preserveEvent(event);
        }
        bool need_reupdate = false;
        if(monitor_pool.size() > 0)
        {
            need_reupdate = true;
        } else {
            already_in_regions.clear();
        }
        ManagerPoolIter it = monitor_pool.find(this);
        if (it == monitor_pool.end()) {
            monitor_pool.insert(this);
        }
        if(need_reupdate)
        {
            for (auto it = already_in_regions.begin(); it != already_in_regions.end(); ++it) {
                didEnterRegion(*it);
            }
        }
        is_monitoring_for_region = true;
        monitor_pool_locker.unlock();
    }
    void MGLocationManager::stopMonitoringForRegion()
    {
        monitor_pool_locker.lock();
        ManagerPoolIter it = monitor_pool.find(this);
        if (it != monitor_pool.end()) {
            monitor_pool.erase(this);
        }
        if (monitor_pool.size() == 0) {
            MIEvent* event = new MIEvent;
            event->_event_type = STOP_MONITORING_FOR_REGION_EVENT;
            event->_responder = NULL;
            GuiContainer::container->preserveEvent(event);
            is_monitoring_for_region = false;
        }
        location_pool_locker.lock();
        heading_pool_locker.lock();
        if (location_pool.size() == 0 && heading_pool.size() == 0 && monitor_pool.size() == 0) {
            MIEvent* event = new MIEvent;
            event->_event_type = RELEASE_LOCATION_SERVICE_EVENT;
            event->_responder = NULL;
            GuiContainer::container->preserveEvent(event);
        }
        heading_pool_locker.lock();
        location_pool_locker.lock();
        monitor_pool_locker.unlock();
    }
    void MGLocationManager::startRangingForRegion()
    {
        monitor_pool_locker.lock();
        if (monitor_pool.size() == 0) {
            MIEvent* event = new MIEvent;
            event->_event_type = START_RANGING_FOR_REGION_EVENT;
            event->_responder = NULL;
            MGTextContent* context = new MGTextContent();
            context->text_content = beacon_uuid;
            event->_content = context;
            GuiContainer::container->preserveEvent(event);
        }
        bool need_reupdate = false;
        if(monitor_pool.size() > 0)
        {
            need_reupdate = true;
        } else {
            already_in_regions.clear();
        }
        ManagerPoolIter it = monitor_pool.find(this);
        if (it == monitor_pool.end()) {
            monitor_pool.insert(this);
        }
        if(need_reupdate)
        {
            for (auto it = already_in_regions.begin(); it != already_in_regions.end(); ++it) {
                didEnterRegion(*it);
            }
        }
        is_monitoring_for_region = true;
        monitor_pool_locker.unlock();
    }
    void MGLocationManager::stopRangingForRegion()
    {
        monitor_pool_locker.lock();
        ManagerPoolIter it = monitor_pool.find(this);
        if (it != monitor_pool.end()) {
            monitor_pool.erase(this);
        }
        if (monitor_pool.size() == 0) {
            MIEvent* event = new MIEvent;
            event->_event_type = STOP_RANGING_FOR_REGION_EVENT;
            event->_responder = NULL;
            GuiContainer::container->preserveEvent(event);
            is_monitoring_for_region = false;
        }
        location_pool_locker.lock();
        heading_pool_locker.lock();
        if (location_pool.size() == 0 && heading_pool.size() == 0 && monitor_pool.size() == 0) {
            MIEvent* event = new MIEvent;
            event->_event_type = RELEASE_LOCATION_SERVICE_EVENT;
            event->_responder = NULL;
            GuiContainer::container->preserveEvent(event);
        }
        heading_pool_locker.lock();
        location_pool_locker.lock();
        monitor_pool_locker.unlock();
    }
    void MGLocationManager::didUpdateLocation(const MGLocation& to_location, const MGLocation& from_location)
    {
        location_pool_locker.lock();
		MGLocation _to_location=to_location;
		if (reader_thread!=NULL) //模拟数据 //
		{
			//to_location.course=from_location.
			//double eps=1e-6;
			//if ((_to_location.longitude-from_location.longitude<eps)&&(_to_location.longitude-from_location.longitude>-eps))
			//{
			//	_to_location.course=0;
			//}
			//else
			//{
			//	_to_location.course=atan2((_to_location.latitude-from_location.latitude),(_to_location.longitude-from_location.longitude))/M_PI*180;
			//}
			_to_location.course=90-atan2((_to_location.latitude-from_location.latitude),(_to_location.longitude-from_location.longitude))/M_PI*180;
			if (_to_location.course<0)
			{
				_to_location.course+=360;
			}
			
			
		}
		
        ManagerPoolIter it;
        for (it = location_pool.begin(); it != location_pool.end(); ++it) {
            if ((*it)->delegate != NULL) {
                (*it)->delegate->updatingLocation(_to_location, from_location);
            }
        }
        last_location = _to_location;
        location_pool_locker.unlock();
    }
    void MGLocationManager::didUpdateHeading(const z11::MGHeading &new_heading)
    {
        heading_pool_locker.lock();
        ManagerPoolIter it;
        for (it = heading_pool.begin(); it != heading_pool.end(); ++it) {
            if ((*it)->delegate != NULL) {
                (*it)->delegate->updatingHeading(new_heading);
            }
        }
        last_heading = new_heading;
        heading_pool_locker.unlock();
    }
    void MGLocationManager::didEnterRegion(const MGLocationRegion& region)
    {
        monitor_pool_locker.lock();
        ManagerPoolIter it;
        for (it = monitor_pool.begin(); it != monitor_pool.end(); ++it) {
            if ((*it)->delegate != NULL) {
                (*it)->delegate->enteringRegion(region);
            }
        }
        auto rit = already_in_regions.find(region);
        if (rit == already_in_regions.end()) {
            already_in_regions.insert(region);
        }
        monitor_pool_locker.unlock();
    }
    void MGLocationManager::didExitRegion(const MGLocationRegion& region)
    {
        monitor_pool_locker.lock();
        ManagerPoolIter it;
        for (it = monitor_pool.begin(); it != monitor_pool.end(); ++it) {
            if ((*it)->delegate != NULL) {
                (*it)->delegate->exitingRegion(region);
            }
        }
        auto rit = already_in_regions.find(region);
        if (rit != already_in_regions.end()) {
            already_in_regions.erase(region);
        }
        monitor_pool_locker.unlock();
    }
    void MGLocationManager::didRangeBeacons(const deque<z11::MGBeacon> beacons)
    {
        monitor_pool_locker.lock();
        ManagerPoolIter it;
        for (it = monitor_pool.begin(); it != monitor_pool.end(); ++it) {
            if ((*it)->delegate != NULL) {
                (*it)->delegate->rangingBeacons(beacons);
            }
        }
        monitor_pool_locker.unlock();
    }
    void MGLocationManager::didUpdateLocationFailWithError(const std::string &error)
    {
        location_pool_locker.lock();
        ManagerPoolIter it;
        for (it = location_pool.begin(); it != location_pool.end(); ++it) {
            if ((*it)->delegate != NULL) {
                (*it)->delegate->updatingLocationFailWithError(error);
            }
        }
        location_pool_locker.unlock();
    }

	void MGLocationManager::startUpdatingLocationWithSimulateGPSData( const std::string&simulate_data, int simulate_speed )
	{
		location_pool_locker.lock();
		if (location_pool.size() == 0) {
			if(reader_thread==NULL)
				reader_thread=new MKSimulateGPSReaderThread(simulate_data);
			reader_thread->simulate_speed = simulate_speed ;
			reader_thread->run();
			//MIEvent* event = new MIEvent;
			//event->_event_type = START_LOCATION_SERVICE_EVENT;
			//event->_responder = NULL;
			//GuiContainer::container->preserveEvent(event);
		}
		bool need_reupdate = false;
		if(location_pool.size() > 0)
		{
			need_reupdate = true;
		}
		ManagerPoolIter it = location_pool.find(this);
		if (it == location_pool.end()) {
			location_pool.insert(this);
		}
		if(need_reupdate)
		{
			didUpdateLocation(last_location, last_location);
		}
		is_updating_location = true;
		location_pool_locker.unlock();
	}

} //namespace z11
