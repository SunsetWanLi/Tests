//
//  MIEditableTextView.h
//  Gui
//
//  Created by Wang ZhiPeng on 11-6-13.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//
#pragma once
#include <string>
#include "../Base/MIObject.h"
#include "../../CommonBase/MGString.h"

using namespace std;

namespace z11
{
    typedef enum
    {
        MIKeyboardTypeDefault,                // Default type for the current input method.
        MIKeyboardTypeASCIICapable,           // Displays a keyboard which can enter ASCII characters, non-ASCII keyboards remain active
        MIKeyboardTypeNumbersAndPunctuation,  // Numbers and assorted punctuation.
        MIKeyboardTypeURL,                    // A type optimized for URL entry (shows . / .com prominently).
        MIKeyboardTypeNumberPad,              // A number pad (0-9). Suitable for PIN entry.
        MIKeyboardTypePhonePad,               // A phone pad (1-9, *, 0, #, with letters under the numbers).
        MIKeyboardTypeNamePhonePad,           // A type optimized for entering a person's name or phone number.
        MIKeyboardTypeEmailAddress,           // A type optimized for multiple email address entry (shows space @ . prominently).
        MIKeyboardTypeDecimalPad,             // A number pad with a decimal point.
        MIKeyboardTypeTwitter,                // A type optimized for twitter text entry (easy access to @ #)
        
    } MIKeyboardType;
    
    typedef enum {
        MIReturnKeyDefault,
        MIReturnKeyGo,
        MIReturnKeyGoogle,
        MIReturnKeyJoin,
        MIReturnKeyNext,
        MIReturnKeyRoute,
        MIReturnKeySearch,
        MIReturnKeySend,
        MIReturnKeyYahoo,
        MIReturnKeyDone,
        MIReturnKeyEmergencyCall,
    } MIReturnKeyType;
    
    class MIKeyboardTypeObj : public MIObject
    {
    public:
        MIKeyboardType keyboard_type;
        MIReturnKeyType return_key_type;
    };
    
    class MIEditableTextView
    {
    public:
        MIEditableTextView();
        virtual ~MIEditableTextView() = 0;
        
    public:
        // Managing Editor's State
        // m_editing is true if one cursor is visible. m_selecting is true when select_cursor is_visible.
        MGPROPERTY_READONLY(bool, m_editing, Editing);
        MGPROPERTY_READONLY(bool, m_selecting, Selecting);
        // Priority: m_editable > m_selectable > m_copyable
        MGPROPERTY(bool, m_editable, Editable);
        MGPROPERTY(bool, m_selectable, Selectable);
        MGPROPERTY(bool, m_copyable, Copyable);
        //接口//
        virtual bool hasText() = 0;
        virtual void insertText(const string& text) = 0;
        virtual void insertPreText(const string& pre_text) = 0;
        virtual void deleteBackward() = 0;
        
        virtual void selectText() = 0;
        virtual void selectAllText() = 0;
        virtual void copySelectedText() = 0;
        virtual void cutSelectedText() = 0;
        virtual void pasteClipboardText() = 0;
        
    protected:
        string checkInputString(const string& content, const string& str);
        
    public:
        MIKeyboardType keyboard_type;
        MIReturnKeyType return_key_type;
    };
#define MIEDITABLETEXTVIEW_DECLARE \
        virtual bool hasText(); \
        virtual void insertText(const string& text); \
        virtual void insertPreText(const string& pre_text); \
        virtual void deleteBackward(); \
        \
        virtual void selectText(); \
        virtual void selectAllText(); \
        virtual void copySelectedText(); \
        virtual void cutSelectedText(); \
        virtual void pasteClipboardText();
} //namespace z11