//
//  MITextField.h
//  Gui
//
//  Created by Wang ZhiPeng on 11-6-13.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//
#pragma once

#include "MIControl.h"
#include "MIEditableTextView.h"
#include "../../CommonBase/MGString.h"
#include <string>

#define MITEXTFIELD_DEFAULT_BORDER_X_VALUE (4.0f)
#define MITEXTFIELD_DEFAULT_BORDER_Y_VALUE (2.0f)

namespace z11
{
	typedef enum
    {
		MITextBorderStyleNone,
		MITextBorderStyleLine,
		MITextBorderStyleBezel,
        MITextBorderStyleSkia,  // 通过Skia绘制的精细边框 //
		MITextBorderStyleRoundedRect
	}
    MITextBorderStyle;

	typedef enum
    {
		MITextFieldViewModeNever,
		MITextFieldViewModeWhileEditing,
		MITextFieldViewModeUnlessEditing,
		MITextFieldViewModeAlways
	}
    MITextFieldViewMode;
    
    typedef enum
    {
        MITextFieldContentModeNormal,   // 正常输入 //
        MITextFieldContentModePassword  // 密码输入，目前密码输入直接指定用*来表示，以后可以通过类型扩展，增加自定义替代字符或者黑色圆点的模式 //
    }
    MITextFieldContentMode;

	class MITextFieldDelegate ;
    class MGFont;
	class MICursor;
    class MILongPressGestureRecognizer;
    class MITapGestureRecognizer;
    class MITextEditMenu;
    class MIScaleView;
    
    class MITextField : public MIControl, public MIEditableTextView
    {
    public:
        MITextField();
        virtual ~MITextField();
        
    public:
        MIVIEW_DECLARE
        MIEDITABLETEXTVIEW_DECLARE
        MIRESPONDER_TOUCH_DECLARE
        virtual bool canBecomeFirstResponder();
        virtual void becomeFirstResponder();
        virtual void resignFirstResponder(bool closeKB = true);
        virtual void clearContext(MIView* view);
        /*
         键盘的特殊处理函数，获得键盘高度后进行相应调整
         */
        virtual int adjustWithKeyboardHeight(int height);
    public:
		//Accessing the Text Attributes
		MGPROPERTY(string, placeholder, Placeholder);
        MGPROPERTY(MILineBreakMode, m_lineBreakMode, LineBreakMode);
        MGPROPERTY(MITextFieldContentMode, textContentMode, TextContentMode);
        MGPROPERTY_READONLY(MGFont*, m_text_font, Font);
        MGPROPERTY_READONLY(MGFont*, m_placeholder_font, PlaceholderFont);
        
		//Sizing the Text Field’s Text
		MGPROPERTY(bool, adjustsFontSizeToFitWidth, AdjustsFontSizeToFitWidth);
		MGPROPERTY(float, minimumFontSize, MinimumFontSize);
        
		//Managing the Editing Behavior
		MGPROPERTY(bool, clearsOnBeginEditing, ClearsOnBeginEditing);
        
		//Setting the View’s Background Appearance
		MGPROPERTY(MITextBorderStyle, borderStyle, BorderStyle);
		
		//Managing Overlay Views
		MGPROPERTY(MITextFieldViewMode, clearButtonMode, ClearButtonMode);
		MGPROPERTY_RETAIN_SUBVIEW(MIView*, m_leftView, LeftView);   //retain
		MGPROPERTY(MITextFieldViewMode, leftViewMode, LeftViewMode);
		MGPROPERTY_RETAIN_SUBVIEW(MIView*, m_rightView, RightView);  //retain
		MGPROPERTY(MITextFieldViewMode, rightViewMode, RightViewMode);
        
		//Accessing the Delegate
        MGPROPERTY(MITextFieldDelegate*, text_field_delegate, TextFieldDelegate);
        
    private:
        MITextAlignment m_text_alignment;
    public:
        void setTextAlignment(MITextAlignment align);
        MITextAlignment getTextAlignment();
		void setTextColor(const RGBAColorRef &color);
		RGBAColorRef getTextColor();
        void setPlaceholderColor(const RGBAColorRef &color);
        RGBAColorRef getPlaceholderColor();
        void setFontSizeAndColor(uint_32 font_size, const RGBAColorRef &font_color = RGBAColorRef::black());
        void setPlaceholderFontSizeAndColor(uint_32 font_size, const RGBAColorRef &font_color = RGBAColorRef::darkGray());
        void setText(const string &text);
        string getText();
        uint_32 getTextLength();
        
    protected:
        //Drawing and Positioning Overrides
        virtual void drawTextInRect(const MGRect& rect);
        virtual void drawPlaceholderInRect(const MGRect& rect);
        virtual void drawSelectTextBackground();
        
		virtual MGRect textRectForBounds(const MGRect& rect);
        virtual MGRect placeholderRectForBounds(const MGRect& rect);
		virtual MGRect borderRectForBounds(const MGRect& rect);
		virtual MGRect editingRectForBounds(const MGRect& rect);
		virtual MGRect clearButtonRectForBounds(const MGRect& rect);
		virtual MGRect leftViewRectForBounds(const MGRect& rect);
		virtual MGRect rightViewRectForBounds(const MGRect& rect);
        
        float getTextWidthForLocation(uint_32 location);
        void setCursorFrameWithPoint(MICursor* cursor, const MGPoint &point);
        MGPoint touchPointInContainerView(const MITouch* const touch);
        
    protected:
        // managing touches
        void longPressCall(MGObject* _sender);
        void singleTapCall(MGObject* _sender);
        void doubleTapCall(MGObject* _sender);
        
        // text选择与编辑 //
        void startSelecting();
        void finishSelecting();
        void startEditing();
        void finishEditing();
        void showMenuWithAnimation(bool is_selecting);
        void hideMenuWithAnimation();
        // cursors
        void moveCursorLeft(MICursor* cursor, float diff, const MGPoint &point);
        void moveCursorRight(MICursor* cursor, float diff, const MGPoint &point);
        void adjustCursorLocation(MICursor* cursor, bool is_moving_cursor = true);
        MICursor* getSelectCurserNearby(const MGPoint &point);
        
    public:
        MITextEditMenu* text_edit_menu;
        MIScaleView* text_scale_view;
        
	protected:
        MGPoint m_text_pos; // 绘制文字的起始位置，相对于textRect //
        MGPoint m_pre_text_save_pos;
        MICursor* m_previous_moving_cursor;
		MICursor* m_cursor;
        MICursor* m_select_begin_cursor;
        MICursor* m_select_end_cursor;
		MGImage* m_unselected_image;
		MGImage* m_selected_image;
		MGImage* m_bounds_image;
        MGUTF8String m_text_content;
        MGUTF8String m_preinput_text;    // 中文等输入法的预输入字母 //
        map<uint_32, float> text_width_caches;
        
        // gestures
        MILongPressGestureRecognizer* m_long_press_gesture;
        MITapGestureRecognizer* m_single_tap_gesture;
        MITapGestureRecognizer* m_double_tap_gesture;
        bool m_long_pressed;
        MITouch m_long_press_touch;
    };

	class MITextFieldDelegate 
	{
	public:
		//Managing Editing
		virtual bool shouldBeginEditing(MITextField* textField);
		virtual void didBeginEditing(MITextField* textField);
		virtual bool shouldEndEditing(MITextField* textField);
		virtual void didEndEditing(MITextField* textField);
	
	public:
		virtual bool shouldChangeCharactersInRange(MITextField* textField, const MGRange &range, const std::string &text);
		virtual bool shouldClear(MITextField* textField);
		virtual bool shouldReturn(MITextField* textField);
        
        virtual int shouldAdjustKeyboardWithHeight(MITextField* textField, int height);
	};
} //namespace z11