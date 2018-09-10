//
//  MIEditableTextView.cpp
//  Gui
//
//  Created by Wang ZhiPeng on 11-6-14.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#include "MIEditableTextView.h"

namespace z11
{
    MIEditableTextView::MIEditableTextView()
    {
        keyboard_type = MIKeyboardTypeDefault;
        return_key_type = MIReturnKeyDefault;
        m_editing = false;
        m_selecting = false;
        
        m_editable = true;
        m_selectable = true;
        m_copyable = true;
    }
    
    MIEditableTextView::~MIEditableTextView()
    {
        
    }
    
    string MIEditableTextView::checkInputString(const string& content, const string& str)
    {
        string ret = str;
        if (str.size() == 0) {
            return ret;
        }
        switch (keyboard_type) {
            case MIKeyboardTypeEmailAddress:
            {
                if (content.find('@') != string::npos) {
                    if (str.find('@') != string::npos) {
                        ret = "";
                    }
                }else{
                    if ((str.find_first_of('@') != str.find_last_of('@')) || (content.size() == 0 && str.at(0) == '@')) {
                        ret = "";
                    }
                }
            }
                break;
            case MIKeyboardTypeDecimalPad:
            {
                if (content.find('.') != string::npos) {
                    if (str.find('.') != string::npos) {
                        ret = "";
                    }
                }else{
                    if (str.find_first_of('.') != str.find_last_of('.')) {
                        ret = "";
                    } else if (content.size() == 0 && str.at(0) == '.') {
                        ret = "0"+str;
                    }
                }
            }
                break;
            default:
                break;
        }
        return ret;
    }
}