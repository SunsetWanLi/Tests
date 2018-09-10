//
//  MIResponder.cpp
//  Gui
//
//  Created by Wang ZhiPeng on 11-5-25.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#include "MIResponder.h"
#include <assert.h>
#include "../../System/Shared/MGLog.h"

namespace z11
{
    MIResponder::FirstResponderLock* MIResponder::_first_responder_locker = NULL;
    MIResponder* MIResponder::_first_responder = NULL;
    MIResponder::MIResponder()
    {
        class_name = "MIResponder";
    }
    MIResponder::~MIResponder()
    {
		class_name = "NULL";
        MIResponder::resignFirstResponderWithResponder(this);
    }
    // 当有触摸时，且触摸不发生在First reponder上，需要做清理工作，比如输入框取消“复制、粘贴”栏 //
    void MIResponder::clearContext(MIView* view)
    {
        
    }
    //管理响应链
    //返回响应链的下一个响应者 
    MIResponder* MIResponder::nextResponder()
    {
        return NULL;
    }
    bool MIResponder::isFirstResponder()
    {
        bool ret = false;
        MIResponder* first_responder = getFirstResponder();
        if (first_responder == this) {
            ret = true;
        }
        setFirstResponder(first_responder);
        return ret;
    }
    //是否能够被设置为第一响应者 return false; in default 
    bool MIResponder::canBecomeFirstResponder()
    {
        return false;
    }
    void MIResponder::becomeFirstResponder()
    {
        if (!canBecomeFirstResponder()) {
            return;
        }
        MIResponder* first_responder = getFirstResponder();
        if (first_responder == this) {
            setFirstResponder(this);
            return;
        }
        if (first_responder != NULL) {
            if (first_responder->canResignFirstResponder()) {
                first_responder->resignFirstResponder(false);
            }else{
                setFirstResponder(first_responder);
                return;
            }            
        }
        setFirstResponder(this);
    }
    //是否能够被取消当前对象为第一响应者 return true; in default 
    bool MIResponder::canResignFirstResponder()
    {
        return true;
    }
    //取消当前对象为第一响应者 
    void MIResponder::resignFirstResponder(bool closeKB)
    {
        if (!canResignFirstResponder()) {
            return;
        }
        MIResponder::resignFirstResponderWithResponder(this);
    }
    //触摸事件的响应 
    MIEventHandleState MIResponder::_touchesBegan(const std::set<MITouch*>& touches,
                              const MIEvent* const event)
    {
        uint_32 ret = MIEventHandleStateNormal;
        ret = touchesBegan(touches, event);
        for (std::set<MITouch*>::const_iterator it = touches.begin(); it != touches.end(); ++it) {
            if ((*it)->maintained_view == (MIView*)this) {
                touch_manager[(*it)->tag] = MITouch(*it);
            }
        }
        if (ret == MIEventHandleStatePass) {
            handleTouchesPassEvent(touches);
            MIResponder* responder = nextResponder();
            if (responder != NULL) {
                responder->handleSubTouchesPassEvent(touches);
                return responder->_touchesBegan(touches, event);
            }
        }
        return ret;
    }
    MIEventHandleState MIResponder::_touchesMoved(const std::set<MITouch*>& touches,
                              const MIEvent* const event)
    {
        uint_32 ret = MIEventHandleStateNormal;
        ret = touchesMoved(touches, event);
        if (ret == MIEventHandleStatePass) {
            handleTouchesPassEvent(touches);
            MIResponder* responder = nextResponder();
            if (responder != NULL) {
                responder->handleSubTouchesPassEvent(touches);
                return responder->_touchesMoved(touches, event);
            }
        }
        return ret;
    }
    MIEventHandleState MIResponder::_touchesEnded(const std::set<MITouch*>& touches,
                              const MIEvent* const event)
    {
        uint_32 ret = MIEventHandleStateNormal;
        for (std::set<MITouch*>::const_iterator it = touches.begin(); it != touches.end(); ++it) {
            if ((*it)->maintained_view != (MIView*)this)
                break;
            TouchMIt t_it = touch_manager.find((*it)->tag);
            if(t_it != touch_manager.end())
                touch_manager.erase(t_it);
        }
        ret = touchesEnded(touches, event);
        if (ret == MIEventHandleStatePass) {
            handleTouchesPassEvent(touches);
            MIResponder* responder = nextResponder();
            if (responder != NULL) {
                responder->handleSubTouchesPassEvent(touches);
                return responder->_touchesEnded(touches, event);
            }
        }
        return ret;
    }
    MIEventHandleState MIResponder::_touchesCancelled(const std::set<MITouch*>& touches,
                                  const MIEvent* const event)
    {
        uint_32 ret = MIEventHandleStateNormal;
        for (std::set<MITouch*>::const_iterator it = touches.begin(); it != touches.end(); ++it) {
            if ((*it)->maintained_view != (MIView*)this)
                break;
            TouchMIt t_it = touch_manager.find((*it)->tag);
            if(t_it != touch_manager.end())
                touch_manager.erase(t_it);
        }
        ret = touchesCancelled(touches, event);
        if (ret == MIEventHandleStatePass) {
            handleTouchesPassEvent(touches);
            MIResponder* responder = nextResponder();
            if (responder != NULL) {
                responder->handleSubTouchesPassEvent(touches);
                return responder->_touchesCancelled(touches, event);
            }
        }
        return ret;
    }
    //事件响应 
    MIEventHandleState MIResponder::_handleEvent(const MIEvent* const event)
    {
        MIEventHandleState ret = MIEventHandleStateError;
        do {
            if (event == NULL) {
                ret = MIEventHandleStateError;
                break; // while(0);
            }
            if (event->_responder == this && event->_handler.sel_func != NULL) {
                SELFunc sel_func = event->_handler.sel_func;
                (this->*sel_func)(event->_handler.sender);
                ret = MIEventHandleStateDone;
                break; // while(0);
            }
            ret = handleEvent(event);
        }while(0);
        return ret;
    }
    //first responder//
    MIResponder* MIResponder::getFirstResponder()
    {
        MIResponder* ret = NULL;
        assert(_first_responder_locker != NULL);
        _first_responder_locker->lock();
        ret = _first_responder;
        return ret;
    }
    void MIResponder::setFirstResponder(z11::MIResponder *first_responder)
    {
        assert(_first_responder_locker != NULL);
        _first_responder = first_responder;
        _first_responder_locker->unlock();
    }
    void MIResponder::resignFirstResponderWithResponder(z11::MIResponder *responder)
    {
		if (_first_responder_locker == NULL)
			return;
        _first_responder_locker->lock();
        if (_first_responder == responder) {
            _first_responder = NULL;
        }
        _first_responder_locker->unlock();
    }

	bool MIResponderSEL::isNull()
	{
		return (responder==NULL) || (action.sender==NULL) || (action.sel_func==NULL);
	}
    
#pragma mark - MGClassName -
    string MGClassName::className()
    {
        return class_name.substr(0, class_name.find_first_of(":"));
    }
    string MGClassName::inheritanceNames()
    {
        return class_name;
    }
}