//
//  AppMainEx.mm
//  iOSApp
//
//  Created by Wang ZhiPeng on 11-6-2.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//
#include "AppMain.h"

#include <UIKit/UIKit.h>

#include "MIKit.h"
#include "iOSApp/z11AppViewController.h"
#include "MGFoundation.h"
#include "System/WebService/MGWebService.h"
#import "GUIRootViewController.h"

using namespace z11;

AppMain::AppMain()
{
    maintain_context = NULL;
}

AppMain::~AppMain()
{
    
}


void AppMain::initialise(WndHandle wnd)
{
    GuiContainer::initialise(wnd);
    gui_root = GUIRootViewController::getInstance(false);
    gui_root->initManually();
    
}