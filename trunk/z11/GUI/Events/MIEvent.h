//
//  MIEvent.h
//  Gui
//
//  Created by Wang ZhiPeng on 11-5-22.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//
#pragma once

#include "MITouch.h"
#include "MIEventDefine.h"
#include <string>
#include <set>

using namespace std;

namespace z11
{
    class MIResponder;
    
    class MIEvent : public MIObject
    {
    public:
        MIEvent();
        MIEvent(const MIEvent& event);
        virtual ~MIEvent();
    public:
        uint_32 _event_type;
        MGObject* _responder;
        MGObject* _content;
        SELHandler _handler;
        bool isSysCallbackEvent();
        bool isControlRetransTouchesEvent();
        bool isImmediatellyHandleEvent();
        set<MITouch*> touches_in_event;
    };
    typedef set<MITouch*>::iterator TouchesInEventIt;
    
    class MGTextContent : public MGObject
    {
    public:
        MGTextContent();
        virtual ~MGTextContent();
    public:
        string text_content;
		MGRect rect;
    };
    
    class MGRemoteAppContent : public MGObject
    {
    public:
        MGRemoteAppContent();
        virtual ~MGRemoteAppContent();
    public:
        string url_content;
		string id_content;
    };
    
    class MGKeyDownContent : public MGObject
    {
    public:
        MGKeyDownContent();
        virtual ~MGKeyDownContent();
    public:
        char key_code;
    };
    
    class MGPointContent : public MGObject
    {
    public:
        MGPointContent();
        virtual ~MGPointContent();
    public:
        MGPoint point;
    };
    
    class MGIntervalContent : public MGObject
    {
    public:
        MGIntervalContent();
        virtual ~MGIntervalContent();
    public:
        float interval;
    };
    
    class MGWebViewContent : public MGObject
    {
    public:
        MGWebViewContent();
        virtual ~MGWebViewContent();
    public:
        string url_content;
        bool is_post;
        string post_body;
		MGRect rect;
        bool zoom_enabled;
        bool pan_enabled;
    };

} //namespace z11