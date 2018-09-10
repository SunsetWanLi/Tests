#pragma once
#include "../../Platform/NetworkStatus/MGNetworkReachability.h"
#include <stdio.h>
#include <string.h>
#include "../../System/Shared/MGLog.h"

namespace z11
{
    MGNetworkReachability::MGNetworkReachability()
    {
        
    }
    
    MGNetworkReachability::~MGNetworkReachability()
    {
        
    }
    
    bool MGNetworkReachability::isWifiNetworkEnabled()
    {
        bool ret = true;
        
        return ret;
    }
    
    bool MGNetworkReachability::isCarrierNetworkEnabled()
    {
        bool ret = false;
        
        return ret;
    }

    MGNetworkStatus MGNetworkReachability::reachabilityWithHostName(const string &host)
    {
        MGNetworkStatus ret = MGNotReachable;
        if(isWifiNetworkEnabled())
            ret = MGReachableViaWiFi;
        else if(isCarrierNetworkEnabled())
            ret = MGReachableViaWWAN;
        return ret;
    }
    
    MGNetworkStatus MGNetworkReachability::reachabilityWithAddress(const struct sockaddr_in* ip)
    {
        MGNetworkStatus ret = MGNotReachable;
        if(isWifiNetworkEnabled())
            ret = MGReachableViaWiFi;
        else if(isCarrierNetworkEnabled())
            ret = MGReachableViaWWAN;
        return ret;
    }
    
    void MGNetworkReachability::refreshWifiStatus()
    {

    }

	void MGNetworkReachability::receiveWifiStatus(const string& wifi_status)
	{

	}
} //namespace z11