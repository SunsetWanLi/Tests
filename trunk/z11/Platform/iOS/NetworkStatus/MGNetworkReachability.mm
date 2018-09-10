
#import "../../NetworkStatus/MGNetworkReachability.h"
#import "Reachability.h"
#import "WifiStatus.h"
#import "../../NetworkStatus/MGWifiStatusManager.h"
#import "../../../System/Shared/Utility.h"
#import "../../../System/Shared/MGLog.h"

namespace z11 {
    
    MGNetworkReachability::MGNetworkReachability()
    {
        
    }
    
    MGNetworkReachability::~MGNetworkReachability()
    {
        
    }
    
    bool MGNetworkReachability::isWifiNetworkEnabled()
    {
        bool ret = false;
        Reachability* reach = [Reachability reachabilityForLocalWiFi];
        if (reach != nil) {
            ret = ([reach currentReachabilityStatus] != NotReachable);
        }
        [reach release];
        return ret;
    }
    
    bool MGNetworkReachability::isCarrierNetworkEnabled()
    {
        bool ret = false;
        Reachability* reach = [Reachability reachabilityForInternetConnection];
        if (reach != nil) {
            ret = ([reach currentReachabilityStatus] != NotReachable);
        }
        [reach release];
        return ret;
    }
    
    MGNetworkStatus MGNetworkReachability::reachabilityWithHostName(const string &host)
    {
        MGNetworkStatus ret = MGNotReachable;
        NSString* host_name = [NSString stringWithCString: host.c_str()   encoding:NSUTF8StringEncoding];
        Reachability *r = [Reachability reachabilityWithHostName:host_name];
        switch ([r currentReachabilityStatus]) {
            case NotReachable:
                // 没有网络连接
                ret = MGNotReachable;
                break;
            case ReachableViaWWAN:
                // 使用3G网络
                ret = MGReachableViaWWAN;
                break;
            case ReachableViaWiFi:
                // 使用WiFi网络
                ret = MGReachableViaWiFi;
                break;
        }
        [r release];
        return ret;
    }
    
    MGNetworkStatus MGNetworkReachability::reachabilityWithAddress(const struct sockaddr_in* ip)
    {
        MGNetworkStatus ret = MGNotReachable;
        Reachability *r = [Reachability reachabilityWithAddress:ip];
        switch ([r currentReachabilityStatus]) {
            case NotReachable:
                // 没有网络连接
                ret = MGNotReachable;
                break;
            case ReachableViaWWAN:
                // 使用3G网络
                ret = MGReachableViaWWAN;
                break;
            case ReachableViaWiFi:
                // 使用WiFi网络
                ret = MGReachableViaWiFi;
                break;
        }
        [r release];
        return ret;
    }
    
    void MGNetworkReachability::refreshWifiStatus()
    {
        
    }
    
    void MGNetworkReachability::receiveWifiStatus(const string& wifi_status_str)
    {
        WifiStatus * wifi_manager = [[WifiStatus alloc] init];
        [wifi_manager scanNetworks];
        NSDictionary* networks = [wifi_manager networks];
        NSMutableString* result = [[NSMutableString alloc] init];
        for (id key in networks){
            [result appendString:[NSString stringWithFormat:@"%@,%@,%@,%@,%@;",
                                  key, //Station BBSID (MAC Address)
                                  [[networks objectForKey: key] objectForKey:@"SSID_STR"], //Station Name
                                  [[networks objectForKey: key] objectForKey:@"CHANNEL"], // FIXME, using capabilities.
                                  [[networks objectForKey: key] objectForKey:@"CHANNEL"],  //Operating Channel
                                  [[networks objectForKey: key] objectForKey:@"RSSI"] //Signal Strength
                                  ]];
        }
        NSLog(@"%@", result);
        string str = [result cStringUsingEncoding:(NSUTF8StringEncoding)];
        [result release];
        [wifi_manager release];
        
        vector<WifiStatusNode> ret;
        vector<string> nodes_str = Utility::SplitString(str, ";");
        for (vector<string>::iterator nodes_it = nodes_str.begin(); nodes_it != nodes_str.end(); ++nodes_it) {
            WifiStatusNode node;
            string n_str = *nodes_it;
            MGLogD("%s", n_str.c_str());
            vector<string> node_str = Utility::SplitString(n_str, ",");
            if (node_str.size() < 5) {
                continue;
            }
            node.bssid = node_str.at(0);
            node.ssid = node_str.at(1);
            node.capabilities = node_str.at(2);
            node.frequency = Utility::BuiltInTypeFromString<int>(node_str.at(3));
            node.rssi = Utility::BuiltInTypeFromString<int>(node_str.at(4));
            ret.push_back(node);
        }
        MGWifiStatusManager::didUpdateWifiStatus(ret);
    }
    
} //namespace z11