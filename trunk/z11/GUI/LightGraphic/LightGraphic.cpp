//
//  LightGraphic.cpp
//  Gui
//
//  Created by Wang ZhiPeng on 11-5-20.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#include "LightGraphic.h"
#include "../Base/MIView.h"
#include "../../CommonBase/CommonDef.h"
#include "../GuiContainer/GuiContainer.h"
#include "../../System/Shared/MGLog.h"
#include "../Controllers/MIViewController.h"
#include "../Animations/MIAnimationsManager.h"
#include "../../System/Shared/MGTime.h"
#include "../../GraphicCore/MGPersistenceImage.h"
#include "../../System/Device/MGDevice.h"
#include <iostream>
#include <math.h>

//implementation of LightGraphic

#ifndef M_PI
#define M_PI        3.14159265358979323846264338327950288
#endif

using namespace std;

namespace z11
{

LightGraphic* LightGraphic::m_singleton = NULL;
GuiContainer* LightGraphic::m_gui_container = NULL;
    float LightGraphic::m_fps = 0.0f;
    float LightGraphic::m_interval = 0.0f;
    float LightGraphic::m_real_fps = 0.0f;
    bool LightGraphic::m_need_caculate_fps = false;

    LightGraphic::LightGraphic()
    {
        m_fps = default_fps;
        m_interval = default_interval;
        m_real_fps = m_fps;
        m_need_caculate_fps = false;
        m_need_setting_projection = false;
        m_first_frame_drawed = false;
    }

    LightGraphic::~LightGraphic()
    {
		if (m_singleton == this)
			m_singleton = NULL;
    }

	void LightGraphic::setGuiContainer(GuiContainer* container)
	{
		m_gui_container = container;
	}

    LightGraphic* LightGraphic::sharedLightGraphic(GuiContainer* container)
    {
        if(m_singleton == NULL)
            m_singleton = new LightGraphic();
        if (m_singleton != NULL) {
            m_singleton->m_gui_container = container;
        }
        return m_singleton;
    }

    float LightGraphic::setFps(float fps)
    {
        if(fps > 0)
        {
            m_fps = fps;
            m_interval = 1.0f / m_fps;
        }
        return m_interval;
    }

    float LightGraphic::setInterval(float interval)
    {
        if(internal > 0)
        {
            m_interval = interval;
            m_fps = 1.0f / m_interval;
        }
        return m_fps;
    }
    bool LightGraphic::threadLoop()
    {
		bool ret = false;
		MGTimeTicks ticket=MGTimeTicks::Now();//开始计时
		
        if (m_need_setting_projection && m_gui_container != NULL) {
            m_gui_container->setProjectionMode();
            m_need_setting_projection = false;
        }
        
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_gui_container->beforeRenderScreen();
		MGGraphicEngine::sharedGraphicEngine()->cleanContext();
		if (m_gui_container != NULL) {
            if (m_gui_container->gui_root != NULL && m_gui_container->gui_root->view != NULL) {
                MIView::animations_manager->actAnimations();
                m_gui_container->gui_root->view->setAnimationAttributes();
                m_gui_container->gui_root->view->draw();
            }
            //ret = true;
        }
        m_gui_container->renderScreen();
		m_gui_container->afterRenderScreen();
		double delay = (MGTimeDelta::FromSecondsF(m_interval)-(MGTimeTicks::Now()-ticket)).InMillisecondsF();//计算休眠时间
		if (delay > 10.0)
		{
            if (m_need_caculate_fps) {
                m_real_fps = m_fps;
            }
			MGSleep(delay);
		} else {
            if (m_need_caculate_fps) {
                m_real_fps = 1.0f / (m_interval + ((10 - delay) / MGTime::kMillisecondsPerSecond));
            }
            MGSleep(10);
		}

        ret = true;
        return ret;
    }
    
    
    bool LightGraphic::firstFrameLoop()
    {
        if (m_need_setting_projection && m_gui_container != NULL) {
            m_gui_container->setProjectionMode();
            m_need_setting_projection = false;
        }
        
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_gui_container->beforeRenderScreen();
        MGGraphicEngine::sharedGraphicEngine()->cleanContext();
        if (m_gui_container != NULL) {
            if (m_gui_container->gui_root != NULL && m_gui_container->gui_root->view != NULL) {
                MIView::animations_manager->actAnimations();
                m_gui_container->gui_root->view->setAnimationAttributes();
                m_gui_container->gui_root->view->draw();
            }
        }
        m_gui_container->renderScreen();
        return m_gui_container->afterRenderScreen();
    }
    
    int LightGraphic::readyToRun()
    {
        if (m_gui_container == NULL) {
            return -1;
		}
#ifndef __IPHONE_2_0 //目前只有iPhone需要在主线程中初始化资源，其他的都是在绘制线程里初始化
		m_gui_container->initOpenGLES();
#endif
        bool first_draw = false;
        while (!first_draw) {
            first_draw = firstFrameLoop();
        }
        if (first_draw)
        {
            MIEvent *event = new MIEvent;
            event->_event_type = LIGHT_GRAPHIC_FIRST_FRAME_DRAWED_EVENT;
            GuiContainer::container->preserveEvent(event);
        }
        return 0;
    }
    
    void LightGraphic::needSettingProjection()
    {
        m_need_setting_projection = true;
    }

} //namespace z11
