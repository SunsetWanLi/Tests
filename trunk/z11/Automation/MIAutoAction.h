//
//  MIAutoAction.h
//  MIAutoAction
//
//  Created by 王志鹏 on 16/8/16.
//  Copyright © 2016年 Rayme Tech Co.,LTD. All rights reserved.
//

#pragma once

#include <MGFoundation.h>
#include <MIKit.h>

#include <string>
#include <deque>
#include "SocketServer/RaymeCommand.h"

using namespace std;
using namespace RAYME_SERVER;

namespace z11 {
    
    class MIAutoObject {
    public:
        MIAutoObject();
        ~MIAutoObject();
    public:
        string class_name;
        int_32 index;
    };
    
    class MIAutoAction {
    public:
        MIAutoAction();
        virtual ~MIAutoAction() = 0;
    public:
        virtual bool execute() = 0;
        virtual string actionScript() = 0;
        void addObjectChain(const string& chain);
    protected:
        MIView* objectView();
        deque<string> findViews();
        MIView* viewForChain(deque<MIAutoObject>& chain, MIView* view);
        MIView* viewForTag(const string& tag, MIView* view);
        MIView* viewForText(const string& text, MIView* view);
        MIView* viewForPlaceholder(const string& place_holder, MIView* view);
        deque<string> pathForText(const string& text, MIView* view);
    protected:
        deque<MIAutoObject> object_chain;
    public:
        uint_32 retry_num;
        RaymeRequest req;
    };
    
    class MIAutoTap : public MIAutoAction {
    public:
        MIAutoTap();
        virtual ~MIAutoTap();
    public:
        virtual bool execute();
        virtual string actionScript();
    public:
        MGIntPoint tap_point;
        int_32 interval_time;
    };
    
    class MIAutoHide : public MIAutoAction {
    public:
        MIAutoHide();
        virtual ~MIAutoHide();
    public:
        virtual bool execute();
        virtual string actionScript();
    };
    
    class MIAutoShow : public MIAutoAction {
    public:
        MIAutoShow();
        virtual ~MIAutoShow();
    public:
        virtual bool execute();
        virtual string actionScript();
    };
    
    class MIAutoScroll : public MIAutoAction {
    public:
        MIAutoScroll();
        MIAutoScroll(int x, int y);
        virtual ~MIAutoScroll();
    public:
        virtual bool execute();
        virtual string actionScript();
    public:
        MGIntPoint init_point;
        int_32 scroll_x;
        int_32 scroll_y;
    public:
        int_32 interval_time;
    };
    
    class MIAutoTextIn : public MIAutoAction {
    public:
        MIAutoTextIn();
        virtual ~MIAutoTextIn();
    public:
        virtual bool execute();
        virtual string actionScript();
    public:
        string text_in;
    };
    
    class MIAutoWait : public MIAutoAction {
    public:
        MIAutoWait();
        virtual ~MIAutoWait();
    public:
        virtual bool execute();
        virtual string actionScript();
    public:
        int_32 wait_time;
    };
    
    class MIAutoUITree : public MIAutoAction {
    public:
        MIAutoUITree();
        virtual ~MIAutoUITree();
    public:
        virtual bool execute();
        virtual string actionScript();
    public:
        string ui_tree;
    };
    
    class MIAutoUITreeJson : public MIAutoAction {
    public:
        MIAutoUITreeJson();
        virtual ~MIAutoUITreeJson();
    public:
        virtual bool execute();
        virtual string actionScript();
    public:
        string ui_tree;
    };
    
    class MIAutoUIFind : public MIAutoAction {
    public:
        MIAutoUIFind();
        virtual ~MIAutoUIFind();
    public:
        virtual bool execute();
        virtual string actionScript();
    public:
        string ui_pathes;
    };
    
    class MIAutoTextOut : public MIAutoAction {
    public:
        MIAutoTextOut();
        virtual ~MIAutoTextOut();
    public:
        virtual bool execute();
        virtual string actionScript();
    public:
        string text_out;
    };
    
    class MIAutoExist : public MIAutoAction {
    public:
        MIAutoExist();
        virtual ~MIAutoExist();
    public:
        virtual bool execute();
        virtual string actionScript();
    };
    
    class MIAutoEnabled : public MIAutoAction {
    public:
        MIAutoEnabled();
        virtual ~MIAutoEnabled();
    public:
        virtual bool execute();
        virtual string actionScript();
    };
    
    class MIAutoLock : public MIAutoAction {
    public:
        MIAutoLock();
        virtual ~MIAutoLock();
    public:
        virtual bool execute();
        virtual string actionScript();
    };
    
    class MIAutoUnlock : public MIAutoAction {
    public:
        MIAutoUnlock();
        virtual ~MIAutoUnlock();
    public:
        virtual bool execute();
        virtual string actionScript();
    };
    
    class MIAutoURL : public MIAutoAction {
    public:
        MIAutoURL();
        virtual ~MIAutoURL();
    public:
        virtual bool execute();
        virtual string actionScript();
    public:
        string url;
    };
    
    class MIAutoKeyboardShown : public MIAutoAction {
    public:
        MIAutoKeyboardShown();
        virtual ~MIAutoKeyboardShown();
    public:
        virtual bool execute();
        virtual string actionScript();
    public:
        string shown;
    };
    
    class MIAutoInfo : public MIAutoAction {
    public:
        MIAutoInfo();
        virtual ~MIAutoInfo();
    public:
        virtual bool execute();
        virtual string actionScript();
    public:
        string info;
    };
    
    class MIAutoUnknown : public MIAutoAction {
    public:
        MIAutoUnknown();
        virtual ~MIAutoUnknown();
    public:
        virtual bool execute();
        virtual string actionScript();
    };
    
    class MIAutoScreenShot : public MIAutoAction {
    public:
        MIAutoScreenShot();
        virtual ~MIAutoScreenShot();
    public:
        virtual bool execute();
        virtual string actionScript();
    public:
        vector<char> img;
    };
}
