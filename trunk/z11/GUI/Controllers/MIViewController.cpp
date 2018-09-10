//
//  MIViewController.cpp
//  Gui
//
//  Created by Wang ZhiPeng on 11-6-3.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#include "MIViewController.h"
#include "../Base/MIView.h"
#include "../Animations/MIAnimationsManager.h"

namespace z11
{
    MIViewController::MIViewController()
    {
        class_name = "MIVewController";
        view = NULL;
    }
    MIViewController::~MIViewController()
    {
        if (MIView::animations_manager != NULL) {
            MIView::animations_manager->detachAnimationSelectorFromResponder(this);
        }
        if (view != NULL) {
            view->manualClean();
            view->detachFromSuperview();
			delete_and_set_null(view);
        }
    }
    MIViewController* MIViewController::initWithMixNameInBundle(const string &name, const z11::MGBundle *bundle)
    {
        MIViewController* ret = this;
        //Do nothing here.
        return ret;
    }
    MIViewController* MIViewController::initManually()
    {
        MIViewController* ret = this;
        //Do nothing here.
        return ret;
    }
    //管理响应链 
    void MIViewController::handleTouchesPassEvent(std::set<MITouch*> touches)
    {
        // Do nothing here.
    }
    void MIViewController::handleSubTouchesPassEvent(std::set<MITouch*> touches)
    {
        // Do nothing here.
    }
    //返回响应链的下一个响应者 
    MIResponder* MIViewController::nextResponder()
    {
        if (view == NULL) {
            return NULL;
        }
        if (view->m_superview != NULL) {
            return view->m_superview;
        }
        return NULL;
    }
    bool MIViewController::isFirstResponder()
    {
        return false;
    }
    //是否能够被设置为第一响应者 return false; in default 
    bool MIViewController::canBecomeFirstResponder()
    {
        return false;
    }
    void MIViewController::becomeFirstResponder()
    {
        if (!canBecomeFirstResponder()) {
            return;
        }
    }
    //是否能够被取消当前对象为第一响应者 return true; in default 
    bool MIViewController::canResignFirstResponder()
    {
        return true;
    }
    //取消当前对象为第一响应者 
    void MIViewController::resignFirstResponder(bool closeKB)
    {
        if (canResignFirstResponder()) {
            return;
        }
    }
    //触摸事件的响应 
    MIEventHandleState MIViewController::touchesBegan(const std::set<MITouch*>& touches, const MIEvent* const event)
    {
        return MIEventHandleStatePass;
    }
    MIEventHandleState MIViewController::touchesMoved(const std::set<MITouch*>& touches, const MIEvent* const event)
    {
        return MIEventHandleStatePass;
    }
    MIEventHandleState MIViewController::touchesEnded(const std::set<MITouch*>& touches, const MIEvent* const event)
    {
        return MIEventHandleStatePass;
    }
    MIEventHandleState MIViewController::touchesCancelled(const std::set<MITouch*>& touches, const MIEvent* const event)
    {
        return MIEventHandleStatePass;
    }
    //事件响应 
    MIEventHandleState MIViewController::handleEvent(const MIEvent* const event)
    {
        return MIEventHandleStatePass;
    }
    // 应用在系统的调度事件处理 //
    void MIViewController::applicationWillResignActive()
    {
        
    }
    void MIViewController::applicationDidEnterBackground()
    {
        
    }
    void MIViewController::applicationWillEnterForeground()
    {
        
    }
    void MIViewController::applicationDidBecomeActive()
    {
        
    }
    void MIViewController::applicationDidReceiveMemoryWarning()
    {
        
    }
    bool MIViewController::applicationNeedHandleBackKeyDown()
    {
        return false;
    }
    bool MIViewController::applicationNeedHandleMenuKeyDown()
    {
        return false;
    }
    void MIViewController::handleForeignerSDKApiBack(const string& response)
    {
        // do anything here you want, always should impelement in GUIRootViewController //
    }

	void MIViewController::applicationWillExit()
	{
	
	}
}