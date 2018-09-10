//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (MITextView.h)  
/// @brief (±æŒƒº˛ µœ÷µƒπ¶ƒ‹µƒºÚ ˆ)  
///  
///(±æŒƒº˛ µœ÷µƒπ¶ƒ‹µƒœÍ ˆ)  
///  
/// @version 0.1   (∞Ê±æ…˘√˜)  
/// @author        (Gu RongFang)  
/// @date          (11-11-11)  
///  
///  
///    –ﬁ∂©Àµ√˜£∫◊Ó≥ı∞Ê±æ  
////////////////////////////////////////////////////////////////////////// 
#pragma once
#include "MIScrollView.h"
#include "MITextField.h"
#include "MIEditableTextView.h"

#define MITEXTVIEW_DEFAULT_BORDER_VALUE (4.0f)

namespace z11
{
    typedef enum
    {
        MITextViewDisplayModeNormal,   // 正常显示 //
        MITextViewDisplayModeAutoReturn  // 自动换行显示，当输入的内容超出显示范围后，自动换行 //
    }
    MITextViewDisplayMode;
    
    class MICursor;
    class MITapGestureRecognizer;
    class MITextView;
    class MITextEditMenu;
    class MILongPressGestureRecognizer;
    class MITapGestureRecognizer;
    
    class MITextViewDisplayBoard : public MIView
    {
    public:
		MITextViewDisplayBoard();
		virtual ~MITextViewDisplayBoard();
        MIVIEW_DECLARE
        MIRESPONDER_TOUCH_DECLARE
        
    public:
        void setContainer(MITextView* text_view);
        void adjustBoardLocation();
        
    protected:
        // managing touches
        MGPoint touchPointInContainerView(const MITouch* const touch);
        void longPressCall(MGObject* _sender);
        void singleTapCall(MGObject* _sender);
        void doubleTapCall(MGObject* _sender);
        
    protected:
        MITextView* m_container_view;
        MICursor* m_previous_moving_cursor;
        
        // gestures
        MILongPressGestureRecognizer* m_long_press_gesture;
        MITapGestureRecognizer* m_single_tap_gesture;
        MITapGestureRecognizer* m_double_tap_gesture;
        bool m_long_pressed;
        MITouch m_long_press_touch;
        
    private:
        friend class MITextView;
    };
    
	class MGFont;
	class MITextViewDelegate;
	class MITextViewImpl;
    class MIScaleView;
    
	class MITextView : public MIScrollView, public MIEditableTextView
	{
	public:
		MITextView();
		virtual ~MITextView();
        
    public:
        MIVIEW_DECLARE
        MIEDITABLETEXTVIEW_DECLARE
		MIRESPONDER_TOUCH_DECLARE
		virtual bool canBecomeFirstResponder();
		virtual void becomeFirstResponder();
		virtual void resignFirstResponder(bool closeKeyboard = true);
        virtual void clearContext(MIView* view);
        /*
         键盘的特殊处理函数，获得键盘高度后进行相应调整
         */
        virtual int adjustWithKeyboardHeight(int height);
    public:
		// Configuring the Text Attributes
		MGPROPERTY(MIDataDetectorTypes, m_data_detector_types, DataDetectorTypes);
        MGPROPERTY(MITextViewDisplayMode, m_display_mode, DisplayMode);
        MGPROPERTY_READONLY(MGFont*, m_text_font, Font);
        MGPROPERTY(string, placeholder, Placeholder);
        // Setting the View’s Background Appearance
		MGPROPERTY(MITextBorderStyle, borderStyle, BorderStyle);
		// Accessing the Delegate
		MGPROPERTY(MITextViewDelegate*, text_view_delegate, Delegate);			// Need implemention
        
        MGPROPERTY(bool, is_respond_to_enter_key, RespondingEnterKey);
        MGPROPERTY(bool, touch_resign_first_responder, TouchResignFirstResponder);
    private:
        MITextAlignment m_text_alignment;
    public:
        void setTextAlignment(MITextAlignment align);
        MITextAlignment getTextAlignment();
		void setTextColor(const RGBAColorRef &color);
		RGBAColorRef getTextColor();
        void setFontSizeAndColor(uint_32 font_size, const RGBAColorRef &font_color = RGBAColorRef::black());
        void setText(const string &text);
        string getText();
        
        MGRect getTextBoardFrame();
        void setCursorLocation(uint_32 location);
    protected:
        virtual void frameChanged(const MGRect &frame);
        
    protected:
        // Drawing and Positioning Overrides
		virtual void drawTextInRect(const MGRect &rect);
        virtual void drawSelectTextBackground();
        virtual void drawPreInputTextBackground();
		virtual void drawPlaceholderInRect(const MGRect &rect);
        
    protected:
        // 光标定位 //
        void adjustCursorLocation(MICursor* m_cursor);
        void setCursorFrameWithPoint(MICursor* c_cursor, const MGPoint &point);
        // return negative value if m_cursor is in front of m_point
        float distanceBetweenCursorAndPoint(MICursor* m_cursor, const MGPoint &m_point);
        MGRect getLastLineRect(int location);
        MICursor* getSelectCurserNearby(const MGPoint &point);
        
        // point 为目的坐标 //
        void moveCursorLeft(MICursor* m_cursor, float diff_x, float diff_y, const MGPoint& point, int_32 step, uint_32 location);
        void moveCursorRight(MICursor* m_cursor, float diff_x, float diff_y, const MGPoint& point, int_32 step, uint_32 location);
        void moveCursorUp(MICursor* m_cursor, float diff_x, float diff_y, const MGPoint& point);
        void moveCursorDown(MICursor* m_cursor, float diff_x, float diff_y, const MGPoint& point);
        
        // text选择与编辑 //
        void startSelecting();
        void finishSelecting();
        void startEditing();
        void finishEditing();
        bool isSimilar(const MGUTF8String &original, const MGUTF8String &other);
        void showMenuWithAnimation(bool is_selecting);
        void hideMenuWithAnimation();
        
    public:
        MITextEditMenu* text_edit_menu;
        MIScaleView* text_scale_view;
        
	protected:
        MITextViewDisplayBoard* m_text_board;
		MICursor* m_cursor;
        MICursor* m_select_begin_cursor;
        MICursor* m_select_end_cursor;
        MGImage* m_unselected_image;
		MGImage* m_selected_image;
		MGImage* m_bounds_image;
        MGUTF8String m_text_content;
        MGUTF8String m_preinput_text;    // 中文等输入法的预输入字母 //
        map<int, MGRect> last_line_rect_caches;
        
        friend class MITextViewDisplayBoard;
	};

	class MITextViewDelegate
	{
	public:
		//Responding to Editing Notifications
		virtual bool shouldBeginEditing(MITextView* textView);
		virtual void didBeginEditing(MITextView* textView);
		virtual bool shouldEndEditing(MITextView* textView);
		virtual void didEndEditing(MITextView* textView);
        
        virtual bool shouldReturn(MITextView* textView);

		//Responding to Text Changes
		virtual bool shouldChangeTextInRange(MITextView* textView, const MGRange &range, const string &text);
		virtual void didChange(MITextView* textView);

		//Responding to Selection Changes
		virtual void didChangeSelection(MITextView* textView);
        
        virtual int shouldAdjustKeyboardWithHeight(MITextView* textView, int height);
	};
}
