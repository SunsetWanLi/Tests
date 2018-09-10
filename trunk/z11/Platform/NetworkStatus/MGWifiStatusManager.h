//
//  MGWifiStatusManager.h
//  System
//
//  Created by King Asdy on 13-01-16.
//  Copyright (c) 2012年 Zhejiang University. All rights reserved.
//

#pragma once

#include "../../CommonBase/MGObject.h"
#include "../../System/Shared/Syncable.h"
#include "../../CommonBase/CommonDef.h"
#include "MGNetworkReachability.h"
#include <set>
#include <string>

using namespace std;

namespace z11 {

    class MGWifiStatusManagerDelegate;
    class MGWifiStatusManager : public MGObject
    {
    public:
        MGWifiStatusManager();
        virtual ~MGWifiStatusManager();
    protected:
        static set<MGWifiStatusManager*> wifi_manager_pool;
        static Syncable wifi_manager_pool_locker;
        static vector<WifiStatusNode> wifi_status;
    public:
        vector<WifiStatusNode> getWifiStatus();
        void updateWifiStatus();
        void stopUpdatingWifiStatus();
        void setDelegate(MGWifiStatusManagerDelegate* delegate);
    public:
        static void didUpdateWifiStatus(const vector<WifiStatusNode>& new_wifi_status);
    private:
        MGWifiStatusManagerDelegate* delegate;
    };
    typedef set<MGWifiStatusManager*>::iterator WifiStatusManagerPoolIter;
    MIINTERFACE MGWifiStatusManagerDelegate
    {
    public:
        virtual void updatingWifiStatus(const vector<WifiStatusNode>& new_wifi_status) = 0;
    };
#define MGWIFISTATUSMANAGER_DELEGAGE_DECLARE \
virtual void updatingWifiStatus(const vector<WifiStatusNode>& new_wifi_status);
} //namespace z11
