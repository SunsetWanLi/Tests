//
//  MIButton.cpp
//  Gui
//
//  Created by Wang ZhiPeng on 11-5-21.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#include "MIButton.h"
#include "../../System/Shared/MGLog.h"
#include "../../GraphicCore/MGImage.h"
#include <assert.h>
#include "MIRoundedRectButton.h"

namespace z11
{

MIButton::MIButton()
{
    class_name = "MIButton";
    m_is_clicked = 0;
    m_action_type |= MIActionTypeBlend;
    draw_image_mode = ImageScaleToFill;
	for (int i=0;i<MIControlStateNumber;i++)
	{
		titles.push_back("");
		colors.push_back(RGBAColorRef::black());
		images.push_back(NULL);
	}
	m_titleLabel=NULL;
    cornerRadius = _DEFAULT_ROUND_RATE_;
    borderEnabled = true;
	m_background_color=MGColor::fromRGBA(255,255,255);
	m_draw_round_mode = RoundRectAll;
    borderColor = RGBAColorRef::darkGray();
    borderWidth = 1.0;
}

MIButton::~MIButton()
{
	//MGLogD("MIButton %p deconstruction",this);
}

void MIButton::drawself()
{
	MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();
    if (graphic_engine == NULL) {
        return;
    }
	byte_8 alpha=m_alpha;
    RGBAColorRef color_ref;
    if (m_background_color != NULL) {
        color_ref = m_background_color->m_color;
		color_ref.m_alpha_component = (byte_8)(m_alpha * color_ref.m_alpha_component);
		alpha=color_ref.m_alpha_component;
    }else{
        color_ref = RGBAColorMake(0, 0, 0, (byte_8)(m_alpha * 255));
		alpha=0;
    }
	//if (color_ref.m_alpha_component>1e-6)
	{
		if (m_background_color != NULL && m_background_color->m_image != NULL)
		{
			m_background_color->m_image->sendDataToContext();
            if (m_is_enabled == false)
            {
                color_ref = RGBAColorRef::lightGray();
				color_ref.m_alpha_component = (byte_8)(m_alpha * color_ref.m_alpha_component);
            }else if (m_is_clicked != 0 && (m_action_type & MIActionTypeBlend) != 0) {
				color_ref = m_action_blend;
//				color_ref.m_alpha_component = (byte_8)(m_alpha * color_ref.m_alpha_component);
			}
			else if ((m_control_state & MIControlStateSelected) != 0) {
				color_ref = RGBAColorMake(128,128,128);
				color_ref.m_alpha_component = (byte_8)(m_alpha * color_ref.m_alpha_component);
			}
			graphic_engine->drawImage(m_background_color->m_image, m_bounds, color_ref, draw_image_mode);
		}else{

			MGRect rect=m_bounds;
            if(borderEnabled)
            {
                rect.x++;
                rect.y++;
                rect.width-=2;
                rect.height-=2;
            }
			
            if (m_is_enabled == false)
            {
                graphic_engine->drawFilledRoundRect(rect, color_ref, cornerRadius, RGBAColorRef::lightGray(),m_draw_round_mode);
            }
            else if (m_is_clicked == 0 || (m_action_type & MIActionTypeBlend) == 0)
            {
				graphic_engine->drawFilledRoundRect(rect, color_ref, cornerRadius,RGBAColorMake(255,255,255,255),m_draw_round_mode);
			}
            else
            {
				graphic_engine->drawFilledRoundRect(rect, color_ref, cornerRadius, m_action_blend,m_draw_round_mode);
			}
            if(borderEnabled)
            {
                color_ref = borderColor;
                color_ref.m_alpha_component=(byte_8)alpha;
                graphic_engine->drawRoundRect(m_bounds, color_ref, cornerRadius,borderWidth,RGBAColorMake(255,255,255,255),m_draw_round_mode);
            }
		}
	}
	//else do nothing
	
}
    
void MIButton::initWithFrame(MGRect frame)
{
	MIControl::initWithFrame(frame);
	if (m_titleLabel!=NULL)
	{
		MGRect rect=frame;
		rect.x=rect.y=0;
		m_titleLabel->setFrame(rect);
	}
	
}

//管理响应链 
//触摸事件的响应 
MIEventHandleState MIButton::touchesBegan(const std::set<MITouch*>& touches,
                                        const MIEvent* const event)
{
    MIEventHandleState ret = MIControl::touchesBegan(touches, event);
    std::set<MITouch*>::const_iterator touch_it;
    for (touch_it = touches.begin(); touch_it != touches.end(); ++touch_it) {
        MGPoint loc_in_view = (*touch_it)->locationInView(this);
        if (pointInside(loc_in_view, event)) {
            break;
        }
    }
    if (m_is_clicked == 0) {
        if ((m_action_type & MIActionTypeMove) != 0) {
            MGRect rect = getFrame();
            rect.x += 5;
            rect.y += 5;
            setFrame(rect);
        }
        m_is_clicked = 1;
    }
    EventsMapIterator it = events_dispatch_map.find(MIControlEventTouchDown);
    if (it == events_dispatch_map.end()) {
        ret = MIEventHandleStateNormal;
    }else{
        assert(it->second.view_controller != NULL);
        assert(it->second.event_func != NULL);
        EventFunc func = it->second.event_func;
        ((it->second.view_controller)->*func)(this);
        ret = MIEventHandleStateDone;
    }
    return ret;
}
MIEventHandleState MIButton::touchesMoved(const std::set<MITouch*>& touches,
                                        const MIEvent* const event)
{
    return  MIControl::touchesMoved(touches, event);
}
MIEventHandleState MIButton::touchesEnded(const std::set<MITouch*>& touches,
                                        const MIEvent* const event)
{
    MIEventHandleState ret = MIControl::touchesEnded(touches, event);
    std::set<MITouch*>::const_iterator touch_it;
    for (touch_it = touches.begin(); touch_it != touches.end(); ++touch_it) {
        MGPoint loc_in_view = (*touch_it)->locationInView(this);
        if (!pointInside(loc_in_view, event)) {
            break;
        }
    }
    if (m_is_clicked == 1) {
        if ((m_action_type & MIActionTypeMove) != 0) {
            MGRect rect = getFrame();
            rect.x -= 5;
            rect.y -= 5;
            setFrame(rect);
        }
        m_is_clicked = 0;
    }
    if (touch_it == touches.end()) {
        EventsMapIterator it = events_dispatch_map.find(MIControlEventTouchUpInside);
        if (it == events_dispatch_map.end() || (*touches.begin())->maintained_view != this || (*touches.begin())->touched_view != this || m_is_enabled == false) {
            ret = MIEventHandleStateNormal;
        }else{
            assert(it->second.view_controller != NULL);
            assert(it->second.event_func != NULL);
            EventFunc func = it->second.event_func;
            ((it->second.view_controller)->*func)(this);
            ret = MIEventHandleStateDone;
        }
    }else{
        EventsMapIterator it = events_dispatch_map.find(MIControlEventTouchUpOutside);
        if (it == events_dispatch_map.end() || m_is_enabled == false) {
            ret = MIEventHandleStateNormal;
        }else{
            assert(it->second.view_controller != NULL);
            assert(it->second.event_func != NULL);
            EventFunc func = it->second.event_func;
            ((it->second.view_controller)->*func)(this);
            ret = MIEventHandleStateDone;
        }
    }
    return ret;
}
MIEventHandleState MIButton::touchesCancelled(const std::set<MITouch*>& touches,
                                            const MIEvent* const event)
{
    MIControl::touchesCancelled(touches, event);
    if (m_is_clicked == 1) {
        if ((m_action_type & MIActionTypeMove) != 0) {
            MGRect rect = getFrame();
            rect.x -= 5;
            rect.y -= 5;
            setFrame(rect);
        }
        m_is_clicked = 0;
    }
    return MIEventHandleStateNormal;
}
//事件响应 
MIEventHandleState MIButton::handleEvent(const MIEvent* const event)
{
    return MIEventHandleStateNormal;
}

MIButton * MIButton::buttonWithType( MIButtonType buttonType )
{
	MIButton *button=NULL;
	switch (buttonType)
	{
	case MIButtonTypeRoundedRect:
		button = new MIRoundedRectButton();
		break;
    default:
        break;
	}
	if (button!=NULL)
	{
		button->m_buttonType=buttonType;
	}
	return button;
}

void MIButton::setTitleForState( const std::string &str,MIControlState state )
{
	
	switch (state)
	{
	case MIControlStateNormal:
		titles[0]=str;
		break;
	case MIControlStateHighlighted:
		titles[1]=str;
		break;
	}
}

void MIButton::setTitleColorForState( RGBAColorRef color,MIControlState state )
{
	switch (state)
	{
	case MIControlStateNormal:
		colors[0]=color;
		break;
	case MIControlStateHighlighted:
		colors[1]=color;
		break;
	}
}

void MIButton::setTitleShadowColorForState( RGBAColorRef color,MIControlState state )
{

}

const std::string & MIButton::titleForState( MIControlState state )
{
	switch (state)
	{
	case MIControlStateNormal:
		return titles[0];
	case MIControlStateHighlighted:
		return titles[1];
	}
	static std::string str("");
	return str;
}

z11::RGBAColorRef MIButton::titleColorForState( MIControlState state )
{
	switch(state)
	{
	case MIControlStateNormal:
		return colors[0];
	case MIControlStateHighlighted:
		return colors[1];
	}
	static RGBAColorRef ref=RGBAColorRef::white();
	return ref;
}

z11::RGBAColorRef MIButton::titleShadowColorForState( MIControlState state )
{
	static RGBAColorRef ref=RGBAColorRef::white();
	return ref;
}

MGImage * MIButton::backgroundImageForState( MIControlState state )
{
	return NULL;
}

MGImage * MIButton::imageForState( MIControlState state )
{
	switch (state)
	{
	case MIControlStateNormal:
		return images[0];
	case MIControlStateHighlighted:
		return images[1];
	}
	return NULL;
}

void MIButton::setBackgroundImageForState(MGImage *image, MIControlState state )
{

}

void MIButton::setImageForState(MGImage *image, MIControlState state )
{
	switch (state)
	{
	case MIControlStateNormal:
		images[0]=image;
		break;
	case MIControlStateHighlighted:
		images[1]=image;
		break;
	}
}

const string & MIButton::currentTitle()
{
	return titles[m_current_state];
}

const RGBAColorRef& MIButton::currentTitleColor()
{
	return colors[m_current_state];
}

const RGBAColorRef& MIButton::currentTitleShadowColor()
{
	static RGBAColorRef white=RGBAColorRef::white();
	return white;
}

const MGImage * MIButton::currentImage()
{
	return images[m_current_state];
}

const MGImage * MIButton::currentBackgroundImage()
{
	return NULL;
}

MIImageView * MIButton::imageView()
{
	return NULL;
}

z11::MIButtonType MIButton::buttonType()
{
	return m_buttonType;
}

MILabel * MIButton::titleLabel()
{
	if (m_titleLabel==NULL)
	{
		m_titleLabel = new MILabel;
		m_titleLabel->setText("");
		m_titleLabel->setFontSizeAndColor(15,RGBAColorRef::darkBlue());
		m_titleLabel->setTextAlignment(MITextAlignmentCenterMiddle);
		MGRect rect=m_frame;
		rect.x=rect.y=0;
		m_titleLabel->initWithFrame(rect);
		addSubview(m_titleLabel);
	}
	
	return m_titleLabel;
}

void MIButton::setCurrentState( int state )
{
	m_current_state=state;
}




















} //namespace z11