#include "MITextView.h"
#include "MIScaleView.h"
#include "MITextEditMenu.h"
#include "../Events/MIEvent.h"
#include "../../GraphicCore/MGFont.h"
#include "../GuiContainer/GuiContainer.h"
#include "../../System/Shared/MGLog.h"
#include "../../System/Device/MGDevice.h"
#include "../../System/IO/MGFileManager.h"
#include "../../GraphicCore/MGCanvas.h"
#include "../GestureRecognizer/MILongPressGestureRecognizer.h"
#include "../GestureRecognizer/MITapGestureRecognizer.h"
#include "../../System/ClipBoard/MGClipBoard.h"
#include "MICursor.h"
#include <stdio.h>
#include <algorithm>
#include <math.h>

using namespace std;

#define        TEXT_SCALE_VIEW_WIDTH                          (300.0f/640.0f)
#define        TEXT_SCALE_VIEW_ASPECT_RATIO                   0.3f
#define        TEXT_VIEW_MENU_EDGE                            (30/640.0f)

namespace z11
{
#pragma mark - MITextViewDisplayBoard implementation -
    MITextViewDisplayBoard::MITextViewDisplayBoard()
    {
        class_name = "MITextViewDisplayBoard";
        m_container_view = NULL;
        m_previous_moving_cursor = NULL;
        _view_flags.clip_subviews = 0;
        
        m_long_pressed = false;
        m_long_press_touch.tag = INVALID_TOUCH_TAG;
        
        // 初始化手势识别 //
        m_long_press_gesture = new MILongPressGestureRecognizer();
        m_single_tap_gesture = new MITapGestureRecognizer();
        m_double_tap_gesture = new MITapGestureRecognizer();
        
        m_double_tap_gesture->numberOfTapsRequired = 2;
        
        SELHandler sel_handler;
        sel_handler.sender = m_single_tap_gesture;
		sel_handler.sel_func = (SELFunc)&MITextViewDisplayBoard::singleTapCall;
		m_single_tap_gesture->initWithTarget(this, sel_handler);
		addGestureRecognizer(m_single_tap_gesture);
        
        sel_handler.sender = m_double_tap_gesture;
		sel_handler.sel_func = (SELFunc)&MITextViewDisplayBoard::doubleTapCall;
		m_double_tap_gesture->initWithTarget(this, sel_handler);
		addGestureRecognizer(m_double_tap_gesture);
        
        m_single_tap_gesture->requireGestureRecognizerToFail(m_double_tap_gesture);
        
        m_long_press_gesture->allowableMovement = 999999;
		m_long_press_gesture->setMinimumPressDuration(MGTimeDelta::FromSecondsF(0.5f));
        sel_handler.sender = m_long_press_gesture;
		sel_handler.sel_func = (SELFunc)&MITextViewDisplayBoard::longPressCall;
		m_long_press_gesture->initWithTarget(this, sel_handler);
		addGestureRecognizer(m_long_press_gesture);
        
        m_single_tap_gesture->requireGestureRecognizerToFail(m_long_press_gesture);
    }
    
    MITextViewDisplayBoard::~MITextViewDisplayBoard()
    {
        delete_and_set_null(m_long_press_gesture);
        delete_and_set_null(m_single_tap_gesture);
        delete_and_set_null(m_double_tap_gesture);
    }
    
    void MITextViewDisplayBoard::initWithFrame(MGRect frame)
    {
        MIView::initWithFrame(frame);
    }
    
    void MITextViewDisplayBoard::drawself()
    {
        assert(m_container_view != NULL);
        MGRect rect = getBounds();
        
        // 绘制文本 //
		if(!m_container_view->m_text_content.empty() || !m_container_view->m_preinput_text.empty())
		{
			m_container_view->drawTextInRect(rect);
		}
		else
		{
			m_container_view->drawPlaceholderInRect(rect);
		}
    }
    
    void MITextViewDisplayBoard::setContainer(MITextView* text_view)
    {
        m_container_view = text_view;
    }
    
    void MITextViewDisplayBoard::adjustBoardLocation()
    {
        MGRect rect = m_container_view->getBounds();
        float right = rect.x + rect.width - MITEXTVIEW_DEFAULT_BORDER_VALUE;
        float left = rect.x + MITEXTVIEW_DEFAULT_BORDER_VALUE;
        float top = rect.y + MITEXTVIEW_DEFAULT_BORDER_VALUE;
        float bottom = rect.y + rect.height - MITEXTVIEW_DEFAULT_BORDER_VALUE;
        MGRect frame = m_container_view->m_cursor->getFrame();
        float cursor_left = rect.x + frame.x + m_container_view->getSubRectTransition().axis_x;
        float cursor_right = rect.x + frame.x + frame.width + m_container_view->getSubRectTransition().axis_x;
        float cursor_top = rect.y + frame.y + m_container_view->getSubRectTransition().axis_y;
        float cursor_bottom = rect.y + frame.y + frame.height + m_container_view->getSubRectTransition().axis_y;
        if(cursor_left < left)
        {
            m_container_view->sub_transition.axis_x += left - cursor_left + (right - left) - frame.width;
            if(m_container_view->sub_transition.axis_x > 0)
            {
                m_container_view->sub_transition.axis_x = 0;
            }
        }
        if(cursor_right > right)
        {
            m_container_view->sub_transition.axis_x += right - cursor_right;
        }
        if(cursor_top < top)
        {
            m_container_view->sub_transition.axis_y += top - cursor_top + (bottom - top) - frame.height;
            if(m_container_view->sub_transition.axis_y > 0)
            {
                m_container_view->sub_transition.axis_y = 0;
            }
        }
        if(cursor_bottom > bottom)
        {
            m_container_view->sub_transition.axis_y += bottom - cursor_bottom;
        }
    }
    
    void MITextViewDisplayBoard::longPressCall(MGObject* _sender)
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
        
        // show text_scale_view
        if(m_container_view->text_scale_view->isHide() && m_container_view->m_preinput_text.empty())
        {
            m_container_view->text_scale_view->showWithCaptureCenter(m_long_press_touch.screen_point);
        }
        
        // stop m_cursor's animation
        if(!m_container_view->m_cursor->isHide())
        {
            m_container_view->m_cursor->animate(false);
            m_container_view->m_cursor->show();
        }
        
        // hide text_edit_menu
        m_container_view->text_edit_menu->hideAndStopAnimation();
    }
    
    void MITextViewDisplayBoard::singleTapCall(MGObject* _sender)
    {
        MITapGestureRecognizer* recognizer = dynamic_cast<MITapGestureRecognizer*>(_sender);
        if(recognizer == NULL)
        {
            return;
        }
        if(!m_container_view->m_editable)
        {
            return;
        }
        
        // important !!!
        m_container_view->becomeFirstResponder();
        
        // hide menu
        m_container_view->hideMenuWithAnimation();
        m_container_view->text_scale_view->hide();
        
        // m_selecting
        if(m_container_view->m_selecting)
        {
            m_container_view->finishSelecting();
        }
        
        m_long_press_touch.tag = INVALID_TOUCH_TAG;
        m_long_pressed = false;
    }
    
    void MITextViewDisplayBoard::doubleTapCall(MGObject* _sender)
    {
        MITapGestureRecognizer* recognizer = dynamic_cast<MITapGestureRecognizer*>(_sender);
        if(recognizer == NULL)
        {
            return;
        }
        if(!m_container_view->m_preinput_text.empty())
        {
            return;
        }
        if(m_container_view->m_editable)
        {
            if(!isFirstResponder())
            {
                becomeFirstResponder();
            }
            if(m_container_view->m_text_content.empty())
            {
                m_container_view->startEditing();
            }
            else if(m_container_view->m_selectable)
            {
                m_container_view->startSelecting();
            }
        }
        else if(m_container_view->m_selectable)
        {
            if(!m_container_view->m_text_content.empty())
            {
                m_container_view->startSelecting();
            }
        }
        
        m_long_press_touch.tag = INVALID_TOUCH_TAG;
        m_long_pressed = false;
    }
    
    MGPoint MITextViewDisplayBoard::touchPointInContainerView(const MITouch* const touch)
    {
        MGPoint point = touch->locationInView(m_container_view);
        point.x = point.x - m_container_view->getBounds().x - m_container_view->getSubRectTransition().axis_x;
        point.y = point.y - m_container_view->getBounds().y - m_container_view->getSubRectTransition().axis_y;
        
        return point;
    }
    
#pragma mark - TEXTBOARD MIRESPONDER TOUCH DECLARE -
	MIEventHandleState MITextViewDisplayBoard::touchesBegan(const set<MITouch*> &touches, const MIEvent* const event)
	{
        MIEventHandleState ret = MIView::touchesBegan(touches, event);
        MITouch* touch = *touches.begin();
        if (touch->maintained_view != this)
        {
            return ret;
        }
        if (m_long_press_touch.tag == INVALID_TOUCH_TAG)
        {
            if(m_container_view->m_editable || m_container_view->m_selectable)
            {
                m_long_press_touch = *touch;
                m_long_pressed = false;
                if(m_container_view->m_preinput_text.empty())
                {
                    m_container_view->setCursorFrameWithPoint(m_container_view->m_cursor, touchPointInContainerView(touch));
                }
            }
            else if(m_container_view->m_copyable)
            {
                m_long_press_touch = *touch;
                m_long_pressed = false;
            }
        }
        return MIEventHandleStateDone;
	}
    
	MIEventHandleState MITextViewDisplayBoard::touchesMoved(const set<MITouch*> &touches, const MIEvent* const event)
	{
		MIEventHandleState ret = MIView::touchesMoved(touches, event);
        MITouch* touch = *touches.begin();
        if(touch->maintained_view != this)
        {
            return ret;
        }
        
        do
        {
            for(set<MITouch*>::const_iterator it = touches.begin(); it != touches.end(); ++it)
            {
                if((*it)->tag == m_long_press_touch.tag && m_container_view->m_preinput_text.empty())
                {
                    MGPoint touch_point = touchPointInContainerView(*it);
                    if(!m_container_view->m_cursor->isHide())
                    {
                        m_container_view->m_cursor->animate(false);
                        m_container_view->m_cursor->show();
                        m_container_view->setCursorFrameWithPoint(m_container_view->m_cursor, touch_point);
                    }
                    else if(m_container_view->m_editable || m_container_view->m_selectable)
                    {
                        MICursor* select_cursor_nearby = m_previous_moving_cursor;
                        if(m_previous_moving_cursor == NULL)
                        {
                            select_cursor_nearby = m_container_view->getSelectCurserNearby(touch_point);
                            m_previous_moving_cursor = select_cursor_nearby;
                        }
                        if(select_cursor_nearby != NULL)
                        {
                            m_container_view->setCursorFrameWithPoint(select_cursor_nearby, touch_point);
                            
                            uint_32 select_begin_location = m_container_view->m_select_begin_cursor->getLocation();
                            uint_32 select_end_location = m_container_view->m_select_end_cursor->getLocation();
                            
                            if(select_begin_location >= select_end_location)
                            {
                                if(select_cursor_nearby == m_container_view->m_select_begin_cursor)
                                {
                                    m_container_view->m_select_begin_cursor->setLocation(select_end_location-1);
                                    m_container_view->adjustCursorLocation(m_container_view->m_select_begin_cursor);
                                }
                                else
                                {
                                    m_container_view->m_select_end_cursor->setLocation(select_begin_location+1);
                                    m_container_view->adjustCursorLocation(m_container_view->m_select_end_cursor);
                                }
                            }
                        }
                    }
                    
                    // set text_scale_view center
                    if(m_long_pressed && m_container_view->m_preinput_text.empty())
                    {
                        m_container_view->text_scale_view->showWithCaptureCenter((*it)->screen_point);
                    }
                    if(!m_container_view->m_preinput_text.empty())
                    {
                        m_container_view->text_scale_view->hide();
                    }
                    m_container_view->hideMenuWithAnimation();
                    
                    break;
                }
            }
            
            ret = MIEventHandleStateDone;
        }
        while(false);
        
        return ret;
	}
    
	MIEventHandleState MITextViewDisplayBoard::touchesEnded(const set<MITouch*> &touches, const MIEvent* const event)
	{
        MIEventHandleState ret = MIView::touchesEnded(touches, event);
        // 不是最后一个触摸结束，直接返回 //
        if(touch_manager.size() > 0)
        {
            return ret;
        }
        
        m_container_view->text_scale_view->hide();
        if(m_container_view->m_editing && !m_container_view->m_selecting)
        {
            m_container_view->startEditing();
        }
        
        if(m_long_pressed)
        {
            if(m_container_view->getSelecting())
            {
                if(m_container_view->m_preinput_text.empty())
                {
                    m_container_view->showMenuWithAnimation(true);
                }
            }
            else
            {
                if(m_container_view->m_preinput_text.empty() && m_container_view->isFirstResponder())
                {
                    m_container_view->showMenuWithAnimation(false);
                }
            }
        }
        
        m_long_press_touch.tag = INVALID_TOUCH_TAG;
        m_previous_moving_cursor = NULL;
        m_long_pressed = false;
        
        return MIEventHandleStateDone;
	}
    
	MIEventHandleState MITextViewDisplayBoard::touchesCancelled(const std::set<MITouch*>& touches, const MIEvent* const event)
	{
		MIEventHandleState ret = MIView::touchesCancelled(touches, event);
        m_container_view->text_scale_view->hide();
        
        MITouch* touch = *touches.begin();
        if(touch->maintained_view != this)
        {
            return ret;
        }
        
        for (std::set<MITouch*>::const_iterator it = touches.begin(); it != touches.end(); ++it)
        {
            if((*it)->tag == m_long_press_touch.tag)
            {
                MGLogD("WHY??? MITextViewDisplayBoard::touchesCancelled");
                m_container_view->resignFirstResponder();
                m_long_press_touch.tag = INVALID_TOUCH_TAG;
                m_long_pressed = false;
                break;
            }
        }
        return ret;
	}
    
	MIEventHandleState MITextViewDisplayBoard::handleEvent(const MIEvent* const event)
	{
		MIEventHandleState ret = MIView::handleEvent(event);
		return ret;
	}
    
#pragma mark - MITextView implementation -
	MITextView::MITextView()
	{
        class_name = "MITextView";
		_scroll_view_flags.vertical_bounce_enabled = 1;
        _scroll_view_flags.vertical_indicator_show = 1;
        _scroll_view_flags.vertical_scroll_enabled = 1;
		_scroll_view_flags.horizontal_bounce_enabled = 0;
		_scroll_view_flags.horizontal_indicator_show = 1;
		_scroll_view_flags.horizontal_scroll_enabled = 0;
		_scroll_view_flags.inertia_enable = 1;
		_scroll_view_flags.decelarate_enable = 1;
		_scroll_view_flags.zoom_enable = 0;
		_scroll_view_flags.passing_vertical_scroll_enable = 0;
		_scroll_view_flags.passing_horizontal_scroll_enable = 0;
		_scroll_view_flags.hide_keyboard_when_scrolling = 0;
        _scroll_view_flags.always_bounce_horizontal = 0;
        _scroll_view_flags.always_bounce_vertical = 0;
        _scroll_view_flags.horizontal_bounce_enabled = 0;
        _scroll_view_flags.vertical_bounce_enabled = 0;

        text_view_delegate = NULL;
		m_text_font = MGFont::create(12, RGBAColorMake(0, 0, 0));
		m_cursor = NULL;
        
		m_text_alignment = MITextAlignmentLeftTop;
		borderStyle = MITextBorderStyleSkia;
        m_preinput_text = "";
        m_text_content = "";
        m_unselected_image = NULL;
		m_selected_image = NULL;
		m_bounds_image = NULL;
        m_text_board = NULL;
		m_display_mode = MITextViewDisplayModeAutoReturn;
        
        m_editing = false;
        m_selecting = false;
        
        m_editable = false;
        m_selectable = false;
        m_copyable = false;
        is_respond_to_enter_key = false;
        touch_resign_first_responder = true;
        
        // init text_scale_view
        text_scale_view = new MIScaleView();
        text_scale_view->setScaleValue(1.5f);
        float text_scale_view_width = TEXT_SCALE_VIEW_WIDTH*MINVALUE(MGDevice::getScreenWidth(), MGDevice::getScreenHeight());
        text_scale_view->initWithFrame(MGRectMake(0, 0, text_scale_view_width, TEXT_SCALE_VIEW_ASPECT_RATIO*text_scale_view_width));
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

	MITextView::~MITextView()
	{
        if(isFirstResponder())
        {
            resignFirstResponder();
        }
        
        delete_and_set_null(m_selected_image);
		delete_and_set_null(m_unselected_image);
		delete_and_set_null(m_text_font);
        
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
    void MITextView::initWithFrame(MGRect frame)
	{
		MIScrollView::initWithFrame(frame);
        MGRect rect = getBounds();
        assert(m_text_board == NULL);
        m_text_board = new MITextViewDisplayBoard();
        m_text_board->initWithFrame(MGRectMake(MITEXTVIEW_DEFAULT_BORDER_VALUE, MITEXTVIEW_DEFAULT_BORDER_VALUE, rect.width - 2 * MITEXTVIEW_DEFAULT_BORDER_VALUE, rect.height - 2 * MITEXTVIEW_DEFAULT_BORDER_VALUE));
        m_text_board->setBackgroundAlphaInByte(0);
        m_text_board->autoClean();
        m_text_board->setContainer(this);
        addSubview(m_text_board);
        
        if(m_text_font == NULL)
        {
            m_text_font = MGFont::create(rect.height * 0.5, RGBAColorMake(0, 0, 0));
        }
        
        // init cursors
        do
        {
            float cursor_height = m_text_font->getFontSize() * 1.5;
            if(cursor_height > rect.height)
            {
                cursor_height = rect.height;
            }
            
            // m_cursor
            m_cursor = new MICursor();
            m_cursor->initWithFrame(MGRectMake(rect.x - m_bounds.x, rect.y - m_bounds.y, 2, cursor_height));
            m_cursor->setBackgroundColor(MGColor::fromColorRef(RGBAColorRef::skyBlue()));
            m_cursor->hide();
            addSubview(m_cursor);
            
            // select begin cursor
            m_select_begin_cursor = new MICursor();
            m_select_begin_cursor->initWithFrame(m_cursor->getFrame());
            m_select_begin_cursor->setBackgroundColor(MGColor::fromColorRef(RGBAColorRef::skyBlue()));
            m_select_begin_cursor->hide();
            addSubview(m_select_begin_cursor);
            
            // select end cursor
            m_select_end_cursor = new MICursor();
            m_select_end_cursor->initWithFrame(m_cursor->getFrame());
            m_select_end_cursor->setBackgroundColor(MGColor::fromColorRef(RGBAColorRef::skyBlue()));
            m_select_end_cursor->hide();
            addSubview(m_select_end_cursor);
        }
        while(false);
        
		if(m_background_color == NULL)
		{
			string path=MGBundle::mainBundle()->tmpPathWithFileName("controls/");
			MGFileManager::defaultManager()->contentsOfDirectoryAtURL(path);
            
			char filename[256];
			int width=frame.width;
			int height=frame.height;
			sprintf(filename,"TextField%dX%d_unselected.png",width,height);
			path+=filename;
			if(!MGFileManager::defaultManager()->fileExistsAtPath(path))
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
			
			m_selected_image = MGImage::imageWithContentsOfFile(path);
		}
	}
	void MITextView::drawself()
	{
        MGGraphicEngine* engine = MGGraphicEngine::sharedGraphicEngine();
        
        // 绘制背景 //
        RGBAColorRef color_ref;
        if(m_background_color != NULL)
        {
            color_ref = m_background_color->m_color;
            color_ref.m_alpha_component = (byte_8)(m_alpha * color_ref.m_alpha_component);
        }
        else
        {
            color_ref = RGBAColorMake(255, 255, 255, (byte_8)(m_alpha * 255));
        }
		if(m_background_color != NULL && m_background_color->m_image != NULL)
        {
			m_background_color->m_image->sendDataToContext();
			MGRect image_rect;
			image_rect.width = m_background_color->m_image->getWidth();
			image_rect.height = m_background_color->m_image->getHeight();
			engine->drawImage(m_background_color->m_image, m_bounds, color_ref, draw_image_mode, image_rect);
		}
        else
        {
            switch(borderStyle)
            {
                case MITextBorderStyleNone:
                {
                    break;
                }
                case MITextBorderStyleLine:
                {
                    if(m_editing)
                    {
                        engine->drawFilledRect(m_bounds, color_ref);
                        engine->drawRect(m_bounds, RGBAColorRef::lightGray());
                    }
                    else
                    {
                        engine->drawFilledRect(m_bounds, color_ref);
                        engine->drawRect(m_bounds, RGBAColorRef::lightGray());
                    }
                    break;
                }
                case MITextBorderStyleBezel:
                {
                    break;
                }
                case MITextBorderStyleRoundedRect:
                {
                    if(m_editing)
                    {
                        engine->drawFilledRoundRect(m_bounds, color_ref);
                        engine->drawRoundRect(m_bounds, RGBAColorRef::lightGray());
                    }
                    else
                    {
                        engine->drawFilledRoundRect(m_bounds, color_ref);
                        engine->drawRoundRect(m_bounds, RGBAColorRef::lightGray());
                    }
                    break;
                }
                case MITextBorderStyleSkia:
                {
                    if(m_bounds_image!=NULL)
                    {
                        MGRect image_rect;
                        image_rect.width = m_bounds_image->getWidth();
                        image_rect.height = m_bounds_image->getHeight();
                        MGRect image_bounds = m_bounds;
                        {
                            image_bounds.x -= (image_rect.width-image_bounds.width)/2;
                            image_bounds.y -= (image_rect.height-image_bounds.height)/2;
                            image_bounds.width = image_rect.width;
                            image_bounds.height = image_rect.height;
                        }
                        m_bounds_image->sendDataToContext();
                        engine->drawImage(m_bounds_image, image_bounds, color_ref, draw_image_mode, image_rect);
                        break;
                    }
                }
                default:
                {
                    break;
                }
            }
		}
	}
    void MITextView::frameChanged(const MGRect& frame)
    {
        MIScrollView::frameChanged(frame);
        MGRect rect = getBounds();
        assert(m_text_board != NULL);
        m_text_board->setFrame(MGRectMake(MITEXTVIEW_DEFAULT_BORDER_VALUE, MITEXTVIEW_DEFAULT_BORDER_VALUE, rect.width - 2 * MITEXTVIEW_DEFAULT_BORDER_VALUE, rect.height - 2 * MITEXTVIEW_DEFAULT_BORDER_VALUE));
    }
    
    MGRect MITextView::getTextBoardFrame()
    {
        return m_text_board->getFrame();
    }

#pragma mark - MIEditableTextView interface -
    bool MITextView::hasText()
	{
		return m_text_content.length() > 0;
	}
    
    void MITextView::setCursorLocation(uint_32 location)
    {
        m_cursor->setLocation(location);
        adjustCursorLocation(m_cursor);
    }
    
    void MITextView::insertText(const string &text)
	{
        string ins_txt = checkInputString(m_text_content.toSTLString(), text);
        if(ins_txt.size() == 0)
        {
            becomeFirstResponder();
            return;
        }
        last_line_rect_caches.clear();
        
        // 根据输入内容和选定的位置替换文本内容，并设置新的光标位置 //
        MGUTF8String text_content(ins_txt);
        if(!m_preinput_text.empty())
        {
            string tmp_text = ins_txt;
            tmp_text.erase(std::remove(tmp_text.begin(), tmp_text.end(), ' '), tmp_text.end());
            text_content = MGUTF8String(tmp_text);
        }
        m_preinput_text = "";
        
        uint_32 select_start_position = m_select_begin_cursor->getLocation();
        uint_32 select_end_position = m_select_end_cursor->getLocation();
        assert(select_start_position <= select_end_position);
        int length = select_end_position-select_start_position;
        if(length > 0)
        {
            m_text_content.replace(select_start_position, length, text_content);
            m_cursor->setLocation(select_start_position+text_content.length());
            startEditing();
        }
        else if(length == 0)
        {
            m_text_content.replace(m_cursor->getLocation(), 0, text_content);
            m_cursor->setLocation(m_cursor->getLocation()+text_content.length());
            m_select_begin_cursor->setLocation(m_cursor->getLocation());
            m_select_end_cursor->setLocation(m_cursor->getLocation());
        }
        
        hideMenuWithAnimation();
        adjustCursorLocation(m_cursor);
        adjustCursorLocation(m_select_begin_cursor);
        adjustCursorLocation(m_select_end_cursor);
        m_text_board->adjustBoardLocation();
	}
    
    // 预输入状态下 不能手动改变光标位置 //
    void MITextView::insertPreText(const string &pre_text)
    {
        if(m_selecting)
        {
            insertText("");
        }
        
        last_line_rect_caches.clear();
        int m_cursor_location = m_cursor->getLocation();
        
        if(m_preinput_text.toSTLString() == "")
        {
            m_select_begin_cursor->setLocation(m_cursor_location);
            adjustCursorLocation(m_select_begin_cursor);
        }
        
        m_preinput_text = pre_text;
        adjustCursorLocation(m_cursor);
        
        m_select_end_cursor->setLocation(m_cursor_location);
        adjustCursorLocation(m_select_end_cursor);
        m_text_board->adjustBoardLocation();
    }
    
	void MITextView::deleteBackward()
	{
		if(!m_text_content.empty())
		{
            last_line_rect_caches.clear();
            // 根据当前选定状态，删除对应文本，并设置新的光标位置 //
            uint_32 start_location = m_select_begin_cursor->getLocation();
            uint_32 end_location = m_select_end_cursor->getLocation();
            assert(start_location <= end_location);
            uint_32 length = end_location-start_location;
            if(length > 0)
            {
                m_text_content.replace(start_location, length, MGUTF8String(""));
                m_cursor->setLocation(start_location);
                finishSelecting();
                startEditing();
            }
            else
            {
                start_location = m_cursor->getLocation();
                uint_32 step = 1;
                if(start_location == 0)
                {
                    return;
                }
                --start_location;
                MGUTF8String str = m_text_content.substr(start_location, 1);
                if(str.toSTLString() == "\n")
                {
                    --start_location;
                    ++step;
                    MGUTF8String sstr = m_text_content.substr(start_location, 1);
                    if(sstr.toSTLString() != "\r")
                    {
                        ++start_location;
                        --step;
                    }
                }
                assert(start_location >= 0);
                m_cursor->setLocation(start_location);
                m_text_content.replace(start_location, step, MGUTF8String(""));
            }
            
            adjustCursorLocation(m_cursor);
            m_text_board->adjustBoardLocation();
		}
	}
    
    void MITextView::selectText()
    {
        startSelecting();
    }
    
    void MITextView::selectAllText()
    {
        if(hasText())
        {
            startSelecting();
            m_select_begin_cursor->setLocation(0);
            m_select_end_cursor->setLocation(m_text_content.length());
            adjustCursorLocation(m_select_begin_cursor);
            adjustCursorLocation(m_select_end_cursor);
        }
    }
    
    void MITextView::copySelectedText()
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
    
    void MITextView::cutSelectedText()
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
        deleteBackward();
        
        finishSelecting();
        hideMenuWithAnimation();
        
        if(!isFirstResponder())
        {
            becomeFirstResponder();
        }
        
        if(text_view_delegate!=NULL){
            text_view_delegate->didChange(this);
        }
    }
    
    void MITextView::pasteClipboardText()
    {
        insertText(MGClipBoard::getString());
        if(!isFirstResponder())
        {
            becomeFirstResponder();
        }
        if(text_view_delegate!=NULL){
            text_view_delegate->didChange(this);
        }
    }
    
#pragma mark - Configuring the Text Attributes -
    void MITextView::setFontSizeAndColor(uint_32 font_size, const RGBAColorRef &font_color/*= RGBAColorRef::black() */)
	{
		assert(m_text_font != NULL);
        m_text_font->setFaceSize(font_size);
        m_text_font->setColor(font_color);
	}
    
    void MITextView::setText(const string &_text)
	{
        last_line_rect_caches.clear();
        m_preinput_text = "";
        
        string tmp_text = _text;
        
#if defined(ANDROID_NDK) || defined(WIN32)
        tmp_text.erase(std::remove(tmp_text.begin(), tmp_text.end(), 0x09), tmp_text.end());
        tmp_text.erase(std::remove(tmp_text.begin(), tmp_text.end(), 0x0D), tmp_text.end());
#else
        tmp_text.erase(std::remove(tmp_text.begin(), tmp_text.end(), 0x09), tmp_text.end());
#endif
		m_text_content = tmp_text;
        
        if(m_cursor != NULL)
        {
            m_cursor->setLocation(0);
        }
        if(m_editing)
        {
            startEditing();
        }
        if(m_text_board != NULL)
        {
            adjustCursorLocation(m_cursor);
            m_text_board->adjustBoardLocation();
        }
	}
    
	string MITextView::getText()
	{
		return m_text_content.toSTLString();
	}
    
	void MITextView::setTextColor( const RGBAColorRef &color )
	{
		m_text_font->setColor(color);
	}
    
	RGBAColorRef MITextView::getTextColor()
	{
		return m_text_font->getColor();
	}
    
#pragma mark - Drawing and Positioning Overrides -
    void MITextView::drawTextInRect(const MGRect &_rect)
    {
        MGRect rect = _rect;
        MGGraphicEngine* engine = MGGraphicEngine::sharedGraphicEngine();
        MGUTF8String content = m_text_content;
        uint_32 cursor_location = m_cursor->getLocation();
        if (cursor_location > content.size()) {
            cursor_location = content.size();
        }
        MGUTF8String before_str = content.substr(0, cursor_location);
        MGUTF8String after_str = content.substr(cursor_location);
        MGUTF8String draw_text = before_str + m_preinput_text + after_str;
        
        MGRect text_real_rect;
        if (m_display_mode == MITextViewDisplayModeNormal) {
            text_real_rect = m_text_font->getStringBoundForWidth(draw_text.toSTLString(), 999999);
            text_real_rect.width += text_real_rect.x + m_text_font->getFontSize(); // 增加显示区域，因为计算的边框不能直接作为显示区域来设置，计算的边框要小于显示区域 //
        } else if (m_display_mode == MITextViewDisplayModeAutoReturn)
        {
            text_real_rect = m_text_font->getStringBoundForWidth(draw_text.toSTLString(), rect.width);
            if (text_real_rect.width > rect.width) {
                text_real_rect.width = rect.width - text_real_rect.width + rect.width;
            } else
                text_real_rect.width = rect.width;
        }
        transAndScaleClipRect(rect);
        engine->enableClip(rect);
        MGRect board_frame = m_text_board->getFrame();
        rect = _rect;
        rect.width = text_real_rect.width;
        rect.height = text_real_rect.height + getFont()->getFontSize() - text_real_rect.y;
        rect.width = rect.width > _rect.width ? rect.width : _rect.width;
        rect.height = rect.height > _rect.height ? rect.height : _rect.height;
        board_frame.width = rect.width > board_frame.width ? rect.width : board_frame.width;
        board_frame.height = rect.height > board_frame.height ? rect.height : board_frame.height;
        m_text_board->setFrame(board_frame);
        float new_bottom = (m_content_size.top + rect.height);
        if (new_bottom > m_frame.height) {
            m_content_size.bottom = new_bottom;
        } else {
            m_content_size.bottom = m_frame.height;
        }
        float new_right = m_content_size.left + rect.width;
        if (new_right > m_frame.width) {
            m_content_size.right = new_right;
        } else {
            m_content_size.right = m_frame.width;
        }
        if (m_content_size.right - m_content_size.left > m_frame.width) {
            _scroll_view_flags.horizontal_indicator_show = 1;
            _scroll_view_flags.horizontal_scroll_enabled = 1;
        } else {
            _scroll_view_flags.horizontal_indicator_show = 0;
            _scroll_view_flags.horizontal_scroll_enabled = 0;
        }
        engine->drawText(draw_text.toSTLString(), m_text_font, rect, MILineBreakModeWordWrap, m_text_alignment);
        if(m_preinput_text.length() > 0)
        {
            rect = _rect;
            MGRect bound = m_text_font->getStringBound(before_str.toSTLString());
            MGRect marked_bound = m_text_font->getStringBound(m_preinput_text.toSTLString());
            if(before_str.size() == 0)
            {
                bound.width = 0;
            }
            if(m_preinput_text.size() == 0)
            {
                marked_bound.width = 0;
            }
            marked_bound.height = bound.height;
//            engine->drawFilledRect(MGRectMake(rect.x+ m_text_pos.x + bound.x + bound.width, rect.y,marked_bound.width,marked_bound.height),RGBAColorRef::lightBlue(100));
        }
        
        drawSelectTextBackground();
        
        engine->disableClip();
    }
    
    void MITextView::drawSelectTextBackground()
    {
        MGGraphicEngine* engine = MGGraphicEngine::sharedGraphicEngine();
        if(engine == NULL)
        {
            return;
        }
        if(m_select_begin_cursor->getLocation() > m_select_end_cursor->getLocation())
        {
            return;
        }
        
        MGRect begin_cursor_rect = m_select_begin_cursor->getBounds();
        MGRect end_cursor_rect = m_select_end_cursor->getBounds();
        RGBAColorRef color_ref = RGBAColorRef::lightBlue(100);
        if(fabs(end_cursor_rect.y-begin_cursor_rect.y) < begin_cursor_rect.height*0.2f)
        {
            MGRect draw_rect = MGRectMake(begin_cursor_rect.x, begin_cursor_rect.y, end_cursor_rect.x-begin_cursor_rect.x, begin_cursor_rect.height);
            if(draw_rect.width > 0.0f)
            {
                engine->drawFilledRect(draw_rect, color_ref);
            }
        }
        else
        {
            MGRect text_bounds = m_bounds;
            MGRect first_rect = MGRectMake(begin_cursor_rect.x, begin_cursor_rect.y, text_bounds.x+text_bounds.width-begin_cursor_rect.x, begin_cursor_rect.height);
            if(first_rect.width > 0.0f)
            {
                engine->drawFilledRect(first_rect, color_ref);
            }
            
            MGRect second_rect = MGRectMake(text_bounds.x, begin_cursor_rect.y+begin_cursor_rect.height, text_bounds.width, end_cursor_rect.y-begin_cursor_rect.y-begin_cursor_rect.height);
            if(second_rect.height > 0.2f*begin_cursor_rect.height || (second_rect.height > 0.0f && begin_cursor_rect.x < end_cursor_rect.x))
            {
                engine->drawFilledRect(second_rect, color_ref);
            }
            
            MGRect third_rect = MGRectMake(text_bounds.x, end_cursor_rect.y, end_cursor_rect.x-text_bounds.x, end_cursor_rect.height);
            if(third_rect.y < begin_cursor_rect.y+begin_cursor_rect.height)
            {
                third_rect.y = begin_cursor_rect.y+begin_cursor_rect.height;
                third_rect.height = end_cursor_rect.y+end_cursor_rect.height-third_rect.y;
            }
            
            if(third_rect.width > 0.0f)
            {
                engine->drawFilledRect(third_rect, color_ref);
            }
        }
    }
    
    void MITextView::drawPreInputTextBackground()
    {
        assert(m_preinput_text.length() > 0);
        
        // todo...
    }
    
    void MITextView::drawPlaceholderInRect(const MGRect &rect)
    {
        MGGraphicEngine* engine = MGGraphicEngine::sharedGraphicEngine();
#if FONT_SHARED
        MGFont *shared_grey_font = MGFont::shareFont();
        shared_grey_font->setFaceSize(m_text_font->getFontSize());
        shared_grey_font->setColor(RGBAColorRef::darkGray());
#else
        MGFont* shared_grey_font = MGFont::shareFont();
        shared_grey_font->setFaceSize(m_text_font->getFontSize());
        shared_grey_font->setColor(RGBAColorRef::darkGray());
#endif
        engine->drawText(placeholder, shared_grey_font, rect, MILineBreakModeWordWrap, m_text_alignment);
    }
    
#pragma mark - MIRESPONDER DECLARE -
	bool MITextView::canBecomeFirstResponder()
	{
		return true;
	}
    
	void MITextView::becomeFirstResponder()
	{
        if(!m_editable)
        {
            return;
        }
        if(text_view_delegate != NULL)
        {
            if(!text_view_delegate->shouldBeginEditing(this))
            {
                return;
            }
        }
		
        if(isFirstResponder() && !m_preinput_text.empty())
        {
            insertText(m_preinput_text.toSTLString());
            m_preinput_text.clear();
        }
        
        if(!isFirstResponder())
        {
            m_text_board->adjustBoardLocation();
        }
        
        MIScrollView::becomeFirstResponder();
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
		
        // show cursor
        startEditing();
        adjustCursorLocation(m_cursor);
        
		m_bounds_image = m_selected_image;
        
        if (text_view_delegate != NULL) {
            text_view_delegate->didBeginEditing(this);
        }
	}
    
    void MITextView::clearContext(MIView* view)
    {
        if(text_edit_menu != NULL && !text_edit_menu->containedSubview(view))
        {
            text_edit_menu->hideAndStopAnimation();
        }
    }
    
	void MITextView::resignFirstResponder(bool closeKeyboard)
	{
        if (text_view_delegate != NULL) {
            if(!text_view_delegate->shouldEndEditing(this))
                return;
        }
        
		MIScrollView::resignFirstResponder(closeKeyboard);
        if(closeKeyboard == true)
        {
            MIEvent* event = new MIEvent;
            event->_event_type = RESIGN_FIRST_RESPONDER_EVENT;
            event->_responder = this;
            GuiContainer::container->preserveEvent(event);
        }
        
		// cursor
        finishEditing();
        finishSelecting();
        
        m_select_begin_cursor->setLocation(m_cursor->getLocation());
        m_select_end_cursor->setLocation(m_cursor->getLocation());
        adjustCursorLocation(m_select_begin_cursor);
        adjustCursorLocation(m_select_end_cursor);
        
        sub_transition.axis_x = 0;
        sub_transition.axis_y = 0;
        m_preinput_text.clear();
        last_line_rect_caches.clear();
        
        m_bounds_image = m_unselected_image;
        
        if (text_view_delegate != NULL) {
            text_view_delegate->didEndEditing(this);
        }
	}
    
    int MITextView::adjustWithKeyboardHeight(int height)
    {
        int ret = 0;
        MGFont* font = getFont();
        if (font != NULL) {
            ret = font->getFontSize();
        }
        if (text_view_delegate != NULL) {
            ret += text_view_delegate->shouldAdjustKeyboardWithHeight(this, height);
        }
        return ret;
    }
    
#pragma mark - MIRESPONDER TOUCH DECLARE -
	MIEventHandleState MITextView::touchesBegan(const set<MITouch*> &touches, const MIEvent* const event)
	{
		MIEventHandleState ret = MIScrollView::touchesBegan(touches, event);
        MITouch* touch = *touches.begin();
        if(touch->maintained_view != this)
        {
            return ret;
        }
        
        return MIEventHandleStateDone;
	}
    
	MIEventHandleState MITextView::touchesMoved(const set<MITouch*> &touches, const MIEvent* const event)
	{
		MIEventHandleState ret = MIScrollView::touchesMoved(touches, event);
        MITouch* touch = *touches.begin();
        
        if(touch->maintained_view != this)
        {
            return ret;
        }
        if(_scroll_view_flags.passing_horizontal_scroll_enable == 1 || _scroll_view_flags.passing_vertical_scroll_enable == 1)
        {
            return ret;
        }
        
        return MIEventHandleStateDone;
	}
    
	MIEventHandleState MITextView::touchesEnded(const set<MITouch*> &touches, const MIEvent* const event)
	{
        MIEventHandleState ret = MIScrollView::touchesEnded(touches, event);
        // 不是最后一个触摸结束，直接返回 //
        if(touch_manager.size() > 0)
        {
            return ret;
        }
        
        // 处理选中和反选中，即开始输入和结束输入 //
        std::set<MITouch*>::const_iterator touch_it;
        for(touch_it = touches.begin(); touch_it != touches.end(); ++touch_it)
        {
            MGPoint loc_in_view = (*touch_it)->locationInView(this);
            if(!pointInside(loc_in_view, event))
            {
                break;
            }
        }
        if(touch_it != touches.end())
        {
            if(isFirstResponder()&&touch_resign_first_responder)
            {
                resignFirstResponder();
            }
            if(m_preinput_text.empty())
            {
                finishSelecting();
            }
        }
        
        return MIEventHandleStateDone;
	}
    
	MIEventHandleState MITextView::touchesCancelled(const set<MITouch*> &touches, const MIEvent* const event)
	{
		return MIScrollView::touchesCancelled(touches, event);
	}
    
	MIEventHandleState MITextView::handleEvent(const MIEvent* const event)
	{
		MIEventHandleState ret = MIScrollView::handleEvent(event);
		MGUTF8String last_text_content = m_text_content;
        switch (event->_event_type)
        {
            case KEYBOARD_INPUT:
            {
                MGTextContent* content = static_cast<MGTextContent*>(event->_content);
                if(content != NULL)
                {
                    if(content->text_content == "\n" || content->text_content == "\r" || content->text_content == "\r\n")
                    {
#if MG_TARGET_PLATFORM == MG_PLATFORM_WIN32 || MG_TARGET_PLATFORM == MG_PLATFORM_ANDROID
                        content->text_content = "\n";
#else
                        content->text_content = "\r\n";
#endif
                        
                        if(is_respond_to_enter_key){
                            if(text_view_delegate!=NULL){
                                text_view_delegate->shouldReturn(this);
                            }
                        }
                        else{
                            insertText(content->text_content);
                        }
                    }
                    else{
                        insertText(content->text_content);
                    }
                }
                break;
            }
            case KEYBOARD_PRE_INPUT:
            {
                MGTextContent* content = static_cast<MGTextContent*>(event->_content);
                if(content != NULL)
                {
                    insertPreText(content->text_content);
                }
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
                if(content != NULL)
                {
                    char ctr[2];
                    ctr[0] = content->key_code;
                    ctr[1] = 0;
                    string str = ctr;
                    
                    if(str == "\n" || str == "\r" || str == "\r\n")
                    {
#if MG_TARGET_PLATFORM == MG_PLATFORM_WIN32 || MG_TARGET_PLATFORM == MG_PLATFORM_ANDROID
                        str = "\n";
#else
                        str = "\r\n";
#endif
                        
                        if(is_respond_to_enter_key){
                            if(text_view_delegate!=NULL){
                                text_view_delegate->shouldReturn(this);
                            }
                        }
                        else{
                            insertText(str);
                        }
                    }
                    else{
                        insertText(str);
                    }  
                }
                break;
            }
            default:
            {
                break;
            }
        }

		if (last_text_content.toSTLString() != m_text_content.toSTLString()) 
		{
			if(text_view_delegate!=NULL)
				text_view_delegate->didChange(this);
		}
		return ret;
	}

#pragma mark - Adjust Cursor -
    void MITextView::adjustCursorLocation(MICursor* m_cursor)
    {
        uint_32 location = m_cursor->getLocation();
        assert(location <= m_text_content.length());
        MGRect last_line_rect = getLastLineRect(location);
        float y_pos = m_text_board->getBounds().y - m_bounds.y;
        
        if(last_line_rect.height > 0)
        {
            y_pos += (getFont()->getFontSize() - last_line_rect.height) * 0.5 - last_line_rect.y;
        }
        else
        {
            y_pos -= last_line_rect.height + last_line_rect.y;
        }
        MGRect new_frame = m_cursor->getFrame();
        new_frame.x = m_text_board->getBounds().x - m_bounds.x + last_line_rect.x + last_line_rect.width;
        new_frame.height = getFont()->getFontSize() * 1.5;
        // 使光标位置高度居中 //
        new_frame.y = y_pos - getFont()->getFontSize() * 0.25;
        m_cursor->setFrame(new_frame);
    }
    
    void MITextView::setCursorFrameWithPoint(MICursor* m_cursor, const MGPoint &point)
    {
        assert(m_preinput_text.empty());
        MGPoint cursor_point = MGPointMake(m_cursor->getFrame().x, m_cursor->getFrame().y);
        float diff_x = point.x - cursor_point.x;
        float diff_y = point.y - cursor_point.y;
        if(diff_x == 0 && diff_y == 0)
        {
            return;
        }
        
        if(diff_y < 0)
        {
            moveCursorUp(m_cursor, diff_x, diff_y, point);
        }
        else if(diff_y > m_cursor->getFrame().height)
        {
            moveCursorDown(m_cursor, diff_x, diff_y, point);
        }
        else
        {
            if(diff_x < 0)
            {
                moveCursorLeft(m_cursor, diff_x, diff_y, point, 0, m_cursor->getLocation());
            }
            else if(diff_x > 0)
            {
                moveCursorRight(m_cursor, diff_x, diff_y, point, 0, m_cursor->getLocation());
            }
        }
    }
    
    float MITextView::distanceBetweenCursorAndPoint(MICursor* m_cursor, const MGPoint &m_point)
    {
        bool is_cursor_font_of_point = true;
        MGRect cursor_frame = m_cursor->getFrame();
        if(m_point.y < cursor_frame.y)
        {
            is_cursor_font_of_point = true;
        }
        else if(m_point.y > cursor_frame.y+cursor_frame.height)
        {
            is_cursor_font_of_point = false;
        }
        else if(m_point.x < cursor_frame.x)
        {
            is_cursor_font_of_point = true;
        }
        else if(m_point.x > cursor_frame.x+cursor_frame.width)
        {
            is_cursor_font_of_point = false;
        }
        else
        {
            // m_point is inside of m_cursor
            return 0.0f;
        }
        
        MGSize distance_size;
        distance_size.width = cursor_frame.x+cursor_frame.width/2.0f-m_point.x;
        distance_size.height = cursor_frame.y+cursor_frame.height/2.0f-m_point.y;
        double distance = sqrt(distance_size.width*distance_size.width+distance_size.height*distance_size.height);
        distance = fabs(distance);
        if(is_cursor_font_of_point)
        {
            distance = 0.0f-distance;
        }
        
        return distance;
    }
    
    MGRect MITextView::getLastLineRect(int location)
    {
        MGRect ret;
        
        map<int, MGRect>::iterator location_it = last_line_rect_caches.find(location);
        if(location_it != last_line_rect_caches.end())
        {
            ret = location_it->second;
        }
        else
        {
            MGUTF8String before_str = m_text_content.substr(0, location)+m_preinput_text;
            string before_string = before_str.toSTLString();
            vector<MGRect> bboxs;
            if(m_display_mode == MITextViewDisplayModeNormal)
            {
                bboxs = m_text_font->BBoxVector(before_string, 999999);
            }
            else if(m_display_mode == MITextViewDisplayModeAutoReturn)
            {
                bboxs = m_text_font->BBoxVector(before_string, m_text_board->getBounds().width);
            }
            if(before_string.size() > 0 && before_string.at(before_string.size()-1) == ' ')
            {
                before_string.append(" ");
                bboxs = m_text_font->BBoxVector(before_string, 999999);
            }
            if(!bboxs.empty())
            {
                ret = bboxs.back();
                last_line_rect_caches[location] = ret;
            }
        }
        
        return ret;
    }
    
    MICursor* MITextView::getSelectCurserNearby(const MGPoint &point)
    {
        MICursor* ret = NULL;
        
        do
        {
            if(m_select_begin_cursor->isHide() || m_select_end_cursor->isHide())
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
    
#pragma mark - Move Cursor -
    void MITextView::moveCursorLeft(MICursor* m_cursor, float diff_x, float diff_y, const MGPoint& point, int_32 step, uint_32 location)
    {
        //MGLogD("MITextView::moveCursorLeft: diff_x=%.1f, diff_y=%.1f, point=(%.1f, %.1f), step=%d, location=%d", diff_x, diff_y, point.x, point.y, step, (int)location);
        
        if(step <= 0)
        {
            step = location;
        }
        while(diff_x < 0) {
            // 需要向左移动光标 //
            location -= step;
            if (location <= 0) {
                location = 0;
            }
            
            MGRect new_frame = m_cursor->getFrame();
            //if (!bboxs.empty())
            {
                MGRect last_line_rect = getLastLineRect(location);
                float y_pos = m_text_board->getBounds().y - getBounds().y;
                if (last_line_rect.height > 0) {
                    y_pos += (getFont()->getFontSize() - last_line_rect.height) * 0.5 - last_line_rect.y;
                } else {
                    y_pos -= last_line_rect.height + last_line_rect.y;
                }
                new_frame.x = m_text_board->getBounds().x - getBounds().x + last_line_rect.x + last_line_rect.width;
                new_frame.height = getFont()->getFontSize() * 1.5;
                // 使光标位置高度居中 //
                new_frame.y = y_pos - getFont()->getFontSize() * 0.25;
            }
            
            float curr_diff = point.x - new_frame.x;
            if (point.y > new_frame.y + new_frame.height)
            {
                location += step;
                if (step <= 1) {
                    break;
                }
            }else{
                if (curr_diff < 0) {
                    if (location <= 0) {
                        break;
                    }
                    diff_x = curr_diff;
                } else {
                    location += step;
                    if (step <= 1) {
                        if (fabs(curr_diff) > fabs(diff_x)) {
                            break;
                        } else {
                            location -= step;
                            break;
                        }
                    }
                }
            }
            if (step >= 2) {
                step /= 2;
            }
        }
        if (location <= 0) {
            location = 0;
        } else {
            MGUTF8String str = m_text_content.substr(location, 1);
            if (str.toSTLString() == "\n") {
                --location;
                MGUTF8String sstr = m_text_content.substr(location, 1);
                if (sstr.toSTLString() != "\r") {
                    ++location;
                }
            } else if (str.toSTLString() == "\r") {
                ++location;
                MGUTF8String sstr = m_text_content.substr(location, 1);
                if (sstr.toSTLString() != "\n") {
                    --location;
                }
            }
        }
        m_cursor->setLocation(location);
        adjustCursorLocation(m_cursor);
        m_text_board->adjustBoardLocation();
    }
    
    void MITextView::moveCursorRight(MICursor* m_cursor, float diff_x, float diff_y, const MGPoint& point, int_32 step, uint_32 location)
    {
        if (step <= 0) {
            step = m_text_content.size();
            step = step - location;
        }
        while (diff_x > 0) {
            // 需要向右移动光标 //
            location += step;
            if (location >= m_text_content.size()) {
                location = m_text_content.size();
            }
            
            MGRect new_frame = m_cursor->getFrame();
            //if (!bboxs.empty())
            {
                MGRect last_line_rect = getLastLineRect(location);
                float y_pos = m_text_board->getBounds().y - getBounds().y;
                if (last_line_rect.height > 0) {
                    y_pos += (getFont()->getFontSize() - last_line_rect.height) * 0.5 - last_line_rect.y;
                } else {
                    y_pos -= last_line_rect.height + last_line_rect.y;
                }
                new_frame.x = m_text_board->getBounds().x - getBounds().x + last_line_rect.x + last_line_rect.width;
                new_frame.height = getFont()->getFontSize() * 1.5;
                // 使光标位置高度居中 //
                new_frame.y = y_pos - getFont()->getFontSize() * 0.25;
            }
            
            float curr_diff = point.x - new_frame.x;
            if (point.y < new_frame.y)
            {
                location -= step;
                if (step <= 1) {
                    break;
                }
            }else{
                if (curr_diff > 0) {
                    if (location >= m_text_content.size()) {
                        break;
                    }
                    diff_x = curr_diff;
                } else {
                    location -= step;
                    if (step <= 1) {
                        if (fabs(curr_diff) > fabs(diff_x)) {
                            break;
                        } else {
                            location += step;
                            break;
                        }
                    }
                }
            }
            if (step >= 2) {
                step /= 2;
            }
        }
        if (location >= m_text_content.size()) {
            location = m_text_content.size();
        } else {
            MGUTF8String str = m_text_content.substr(location, 1);
            if (str.toSTLString() == "\n") {
                --location;
                MGUTF8String sstr = m_text_content.substr(location, 1);
                if (sstr.toSTLString() != "\r") {
                    ++location;
                }
            } else if (str.toSTLString() == "\r") {
                ++location;
                MGUTF8String sstr = m_text_content.substr(location, 1);
                if (sstr.toSTLString() != "\n") {
                    --location;
                }
            }
        }
        m_cursor->setLocation(location);
        adjustCursorLocation(m_cursor);
        m_text_board->adjustBoardLocation();
    }
    
    void MITextView::moveCursorUp(MICursor* m_cursor, float diff_x, float diff_y, const MGPoint& point)
    {
        //MGLogD("MITextView::moveCursorUp: diff_x=%.1f, diff_y=%.1f, point=(%.1f, %.1f)", diff_x, diff_y, point.x, point.y);
        
        int_32 location = m_cursor->getLocation();
        int_32 step = location;
        while (diff_y < 0) {
            // 需要向上移动光标 //
            location -= step;
            if (location <= 0) {
                location = 0;
            }
            
            MGRect new_frame = m_cursor->getFrame();
            //if (!bboxs.empty())
            {
                MGRect last_line_rect = getLastLineRect(location);
                float y_pos = m_text_board->getBounds().y - getBounds().y;
                if (last_line_rect.height > 0) {
                    y_pos += (getFont()->getFontSize() - last_line_rect.height) * 0.5 - last_line_rect.y;
                } else {
                    y_pos -= last_line_rect.height + last_line_rect.y;
                }
                new_frame.x = m_text_board->getBounds().x - getBounds().x + last_line_rect.x + last_line_rect.width;
                new_frame.height = getFont()->getFontSize() * 1.5;
                // 使光标位置高度居中 //
                new_frame.y = y_pos - getFont()->getFontSize() * 0.25;
            }
            
            float curr_diff = point.y - new_frame.y;
            float curr_diff_x = point.x - new_frame.x;
            if (curr_diff < 0) {
                diff_x = curr_diff_x;
                if (location <= 0) {
                    break;
                }
                diff_y = curr_diff;
            } else if (curr_diff > new_frame.height) {
                location += step;
                if (step <= 1) {
                    if (fabs(curr_diff - new_frame.height) > fabs(diff_y)) {
                        break;
                    } else {
                        location -= step;
                        diff_x = curr_diff_x;
                        break;
                    }
                }
            } else {
                diff_x = curr_diff_x;
                break;
            }
            if (step >= 2) {
                step /= 2;
            }
        }
        if (location <= 0) {
            location = 0;
        } else {
            MGUTF8String str = m_text_content.substr(location, 1);
            if (str.toSTLString() == "\n") {
                --location;
                MGUTF8String sstr = m_text_content.substr(location, 1);
                if (sstr.toSTLString() != "\r") {
                    ++location;
                }
            } else if (str.toSTLString() == "\r") {
                ++location;
                MGUTF8String sstr = m_text_content.substr(location, 1);
                if (sstr.toSTLString() != "\n") {
                    --location;
                }
            }
        }
        
        assert(location >= 0);
        if(diff_x < 0)
        {
            moveCursorLeft(m_cursor, diff_x, diff_y, point, 0, location);
        }
        else if(diff_x > 0)
        {
            moveCursorRight(m_cursor, diff_x, diff_y, point, 0, location);
        }
        else
        {
            m_cursor->setLocation(location);
            adjustCursorLocation(m_cursor);
            m_text_board->adjustBoardLocation();
        }
    }
    
    void MITextView::moveCursorDown(MICursor* m_cursor, float diff_x, float diff_y, const MGPoint& point)
    {
        //MGLogD("MITextView::moveCursorDown: diff_x=%.1f, diff_y=%.1f, point=(%.1f, %.1f)", diff_x, diff_y, point.x);
        
        int_32 location = m_cursor->getLocation();
        int_32 step = m_text_content.size();
        step = step - location;
        while (diff_y > m_cursor->getFrame().height) {
            // 需要向下移动光标 //
            location += step;
            if (location >= m_text_content.size()) {
                location = m_text_content.size();
            }
            
            MGRect new_frame = m_cursor->getFrame();
            //if (!bboxs.empty())
            {
                MGRect last_line_rect = getLastLineRect(location);
                float y_pos = m_text_board->getBounds().y - getBounds().y;
                if (last_line_rect.height > 0) {
                    y_pos += (getFont()->getFontSize() - last_line_rect.height) * 0.5 - last_line_rect.y;
                } else {
                    y_pos -= last_line_rect.height + last_line_rect.y;
                }
                new_frame.x = m_text_board->getBounds().x - getBounds().x + last_line_rect.x + last_line_rect.width;
                new_frame.height = getFont()->getFontSize() * 1.5;
                // 使光标位置高度居中 //
                new_frame.y = y_pos - getFont()->getFontSize() * 0.25;
            }
            
            float curr_diff = point.y - new_frame.y;
            float curr_diff_x = point.x - new_frame.x;
            if (curr_diff > new_frame.height) {
                diff_x = curr_diff_x;
                if (location >= m_text_content.size()) {
                    break;
                }
                diff_y = curr_diff;
            } else if (curr_diff < 0) {
                location -= step;
                if (step <= 1) {
                    if (fabs(curr_diff) > fabs(diff_y)) {
                        break;
                    } else {
                        location += step;
                        diff_x = curr_diff_x;
                        break;
                    }
                }
            } else {
                diff_x = curr_diff_x;
                break;
            }
            if (step >= 2) {
                step /= 2;
            }
        }
        if (location >= m_text_content.size()) {
            location = m_text_content.size();
        } else {
            MGUTF8String str = m_text_content.substr(location, 1);
            if (str.toSTLString() == "\n") {
                --location;
                MGUTF8String sstr = m_text_content.substr(location, 1);
                if (sstr.toSTLString() != "\r") {
                    ++location;
                }
            } else if (str.toSTLString() == "\r") {
                ++location;
                MGUTF8String sstr = m_text_content.substr(location, 1);
                if (sstr.toSTLString() != "\n") {
                    --location;
                }
            }
        }
        
        assert(location >= 0);
        if(diff_x < 0)
        {
            moveCursorLeft(m_cursor, diff_x, diff_y, point, 0, location);
        }
        else if(diff_x > 0)
        {
            moveCursorRight(m_cursor, diff_x, diff_y, point, 0, location);
        }
        else
        {
            m_cursor->setLocation(location);
            adjustCursorLocation(m_cursor);
            m_text_board->adjustBoardLocation();
        }
    }
    
#pragma mark - Selecting and Editing -
    void MITextView::setTextAlignment(MITextAlignment align)
    {
        if (getEditable()) {
            m_text_alignment = MITextAlignmentLeftTop;
        } else {
            m_text_alignment = align;
        }
    }
    
    MITextAlignment MITextView::getTextAlignment()
    {
        return m_text_alignment;
    }
    
    void MITextView::startEditing()
    {
        finishSelecting();
        
        m_editing = true;
        if(m_cursor != NULL)
        {
            m_cursor->animate(true);
        }
    }
    
    void MITextView::finishEditing()
    {
        m_editing = false;
        m_cursor->animate(false);
        m_cursor->hide();
        hideMenuWithAnimation();
    }
    
#pragma mark - selecting -
    void MITextView::startSelecting()
    {
        assert(m_text_content.length() > 0);
        
        m_cursor->animate(false);
        m_cursor->hide();
        
        m_selecting = true;
        m_select_begin_cursor->hide();
        m_select_end_cursor->hide();
        
        // init location
        int_32 select_start_location = 0;
        int_32 select_end_location = m_text_content.length();
       
        if(m_editable || m_selectable)
        {
            select_end_location = m_cursor->getLocation();
            const uint_32 select_text_max_size = 101;
            
            if(select_end_location == 0)
            {
                MGUTF8String special_key = m_text_content.substr(0, 1);
                
                select_end_location = 1;
                uint_32 step = 0;
                while((step++) < select_text_max_size)
                {
                    if(select_end_location >= m_text_content.size())
                    {
                        select_end_location = m_text_content.size()-1;
                        break;
                    }
                    MGUTF8String current_utf8_string = m_text_content.substr(select_end_location-1, 1);
                    if(special_key.toSTLString() == " ")
                    {
                        special_key = current_utf8_string;
                    }
                    string stl_string = current_utf8_string.toSTLString();
                    if(!isSimilar(special_key, current_utf8_string))
                    {
                        break;
                    }
                    
                    ++select_end_location;
                }
            }
            else
            {
                MGUTF8String special_key = m_text_content.substr(select_end_location-1, 1);
                select_start_location = select_end_location-1;
                
                uint_32 step = 0;
                while((step++) < select_text_max_size)
                {
                    if(select_start_location <= 0)
                    {
                        select_start_location = 0;
                        break;
                    }
                    MGUTF8String current_utf8_string = m_text_content.substr(select_start_location-1, 1);
                    if(special_key.toSTLString() == " ")
                    {
                        special_key = current_utf8_string;
                    }
                    if(!isSimilar(special_key, current_utf8_string))
                    {
                        break;
                    }
                    
                    --select_start_location;
                }
                
                select_end_location = select_start_location+1;
                step = 0;
                while((step++) < select_text_max_size)
                {
                    if(select_end_location >= m_text_content.size())
                    {
                        select_end_location = m_text_content.size();
                        break;
                    }
                    
                    MGUTF8String current_utf8_string = m_text_content.substr(select_end_location, 1);
                    if(special_key.toSTLString() == " ")
                    {
                        special_key = current_utf8_string;
                    }
                    if(!isSimilar(special_key, current_utf8_string))
                    {
                        break;
                    }
                    
                    ++select_end_location;
                }
            }
        }
        
        assert(select_start_location != select_end_location);
        m_select_begin_cursor->setLocation(select_start_location);
        m_select_end_cursor->setLocation(select_end_location);
        adjustCursorLocation(m_select_begin_cursor);
        adjustCursorLocation(m_select_end_cursor);
        
        showMenuWithAnimation(true);
        
        if(m_editable || m_selectable)
        {
            m_select_begin_cursor->show();
            m_select_end_cursor->show();
        }
    }
    
    void MITextView::finishSelecting()
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
        if(m_preinput_text.empty())
        {
            adjustCursorLocation(m_select_begin_cursor);
        }
        adjustCursorLocation(m_select_end_cursor);
        
        hideMenuWithAnimation();
    }
    
    bool MITextView::isSimilar(const MGUTF8String &original, const MGUTF8String &other)
    {
        bool ret = false;
        
        do
        {
            assert(original.length() == 1 && other.length() == 1);
            
            string stl_original = original.toSTLString();
            string stl_other = other.toSTLString();
            if(stl_original == stl_other)
            {
                ret = true;
                break;
            }
            
            if(stl_original.length() != stl_other.length())
            {
                break;
            }
            
            if(stl_other == "\r" ||
               stl_other == "\n" ||
               stl_other == " ")
            {
                break;
            }
            
            if(stl_original.length() == 1)
            {
                assert(stl_other.length() == 1);
                
                char ch_original = stl_original.at(0);
                char ch_other = stl_other.at(0);
                if(isdigit(ch_original) && isdigit(ch_other))
                {
                    ret = true;
                    break;
                }
                else if(isalpha(ch_original) && isalpha(ch_other))
                {
                    ret = true;
                    break;
                }
            }
            else
            {
                ret = true;
            }
        }
        while(false);
        
        return ret;
    }
    
    void MITextView::showMenuWithAnimation(bool is_selecting)
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
            float menu_edge = TEXT_VIEW_MENU_EDGE*MINVALUE(MGDevice::getScreenWidth(), MGDevice::getScreenHeight());
            
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
    
    void MITextView::hideMenuWithAnimation()
    {
        text_edit_menu->hideAndStopAnimation();
    }
    
#pragma mark - MITextViewDelegate Interface -
    bool MITextViewDelegate::shouldBeginEditing(MITextView* textView)
    {
        return true;
    }
    
    void MITextViewDelegate::didBeginEditing(MITextView* textView)
    {
        
    }
    
    bool MITextViewDelegate::shouldEndEditing(MITextView* textView)
    {
        return true;
    }
    
    void MITextViewDelegate::didEndEditing(MITextView* textView)
    {
        
    }
    
    bool MITextViewDelegate::shouldChangeTextInRange(MITextView* textView, const MGRange &range, const std::string &text)
    {
        return true;
    }
    
    void MITextViewDelegate::didChange(MITextView* textView)
    {
        
    }
    
    void MITextViewDelegate::didChangeSelection(MITextView* textView)
    {
        
    }
        
    int MITextViewDelegate::shouldAdjustKeyboardWithHeight(z11::MITextView *textView, int height)
    {
        return 0;
    }
    
    bool MITextViewDelegate::shouldReturn(MITextView* textView)
    {
        return true;
    }
}
