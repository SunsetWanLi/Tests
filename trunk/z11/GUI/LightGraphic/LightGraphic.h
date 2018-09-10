//
//  LightGraphic.h
//  Gui
//  
//  轻量级的图形绘制线程容器类，应该在应用程序初始化的时候建立该类的对象。
//  可以通过设置FPS来对屏幕刷新的帧率进行控制。
//
//  Created by Wang ZhiPeng on 11-5-20.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//
#pragma once

#include "../../System/Shared/NonCopyable.h"
#include "../Base/MIDataTypeDef.h"
#include "../Base/MIObject.h"
#include "../../System/Thread/MGThread.h"

namespace z11
{
class GuiContainer;
#define default_fps 60.0f
#define default_interval 1.0f/default_fps

class LightGraphic : public MGThread, public NonCopyable
{
protected:
    LightGraphic();
public:
    virtual ~LightGraphic();
private:
    static LightGraphic *m_singleton;
    static float m_fps;
    static float m_interval;
    static float m_real_fps;
    static bool m_need_caculate_fps;
    static GuiContainer* m_gui_container;
public:
	static void setGuiContainer(GuiContainer* container);
    static LightGraphic* sharedLightGraphic(GuiContainer* container);
    // 设置FPS值，参数为要设置的FPS值，返回为当前FPS设置下的刷新频率时间间隔值 
    // 比如FPS为30.0，则返回值为1/30.0 
    static float setFps(float fps);
    static float getSettingFps(){return m_fps;}
    // 设置屏幕刷新频率时间间隔值，参数为要设置的间隔值，返回为当前间隔设置下的FPS值 
    // 比如Internal为1/30.0，则返回值为30.0 
    static float setInterval(float interval);
    static float getInterval(){return m_interval;}
    static float getRealFps(){return m_real_fps;}
    static void caculateFpsOn(){m_need_caculate_fps = true;}
    static void caculateFpsOff(){m_need_caculate_fps = false;}
public:
    virtual bool threadLoop();
    virtual int readyToRun();
public:
    void needSettingProjection();
    bool firstFrameLoop();
private:
    bool m_need_setting_projection;
    bool m_first_frame_drawed;
};

} //namespace z11