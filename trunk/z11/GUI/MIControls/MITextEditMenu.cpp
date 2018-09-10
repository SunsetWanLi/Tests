//
//  MITextEditMenu.cpp
//  Gui
//
//  Created by slj on 13-5-6.
//  Copyright (c) 2013年 Zhejiang University. All rights reserved.
//

#include "MITextEditMenu.h"
#include "MIEditableTextView.h"
#include "MIButton.h"
#include "MITextView.h"
#include "../../System/Device/MGDevice.h"
#include "../../System/Shared/Utility.h"
#include "../../System/ClipBoard/MGClipBoard.h"
#include "../../GUI/MILayoutEngine/MILayoutEngine.h"
#include "../../GUI/GuiContainer/GuiContainer.h"
#include "assert.h"

#define           TEXT_MENU_SEGMENT_WIDTH                    (120.0f/640.0f)
#define           TEXT_MENU_SEGMENT_ASPECT_RATIO             0.5f
#define           TEXT_MENU_ANIMATION_DURATION               0.24f
#define           TEXT_MENU_ANIMATION_NAME                   "TEXT_MENU_ANIMATION_NAME"

namespace z11
{
#pragma mark - MITextEditMenu constructor -
    MITextEditMenu::MITextEditMenu()
    {
        class_name = "MITextEditMenu";
        _view_flags.clip_subviews = 1;
        content_view = NULL;
        text_menu_delegate = NULL;
        button_text_enabled = true;
    }
    
    MITextEditMenu::~MITextEditMenu()
    {
        delete_and_set_null(text_menu_delegate);
    }
    
    void MITextEditMenu::initWithFrame(MGRect frame)
    {
        assert(text_menu_delegate != NULL);
        MIView::initWithFrame(frame);
    }
    
    void MITextEditMenu::hideAndStopAnimation()
    {
        MIView::stopAnimation(TEXT_MENU_ANIMATION_NAME);
        hide();
    }
    
    void MITextEditMenu::showWithAnimation()
    {
        hideAndStopAnimation();
        show();
        setAlpha(0.0f);
        MIView::beginAnimation(TEXT_MENU_ANIMATION_NAME);
        MIView::setAnimationDuration(TEXT_MENU_ANIMATION_DURATION);
        MIView::setAnimationCurve(MIViewAnimationOptionCurveLinear);
        setAlpha(1.0f);
        MIView::commitAnimation();
    }
    
#pragma mark - MITextEditMenu set -
    void MITextEditMenu::setTextMenuDelegate(MITextEditMenuDelegate* delegate)
    {
        delete_and_set_null(text_menu_delegate);
        text_menu_delegate = delegate;
        text_menu_delegate->view = this;
    }
    
    void MITextEditMenu::setContentView(MIEditableTextView* content_view)
    {
        this->content_view = content_view;
    }
    
    void MITextEditMenu::setButtonTextEnabled(bool flag)
    {
        this->button_text_enabled = flag;
    }
    
#pragma mark - MITextEditMenu reload -
    void MITextEditMenu::reloadWithType(MITextEditMenuType menu_type)
    {
        assert(_view_flags.view_initialized == 1);
        removeAllSubviews();
        
        switch(menu_type)
        {
            case MITextEditMenuEdit:
            {
                initTextMenuEdit();
                break;
            }
            case MITextEditMenuSelect:
            {
                initextMenuSelect();
                break;
            }
            default:
            {
                assert(0);
                break;
            }
        }
    }
    
    void MITextEditMenu::initTextMenuEdit()
    {
        // init params
        float start_x = 0.0f;
        float start_y = 0.0f;
        float button_width = MINVALUE(MGDevice::getScreenWidth(), MGDevice::getScreenHeight())*TEXT_MENU_SEGMENT_WIDTH;
        float button_height = button_width*TEXT_MENU_SEGMENT_ASPECT_RATIO;
        EventHandler handler = {text_menu_delegate, NULL};
        float round_radius = 1;
        
        // init titles
        string string_select = "Select";
        string string_select_all = "Select All";
        string string_paste = "Paste";
        do
        {
            const vector<string>* string_select_vec = MILayoutEngine::sharedEngine().findStringsById("text_edit_menu_select");
            const vector<string>* string_select_all_vec = MILayoutEngine::sharedEngine().findStringsById("text_edit_menu_select_all");
            const vector<string>* string_paste_vec = MILayoutEngine::sharedEngine().findStringsById("text_edit_menu_paste");
            if(string_select_vec != NULL)
            {
                string_select = string_select_vec->at(0);
            }
            if(string_select_all_vec != NULL)
            {
                string_select_all = string_select_all_vec->at(0);
            }
            if(string_paste_vec != NULL)
            {
                string_paste = string_paste_vec->at(0);
            }
        }
        while(false);
        
    
        // init font_size and font_color
        uint_32 font_size = button_height*0.45f;
        RGBAColorRef font_color = RGBAColorRef::white();
        
        MIButton *select_btn = NULL;
        MIButton *select_all_btn = NULL;
        MIButton *paste_btn = NULL;
        // add buttons
        if(content_view->hasText())
        {
            // select
            do
            {
                if(!content_view->getSelectable())
                {
                    break;
                }
                select_btn = new MIButton();
                select_btn->setCornerRadius(round_radius);
                select_btn->initWithFrame(MGRectMake(start_x, start_y, button_width, button_height));
                if(button_text_enabled)
                {
                    select_btn->titleLabel()->setText(string_select);
                    select_btn->titleLabel()->setFontSizeAndColor(font_size, font_color);
                }
                
                select_btn->setBackgroundColor(MGColor::fromColorRef(RGBAColorRef::black()));
                handler.event_func = (EventFunc)&MITextEditMenuDelegate::selectCall;
                select_btn->registerEventHandler(MIControlEventTouchUpInside, handler);
                addSubview(select_btn);
                
                start_x += button_width;
            }
            while(false);
            
            // select_all
            do
            {
                if(!content_view->getSelectable())
                {
                    break;
                }
                select_all_btn = new MIButton();
                select_all_btn->setCornerRadius(round_radius);
                select_all_btn->initWithFrame(MGRectMake(start_x, start_y, button_width, button_height));
                if(button_text_enabled)
                {
                    select_all_btn->titleLabel()->setText(string_select_all);
                    select_all_btn->titleLabel()->setFontSizeAndColor(font_size, font_color);
                }
                
                select_all_btn->setBackgroundColor(MGColor::fromColorRef(RGBAColorRef::black()));
                handler.event_func = (EventFunc)&MITextEditMenuDelegate::selectAllCall;
                select_all_btn->registerEventHandler(MIControlEventTouchUpInside, handler);
                addSubview(select_all_btn);
                
                start_x += button_width;
            }
            while(false);
        }
        
        // paste
        do
        {
            if(!MGClipBoard::hasData())
            {
                break;
            }
            
            paste_btn = new MIButton();
            paste_btn->setCornerRadius(round_radius);
            paste_btn->initWithFrame(MGRectMake(start_x, start_y, button_width, button_height));
            if(button_text_enabled)
            {
                paste_btn->titleLabel()->setText(string_paste);
                paste_btn->titleLabel()->setFontSizeAndColor(font_size, font_color);
            }
            
            paste_btn->setBackgroundColor(MGColor::fromColorRef(RGBAColorRef::black()));
            handler.event_func = (EventFunc)&MITextEditMenuDelegate::pasteCall;
            paste_btn->registerEventHandler(MIControlEventTouchUpInside, handler);
            addSubview(paste_btn);
            
            start_x += button_width;
        }
        while(false);
        
        /*
        if(select_btn!=NULL){
            select_btn->view_round_mode = RoundRectLeft;
            if(select_all_btn!=NULL){
                if(paste_btn!=NULL){
                    select_all_btn->view_round_mode = RoundRectNone;
                    paste_btn->view_round_mode = RoundRectRight;
                }
                else{
                    select_all_btn->view_round_mode = RoundRectRight;
                }
            }
            else{
                if(paste_btn!=NULL){
                    paste_btn->view_round_mode = RoundRectRight;
                }
                else{
                    select_btn->view_round_mode = RoundRectAll;
                }
            }
        }
        else{
            if(select_all_btn!=NULL){
                if(paste_btn!=NULL){
                    select_all_btn->view_round_mode = RoundRectLeft;
                    paste_btn->view_round_mode = RoundRectRight;
                }
                else{
                    select_all_btn->view_round_mode = RoundRectAll;
                }
            }
            else{
                if(paste_btn!=NULL){
                    paste_btn->view_round_mode = RoundRectAll;
                }
            }
        }
         */
        
        
        
        // set frame
        MGRect frame = getFrame();
        frame.width = start_x;
        frame.height = button_height;
        setFrame(frame);
    }
    
    void MITextEditMenu::initextMenuSelect()
    {
        assert(content_view->hasText());
        
        // init params
        float start_x = 0.0f;
        float start_y = 0.0f;
        float button_width = MINVALUE(MGDevice::getScreenWidth(), MGDevice::getScreenHeight())*TEXT_MENU_SEGMENT_WIDTH;
        float button_height = button_width*TEXT_MENU_SEGMENT_ASPECT_RATIO;
        EventHandler handler = {text_menu_delegate, NULL};
        float round_radius = 1;
        
        // init titles
        string string_cut = "Cut";
        string string_copy = "Copy";
        string string_paste = "Paste";
        do
        {
            const vector<string>* string_cut_vec = MILayoutEngine::sharedEngine().findStringsById("text_edit_menu_cut");
            const vector<string>* string_copy_vec = MILayoutEngine::sharedEngine().findStringsById("text_edit_menu_copy");
            const vector<string>* string_paste_vec = MILayoutEngine::sharedEngine().findStringsById("text_edit_menu_paste");
            if(string_cut_vec != NULL)
            {
                string_cut = string_cut_vec->at(0);
            }
            if(string_copy_vec != NULL)
            {
                string_copy = string_copy_vec->at(0);
            }
            if(string_paste_vec != NULL)
            {
                string_paste = string_paste_vec->at(0);
            }
        }
        while(false);
        
        
        // init font_size and font_color
        uint_32 font_size = button_height*0.45f;
        RGBAColorRef font_color = RGBAColorRef::white();
        
        // add buttons
        MIButton *copy_btn = NULL;
        MIButton *cut_btn = NULL;
        MIButton *paste_btn = NULL;
        
        if(!content_view->getEditable())
        {
            // copy
            do
            {
                if(!content_view->getCopyable())
                {
                    break;
                }
                
                copy_btn = new MIButton();
                copy_btn->setCornerRadius(round_radius);
                copy_btn->initWithFrame(MGRectMake(start_x, start_y, button_width, button_height));
                if(button_text_enabled)
                {
                    copy_btn->titleLabel()->setText(string_copy);
                    copy_btn->titleLabel()->setFontSizeAndColor(font_size, font_color);
                }
                copy_btn->setBackgroundColor(MGColor::fromColorRef(RGBAColorRef::black()));
                handler.event_func = (EventFunc)&MITextEditMenuDelegate::copyCall;
                copy_btn->registerEventHandler(MIControlEventTouchUpInside, handler);
                addSubview(copy_btn);
//                copy_btn->view_round_mode = RoundRectAll;
                
                start_x += button_width;
            }
            while(false);
        }
        else
        {
            // cut
            do
            {
                if(!content_view->getCopyable())
                {
                    break;
                }
                cut_btn = new MIButton();
                cut_btn->setCornerRadius(round_radius);
                cut_btn->initWithFrame(MGRectMake(start_x, start_y, button_width, button_height));
                if(button_text_enabled)
                {
                    cut_btn->titleLabel()->setText(string_cut);
                    cut_btn->titleLabel()->setFontSizeAndColor(font_size, font_color);
                }
                
                cut_btn->setBackgroundColor(MGColor::fromColorRef(RGBAColorRef::black()));
                handler.event_func = (EventFunc)&MITextEditMenuDelegate::cutCall;
                cut_btn->registerEventHandler(MIControlEventTouchUpInside, handler);
                addSubview(cut_btn);
                
                start_x += button_width;
            }
            while(false);
            
            // copy
            do
            {
                if(!content_view->getCopyable())
                {
                    break;
                }
                copy_btn = new MIButton();
                copy_btn->setCornerRadius(round_radius);
                copy_btn->initWithFrame(MGRectMake(start_x, start_y, button_width, button_height));
                if(button_text_enabled)
                {
                    copy_btn->titleLabel()->setText(string_copy);
                    copy_btn->titleLabel()->setFontSizeAndColor(font_size, font_color);
                }
                
                copy_btn->setBackgroundColor(MGColor::fromColorRef(RGBAColorRef::black()));
                handler.event_func = (EventFunc)&MITextEditMenuDelegate::copyCall;
                copy_btn->registerEventHandler(MIControlEventTouchUpInside, handler);
                addSubview(copy_btn);
                
                start_x += button_width;
            }
            while(false);
            
            // paste
            do
            {
                if(!MGClipBoard::hasData())
                {
                    break;
                }
                paste_btn = new MIButton();
                paste_btn->setCornerRadius(round_radius);
                paste_btn->initWithFrame(MGRectMake(start_x, start_y, button_width, button_height));
                if(button_text_enabled)
                {
                    paste_btn->titleLabel()->setText(string_paste);
                    paste_btn->titleLabel()->setFontSizeAndColor(font_size, font_color);
                }
                
                paste_btn->setBackgroundColor(MGColor::fromColorRef(RGBAColorRef::black()));
                handler.event_func = (EventFunc)&MITextEditMenuDelegate::pasteCall;
                paste_btn->registerEventHandler(MIControlEventTouchUpInside, handler);
                addSubview(paste_btn);
                
                start_x += button_width;
            }
            while(false);
            
            /*
            if(cut_btn!=NULL){
                cut_btn->view_round_mode = RoundRectLeft;
                if(copy_btn!=NULL){
                    if(paste_btn!=NULL){
                        copy_btn->view_round_mode = RoundRectNone;
                        paste_btn->view_round_mode = RoundRectRight;
                    }
                    else{
                        copy_btn->view_round_mode = RoundRectRight;
                    }
                }
                else{
                    if(paste_btn!=NULL){
                        paste_btn->view_round_mode = RoundRectRight;
                    }
                    else{
                        cut_btn->view_round_mode = RoundRectAll;
                    }
                }
            }
            else{
                if(copy_btn!=NULL){
                    if(paste_btn!=NULL){
                        copy_btn->view_round_mode = RoundRectLeft;
                        paste_btn->view_round_mode = RoundRectRight;
                    }
                    else{
                        copy_btn->view_round_mode = RoundRectAll;
                    }
                }
                else{
                    if(paste_btn!=NULL){
                        paste_btn->view_round_mode = RoundRectAll;
                    }
                }
            }
            */
            
        }
        
        // set frame
        MGRect frame = getFrame();
        frame.width = start_x;
        frame.height = button_height;
        setFrame(frame);
    }
    
#pragma mark - delegate -
    MITextEditMenuDelegate::MITextEditMenuDelegate()
    {
        
    }
    
    MITextEditMenuDelegate::~MITextEditMenuDelegate()
    {
        view = NULL;
    }
    
    void MITextEditMenuDelegate::selectCall(MIObject* _sender)
    {
        MITextEditMenu* text_menu = dynamic_cast<MITextEditMenu*>(view);
        if(text_menu != NULL)
        {
            text_menu->content_view->selectText();
        }
    }
    
    void MITextEditMenuDelegate::selectAllCall(MIObject* _sender)
    {
        MITextEditMenu* text_menu = dynamic_cast<MITextEditMenu*>(view);
        if(text_menu != NULL)
        {
            text_menu->content_view->selectAllText();
        }
    }
    
    void MITextEditMenuDelegate::copyCall(MIObject* _sender)
    {
        MITextEditMenu* text_menu = dynamic_cast<MITextEditMenu*>(view);
        if(text_menu != NULL)
        {
            text_menu->content_view->copySelectedText();
        }
    }
    
    void MITextEditMenuDelegate::cutCall(MIObject* _sender)
    {
        MITextEditMenu* text_menu = dynamic_cast<MITextEditMenu*>(view);
        if(text_menu != NULL)
        {
            text_menu->content_view->cutSelectedText();
        }
    }
    
    void MITextEditMenuDelegate::pasteCall(MIObject* _sender)
    {
        MITextEditMenu* text_menu = dynamic_cast<MITextEditMenu*>(view);
        if(text_menu != NULL)
        {
            text_menu->content_view->pasteClipboardText();
        }
    }
}