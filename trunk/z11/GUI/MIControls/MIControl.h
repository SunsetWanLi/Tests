//
//  MIControl.h
//  Gui
//
//  Created by Wang ZhiPeng on 11-5-21.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//
#pragma once

#include "../Base/MIView.h"
#include "../Controllers/MIViewController.h"
#include <map>

using namespace std;

#ifndef FONT_SHARED
#define FONT_SHARED 0
#endif

namespace z11
{
    enum {
        MIControlEventTouchDown           = 1 <<  0,      // on all touch downs
        MIControlEventTouchDownRepeat     = 1 <<  1,      // on multiple touchdowns (tap count > 1)
        MIControlEventTouchDragInside     = 1 <<  2,
        MIControlEventTouchDragOutside    = 1 <<  3,
        MIControlEventTouchDragEnter      = 1 <<  4,
        MIControlEventTouchDragExit       = 1 <<  5,
        MIControlEventTouchUpInside       = 1 <<  6,
        MIControlEventTouchUpOutside      = 1 <<  7,
        MIControlEventTouchCancel         = 1 <<  8,
        
        MIControlEventValueChanged        = 1 << 12,     // sliders, etc.
        
        MIControlEventEditingDidBegin     = 1 << 16,     // UITextField
        MIControlEventEditingChanged      = 1 << 17,
        MIControlEventEditingDidEnd       = 1 << 18,
        MIControlEventEditingDidEndOnExit = 1 << 19,     // 'return key' ending editing
        
        MIControlEventAllTouchEvents      = 0x00000FFF,  // for touch events
        MIControlEventAllEditingEvents    = 0x000F0000,  // for UITextField
        MIControlEventApplicationReserved = 0x0F000000,  // range available for application use
        MIControlEventSystemReserved      = 0xF0000000,  // range reserved for internal framework use
        MIControlEventAllEvents           = 0xFFFFFFFF
    };
    typedef uint_32 MIControlEvents;
    enum {
        MIControlStateNormal		= 0,                       
        MIControlStateHighlighted	= 1 << 0,
        MIControlStateDisabled		= 1 << 1,
        MIControlStateSelected		= 1 << 2,
		MIControlStateNumber		= 4
    };
    typedef uint_32 MIControlState;
    enum {
        MIActionTypeNormal   = 1 << 0,
        MIActionTypeMove     = 1 << 1,
        MIActionTypeBlend    = 1 << 2
    };
    typedef uint_32 MIActionType;

    typedef void (MIViewController::*EventFunc)(MIObject*);
    typedef struct
    {
        MIViewController* view_controller;
        EventFunc event_func;
    }EventHandler;
    
    class MIControl : public MIView
    {
    public:
        MIControl();
        virtual ~MIControl() = 0;
    //控件基础//
    public:
        void enabled();
        void disabled();
        bool isEnabled(){return m_is_enabled;}
        void selected();
        void deselected();
        bool isSelected(){return m_is_selected;}
        void highlighted();
        void unhignlighted();
        bool isHighlighted(){return m_is_highlighted;}
        void setActionType(uint_32 type);
        void addActionType(uint_32 type);
        
        //Action Color
        MGPROPERTY(RGBAColorRef, m_action_blend, ActionBlend);
    protected:
        bool m_is_enabled;     //是否可用//
        bool m_is_selected;     //是否被选中//
        bool m_is_highlighted;  //是否高亮//
        MIControlState m_control_state;
        MIActionType m_action_type;
    /*消息代理相关*/
    protected:
        map<MIControlEvents, EventHandler> events_dispatch_map;
    public:
        /*注册的消息处理函数必须保证Instance存活，在引入MIX文件构建后，可以有效保证不出现因为实例不存活引起的异常，在进行手动初始化ViewController的过程中，要确保ViewController和View及内部控件配对，并保证ViewController销毁时销毁整个View*/
        bool registerEventHandler(const MIControlEvents& event, EventHandler handler);
        void unregisterEventHandler(const MIControlEvents& event);
    /*Inherited*/
    public:
        MIRESPONDER_TOUCH_DECLARE;
    };
    typedef map<MIControlEvents, EventHandler>::iterator EventsMapIterator;
} //namespace z11