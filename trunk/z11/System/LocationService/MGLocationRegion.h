//
//  MGLocationRegion.h
//  System
//
//  Created by King Asdy on 16-7-27.
//  Copyright (c) 2016年 Zhejiang University. All rights reserved.
//

#pragma once

#include "../../CommonBase/MGObject.h"
#include "../Shared/MGTime.h"

namespace z11 {
	
    typedef enum {
        BEACON_UNKNOWN = 0,
        BEACON_IMMEDIATELY,
        BEACON_NEAR,
        BEACON_FAR
    }BeaconDistance;


    class MGLocationRegion : public MGObject
    {
    public:
        MGLocationRegion();
        virtual ~MGLocationRegion();
    public:
        string uuid;
		int_64 major;
		int_64 minor;
		int_64 rssi;
		BeaconDistance distance;
    public:
        // 比较操作. //
        bool operator == (const MGLocationRegion &other) const
        {
            return uuid == other.uuid;
        }
        bool operator != (const MGLocationRegion &other) const
        {
            return uuid != other.uuid;
        }
        bool operator < (const MGLocationRegion &other) const
        {
            return uuid < other.uuid;
        }
        bool operator <= (const MGLocationRegion &other) const
        {
            return uuid <= other.uuid;
        }
        bool operator > (const MGLocationRegion &other) const
        {
            return uuid > other.uuid;
        }
        bool operator >= (const MGLocationRegion &other) const
        {
            return uuid >= other.uuid;
        }
    };
    
    
    class MGBeacon : public MGObject
    {
    public:
        MGBeacon();
        virtual ~MGBeacon();
    public:
        string uuid;
		int_64 major;
		int_64 minor;
        int_64 rssi;
        BeaconDistance distance;
    };
} //namespace z11
