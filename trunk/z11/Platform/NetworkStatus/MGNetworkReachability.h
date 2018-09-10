//
//  MGNetworkReachability.h
//  System
//
//  Created by King Asdy on 11-10-19.
//  Copyright (c) 2011年 Zhejiang University. All rights reserved.
//

#pragma once

#include "../../CommonBase/MGObject.h"
#include "../../CommonBase/CommonDef.h"
#include "../../CommonBase/MGStructure.h"
#include <string>
#include <vector>

#if defined(_WIN32) || defined(_WIN64)
#ifdef WINCE
#include <winsock2.h>
#include <windows.h>
#else
#include <winsock2.h>
#include <windows.h>
#endif
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif 

using namespace std;

namespace z11 {
    
    typedef enum
    {
        MGNotReachable,
        MGReachableViaWiFi,
        MGReachableViaWWAN
    }MGNetworkStatus;
    
    class WifiStatusNode
    {
    public:
        string bssid;
        string ssid;
        string capabilities;
        int frequency;
        int rssi;
    };
    
    class MGNetworkReachability : public MGObject
    {
    public:
        MGNetworkReachability();
        virtual ~MGNetworkReachability();
    public:
        static bool isWifiNetworkEnabled();
        static bool isCarrierNetworkEnabled();
        static MGNetworkStatus reachabilityWithHostName(const string& host);
        static MGNetworkStatus reachabilityWithAddress(const struct sockaddr_in* ip);
        static void refreshWifiStatus();
        static void receiveWifiStatus(const string &wifiStatus);
    };
} //namespace z11
