//
//  MGWifiStatusManager.cpp
//  System
//
//  Created by King Asdy on 13-01-07.
//  Copyright (c) 2012年 Zhejiang University. All rights reserved.
//

#include "MGWifiStatusManager.h"
#include "../../GUI/Events/MIEventDefine.h"
#include "../../GUI/Events/MIEvent.h"
#include "../../GUI/GuiContainer/GuiContainer.h"
#include "../../System/Shared/MGLog.h"

namespace z11 {
    
    set<MGWifiStatusManager*> MGWifiStatusManager::wifi_manager_pool;
    Syncable MGWifiStatusManager::wifi_manager_pool_locker;
    vector<WifiStatusNode> MGWifiStatusManager::wifi_status;
    
    MGWifiStatusManager::MGWifiStatusManager()
    {
        delegate = NULL;
    }
    
    MGWifiStatusManager::~MGWifiStatusManager()
    {
        stopUpdatingWifiStatus();
        delegate = NULL;
    }
    
    vector<WifiStatusNode> MGWifiStatusManager::getWifiStatus()
    {
        vector<WifiStatusNode> ret;
        wifi_manager_pool_locker.lock();
        ret = wifi_status;
        wifi_manager_pool_locker.unlock();
        return ret;
    }
    
    void MGWifiStatusManager::setDelegate(MGWifiStatusManagerDelegate* dlg)
    {
        wifi_manager_pool_locker.lock();
        delegate = dlg;
        wifi_manager_pool_locker.unlock();
    }
    void MGWifiStatusManager::updateWifiStatus()
    {
        wifi_manager_pool_locker.lock();
        
        MIEvent* event = new MIEvent;
        event->_event_type = SCAN_WIFI_STATUS_EVENT;
        event->_responder = NULL;
        GuiContainer::container->preserveEvent(event);

        WifiStatusManagerPoolIter it = wifi_manager_pool.find(this);
        if (it == wifi_manager_pool.end()) {
            wifi_manager_pool.insert(this);
        }
       
        wifi_manager_pool_locker.unlock();
    }
    void MGWifiStatusManager::stopUpdatingWifiStatus()
    {
        wifi_manager_pool_locker.lock();
        WifiStatusManagerPoolIter it = wifi_manager_pool.find(this);
        if (it != wifi_manager_pool.end()) {
            wifi_manager_pool.erase(this);
        }
        wifi_manager_pool_locker.unlock();
    }
    
    void MGWifiStatusManager::didUpdateWifiStatus(const vector<z11::WifiStatusNode> &new_wifi_status)
    {
        wifi_manager_pool_locker.lock();
        WifiStatusManagerPoolIter it;
        for (it = wifi_manager_pool.begin(); it != wifi_manager_pool.end(); ++it) {
            if ((*it)->delegate != NULL) {
                (*it)->delegate->updatingWifiStatus(new_wifi_status);
            }
        }
        wifi_status = new_wifi_status;
        wifi_manager_pool_locker.unlock();
    }

} //namespace z11
