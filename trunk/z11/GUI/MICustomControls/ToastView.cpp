//
//  ToastView.cpp
//  WaterMobileGIS
//
//  Created by MacOSDevelop on 12-11-28.
//  Copyright (c) 2012年 Zhejiang University. All rights reserved.
//
#include "ToastView.h"
#include "../MIControls/MITextView.h"
#include "../MICustomControls/ImageResizableButton.h"
#include "../GUIContainer/GUIContainer.h"
#include "../../GraphicCore/MGFont.h"
#include "../../GraphicCore/MGCanvas.h"
#include "../../System/Shared/Utility.h"
#include <assert.h>
#include "stdio.h"
#define TOAST_VIEW_CONTENT_MAX_WIDTH            0.6f
#define TOAST_VIEW_BORDER_MARGIN                0.025f
#define TOAST_VIEW_ANIMATION_DURATION           0.2f
#define TOAST_VIEW_ANIMATION_NAME               "==========TOAST_ANIMATION_NAME=========="

#define TEXT_VIEW_FIXED_SIZE                    5

#define TEXT_BKG_VIEW_ROUND_RADIUS              12/640.0f
#define TEXT_BKG_VIEW_MIDDLE_PIXELS             10

namespace z11
{
    const float ToastView::LENGTH_SHORT = 1.0f;
    const float ToastView::LENGTH_LONG = 2.0f;
    
    Syncable ToastView::toast_locker;
    ToastView* ToastView::toast_view = NULL;
    uint_32 ToastView::toast_font_size = 32;
    float ToastView::text_view_fixed_size = TEXT_VIEW_FIXED_SIZE;
    ToastView::ToastView()
    {
        text_view = NULL;
        bkg_view = NULL;
        show_second = 1.0f;
        is_animating = false;
		_view_flags.user_interaction_enabled = 0;
        _view_flags.clip_subviews = 0;
        
        toast_font_size = 0.042f*MINVALUE(MGDevice::getScreenWidth(), MGDevice::getScreenHeight());
        text_view_fixed_size = 0.0078f*MINVALUE(MGDevice::getScreenWidth(), MGDevice::getScreenHeight());
    }
    
    ToastView::~ToastView()
    {
    }
    
    void ToastView::initWithFrame(MGRect frame)
    {
        MIView::initWithFrame(frame);
    }
    
    void ToastView::drawself()
    {
        // get visible time
        do
        {
            if(is_animating)
                break;
            MGTimeDelta time_delta = MGTimeTicks::Now()-begin_ticks;
            float delta_time = time_delta.InSecondsF();
            if(delta_time > show_second)
            {
                is_animating = true;
                hideAnimation();
                break;
            }
        }
        while(false);
        
        // init bkg
        do
        {
            if(bkg_view->getBackgroundColor() != NULL)
                break;
            
            // canvas
            int_32 radius = getRadious();
            float canvas_width = radius*2+TEXT_BKG_VIEW_MIDDLE_PIXELS;
            float canvas_height = radius*2+TEXT_BKG_VIEW_MIDDLE_PIXELS;
            char str[128];
            sprintf(str, "controls/toast_bkg_radius_%d.png", radius);
            string bkg_file_path = MGBundle::mainBundle()->tmpPathWithFileName(str);
            
            if(!Utility::isFileORDirExist(bkg_file_path))
            {
                Utility::createDirOnPath(bkg_file_path);
                
                MGCanvas canvas = MGCanvas(canvas_width, canvas_height, 32);
                MGRect draw_rect = MGRectMake(0.0f, 0.0f, canvas_width, canvas_height);
                
                canvas.drawRoundedRect(draw_rect, radius, radius, RGBAColorRef::black(200));
                canvas.writeToFile(bkg_file_path);

            }
            MGImage* image = MGImage::imageWithContentsOfFile(bkg_file_path);
            if(image == NULL)
            {
                Utility::delAFile(bkg_file_path);
            }
            else
            {
                bkg_view->setBackgroundColor(MGColor::fromImage(image));
            }
        }
        while(false);
        
        MIView::drawself();
    }
    
    void ToastView::hideAnimation()
    {
		is_time_forced = false;
        MIView::beginAnimation(TOAST_VIEW_ANIMATION_NAME);
        MIView::setAnimationCurve(MIViewAnimationOptionCurveLinear);
        MIView::setAnimationDuration(TOAST_VIEW_ANIMATION_DURATION);
        this->setAlpha(0.0f);
        MIView::commitAnimation();
    }
    
    void ToastView::init()
    {
        MGRect init_rect = MGRectMake(0.0f, 0.0f, 100.0f, 100.0f);
        initWithFrame(init_rect);
        _view_flags.clip_subviews = 0;
        GuiContainer::container->gui_root->view->addSubview(toast_view);
       
        setAlpha(0.0f);
        
        bkg_view = new ImageResizableButton();
        bkg_view->setBackgroundColor(NULL);
        bkg_view->initWithFrame(init_rect);
        addSubview(bkg_view);
        
        text_view = new MITextView();
        text_view->_view_flags.clip_subviews = 0;
        text_view->_scroll_view_flags.vertical_bounce_enabled = 0;
        text_view->_scroll_view_flags.vertical_bouncing = 0;
        text_view->_scroll_view_flags.vertical_indicator_show = 0;
        text_view->_scroll_view_flags.vertical_scroll_enabled = 0;
        text_view->_scroll_view_flags.passing_horizontal_scroll_enable = 1;
        text_view->_scroll_view_flags.passing_vertical_scroll_enable = 1;
        text_view->setBackgroundColor(MGColor::fromRGBA(0, 0, 0, 0));
        text_view->initWithFrame(init_rect);
        addSubview(text_view);
    }
    
    int_32 ToastView::getRadious()
    {
        return TEXT_BKG_VIEW_ROUND_RADIUS*MGDevice::getScreenWidth();
    }
    
    void ToastView::setFontSize(uint_32 font_size)
    {
        toast_font_size = font_size;
    }
    
    void ToastView::hide()
    {
        if(toast_view != NULL)
        {
            MIView::stopAnimation(TOAST_VIEW_ANIMATION_NAME);
            toast_view->MIView::hide();
        }
    }
    
    void ToastView::showText(const string &text, float show_second/* = 1.0f*/, bool time_forced/* = false*/)
    {
        if(toast_view == NULL)
        {
            toast_view = new ToastView();
            toast_view->init();
			toast_view->is_time_forced = false;
        }
		if (toast_view->is_time_forced)
		{
			return;
		}
        toast_view->text_view->setTextAlignment(MITextAlignmentLeftTop);
        toast_view->text_view->setFontSizeAndColor(toast_font_size, RGBAColorRef::white());
        toast_view->text_view->setText(text);
		toast_view->is_time_forced = time_forced;
        // get text rect
        MGFont* text_font = toast_view->text_view->getFont();
        float font_size = text_font->getFontSize();
        MGRect text_rect = text_font->getStringBound(text);
        if(text_rect.height < text_view_fixed_size+font_size)
            text_rect.height = text_view_fixed_size+font_size;
        if(text_rect.width > MGDevice::getScreenWidth()*TOAST_VIEW_CONTENT_MAX_WIDTH)
        {
            text_rect.width = MGDevice::getScreenWidth()*TOAST_VIEW_CONTENT_MAX_WIDTH;
            MGRect suggest_text_rect = text_font->getStringBoundForWidth(toast_view->text_view->getText(), text_rect.width-MITEXTVIEW_DEFAULT_BORDER_VALUE*2);
            suggest_text_rect.height += MITEXTVIEW_DEFAULT_BORDER_VALUE*2;
            text_rect.height = suggest_text_rect.height;
        }
        else
        {
            text_rect.width += 2*MITEXTVIEW_DEFAULT_BORDER_VALUE + 3; // 有时宽度不够 加3 //
            text_rect.height += 2*MITEXTVIEW_DEFAULT_BORDER_VALUE;
        }
        
        // set frame
        float margin_size = MGDevice::getScreenWidth()*TOAST_VIEW_BORDER_MARGIN;
        MGRect toast_view_rect = MGRectMake(0.0f, 0.0f, text_rect.width+2*margin_size, text_rect.height+2*margin_size);
        toast_view_rect.x = (MGDevice::getScreenWidth()-toast_view_rect.width)/2.0f;
        toast_view_rect.y = (MGDevice::getScreenHeight()-toast_view_rect.height)*0.4f;
        
        MIView* parent_view = toast_view->parent();
        toast_view->detachFromSuperview();
        toast_view->setFrame(toast_view_rect);
        parent_view->addSubview(toast_view);
        
        toast_view->bkg_view->setFrame(MGRectMake(0.0f, 0.0f, toast_view_rect.width, toast_view_rect.height));
        int_32 radious = toast_view->getRadious();
        toast_view->bkg_view->resizableWithCapInsets(MGEdgeInsetsMake(radious, radious, radious, radious));
        
        text_rect.x = text_rect.y = margin_size;
        toast_view->text_view->detachFromSuperview();
        toast_view->text_view->setFrame(text_rect);
        toast_view->text_view->_view_flags.clip_subviews = 0;
        toast_view->addSubview(toast_view->text_view);
        
        // init params
        toast_view->show_second = show_second;
        toast_view->begin_ticks = MGTimeTicks::Now();
        toast_view->stopAnimation(TOAST_VIEW_ANIMATION_NAME);
        toast_view->setBackgroundColor(NULL);
        toast_view->is_animating = false;
        
        toast_view->show();
        toast_view->setAlpha(1.0f);
    }

	bool ToastView::isHidden()
	{
		bool ret = true;
		if(toast_view != NULL)
		{
			ret = toast_view->isHide();
		}
		return ret;
	}
}