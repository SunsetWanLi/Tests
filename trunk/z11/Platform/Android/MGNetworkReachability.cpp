#include "../../Platform/NetworkStatus/MGNetworkReachability.h"
#include "../../Platform/NetworkStatus/MGWifiStatusManager.h"
#include <stdio.h>
#include <string.h>
#include "../../System/Shared/Utility.h"
#include "../../System/Shared/MGLog.h"
#include "jni/JniHelper.h"

#ifndef JAVAVM
#define JAVAVM	z11::JniHelper::getJavaVM()
#endif

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
        bool ret = false;
        JniMethodInfo t;
        bool isAttached = false;
        int status = JniHelper::getJavaVM()->GetEnv((void **) &t.env, JNI_VERSION_1_4);
        if(status != JNI_OK) {
            status = JniHelper::getJavaVM()->AttachCurrentThread(&t.env, NULL);
            if(status < 0) {
                MGLogD("Failed to getJavaVM()->AttachCurrentThread in MGNerworkReachability::isWifiNetworkEnabled");
                return ret;
            }
            isAttached = true;
        }
        t.classID=JniHelper::m_class_reference[JNI_CLASS_NETWORKSTATUS];
        
        MGLogD("Ready to GetStaticMethodID isWifiNetworkEnabled");
        t.methodID = t.env->GetStaticMethodID(t.classID,"isWifiNetworkEnabled"  ,"()I");
        
        if(t.methodID!=NULL) 
        {
            ret = t.env->CallStaticIntMethod(t.classID,t.methodID);
        }
        
        if(isAttached)
            JniHelper::getJavaVM()->DetachCurrentThread();
        
        return ret;
    }
    
    bool MGNetworkReachability::isCarrierNetworkEnabled()
    {
        bool ret = false;
        JniMethodInfo t;
        bool isAttached = false;
        int status = JniHelper::getJavaVM()->GetEnv((void **) &t.env, JNI_VERSION_1_4);
        if(status != JNI_OK) {
            status = JniHelper::getJavaVM()->AttachCurrentThread(&t.env, NULL);
            if(status < 0) {
                MGLogD("Failed to getJavaVM()->AttachCurrentThread in MGNetworkReachability::isCarrierNetworkEnabled");
                return ret;
            }
            isAttached = true;
        }
        t.classID=JniHelper::m_class_reference[JNI_CLASS_NETWORKSTATUS];
        
        t.methodID = t.env->GetStaticMethodID(t.classID,"isCarrierNetworkEnabled"  ,"()I");
        
        if(t.methodID!=NULL) 
        {
            ret = t.env->CallStaticIntMethod(t.classID,t.methodID);
        }
        
        if(isAttached)
            JniHelper::getJavaVM()->DetachCurrentThread();
        
        return ret;
    }
    
    MGNetworkStatus MGNetworkReachability::reachabilityWithHostName(const string &host)
    {
        MGNetworkStatus ret = MGNotReachable;
        
        JniMethodInfo t;
        
        bool isAttached = false;
        int status = JniHelper::getJavaVM()->GetEnv((void **) &t.env, JNI_VERSION_1_4);
        if(status != JNI_OK) {
            status = JniHelper::getJavaVM()->AttachCurrentThread(&t.env, NULL);
            if(status < 0) {
                MGLogD("Failed to getJavaVM()->AttachCurrentThread in MGNetworkReachability::reachabilityWithHostName");
                return ret;
            }
            isAttached = true;
        }
        
        t.classID=JniHelper::m_class_reference[JNI_CLASS_NETWORKSTATUS];
        
        t.methodID = t.env->GetStaticMethodID(t.classID,"reachabilityWithHostName"  ,"(Ljava/lang/String;)I");
        
        if(t.methodID!=NULL)
        {
            ret = (MGNetworkStatus)t.env->CallStaticIntMethod(t.classID,t.methodID, t.env->NewStringUTF(host.c_str()));
        }
        
        if(isAttached)
            JniHelper::getJavaVM()->DetachCurrentThread();
        return ret;
    }
    
    MGNetworkStatus MGNetworkReachability::reachabilityWithAddress(const struct sockaddr_in* ip)
    {
        MGNetworkStatus ret = MGNotReachable;
        if(isWifiNetworkEnabled())
            ret = MGReachableViaWiFi;
        else if(isCarrierNetworkEnabled)
            ret = MGReachableViaWWAN;
        return ret;
    }
	
	void MGNetworkReachability::refreshWifiStatus()
	{
        JniMethodInfo t;
        bool isAttached = false;
        int status = JniHelper::getJavaVM()->GetEnv((void **) &t.env, JNI_VERSION_1_4);
        if(status != JNI_OK) {
            status = JniHelper::getJavaVM()->AttachCurrentThread(&t.env, NULL);
            if(status < 0) {
                MGLogD("Failed to getJavaVM()->AttachCurrentThread in MGNetworkReachability::refreshWifiStatus");
                return;
            }
            isAttached = true;
        }
        t.classID=JniHelper::m_class_reference[JNI_CLASS_NETWORKSTATUS];
        
        t.methodID = t.env->GetStaticMethodID(t.classID,"refreshWifiStatus"  ,"()V");
        
        if(t.methodID!=NULL)
        {
            MGLogD("refreshWifiStatus success");
            t.env->CallStaticVoidMethod(t.classID,t.methodID);
        } else {
            MGLogD("refreshWifiStatus Failed");
        }
        
        if(isAttached)
            JniHelper::getJavaVM()->DetachCurrentThread();
	}
    
    void MGNetworkReachability::receiveWifiStatus(const string &wifiStatus)
    {
		vector<WifiStatusNode> ret;
        string str = wifiStatus;
        MGLogD("%s", str.c_str());
        vector<string> nodes_str = Utility::SplitString(str, ';');
        for (vector<string>::iterator nodes_it = nodes_str.begin(); nodes_it != nodes_str.end(); ++nodes_it) {
            WifiStatusNode node;
            string n_str = *nodes_it;
            MGLogD("%s", n_str.c_str());
            vector<string> node_str = Utility::SplitString(n_str, ',');
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
extern "C"
{
    void Java_com_z11_mobile_framework_MGNetworkReachability_nativeWifiScanResults(JNIEnv* env, jobject thiz, jstring wifiStatus)
    {
        z11::MGNetworkReachability::receiveWifiStatus(z11::JniHelper::jstring2string(wifiStatus));
    }
}