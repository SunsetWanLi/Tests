//
//  MITextEditMenu.h
//  Gui
//
//  Created by slj on 13-5-6.
//  Copyright (c) 2013年 Zhejiang University. All rights reserved.
//
#pragma once
#include "../Base/MIView.h"
#include "../Controllers/MIViewController.h"

namespace z11
{
    typedef enum _MITextEditMenuType
    {
        MITextEditMenuEdit,
        MITextEditMenuSelect
    }
    MITextEditMenuType;
    
    class MIEditableTextView;
    class MITextEditMenuDelegate;
    
    class MITextEditMenu : public MIView
    {
    public:
        MITextEditMenu();
        virtual ~MITextEditMenu();
        virtual void initWithFrame(MGRect frame);
        void showWithAnimation();
        void hideAndStopAnimation();
        // set
        void setTextMenuDelegate(MITextEditMenuDelegate* delegate);
        void setContentView(MIEditableTextView* content_view);
        void setButtonTextEnabled(bool flag);
        // reload
        virtual void reloadWithType(MITextEditMenuType menu_type);
    
    protected:
        virtual void initTextMenuEdit();
        virtual void initextMenuSelect();
        
    private:
        MIEditableTextView* content_view;
        MITextEditMenuDelegate* text_menu_delegate;
        bool button_text_enabled;
        
    private:
        friend class MITextEditMenuDelegate;
    };
    
    class MITextEditMenuDelegate : public MIViewController
    {
    public:
        MITextEditMenuDelegate();
        virtual ~MITextEditMenuDelegate();
        
    public:
        // 选择 //
        virtual void selectCall(MIObject* _sender);
        // 全选 //
        virtual void selectAllCall(MIObject* _sender);
        // 拷贝 //
        virtual void copyCall(MIObject* _sender);
        // 剪切 //
        virtual void cutCall(MIObject* _sender);
        // 粘贴 //
        virtual void pasteCall(MIObject* _sender);
    };
}