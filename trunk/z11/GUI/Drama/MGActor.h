//
//  MGActor.h
//  Drama
//
//  Created by King Asdy on 12-4-2.
//  Copyright (c) 2012年 Zhejiang University. All rights reserved.
//

#pragma once

#include "../MIControls/MIControl.h"
#include "../../System/Media/Audio/MGAudioPlayer.h"
#include <map>
#include <deque>

namespace z11 {
    
    class MGAct : public MGObject
    {
    public:
        MGAct();
        virtual ~MGAct();
    public:
        bool one_image;
        vector<string> v_files;
        MGImage *m_image;
        MGAudioPlayer *audio;   //动作对应的声音  //
        deque<MGRect> act_rect; //动作帧￼序列位置  //
        float act_duration; //动作持续时间 单位秒  //
        float act_time; //当前动作已经进行的时间 单位秒  //
        float num_of_loops; //动作循环次数，可以是浮点数，小于0代表无限循环  //
    };
    
    class MGActor : public MIControl
    {
    public:
        MGActor();
        virtual ~MGActor();
    protected:
        //MGActor继承MIView已经有背景图片了，同时，包含的每个MGAct又可以指定不同的背景图片  //
        //如果MGAct没有指定自己的背景图片，则其表示的动作序列位置就是MGActor背景图片的位置  //
        map<string, MGAct*> act_manager;
        string current_act_name;
    public:
        MGRect image_rect;
    protected:
        MIResponder* will_act_responder;
        SELHandler will_act_handler;
        MIResponder* did_act_responder;
        SELHandler did_act_handler;
    public:
        MIRESPONDER_TOUCH_DECLARE;
        MIVIEW_DECLARE;
    public:
        //执行act_name动作times次数  //
        bool act(const string& act_name, float times = 1.0f);
        void addActWithName(const string& act_name, MGAct* act);
        void removeActWithName(const string& act_name);
        void setActorWillActSelector(MIResponder* _responder, SELHandler selector);
        void setActorDidActSelector(MIResponder* _responder, SELHandler selector);
        string getCurrentActName();
    };
    typedef map<string, MGAct*>::iterator ActIter;
} //namespace z11

