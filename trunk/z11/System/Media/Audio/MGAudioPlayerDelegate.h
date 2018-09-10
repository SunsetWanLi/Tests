//
//  MGAudioPlayerDelegate.h
//  System
//
//  Created by King Asdy on 12-3-31.
//  Copyright (c) 2012年 Zhejiang University. All rights reserved.
//

#pragma once

#include "../../../CommonBase/MGObject.h"
#include <string>
#include <map>

using namespace std;

namespace z11 {
    class MGAudioPlayer;
    class MGAudioPlayerDelegate
    {
    public:
        MGAudioPlayerDelegate();
        virtual ~MGAudioPlayerDelegate();
    public:
        virtual void audioPlayerDidFinishPlaying(MGAudioPlayer* player) = 0;
    };
} //namespace z11
