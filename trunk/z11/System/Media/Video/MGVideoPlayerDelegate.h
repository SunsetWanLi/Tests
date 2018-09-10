//
//  MGVideoPlayerDelegate.h
//  System
//
//  Created by King Asdy on 13-01-09.
//  Copyright (c) 2012年 Zhejiang University. All rights reserved.
//

#pragma once

#include "../../../CommonBase/MGObject.h"
#include <string>
#include <map>

using namespace std;

namespace z11 {
    class MGVideoPlayer;
    class MGVideoPlayerDelegate
    {
    public:
        MGVideoPlayerDelegate();
        virtual ~MGVideoPlayerDelegate();
    public:
        virtual void videoPlayerDidFinishPlaying(MGVideoPlayer* player) = 0;
        virtual void videoPlayerDidFinishPlayingByUser(MGVideoPlayer* player) = 0;
    };
} //namespace z11
