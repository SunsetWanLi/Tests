//
//  MIResponder.h
//  Gui
//
//  Created by Wang ZhiPeng on 11-5-21.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//
#pragma once

#include "../../CommonBase/CommonDef.h"
#include "../Events/MIEvent.h"
#include "MIDataTypeDef.h"
#include "../../System/Shared/Syncable.h"
#include "../../System/Shared/MGTime.h"
#include <set>
#include <map>

using namespace std;

namespace z11
{
    enum {
        MIEventHandleStateNormal = 0,
        //事件处理完毕//
        MIEventHandleStateDone = 1,
        //不处理此事件//
        MIEventHandleStatePass = 2,
        //处理事件出错//
        MIEventHandleStateError = 3
    };
    typedef uint_32 MIEventHandleState;
    
    class MGClassName
    {
    public:
        MGClassName(){class_name = "";}
        ~MGClassName(){}
    public:
        string className();
        string inheritanceNames();
    public:
        string class_name;
        void operator=(const string& name)
        {
            if(class_name == "")
                class_name = name;
            else
                class_name = name + ":" + class_name;
        }
    };

    class MIResponder : public MIObject
    {
    public:
        MIResponder();
        virtual ~MIResponder() = 0;
        virtual void clearContext(MIView* view);
    public:
        MGClassName class_name;
        friend class GuiContainer;
    public:
        bool isObjectOfClass(const string& classname)
        {
            bool ret = false;
            string::size_type idx = class_name.class_name.find(classname);
            if ( idx != string::npos )
                ret = true;
            return ret;
        }
    /*
     键盘的特殊处理函数，获得键盘高度后进行相应调整
     */
        virtual int adjustWithKeyboardHeight(int height){return 0;}
    /*
     响应链和触摸事件
     对于非触摸事件的响应，框架优先请求第一响应者进行响应，
     然后依次通过响应链查找最终响应对象。如果是触摸事件，
     则通过View结构，优先通过当前View进行HitDetect区域判断，
     然后递归找到最终的被点击控件，并且从这个控件开始响应链查找。
     */
    //管理响应链===============================================//
        virtual void handleTouchesPassEvent(std::set<MITouch*> touches) = 0;
        virtual void handleSubTouchesPassEvent(std::set<MITouch*> touches) = 0;
        //返回响应链的下一个响应者  //
        virtual MIResponder* nextResponder() = 0;
        virtual bool isFirstResponder() = 0;
        //是否能够被设置为第一响应者 return false; in default
        virtual bool canBecomeFirstResponder() = 0;
        virtual void becomeFirstResponder() = 0;
        //是否能够被取消当前对象为第一响应者 return true; in default
        virtual bool canResignFirstResponder() = 0;
        //取消当前对象为第一响应者  //
        virtual void resignFirstResponder(bool closeKB = true) = 0;
    //=======================================================//

    //====== first responder ================================//
    private:
        static MIResponder* _first_responder;
    public:
        class FirstResponderLock : public Syncable
        {
        public:
            FirstResponderLock(){}
            virtual ~FirstResponderLock(){}
        };
        static FirstResponderLock* _first_responder_locker;
    public:
        //get和set必须配对使用，在get中加锁，在set中解锁，防止对first_responder的错误内存引用//
        static MIResponder* getFirstResponder();
        static void setFirstResponder(MIResponder* first_responder);
        static void resignFirstResponderWithResponder(MIResponder* responder);
    //=======================================================//

    public:
    //触摸事件的响应============================================//
        MIEventHandleState FrameworkImp _touchesBegan(const std::set<MITouch*>& touches,
                                        const MIEvent* const event);
        MIEventHandleState FrameworkImp _touchesMoved(const std::set<MITouch*>& touches,
                                        const MIEvent* const event);
        MIEventHandleState FrameworkImp _touchesEnded(const std::set<MITouch*>& touches,
                                        const MIEvent* const event);
        MIEventHandleState FrameworkImp _touchesCancelled(const std::set<MITouch*>& touches,
                                        const MIEvent* const event);
    protected:
        virtual MIEventHandleState CustomerImp touchesBegan(const std::set<MITouch*>& touches,
                                  const MIEvent* const event) = 0;
        virtual MIEventHandleState CustomerImp touchesMoved(const std::set<MITouch*>& touches,
                                  const MIEvent* const event) = 0;
        virtual MIEventHandleState CustomerImp touchesEnded(const std::set<MITouch*>& touches,
                                  const MIEvent* const event) = 0;
        virtual MIEventHandleState CustomerImp touchesCancelled(const std::set<MITouch*>& touches,
                                      const MIEvent* const event) = 0;
    //========================================================//
    public:
        map<MITouchTag, MITouch> touch_manager;

    //事件响应==================================================//
    public:
        MIEventHandleState FrameworkImp _handleEvent(const MIEvent* const event);
    protected:
        virtual MIEventHandleState CustomerImp handleEvent(const MIEvent* const event) = 0;
    //========================================================//
    };
    typedef map<MITouchTag, MITouch>::iterator TouchMIt;
#define MIRESPONDER_DECLARE \
    virtual void handleTouchesPassEvent(std::set<MITouch*> touches);\
    virtual void handleSubTouchesPassEvent(std::set<MITouch*> touches);\
    virtual MIResponder* nextResponder();\
    virtual bool isFirstResponder();\
    virtual bool canBecomeFirstResponder();\
    virtual void becomeFirstResponder();\
    virtual bool canResignFirstResponder();\
    virtual void resignFirstResponder(bool closeKB = true);
#define MIRESPONDER_TOUCH_DECLARE \
    virtual MIEventHandleState touchesBegan(const std::set<MITouch*>& touches,\
                              const MIEvent* const event);\
    virtual MIEventHandleState touchesMoved(const std::set<MITouch*>& touches,\
                              const MIEvent* const event);\
    virtual MIEventHandleState touchesEnded(const std::set<MITouch*>& touches,\
                              const MIEvent* const event);\
    virtual MIEventHandleState touchesCancelled(const std::set<MITouch*>& touches,\
                                  const MIEvent* const event);\
    virtual MIEventHandleState handleEvent(const MIEvent* const event);


	class MIResponderSEL
	{
	public:
		MIResponderSEL():responder(NULL)
		{

		}
		bool isNull();
		MIResponder *responder;
		z11::SELHandler action;
        bool operator==(MIResponderSEL target)
        {
            if(responder != target.responder)
                return false;
            if(action.sender != target.action.sender)
                return false;
            if(action.sel_func != target.action.sel_func)
                return false;
            return true;
        }
	};

} //namespace z11
