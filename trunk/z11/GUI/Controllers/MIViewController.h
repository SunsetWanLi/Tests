//
//  MIViewController.h
//  Gui
//
//  Created by Wang ZhiPeng on 11-6-3.
//  Copyright 2011年 Zhejiang University. All rights reserved.

#pragma once

#include "../Base/MIResponder.h"
#include "../../System/IO/MGBundle.h"
#include <string>

using namespace std;

namespace z11
{
    class MIView;
    class MIViewController : public MIResponder
    {
    public:
        MIViewController();
        virtual ~MIViewController() = 0;
    //初始化相关//
        /*通过MIX文件构建MIViewController，MIX文件是Mobile Interface XML的简写，是符合XML规范的文件，可以用图形化工具生成*/
        virtual MIViewController* initWithMixNameInBundle(const string& name, const MGBundle* bundle);
        virtual MIViewController* initManually();
        virtual void applicationWillResignActive();
        virtual void applicationDidEnterBackground();
        virtual void applicationWillEnterForeground();
        virtual void applicationDidBecomeActive();
        virtual void applicationDidReceiveMemoryWarning();
        virtual bool applicationNeedHandleBackKeyDown();
        virtual bool applicationNeedHandleMenuKeyDown();
        virtual void handleForeignerSDKApiBack(const string& response);
		virtual void applicationWillExit();
    public:
        MIView* view;
    public:
        MIRESPONDER_DECLARE
        MIRESPONDER_TOUCH_DECLARE
    };
#define MIVIEWCONTROLLER_DECLARE \
	virtual z11::MIViewController* initWithMixNameInBundle(const string& name, const z11::MGBundle* bundle); \
	virtual z11::MIViewController* initManually();
} //namespace z11