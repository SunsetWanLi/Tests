//
//  MITextField.cpp
//  Gui
//
//  Created by Wang ZhiPeng on 11-6-13.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//
#include "MITextField.h"
#include "MIScaleView.h"
#include "MITextEditMenu.h"
#include "../Events/MIEvent.h"
#include "../../GraphicCore/MGFont.h"
#include "../GuiContainer/GuiContainer.h"
#include "../../System/Shared/MGLog.h"
#include "../../System/Device/MGDevice.h"
#include "../../System/IO/MGFileManager.h"
#include "../../GraphicCore/MGCanvas.h"
#include "../../System/ClipBoard/MGClipBoard.h"
#include "../GestureRecognizer/MILongPressGestureRecognizer.h"
#include "../GestureRecognizer/MITapGestureRecognizer.h"
#include "MICursor.h"
#include <stdio.h>
#include <math.h>
#include <algorithm>
using namespace std;

#define        TEXT_FIELD_SCALE_VIEW_WIDTH                    (300.0f/640.0f)
#define        TEXT_FIELD_SCALE_VIEW_ASPECT_RATIO             (0.3f)
#define        TEXT_FIELD_MENU_EDGE                           (30/640.0f)

namespace z11
{
#pragma mark - MITextField implementation - -
    MITextField::MITextField()
    {
        class_name = "MITextField";
		text_field_delegate = NULL;
		m_text_font = MGFont::create(12, RGBAColorMake(0, 0, 0));
        m_placeholder_font = MGFont::create(12, RGBAColorRef::darkGray());;
        
        m_previous_moving_cursor = NULL;
		m_cursor = NULL;
        m_select_begin_cursor = NULL;
        m_select_end_cursor = NULL;
        
		m_editing = false;
		textContentMode = MITextFieldContentModeNormal;
		m_text_alignment = MITextAlignmentLeftMiddle;
        m_lineBreakMode = MILineBreakModeClip;
		m_leftView = NULL;
		m_rightView = NULL;
		borderStyle = MITextBorderStyleSkia;
        m_preinput_text = "";
        m_text_content = "";
		m_unselected_image = NULL;
		m_selected_image = NULL;
		m_bounds_image = NULL;
        m_long_pressed = false;
        m_long_press_touch.tag = INVALID_TOUCH_TAG;
        
        m_editing = false;
        m_selecting = false;
        
        m_editable = true;
        m_selectable = true;
        m_copyable = true;
        
        // init text_scale_view
        text_scale_view = new MIScaleView();
        text_scale_view->setScaleValue(1.5f);
        float text_scale_view_width = TEXT_FIELD_SCALE_VIEW_WIDTH*MINVALUE(MGDevice::getScreenWidth(), MGDevice::getScreenHeight());
        text_scale_view->initWithFrame(MGRectMake(0, 0, text_scale_view_width, TEXT_FIELD_SCALE_VIEW_ASPECT_RATIO*text_scale_view_width));
        float edge_value = (5/640.0f)*MINVALUE(MGDevice::getScreenWidth(), MGDevice::getScreenHeight());
        text_scale_view->setResizableBkgWithCapInsets(MGColor::fromColorRef(RGBAColorRef::lightGray()), MGEdgeInsetsMake(edge_value, edge_value, edge_value, edge_value));
        text_scale_view->hide();
        
        // init text_edit_menu
        text_edit_menu = new MITextEditMenu();
        text_edit_menu->setButtonTextEnabled(true);
        text_edit_menu->setContentView(this);
        text_edit_menu->setTextMenuDelegate(new MITextEditMenuDelegate());
        text_edit_menu->initWithFrame(MGRectMake(0.0f, 0.0f, 10.0f, 10.0f));
    }
    
    MITextField::~MITextField()
	{
        if (isFirstResponder()) {
            resignFirstResponder();
        }
		delete_and_set_null(m_selected_image);
		delete_and_set_null(m_unselected_image);
		delete_and_set_null(m_text_font);
        delete_and_set_null(m_placeholder_font);
        // clear btn, left view, right view, cursor都是subview, 不需要单独清理 //
        delete_and_set_null(m_long_press_gesture);
        delete_and_set_null(m_single_tap_gesture);
        delete_and_set_null(m_double_tap_gesture);
        
        if(text_scale_view != NULL)
        {
            text_scale_view->detachFromSuperview();
            delete_and_set_null(text_scale_view);
        }
        
        if(text_edit_menu != NULL)
        {
            text_edit_menu->detachFromSuperview();
            delete_and_set_null(text_edit_menu);
        }
    }
    
#pragma mark - MIVIEW DECLARE -
    void MITextField::initWithFrame(MGRect frame)
    {
        MIControl::initWithFrame(frame);
        MGRect rect = textRectForBounds(m_bounds);
        m_text_font->setFaceSize(rect.height * 0.5);
        
        // cursors
        do
        {
            m_cursor = new MICursor();
            float cursor_height = m_text_font->getFontSize() * 1.5;
            if (cursor_height > rect.height)
            {
                cursor_height = rect.height;
            }
            else
            {
                rect.y += (rect.height - cursor_height) * 0.5;
            }
            m_cursor->initWithFrame(MGRectMake(rect.x - m_bounds.x, rect.y - m_bounds.y, 2, cursor_height));
            m_cursor->setBackgroundColor(MGColor::fromColorRef(RGBAColorRef::skyBlue()));
            m_cursor->hide();
            addSubview(m_cursor);
        }
        while(false);
        
        do
        {
            m_select_begin_cursor = new MICursor();
            m_select_begin_cursor->initWithFrame(m_cursor->getFrame());
            m_select_begin_cursor->setBackgroundColor(MGColor::fromColorRef(RGBAColorRef::skyBlue()));
            m_select_begin_cursor->hide();
            addSubview(m_select_begin_cursor);
            
            m_select_end_cursor = new MICursor();
            m_select_end_cursor->initWithFrame(m_cursor->getFrame());
            m_select_end_cursor->setBackgroundColor(MGColor::fromColorRef(RGBAColorRef::skyBlue()));
            m_select_end_cursor->hide();
            addSubview(m_select_end_cursor);
        }
        while(false);
        
        // m_background_color
		if (m_background_color == NULL)
		{
			string path=MGBundle::mainBundle()->tmpPathWithFileName("controls/");
			MGFileManager::defaultManager()->contentsOfDirectoryAtURL(path);
            
			char filename[256];
			int width=frame.width;
			int height=frame.height;
			sprintf(filename,"TextField%dX%d_unselected.png",width,height);
			path+=filename;
			if (!MGFileManager::defaultManager()->fileExistsAtPath(path))
			{
				MGCanvas canvas(width+30,height+30,32);
				canvas.drawFuzzyRect(MGRectMake(0,0,width,height),7,7,RGBAColorRef::lightGray());
				canvas.writeToFile(path);
			}
            
			m_bounds_image=m_unselected_image=MGImage::imageWithContentsOfFile(path);
            
			sprintf(filename,"TextField%dX%d_selected.png",width,height);
			path=MGBundle::mainBundle()->tmpPathWithFileName("controls/")+filename;
			if (!MGFileManager::defaultManager()->fileExistsAtPath(path))
			{
				MGCanvas canvas(width+30,height+30,32);
				canvas.drawFuzzyRect(MGRectMake(0,0,width,height),7,7,RGBAColorRef::blue());
				canvas.writeToFile(path);
			}
			
			m_selected_image=MGImage::imageWithContentsOfFile(path);
		}
        
        // 初始化手势识别 //
        m_long_press_gesture = new MILongPressGestureRecognizer();
        m_single_tap_gesture = new MITapGestureRecognizer();
        m_double_tap_gesture = new MITapGestureRecognizer();
        
        m_double_tap_gesture->numberOfTapsRequired = 2;
        
        SELHandler sel_handler;
        sel_handler.sender = m_single_tap_gesture;
		sel_handler.sel_func = (SELFunc)&MITextField::singleTapCall;
		m_single_tap_gesture->initWithTarget(this,sel_handler);
		addGestureRecognizer(m_single_tap_gesture);
        
        sel_handler.sender = m_double_tap_gesture;
		sel_handler.sel_func = (SELFunc)&MITextField::doubleTapCall;
		m_double_tap_gesture->initWithTarget(this,sel_handler);
		addGestureRecognizer(m_double_tap_gesture);
        
        m_single_tap_gesture->requireGestureRecognizerToFail(m_double_tap_gesture);
        
        m_long_press_gesture->allowableMovement = 999999;
		m_long_press_gesture->setMinimumPressDuration(MGTimeDelta::FromSecondsF(0.5f));
        sel_handler.sender = m_long_press_gesture;
		sel_handler.sel_func = (SELFunc)&MITextField::longPressCall;
		m_long_press_gesture->initWithTarget(this, sel_handler);
		addGestureRecognizer(m_long_press_gesture);
        
        m_single_tap_gesture->requireGestureRecognizerToFail(m_long_press_gesture);
    }
    
	void MITextField::drawself()
    {
        MGGraphicEngine* engine = MGGraphicEngine::sharedGraphicEngine();
        
        // 绘制背景 //
        RGBAColorRef color_ref;
        if (m_background_color != NULL) {
            color_ref = m_background_color->m_color;
            color_ref.m_alpha_component = (byte_8)(m_alpha * color_ref.m_alpha_component);
        }else{
            color_ref = RGBAColorMake(255, 255, 255, (byte_8)(m_alpha * 255));
        }
		if (m_background_color != NULL && m_background_color->m_image != NULL) {
			m_background_color->m_image->sendDataToContext();
			MGRect image_rect;
			image_rect.width = m_background_color->m_image->getWidth();
			image_rect.height = m_background_color->m_image->getHeight();
			engine->drawImage(m_background_color->m_image, m_bounds, color_ref, draw_image_mode, image_rect);
		}else{
            switch (borderStyle)
            {
                case MITextBorderStyleNone:
                    break;
                case MITextBorderStyleLine:
                    if (m_editing) {
                        engine->drawFilledRect(m_bounds, color_ref);
                        engine->drawRect(m_bounds, RGBAColorRef::lightGray());
                    } else {
                        engine->drawFilledRect(m_bounds, color_ref);
                        engine->drawRect(m_bounds, RGBAColorRef::lightGray());
                    }
                    break;
                case MITextBorderStyleBezel:
                    break;
                case MITextBorderStyleRoundedRect:
                    if (m_editing) {
                        engine->drawFilledRoundRect(m_bounds, color_ref);
                        engine->drawRoundRect(m_bounds, RGBAColorRef::lightGray());
                    } else {
                        engine->drawFilledRoundRect(m_bounds, color_ref);
                        engine->drawRoundRect(m_bounds, RGBAColorRef::lightGray());
                    }
                    break;
                case MITextBorderStyleSkia:
                    if(m_bounds_image!=NULL)
                    {
                        MGRect image_rect;
                        image_rect.width=m_bounds_image->getWidth();
                        image_rect.height=m_bounds_image->getHeight();
                        MGRect image_bounds=m_bounds;
                        {
                            image_bounds.x-=(image_rect.width-image_bounds.width)/2;
                            image_bounds.y-=(image_rect.height-image_bounds.height)/2;
                            image_bounds.width=image_rect.width;
                            image_bounds.height=image_rect.height;
                        }
                        m_bounds_image->sendDataToContext();
                        engine->drawImage(m_bounds_image, image_bounds, color_ref, draw_image_mode, image_rect);
                        break;
                    }
                default:
                    break;
            }
		}
        
        MGRect rect = textRectForBounds(m_bounds);
        // 绘制文本 //
		if(!m_text_content.empty() || !m_preinput_text.empty())
		{
			drawTextInRect(rect);
		}
		else
		{
            rect = placeholderRectForBounds(m_bounds);
			drawPlaceholderInRect(rect);
		}
        // 绘制选中字背景 //
        drawSelectTextBackground();
    }
    
    void MITextField::drawSelectTextBackground()
    {
        MGGraphicEngine* engine = MGGraphicEngine::sharedGraphicEngine();
        if(engine == NULL)
        {
            return;
        }
        if(m_select_begin_cursor->getLocation() >= m_select_end_cursor->getLocation() || !m_selecting)
        {
            return;
        }
        
        MGRect begin_cursor_rect = m_select_begin_cursor->getBounds();
        MGRect end_cursor_rect = m_select_end_cursor->getBounds();
        RGBAColorRef color_ref = RGBAColorRef::lightBlue(100);
        MGRect draw_rect = MGRectMake(begin_cursor_rect.x+begin_cursor_rect.width, begin_cursor_rect.y, end_cursor_rect.x-(begin_cursor_rect.x+begin_cursor_rect.width), begin_cursor_rect.height);
        if(draw_rect.width > 0.0f)
        {
            engine->drawFilledRect(draw_rect, color_ref);
        }
    }
    
    void MITextField::drawPlaceholderInRect(const MGRect& rect)
    {
        MGGraphicEngine* engine = MGGraphicEngine::sharedGraphicEngine();
        engine->drawText(placeholder,m_placeholder_font,rect,m_lineBreakMode, m_text_alignment);
    }
    
    void MITextField::drawTextInRect(const MGRect& _rect)
    {
        MGRect rect = _rect;
        MGGraphicEngine* engine = MGGraphicEngine::sharedGraphicEngine();
        MGUTF8String content = m_text_content;
        uint_32 m_cursor_location = m_cursor->getLocation();
        if (m_cursor_location > content.size()) {
            m_cursor_location = content.size();
        }
        MGUTF8String before_str = content.substr(0, m_cursor_location);
        MGUTF8String after_str = content.substr(m_cursor_location);
        MGUTF8String draw_text = before_str + m_preinput_text + after_str;
        if (textContentMode == MITextFieldContentModePassword)
        {
            before_str = string(before_str.length(), '*');
            draw_text = before_str + m_preinput_text + MGUTF8String(string(after_str.length(), '*'));
        }
        MGRect text_real_rect = m_text_font->getStringBound(draw_text.toSTLString());
        transAndScaleClipRect(rect);
        engine->enableClip(rect);
        rect = _rect;
        rect.width = text_real_rect.width + text_real_rect.x + m_text_font->getFontSize();  // 计算出的是字符边框，增加一点区域以作为绘制区域 //
        rect.width = rect.width > _rect.width ? rect.width : _rect.width;
        rect.x += m_text_pos.x;
        MILineBreakMode mode = m_lineBreakMode;
        if (m_editing) {
            mode = MILineBreakModeClip;
        }
        engine->drawText(draw_text.toSTLString(), m_text_font, rect, mode, m_text_alignment);
        if(m_preinput_text.length() > 0)
        {
            rect = _rect;
            MGRect bound = text_real_rect;
            MGRect marked_bound = m_text_font->getStringBound(m_preinput_text.toSTLString());
            bound.width -= marked_bound.width;
            if(before_str.size() == 0 || bound.width < 0)
            {
                bound.width = 0;
            }
            if(m_preinput_text.size() == 0)
            {
                marked_bound.width = 0;
            }
            bound.height = m_text_font->getFontSize()*1.5; // 获得的高度只是当前字符的高度，不能作为绘制区域的高度，而且绘制区域的高度应该比字体高度要大 //
            if (bound.height > rect.height) {
                bound.height = rect.height;
            } else{
                rect.y += (rect.height - bound.height) * 0.5;
            }
            marked_bound.height = bound.height;
            engine->drawFilledRect(MGRectMake(rect.x+ m_text_pos.x + bound.x + bound.width, rect.y,marked_bound.width,marked_bound.height),RGBAColorRef::lightBlue(100));
        }
        engine->disableClip();
    }
    
#pragma mark - MIRESPONDER DECLARE -
    bool MITextField::canBecomeFirstResponder()
    {
        return true;
    }
    
    void MITextField::becomeFirstResponder()
    {
        if(text_field_delegate != NULL)
        {
            if(!text_field_delegate->shouldBeginEditing(this))
            {
                return;
            }
        }
        
        if(isFirstResponder() && !m_preinput_text.empty())
        {
            insertText(m_preinput_text.toSTLString());
            m_preinput_text.clear();
        }
        
        //show cursor
		startEditing();
        
        if (!isFirstResponder())
        {
            // 根据当前光标位置，判断字符内容是否需要前移以保证光标可见，设置m_text_pos的值 //
            uint_32 current_location = m_text_content.size();
            m_cursor->setLocation(current_location);
            MGRect rect = textRectForBounds(m_bounds);
            m_text_pos.x = 0;
            float before_text_width = getTextWidthForLocation(current_location);
            if (rect.width < before_text_width + m_text_pos.x - m_cursor->getFrame().width)
            {
                m_text_pos.x = (rect.width - before_text_width - m_cursor->getFrame().width);
            }
            adjustCursorLocation(m_cursor);
        }
        
        MIControl::becomeFirstResponder();
        highlighted();
        MIEvent* event = new MIEvent;
        event->_event_type = BECOME_FIRST_RESPONDER_EVENT;
        event->_responder = this;
        MIKeyboardTypeObj* key_obj = new MIKeyboardTypeObj();
        key_obj->keyboard_type = keyboard_type;
        key_obj->return_key_type = return_key_type;
        event->_content = key_obj;
        GuiContainer::container->preserveEvent(event);
        
        // move view for editing //
        //GuiContainer::container->moveEntirViewForKeyboardWithView(this);
        
		m_bounds_image = m_selected_image;
        
        if (text_field_delegate != NULL) {
            text_field_delegate->didBeginEditing(this);
        }
    }
    
    void MITextField::clearContext(MIView* view)
    {
        if(text_edit_menu != NULL && !text_edit_menu->containedSubview(view))
        {
            text_edit_menu->hideAndStopAnimation();
        }
    }
    
    void MITextField::resignFirstResponder(bool closeKB)
    {
        if (text_field_delegate != NULL) {
            if(!text_field_delegate->shouldEndEditing(this))
                return;
        }
        MIControl::resignFirstResponder(closeKB);
        unhignlighted();
        if (closeKB == true) {
            MIEvent* event = new MIEvent;
            event->_event_type = RESIGN_FIRST_RESPONDER_EVENT;
            event->_responder = this;
            GuiContainer::container->preserveEvent(event);
        }
        
		//show cursor
        finishEditing();
        finishSelecting();
        
        m_text_pos.x = 0;
        m_text_pos.y = 0;
        m_preinput_text.clear();
        text_width_caches.clear();
        
        m_bounds_image = m_unselected_image;
        
        if (text_field_delegate != NULL) {
            text_field_delegate->didEndEditing(this);
        }
    }
    
    int MITextField::adjustWithKeyboardHeight(int height)
    {
        if (text_field_delegate != NULL) {
            return m_bounds.height + text_field_delegate->shouldAdjustKeyboardWithHeight(this, height);
        }
        return m_bounds.height;
    }
    
    MGPoint MITextField::touchPointInContainerView(const MITouch* const touch)
    {
        MGPoint point = touch->locationInView(this);
        point.x = point.x - m_bounds.x;
        point.y = point.y - m_bounds.y;
        return point;
    }
    
#pragma mark - MIEditableTextView interface -
    bool MITextField::hasText()
    {
        return m_text_content.size() > 0;
    }
   
    void MITextField::insertText(const string &text)
    {
        uint_32 current_cursor_location = m_cursor->getLocation();
        if (text_field_delegate != NULL)
        {
            MGRange range;
            range.length = 0;
            range.location = current_cursor_location;
            if(!text_field_delegate->shouldChangeCharactersInRange(this, range, text))
            {
                return;
            }
        }
        string ins_txt = checkInputString(m_text_content.toSTLString(), text);
        if (ins_txt.size() == 0)
        {
            becomeFirstResponder();
            return;
        }
        text_width_caches.clear();
        hideMenuWithAnimation();
        
        // 根据输入内容和选定的位置替换文本内容，并设置新的光标位置 //
        MGUTF8String text_content(ins_txt);
        text_content.cleanSpecialChar('\r');
        text_content.cleanSpecialChar('\n');
        if(!m_preinput_text.empty())
        {
            string tmp_text = ins_txt;
            tmp_text.erase(remove_if(tmp_text.begin(), tmp_text.end(), ::isspace), tmp_text.end());
            text_content = MGUTF8String(tmp_text);
        }
        m_preinput_text = "";
        
        uint_32 m_cursor_location = m_cursor->getLocation();
        uint_32 select_start_position = m_select_begin_cursor->getLocation();
        uint_32 select_end_location = m_select_end_cursor->getLocation();
        assert(select_start_position <= select_end_location);
        uint_32 select_length = select_end_location-select_start_position;
        if(select_length > 0)
        {
            m_text_content.replace(select_start_position, select_length, text_content);
            m_cursor->setLocation(select_start_position+text_content.length());
            startEditing();
        }
        else
        {
            m_text_content.replace(m_cursor_location, 0, text_content);
            m_cursor->setLocation(m_cursor_location+text_content.length());
        }
        
        // 根据当前光标位置，判断字符内容是否需要前移以保证光标可见，设置m_text_pos的值 //
        MGRect rect = textRectForBounds(m_bounds);
        float before_text_width = getTextWidthForLocation(m_cursor->getLocation());
        if (rect.width < before_text_width + m_text_pos.x - m_cursor->getFrame().width)
        {
            m_text_pos.x = (rect.width - before_text_width - m_cursor->getFrame().width);
        }
        
        adjustCursorLocation(m_cursor);
    }
    
    void MITextField::insertPreText(const string &pre_text)
    {
        if (m_preinput_text.size() == 0 && pre_text.size() != 0)
        {
            m_pre_text_save_pos = m_text_pos;
        }
        text_width_caches.clear();
        hideMenuWithAnimation();
        
        if(m_selecting)
        {
            insertText("");
        }
        
        m_preinput_text = pre_text;
        float before_text_width = getTextWidthForLocation(m_cursor->getLocation());
        // 根据当前光标位置，判断字符内容是否需要前移以保证光标可见，设置m_text_pos的值 //
        MGRect rect = textRectForBounds(m_bounds);
        if (rect.width < before_text_width + m_pre_text_save_pos.x - m_cursor->getFrame().width)
        {
            m_text_pos.x = (rect.width - before_text_width - m_cursor->getFrame().width);
        }
        else
        {
            m_text_pos = m_pre_text_save_pos;
        }
        adjustCursorLocation(m_cursor);
    }
    
    void MITextField::deleteBackward()
    {
        // return if text is empty
        if(m_text_content.empty())
        {
            return;
        }
        text_width_caches.clear();
        hideMenuWithAnimation();
        
        // get location
        uint_32 m_cursor_location = m_cursor->getLocation();
        uint_32 select_start_location = m_select_begin_cursor->getLocation();
        uint_32 select_end_location = m_select_end_cursor->getLocation();
        assert(select_start_location <= select_end_location);
        uint_32 select_length = select_end_location-select_start_location;
        
        // return if should not change
        if (text_field_delegate != NULL)
        {
            MGRange range;
            if(select_length > 0)
            {
                range.location = select_start_location;
                range.length = select_length;
            }
            else
            {
                range.location = m_cursor_location;
                range.length = 0;
            }
            if(!text_field_delegate->shouldChangeCharactersInRange(this, range, ""))
            {
                return;
            }
        }
        
        // 根据当前选定状态，删除对应文本，并设置新的光标位置 //
        if (select_length > 0)
        {
            m_text_content.replace(select_start_location, select_length, MGUTF8String(""));
            finishSelecting();
            startEditing();
            m_cursor->setLocation(select_start_location);
        }
        else
        {
            if (m_cursor_location == 0)
            {
                return;
            }
            --m_cursor_location;
            m_cursor->setLocation(m_cursor_location);
            m_text_content.replace(m_cursor_location, 1, MGUTF8String(""));
        }
        
        // 根据当前光标位置，判断字符内容是否需要移动以保证光标不动，设置m_text_pos的值 //
        float before_text_width = getTextWidthForLocation(m_cursor->getLocation());
        if (m_text_pos.x < 0) {
            MGRect rect = textRectForBounds(m_bounds);
            m_text_pos.x -= rect.x - m_bounds.x + m_text_pos.x + before_text_width - m_cursor->getFrame().x;
        }
        if (m_text_pos.x > 0)
        {
            m_text_pos.x = 0;
        }
        adjustCursorLocation(m_cursor);
    }
    
    void MITextField::selectText()
    {
        startSelecting();
    }
    
    void MITextField::selectAllText()
    {
        if(!m_text_content.empty() && m_preinput_text.empty())
        {
            startSelecting();
            m_select_begin_cursor->setLocation(0);
            m_select_end_cursor->setLocation(m_text_content.length());
            adjustCursorLocation(m_select_end_cursor);
            adjustCursorLocation(m_select_begin_cursor, false);
        }
    }
    
    void MITextField::copySelectedText()
    {
        uint_32 start_location = m_select_begin_cursor->getLocation();
        uint_32 end_location = m_select_end_cursor->getLocation();
        assert(start_location <= end_location);
        uint_32 length = end_location-start_location;
        
        if(length > 0)
        {
            MGUTF8String clipboard_text = m_text_content.substr(start_location, length);
            MGClipBoard::setString(clipboard_text.toSTLString());
        }
        
        finishSelecting();
        if(m_editing)
        {
            m_cursor->animate(true);
        }
        else
        {
            m_cursor->animate(false);
        }
        hideMenuWithAnimation();
    }
    
    void MITextField::cutSelectedText()
    {
        uint_32 start_location = m_select_begin_cursor->getLocation();
        uint_32 end_location = m_select_end_cursor->getLocation();
        assert(start_location <= end_location);
        uint_32 length = end_location-start_location;
        
        if(length > 0)
        {
            MGUTF8String clipboard_text = m_text_content.substr(start_location, length);
            MGClipBoard::setString(clipboard_text.toSTLString());
            m_text_content.replace(start_location, length, MGUTF8String(""));
            m_cursor->setLocation(start_location);
            
            EventsMapIterator it = events_dispatch_map.find(MIControlEventValueChanged);
            if (it != events_dispatch_map.end())
            {
                assert(it->second.view_controller != NULL);
                assert(it->second.event_func != NULL);
                EventFunc func = it->second.event_func;
                ((it->second.view_controller)->*func)(this);
            }
        }

        finishSelecting();
        if(m_editing)
        {
            m_cursor->animate(true);
        }
        else
        {
            m_cursor->animate(false);
        }
        hideMenuWithAnimation();
        
        float before_text_width = getTextWidthForLocation(m_cursor->getLocation());
        if (m_text_pos.x < 0) {
            MGRect rect = textRectForBounds(m_bounds);
            m_text_pos.x -= rect.x - m_bounds.x + m_text_pos.x + before_text_width - m_cursor->getFrame().x;
        }
        if (m_text_pos.x > 0)
        {
            m_text_pos.x = 0;
        }
        adjustCursorLocation(m_cursor);
        
        if(!isFirstResponder())
        {
            becomeFirstResponder();
        }
    }
    
    void MITextField::pasteClipboardText()
    {
        MGUTF8String pre = m_text_content;
        insertText(MGClipBoard::getString());
        
        if (pre.toSTLString() != m_text_content.toSTLString())
        {
            EventsMapIterator it = events_dispatch_map.find(MIControlEventValueChanged);
            if (it != events_dispatch_map.end())
            {
                assert(it->second.view_controller != NULL);
                assert(it->second.event_func != NULL);
                EventFunc func = it->second.event_func;
                ((it->second.view_controller)->*func)(this);
            }
        }
        
        if(!isFirstResponder())
        {
            becomeFirstResponder();
        }
    }
    
#pragma mark - Drawing and Positioning Overrides -
    void MITextField::setFontSizeAndColor(uint_32 font_size, const RGBAColorRef &font_color/*= RGBAColorRef::black() */)
	{
        if (m_text_font == NULL) {
            return;
        }
        m_text_font->setFaceSize(font_size);
		setTextColor(font_color);
        if (m_placeholder_font == NULL)
            return;
        m_placeholder_font->setFaceSize(font_size);
	}
    
    void MITextField::setPlaceholderFontSizeAndColor(uint_32 font_size, const RGBAColorRef &font_color/*= RGBAColorRef::darkGray()*/)
    {
        if (m_placeholder_font == NULL) {
            return;
        }
        m_placeholder_font->setFaceSize(font_size);
        setPlaceholderColor(font_color);
    }
    
    void MITextField::setText(const string &text)
    {
        text_width_caches.clear();
        hideMenuWithAnimation();
        
        m_text_pos.x = 0;
        m_text_pos.y = 0;
        m_pre_text_save_pos = m_text_pos;
        m_preinput_text = "";
        m_long_pressed = false;
        m_long_press_touch.tag = INVALID_TOUCH_TAG;
        
        string tmp_text = text;
        tmp_text.erase(std::remove(tmp_text.begin(), tmp_text.end(), 0x09), tmp_text.end());
        tmp_text.erase(std::remove(tmp_text.begin(), tmp_text.end(), 0x0A), tmp_text.end());
		tmp_text.erase(std::remove(tmp_text.begin(), tmp_text.end(), 0x0D), tmp_text.end());
		m_text_content = tmp_text;
        
        if (_view_flags.view_initialized == 1)
        {
            if(m_selecting)
            {
                finishSelecting();
            }
            // 根据当前光标位置，判断字符内容是否需要前移以保证光标可见，设置m_text_pos的值 //
            uint_32 current_cursor_location = m_text_content.length();
            m_cursor->setLocation(current_cursor_location);
            float before_text_width = getTextWidthForLocation(current_cursor_location);
            MGRect rect = textRectForBounds(m_bounds);
            if (rect.width < before_text_width + m_text_pos.x - m_cursor->getFrame().width)
            {
                m_text_pos.x = (rect.width - before_text_width - m_cursor->getFrame().width);
            }
            adjustCursorLocation(m_cursor);
        }
    }
    
    string MITextField::getText()
    {
        return m_text_content.toSTLString();
    }
    
    uint_32 MITextField::getTextLength()
    {
        return m_text_content.length();
    }
    
	void MITextField::setTextColor( const RGBAColorRef &color )
	{
		m_text_font->setColor(color);
	}
    
	z11::RGBAColorRef MITextField::getTextColor()
	{
		return m_text_font->getColor();
	}
    
    void MITextField::setPlaceholderColor( const RGBAColorRef &color )
    {
        m_placeholder_font->setColor(color);
    }
    
    z11::RGBAColorRef MITextField::getPlaceholderColor()
    {
        return m_placeholder_font->getColor();
    }
    
    MGRect MITextField::textRectForBounds(const MGRect& rect)
    {
        MGRect ret = MGRectMake(rect.x + MITEXTFIELD_DEFAULT_BORDER_X_VALUE, rect.y + MITEXTFIELD_DEFAULT_BORDER_Y_VALUE, rect.width - 2*MITEXTFIELD_DEFAULT_BORDER_X_VALUE, rect.height - 2*MITEXTFIELD_DEFAULT_BORDER_Y_VALUE);
        return ret;
    }
    
    MGRect MITextField::placeholderRectForBounds(const MGRect& rect)
    {
        MGRect ret = MGRectMake(rect.x + 4, rect.y + 2, rect.width - 4, rect.height - 8);
        return ret;
    }
    
    MGRect MITextField::borderRectForBounds(const MGRect& rect)
    {
        return m_bounds;
    }
    
    MGRect MITextField::editingRectForBounds(const MGRect& rect)
    {
        return m_bounds;
    }
    
    MGRect MITextField::clearButtonRectForBounds(const MGRect& rect)
    {
        return m_bounds;
    }
    
    MGRect MITextField::leftViewRectForBounds(const MGRect& rect)
    {
        return m_bounds;
    }
    
    MGRect MITextField::rightViewRectForBounds(const MGRect& rect)
    {
        return m_bounds;
    }
    
#pragma mark - MIRESPONDER TOUCH DECLARE -
    MICursor* MITextField::getSelectCurserNearby(const MGPoint &point)
    {
        MICursor* ret = NULL;
        
        do
        {
            if(m_select_begin_cursor->isHide() && m_select_end_cursor->isHide())
            {
                //assert(0);
                break;
            }
            
            MGRect m_select_begin_cursor_bounds = m_select_begin_cursor->getFrame();
            MGRect m_select_end_cursor_bounds = m_select_end_cursor->getFrame();
            
            MGRect cursor_small_bounds = MGRectMake(MINVALUE(m_select_begin_cursor_bounds.x, m_select_end_cursor_bounds.x), MINVALUE(m_select_begin_cursor_bounds.y+m_select_begin_cursor_bounds.height, m_select_end_cursor_bounds.y), fabs(m_select_end_cursor_bounds.x-m_select_begin_cursor_bounds.x), fabs(m_select_end_cursor_bounds.y-m_select_begin_cursor_bounds.height-m_select_begin_cursor_bounds.y));
            
            if(point.y < cursor_small_bounds.y)
            {
                ret = m_select_begin_cursor;
                break;
            }
            else if(point.y > cursor_small_bounds.y+cursor_small_bounds.height)
            {
                ret = m_select_end_cursor;
                break;
            }
            
            if(point.x < cursor_small_bounds.x)
            {
                if(m_select_begin_cursor_bounds.x < m_select_end_cursor_bounds.x)
                {
                    ret = m_select_begin_cursor;
                }
                else
                {
                    ret = m_select_end_cursor;
                }
                break;
            }
            else if(point.x > cursor_small_bounds.x+cursor_small_bounds.width)
            {
                if(m_select_begin_cursor_bounds.x > m_select_end_cursor_bounds.x)
                {
                    ret = m_select_begin_cursor;
                }
                else
                {
                    ret = m_select_end_cursor;
                }
                break;
            }
            
            MGPoint begin_cursor_center = MGPointMake(m_select_begin_cursor_bounds.x+m_select_begin_cursor_bounds.width/2.0f, m_select_begin_cursor_bounds.y+m_select_begin_cursor_bounds.height/2.0f);
            MGPoint end_cursor_center = MGPointMake(m_select_end_cursor_bounds.x+m_select_end_cursor_bounds.width/2.0f, m_select_end_cursor_bounds.y+m_select_end_cursor_bounds.height/2.0f);
            float cursor_begin_distance = distanceOfTwoPoint(point, begin_cursor_center);
            float cursor_end_distance = distanceOfTwoPoint(point, end_cursor_center);
            
            if(cursor_begin_distance < cursor_end_distance)
            {
                ret = m_select_begin_cursor;
                break;
            }
            else
            {
                ret = m_select_end_cursor;
                break;
            }
        }
        while(false);
        
        return ret;
    }

    MIEventHandleState MITextField::touchesBegan(const std::set<MITouch*>& touches, const MIEvent* const event)
    {
        MIEventHandleState ret = MIControl::touchesBegan(touches, event);
        MITouch* touch = *touches.begin();
        if (touch->maintained_view != this)
        {
            return ret;
        }

        if (m_long_press_touch.tag == INVALID_TOUCH_TAG)
        {
            if(m_editable || m_selectable)
            {
                m_long_press_touch = *touch;
                m_long_pressed = false;
                if(m_preinput_text.empty())
                {
                    setCursorFrameWithPoint(m_cursor, touchPointInContainerView(touch));
                }
            }
            else if(m_copyable)
            {
                m_long_press_touch = *touch;
                m_long_pressed = false;
            }
        }
        return MIEventHandleStateDone;
    }
    
    MIEventHandleState MITextField::touchesMoved(const std::set<MITouch*>& touches, const MIEvent* const event)
    {
        MIEventHandleState ret = MIControl::touchesMoved(touches, event);
        MITouch* touch = *touches.begin();
        if (touch->maintained_view != this)
        {
            return ret;
        }
        
        do
        {
            for(set<MITouch*>::const_iterator it = touches.begin(); it != touches.end(); ++it)
            {
                if((*it)->tag == m_long_press_touch.tag)
                {
                    MGPoint touch_point = touchPointInContainerView(*it);
                    if(!m_cursor->isHide())
                    {
                        m_cursor->animate(false);
                        m_cursor->show();
                        setCursorFrameWithPoint(m_cursor, touch_point);
                    }
                    else if(m_editable || m_selectable)
                    {
                        MICursor* select_cursor_nearby = m_previous_moving_cursor;
                        if(m_previous_moving_cursor == NULL)
                        {
                            select_cursor_nearby = getSelectCurserNearby(touch_point);
                            m_previous_moving_cursor = select_cursor_nearby;
                        }
                        if(select_cursor_nearby != NULL)
                        {
                            setCursorFrameWithPoint(select_cursor_nearby, touch_point);
                            if(select_cursor_nearby == m_select_begin_cursor)
                            {
                                adjustCursorLocation(m_select_end_cursor, false);
                            }
                            else
                            {
                                adjustCursorLocation(m_select_begin_cursor, false);
                            }
                            
                            uint_32 select_begin_location = m_select_begin_cursor->getLocation();
                            uint_32 select_end_location = m_select_end_cursor->getLocation();
                            
                            if(select_begin_location >= select_end_location)
                            {
                                if(select_cursor_nearby == m_select_begin_cursor)
                                {
                                    m_select_begin_cursor->setLocation(select_end_location-1);
                                    adjustCursorLocation(m_select_begin_cursor);
                                    adjustCursorLocation(m_select_end_cursor, false);
                                }
                                else
                                {
                                    m_select_end_cursor->setLocation(select_begin_location+1);
                                    adjustCursorLocation(m_select_end_cursor);
                                    adjustCursorLocation(m_select_begin_cursor, false);
                                }
                            }
                        }
                    }
                    
                    // set text_scale_view center
                    if(m_long_pressed)
                    {
                        text_scale_view->showWithCaptureCenter((*it)->screen_point);
                    }
                    hideMenuWithAnimation();
                    
                    break;
                }
            }
            
            ret = MIEventHandleStateDone;
        }
        while(false);
        
        return MIEventHandleStateDone;
    }
    
    MIEventHandleState MITextField::touchesEnded(const std::set<MITouch*>& touches, const MIEvent* const event)
    {
        MIEventHandleState ret = MIControl::touchesEnded(touches, event);
        // 不是最后一个触摸结束，直接返回 //
        if (touch_manager.size() > 0)
        {
            return ret;
        }
        
        // 处理选中和反选中，即开始输入和结束输入 //
        std::set<MITouch*>::const_iterator touch_it;
        for (touch_it = touches.begin(); touch_it != touches.end(); ++touch_it)
        {
            MGPoint loc_in_view = (*touch_it)->locationInView(this);
            if (!pointInside(loc_in_view, event))
            {
                break;
            }
        }
        if(touch_it != touches.end())
        {
            // touch is outside of view
            if(!m_long_pressed)
            {
                if(isFirstResponder())
                {
                    resignFirstResponder();
                }
                if(m_selecting)
                {
                    finishSelecting();
                }
            }
        }
        
        if(m_editing && !m_selecting)
        {
            startEditing();
        }
        
        if(m_long_pressed)
        {
            if(m_copyable)
            {
                if(m_selecting)
                {
                    if(m_preinput_text.empty())
                    {
                        showMenuWithAnimation(true);
                    }
                }
                else
                {
                    if(m_preinput_text.empty() && isFirstResponder())
                    {
                        showMenuWithAnimation(false);
                    }
                }
            }
        }
        else
        {
//            if(m_selecting)
//            {
//                if(m_preinput_text.empty())
//                {
//                    showMenuWithAnimation(true);
//                }
//            }
        }
        
        text_scale_view->hide();
        
        m_long_press_touch.tag = INVALID_TOUCH_TAG;
        m_long_pressed = false;
        
        m_previous_moving_cursor = NULL;

        return MIEventHandleStateDone;
    }
    
    MIEventHandleState MITextField::touchesCancelled(const std::set<MITouch*>& touches, const MIEvent* const event)
    {
        m_long_press_touch.tag = INVALID_TOUCH_TAG;
        return MIControl::touchesCancelled(touches, event);
    }
    
    MIEventHandleState MITextField::handleEvent(const MIEvent* const event)
    {
        MIEventHandleState ret =  MIControl::handleEvent(event);
		MGUTF8String last_text_content = m_text_content;
        switch (event->_event_type)
        {
            case KEYBOARD_INPUT:
            {
                MGTextContent* content = static_cast<MGTextContent*>(event->_content);
                if (content->text_content == "\n" || content->text_content == "\r" || content->text_content == "\r\n")
                {
                    if(text_field_delegate!=NULL){
                        text_field_delegate->shouldReturn(this);
                    }
                }else
                {
                    insertText(content->text_content);
                }
                break;
            }
            case KEYBOARD_PRE_INPUT:
            {
                MGTextContent* content = static_cast<MGTextContent*>(event->_content);
                insertPreText(content->text_content);
                break;
            }
            case KEYBOARD_DELETE_BACKWARD:
            {
                m_preinput_text = "";
				deleteBackward();
                break;
            }
            case KEY_DOWN_EVENT:
            {
                m_preinput_text = "";
                MGKeyDownContent* content = static_cast<MGKeyDownContent*>(event->_content);
                if (content != NULL) 
				{
                    char ctr[2];
                    ctr[0] = content->key_code;
                    ctr[1] = 0;
                    string str = ctr;
                    insertText(str);
                }
                break;
            }
            default:
			{
                break;
			}
        }
		
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//2012-03-09 modify by Hu DanYuan
		//MITextField文字内容改变时触发事件  
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (last_text_content.toSTLString() != m_text_content.toSTLString())
        {
            EventsMapIterator it = events_dispatch_map.find(MIControlEventValueChanged);
            if (it == events_dispatch_map.end())
            {
                ret = MIEventHandleStateNormal;
            }
            else
            {
                assert(it->second.view_controller != NULL);
                assert(it->second.event_func != NULL);
                EventFunc func = it->second.event_func;
                ((it->second.view_controller)->*func)(this);
                ret = MIEventHandleStateDone;
            }
        }

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
        return ret;
    }
    
#pragma mark - managing touches -
    void MITextField::longPressCall(MGObject *_sender)
    {
        MILongPressGestureRecognizer* recognizer = dynamic_cast<MILongPressGestureRecognizer*>(_sender);
        if(recognizer == NULL)
        {
            return;
        }
        if(m_long_press_touch.tag == INVALID_TOUCH_TAG)
        {
            return;
        }
        
        m_long_pressed = true;
        if(!isFirstResponder())
        {
            becomeFirstResponder();
        }
        
        // show text_scale_view
        if(text_scale_view->isHide())
        {
            text_scale_view->showWithCaptureCenter(m_long_press_touch.screen_point);
        }
        
        // stop m_cursor's animation
        if(!m_cursor->isHide())
        {
            m_cursor->animate(false);
            m_cursor->show();
        }
        
        // hide text_edit_menu
        text_edit_menu->hideAndStopAnimation();
    }
    
    void MITextField::singleTapCall(MGObject* _sender)
    {
        MITapGestureRecognizer* recognizer = dynamic_cast<MITapGestureRecognizer*>(_sender);
        if(recognizer == NULL)
        {
            return;
        }
        if(!m_editable)
        {
            return;
        }
        
        // important !!!
        becomeFirstResponder();
        
        // hide menu
        hideMenuWithAnimation();
        text_scale_view->hide();
        
        // m_selecting
        if(m_selecting)
        {
            finishSelecting();
        }
    
        // is first responder
        if(isFirstResponder())
        {
            startEditing();
            m_cursor->animate(true);
        }
        else
        {
            if(m_editable)
            {
                becomeFirstResponder();
            }
        }
        
        m_long_press_touch.tag = INVALID_TOUCH_TAG;
        m_long_pressed = false;
    }
    
    void MITextField::doubleTapCall(MGObject* _sender)
    {
        MITapGestureRecognizer* recognizer = dynamic_cast<MITapGestureRecognizer*>(_sender);
        if(recognizer == NULL)
        {
            return;
        }
        if(!m_preinput_text.empty())
        {
            return;
        }
        if(m_editable)
        {
            if(!isFirstResponder())
            {
                becomeFirstResponder();
            }
            if(m_text_content.empty())
            {
                startEditing();
            }
            else if(m_selectable)
            {
                startSelecting();
            }
        }
        else if(m_selectable)
        {
            if(!m_text_content.empty())
            {
                startSelecting();
            }
        }
        
        m_long_press_touch.tag = INVALID_TOUCH_TAG;
        m_long_pressed = false;
    }
    
    float MITextField::getTextWidthForLocation(uint_32 location)
    {
        assert(location <= m_text_content.length());
        
        float ret = 0.0f;
        map<uint_32, float>::iterator it = text_width_caches.find(location);
        if(it != text_width_caches.end())
        {
            ret = it->second;
        }
        else
        {
            MGUTF8String before_real_str = m_text_content.substr(0, location);
            string before_string = before_real_str.toSTLString();
            MGRect bound = m_text_font->getStringBoundForWidth(before_string+m_preinput_text.toSTLString(), 999999);
            if (textContentMode == MITextFieldContentModePassword)
            {
                string secure_text(before_real_str.length(), '*');
                bound = m_text_font->getStringBoundForWidth(secure_text+m_preinput_text.toSTLString(), 999999);
            }
            if(before_real_str.empty() && m_preinput_text.empty())
            {
                bound.width = 0;
            }else if(before_string.size() > 0 && before_string.at(before_string.size()-1) == ' ')
            {
                before_string.append(" ");
                bound = m_text_font->getStringBoundForWidth(before_string+m_preinput_text.toSTLString(), 999999);
            }
            ret = bound.width;
            
            text_width_caches[location] = ret;
        }
        return ret;
    }
    
    void MITextField::setCursorFrameWithPoint(MICursor* cursor, const MGPoint &point)
    {
        MGPoint cursor_point = MGPointMake(cursor->getFrame().x, cursor->getFrame().y);
        float diff = point.x - cursor_point.x;
        if (diff == 0)
        {
            return;
        }
        if (diff < 0)
        {
            moveCursorLeft(cursor, diff, point);
        }
        else if (diff > 0)
        {
            moveCursorRight(cursor, diff, point);
        }
    }
    
    void MITextField::moveCursorLeft(MICursor* cursor, float diff, const MGPoint &point)
    {
        uint_32 cursor_location = cursor->getLocation();
        int_32 step = cursor_location;
        while (diff < 0)
        {
            // 需要向左移动光标 //
            cursor_location -= step;
            if (cursor_location <= 0)
            {
                cursor_location = 0;
            }
            float before_text_width = getTextWidthForLocation(cursor_location);
            MGRect rect = textRectForBounds(m_bounds);
			MGRect new_frame = m_cursor->getFrame();
            new_frame.x = rect.x - m_bounds.x + m_text_pos.x + before_text_width;
            float curr_diff = point.x - new_frame.x;
            if (curr_diff < 0)
            {
                if (cursor_location <= 0)
                {
                    break;
                }
                diff = curr_diff;
            }
            else
            {
                cursor_location += step;
                if (step <= 1)
                {
                    if (fabs(curr_diff) > fabs(diff))
                    {
                        break;
                    }
                    else
                    {
                        cursor_location -= step;
                        break;
                    }
                }
            }
            
            if (step >= 2)
            {
                step /= 2;
            }
        }
        cursor->setLocation(cursor_location);
        adjustCursorLocation(cursor);
    }
    
    void MITextField::moveCursorRight(MICursor* cursor, float diff, const MGPoint &point)
    {
        uint_32 cursor_location = cursor->getLocation();
        int_32 step = m_text_content.size();
        step = step - cursor_location;
        while (diff > 0)
        {
        // 需要向右移动光标 //
            cursor_location += step;
            if (cursor_location >= m_text_content.size())
            {
                cursor_location = m_text_content.size();
            }
            float before_text_width = getTextWidthForLocation(cursor_location);
            MGRect rect = textRectForBounds(m_bounds);
            MGRect new_frame = m_cursor->getFrame();
            new_frame.x = rect.x - m_bounds.x + m_text_pos.x + before_text_width;
            float curr_diff = point.x - new_frame.x;
            if (curr_diff > 0)
            {
                if (cursor_location >= m_text_content.size())
                {
                    break;
                }
                diff = curr_diff;
            } else
            {
                cursor_location -= step;
                if (step <= 1)
                {
                    if (fabs(curr_diff) > fabs(diff))
                    {
                        break;
                    }
                    else
                    {
                        cursor_location += step;
                        break;
                    }
                }
            }
            if (step >= 2) {
                step /= 2;
            }
        }
        cursor->setLocation(cursor_location);
        adjustCursorLocation(cursor);
    }
    
    void MITextField::adjustCursorLocation(MICursor* cursor, bool is_moving_cursor/* = true*/)
    {
        uint_32 cursor_location = cursor->getLocation();
        float before_text_width = getTextWidthForLocation(cursor_location);
        
        // 绘制光标 //
        if (m_editing || m_selecting)
        {
            if(cursor == m_select_begin_cursor ||
               cursor == m_select_end_cursor)
            {
                cursor->show();
            }
            MGRect rect = textRectForBounds(m_bounds);
            MGRect new_frame;
            new_frame.width = 2;
            new_frame.y = rect.y - m_bounds.y;
            new_frame.x = rect.x - m_bounds.x + m_text_pos.x + before_text_width;
            new_frame.height = m_text_font->getFontSize() * 1.5;
            if (new_frame.height > rect.height) {
                new_frame.height = rect.height;
            } else{
                new_frame.y += (rect.height - new_frame.height) * 0.5;
            }
            
            if(is_moving_cursor)
            {
                if (new_frame.x < rect.x - m_bounds.x) {
                    m_text_pos.x += rect.x - m_bounds.x - new_frame.x;
                    new_frame.x = rect.x - m_bounds.x;
                }
                if (new_frame.x > rect.x - m_bounds.x + rect.width) {
                    m_text_pos.x += rect.x - m_bounds.x + rect.width - new_frame.x;
                    new_frame.x = rect.x - m_bounds.x + rect.width;
                }
            }
            else
            {
                if (new_frame.x < rect.x - m_bounds.x)
                {
                    if(cursor == m_select_begin_cursor ||
                       cursor == m_select_end_cursor)
                    {
                        cursor->hide();
                    }
                    new_frame.x = rect.x - m_bounds.x;
                }
                if (new_frame.x > rect.x - m_bounds.x + rect.width)
                {
                    if(cursor == m_select_begin_cursor ||
                       cursor == m_select_end_cursor)
                    {
                        cursor->hide();
                    }
                    new_frame.x = rect.x - m_bounds.x + rect.width;
                }
            }
            cursor->setFrame(new_frame);
        }
    }
    
#pragma mark - Selecting and Editing -
    void MITextField::setTextAlignment(MITextAlignment align)
    {
        if (getEditable()) {
            m_text_alignment = MITextAlignmentLeftMiddle;
        } else {
            m_text_alignment = align;
        }
    }
    
    MITextAlignment MITextField::getTextAlignment()
    {
        return m_text_alignment;
    }
    
    void MITextField::startEditing()
    {
        finishSelecting();
        
        m_editing = true;
        if(m_cursor != NULL)
        {
            m_cursor->animate(true);
        }
    }
    
    void MITextField::finishEditing()
    {
        m_editing = false;
        m_cursor->animate(false);
        m_cursor->hide();
        hideMenuWithAnimation();
    }
    
    void MITextField::startSelecting()
    {
        assert(!m_text_content.empty() && m_preinput_text.empty());
        
        m_cursor->animate(false);
        m_cursor->hide();
        
        m_selecting = true;
        m_select_begin_cursor->hide();
        m_select_end_cursor->hide();
        
        // init location
        uint_32 select_start_location = 0;
        uint_32 select_end_location = m_text_content.length();
        
        if(m_editable || m_selectable)
        {
            select_end_location = m_cursor->getLocation();
            if(select_end_location == 0)
            {
                select_start_location = 0;
                select_end_location = 1;
            }
            else
            {
                select_start_location = select_end_location-1;
            }
        }
        
        assert(select_start_location != select_end_location);
        m_select_begin_cursor->setLocation(select_start_location);
        m_select_end_cursor->setLocation(select_end_location);
        adjustCursorLocation(m_select_end_cursor);
        adjustCursorLocation(m_select_begin_cursor, false);
        
        showMenuWithAnimation(true);
        
        if(m_editable || m_selectable)
        {
            m_select_begin_cursor->show();
            m_select_end_cursor->show();
        }
    }
    
    void MITextField::finishSelecting()
    {
        m_selecting = false;
        if(m_select_begin_cursor == NULL || m_select_end_cursor == NULL)
        {
            return;
        }
        m_select_begin_cursor->hide();
        m_select_end_cursor->hide();
        uint_32 location = m_cursor->getLocation();
        m_select_begin_cursor->setLocation(location);
        m_select_end_cursor->setLocation(location);
        
        hideMenuWithAnimation();
    }
    
    void MITextField::showMenuWithAnimation(bool is_selecting)
    {
        text_edit_menu->detachFromSuperview();
        if(is_selecting)
        {
            text_edit_menu->reloadWithType(MITextEditMenuSelect);
        }
        else
        {
            text_edit_menu->reloadWithType(MITextEditMenuEdit);
        }
        
        // adjust menu location
        do
        {
            MGRect menu_frame = text_edit_menu->getFrame();
            float menu_edge = TEXT_FIELD_MENU_EDGE*MINVALUE(MGDevice::getScreenWidth(), MGDevice::getScreenHeight());
            
            MGRectTransition total_trans = MGRectTransitionMake(0.0f, 0.0f, 0.0f);
            MIView* tmp_view = this;
            while(tmp_view != NULL)
            {
                MGRectTransition tmp_trans = tmp_view->getRectTransition();
                MGRectTransition tmp_sub_trans = tmp_view->getSubRectTransition();
                
                if(tmp_view->parent() != NULL)
                {
                    total_trans.axis_x += tmp_trans.axis_x+tmp_sub_trans.axis_x;
                    total_trans.axis_y += tmp_trans.axis_y+tmp_sub_trans.axis_y;
                }
                
                tmp_view = tmp_view->parent();
            }
            
            if(is_selecting)
            {
                MGRect m_select_begin_cursor_bounds = m_select_begin_cursor->getBounds();
                MGRect m_select_end_cursor_bounds = m_select_end_cursor->getBounds();
                m_select_begin_cursor_bounds.x += total_trans.axis_x;
                m_select_begin_cursor_bounds.y += total_trans.axis_y;
                m_select_end_cursor_bounds.x += total_trans.axis_x;
                m_select_end_cursor_bounds.y += total_trans.axis_y;
                
                MGRect cursor_mark_bounds = MGRectMake(MINVALUE(m_select_begin_cursor_bounds.x, m_select_end_cursor_bounds.x), MINVALUE(m_select_begin_cursor_bounds.y, m_select_end_cursor_bounds.y), fabs(m_select_end_cursor_bounds.x+m_select_end_cursor_bounds.width-m_select_begin_cursor_bounds.x), fabs(m_select_end_cursor_bounds.y+m_select_end_cursor_bounds.height-m_select_begin_cursor_bounds.y));
                
                // add to top
                if(cursor_mark_bounds.y-menu_edge-menu_frame.height > menu_edge)
                {
                    menu_frame.y = cursor_mark_bounds.y-menu_edge-menu_frame.height;
                }
                // add to middle
                else if(cursor_mark_bounds.height-m_select_begin_cursor_bounds.height-m_select_end_cursor_bounds.height > menu_frame.height+2*menu_edge)
                {
                    menu_frame.y = cursor_mark_bounds.y+m_select_begin_cursor_bounds.height+menu_edge;
                }
                // add to bottom
                else
                {
                    menu_frame.y = cursor_mark_bounds.y+cursor_mark_bounds.height+menu_edge;
                }
                
                float center_x = cursor_mark_bounds.x+cursor_mark_bounds.width/2.0f;
                menu_frame.x = center_x-menu_frame.width/2.0f;
            }
            else
            {
                MGRect m_cursor_bounds = m_cursor->getBounds();
                m_cursor_bounds.x += total_trans.axis_x;
                m_cursor_bounds.y += total_trans.axis_y;
                
                if(m_cursor_bounds.y-menu_edge-menu_frame.height > menu_edge)
                {
                    menu_frame.y = m_cursor_bounds.y-menu_edge-menu_frame.height;
                }
                else
                {
                    menu_frame.y = m_cursor_bounds.y+m_cursor_bounds.height+menu_edge;
                }
                menu_frame.x = m_cursor_bounds.x-menu_frame.width/2.0f;
            }
            
            if(menu_frame.x < menu_edge)
            {
                menu_frame.x = menu_edge;
            }
            else if(menu_frame.x > MGDevice::getScreenWidth()-menu_frame.width-menu_edge)
            {
                menu_frame.x = MGDevice::getScreenWidth()-menu_frame.width-menu_edge;
            }
            
            if(menu_frame.y < menu_edge)
            {
                menu_frame.y = menu_edge;
            }
            else if(menu_frame.y > MGDevice::getScreenHeight()-menu_frame.height-menu_edge)
            {
                menu_frame.y = MGDevice::getScreenHeight()-menu_frame.height-menu_edge;
            }
            
            text_edit_menu->setFrame(menu_frame);
        }
        while(false);
        
        GuiContainer::container->gui_root->view->addSubview(text_edit_menu);
        text_edit_menu->showWithAnimation();
    }
    
    void MITextField::hideMenuWithAnimation()
    {
        text_edit_menu->hideAndStopAnimation();
    }
    
#pragma mark - MITextFieldDelegate Interface -
    bool MITextFieldDelegate::shouldBeginEditing(MITextField* textField)
    {
        return true;
    }
    
    void MITextFieldDelegate::didBeginEditing(MITextField* textField)
    {
        
    }
    
    bool MITextFieldDelegate::shouldEndEditing(MITextField* textField)
    {
        return true;
    }
    
    void MITextFieldDelegate::didEndEditing(MITextField* textField)
    {
        
    }
    
    bool MITextFieldDelegate::shouldChangeCharactersInRange(MITextField* textField, const MGRange &range, const std::string &text)
    {
        if (text == "\t")
        {
            return false;
        }
        return true;
    }
    
    bool MITextFieldDelegate::shouldClear(MITextField *textField)
    {
        return true;
    }
    
    bool MITextFieldDelegate::shouldReturn(MITextField *textField)
    {
        return true;
    }
    
    int MITextFieldDelegate::shouldAdjustKeyboardWithHeight(z11::MITextField *textField, int height)
    {
        return 0;
    }
} //namespace z11