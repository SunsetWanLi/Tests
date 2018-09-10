//
//  MIAutomation.m
//  MIAutomation
//
//  Created by 王志鹏 on 16/8/11.
//  Copyright © 2016年 Rayme Tech Co.,LTD. All rights reserved.
//

#include "MIAutoAction.h"

using namespace z11;

#pragma mark - MIAutoObject -

MIAutoObject::MIAutoObject()
{
    class_name = "";
    index = -1;
}

MIAutoObject::~MIAutoObject()
{
    
}

#pragma mark - MIAutoAction -

MIAutoAction::MIAutoAction()
{
    object_chain.clear();
    retry_num = 0;
}

MIAutoAction::~MIAutoAction()
{
    
}

void MIAutoAction::addObjectChain(const string &chain)
{
    string::size_type bidx = chain.find("[");
    string::size_type eidx = chain.find("]");
    MIAutoObject obj;
    if (bidx == string::npos || eidx == string::npos) {
        obj.index = 0;
        obj.class_name = chain;
    } else if (eidx <= bidx) {
        return;
    } else {
        string cn = chain.substr(0,bidx);
        string idx = chain.substr(bidx+1,eidx-bidx-1);
        obj.class_name = cn;
        obj.index = Utility::BuiltInTypeFromString<int>(idx);
    }
    
    object_chain.push_back(obj);
}

MIView* MIAutoAction::objectView()
{
    MIView* ret = NULL;
    
    if (GuiContainer::container == NULL) {
        return ret;
    }
    
    MIViewController* root_controller = GuiContainer::container->gui_root;
    if (root_controller == NULL || root_controller->view == NULL) {
        return ret;
    }
    
    MIView* root_view = root_controller->view;
    MIView* object_view = root_view;
    deque<MIAutoObject> chain = object_chain;
    
    if (object_chain.size() > 0 && object_chain.at(0).class_name == "*") {
        size_t b_pos = object_chain.at(1).class_name.find("(");
        size_t e_pos = object_chain.at(1).class_name.find(")");
        if (object_chain.at(1).class_name.find("text") == 0 && b_pos != string::npos && e_pos != string::npos && b_pos < e_pos) {
            ret = viewForText(object_chain.at(1).class_name.substr(b_pos+1,e_pos-b_pos-1), object_view);
        } else if (object_chain.at(1).class_name.find("placeholder") == 0 && b_pos != string::npos && e_pos != string::npos && b_pos < e_pos) {
            ret = viewForPlaceholder(object_chain.at(1).class_name.substr(b_pos+1,e_pos-b_pos-1), object_view);
        } else
            ret = viewForTag(object_chain.at(1).class_name, object_view);
        if (ret != NULL && chain.size() > 2) {
            chain.pop_front();
            chain.pop_front();
            ret = viewForChain(chain, ret);
        }
    } else
        ret = viewForChain(chain, object_view);
    
    return ret;
}

deque<string> MIAutoAction::findViews()
{
    deque<string> ret;
    
    if (GuiContainer::container == NULL) {
        return ret;
    }
    
    MIViewController* root_controller = GuiContainer::container->gui_root;
    if (root_controller == NULL || root_controller->view == NULL) {
        return ret;
    }
    
    MIView* root_view = root_controller->view;
    MIView* object_view = root_view;
    deque<MIAutoObject> chain = object_chain;
    
    if (object_chain.size() > 0 && object_chain.at(0).class_name == "*") {
        size_t b_pos = object_chain.at(1).class_name.find("(");
        size_t e_pos = object_chain.at(1).class_name.find(")");
        if (object_chain.at(1).class_name.find("text") == 0 && b_pos != string::npos && e_pos != string::npos && b_pos < e_pos) {
            ret = pathForText(object_chain.at(1).class_name.substr(b_pos+1,e_pos-b_pos-1), object_view);
        }
    }
    
    return ret;
}

MIView* MIAutoAction::viewForChain(deque<z11::MIAutoObject>& chain, z11::MIView *view)
{
    MIView* ret = NULL;
    
    if (chain.size() == 0 || view == NULL) {
        return view;
    }
    
    MIAutoObject object = chain.front();
    chain.pop_front();
    
    MIView* object_view = view->subviewWithTag(object.class_name, object.index);
    
    ret = viewForChain(chain, object_view);
    
    return ret;
}

MIView* MIAutoAction::viewForTag(const string& tag, z11::MIView *view)
{
    if (view == NULL) {
        return view;
    }
    MIView* ret = view->subviewWithTagRecur(tag);
    return ret;
}

MIView* MIAutoAction::viewForText(const string& text, z11::MIView *view)
{
    if (view == NULL) {
        return view;
    }
    
    MIView* ret = NULL;
    
    view->lock();
    SubviewIterator it;
    for (it = view->m_subviews.begin(); it != view->m_subviews.end(); ++it) {
        assert((*it) != NULL);
        if((*it)->isObjectOfClass("MITextField"))
        {
            MITextField* field = dynamic_cast<MITextField*>(*it);
            if (field != NULL && field->getText().compare(text) == 0) {
                ret = *it;
                break;
            }
        }
        if((*it)->isObjectOfClass("MITextView"))
        {
            MITextView* view = dynamic_cast<MITextView*>(*it);
            if (view != NULL && view->getText().compare(text) == 0) {
                ret = *it;
                break;
            }
        }
        if((*it)->isObjectOfClass("MILabel"))
        {
            MILabel* label = dynamic_cast<MILabel*>(*it);
            if (label != NULL && label->getText().compare(text) == 0) {
                ret = *it;
                break;
            }
        }
        if((*it)->isObjectOfClass("MIButton"))
        {
            MIButton* btn = dynamic_cast<MIButton*>(*it);
            if (btn != NULL && btn->titleLabel()->getText().compare(text) == 0) {
                ret = *it;
                break;
            }
        }
    }
    if (ret == NULL) {
        for (it = view->m_subviews.begin(); it != view->m_subviews.end(); ++it) {
            assert((*it) != NULL);
            ret = viewForText(text, *it);
            if (ret != NULL) {
                break;
            }
        }
    }
    view->unlock();
    
    return ret;
}

MIView* MIAutoAction::viewForPlaceholder(const string& place_holder, z11::MIView *view)
{
    if (view == NULL) {
        return view;
    }
    
    MIView* ret = NULL;
    
    view->lock();
    SubviewIterator it;
    for (it = view->m_subviews.begin(); it != view->m_subviews.end(); ++it) {
        assert((*it) != NULL);
        if((*it)->isObjectOfClass("MITextField"))
        {
            MITextField* field = dynamic_cast<MITextField*>(*it);
            if (field != NULL && field->getPlaceholder().compare(place_holder) == 0) {
                ret = *it;
                break;
            }
        }
        if((*it)->isObjectOfClass("MITextView"))
        {
            MITextView* view = dynamic_cast<MITextView*>(*it);
            if (view != NULL && view->getPlaceholder().compare(place_holder) == 0) {
                ret = *it;
                break;
            }
        }
    }
    if (ret == NULL) {
        for (it = view->m_subviews.begin(); it != view->m_subviews.end(); ++it) {
            assert((*it) != NULL);
            ret = viewForPlaceholder(place_holder, *it);
            if (ret != NULL) {
                break;
            }
        }
    }
    view->unlock();
    
    return ret;
}

deque<string> MIAutoAction::pathForText(const string& text, z11::MIView *view)
{
    deque<string> ret;
    if (view == NULL) {
        return ret;
    }
    
    map<string, int> class_count;
    
    view->lock();
    
    SubviewIterator it;
    for (it = view->m_subviews.begin(); it != view->m_subviews.end(); ++it) {
        assert((*it) != NULL);
        if((*it)->isObjectOfClass("MITextField"))
        {
            auto fit = class_count.find((*it)->class_name.className());
            if (fit != class_count.end()) {
                class_count[(*it)->class_name.className()] += 1;
            } else {
                class_count[(*it)->class_name.className()] = 0;
            }
            MITextField* field = dynamic_cast<MITextField*>(*it);
            if (field != NULL && field->getText().find(text) != string::npos) {
                ret.push_back((*it)->class_name.className()+"["+Utility::BuiltInTypeToString(class_count[(*it)->class_name.className()])+"]");
            }
        }
        if((*it)->isObjectOfClass("MITextView"))
        {
            auto fit = class_count.find((*it)->class_name.className());
            if (fit != class_count.end()) {
                class_count[(*it)->class_name.className()] += 1;
            } else {
                class_count[(*it)->class_name.className()] = 0;
            }
            MITextView* textview = dynamic_cast<MITextView*>(*it);
            if (textview != NULL && textview->getText().find(text) != string::npos) {
                ret.push_back((*it)->class_name.className()+"["+Utility::BuiltInTypeToString(class_count[(*it)->class_name.className()])+"]");
            }
        }
        if((*it)->isObjectOfClass("MILabel"))
        {
            auto fit = class_count.find((*it)->class_name.className());
            if (fit != class_count.end()) {
                class_count[(*it)->class_name.className()] += 1;
            } else {
                class_count[(*it)->class_name.className()] = 0;
            }
            MILabel* label = dynamic_cast<MILabel*>(*it);
            if (label != NULL && label->getText().find(text) != string::npos) {
                ret.push_back((*it)->class_name.className()+"["+Utility::BuiltInTypeToString(class_count[(*it)->class_name.className()])+"]");
            }
        }
        if((*it)->isObjectOfClass("MIButton"))
        {
            auto fit = class_count.find((*it)->class_name.className());
            if (fit != class_count.end()) {
                class_count[(*it)->class_name.className()] += 1;
            } else {
                class_count[(*it)->class_name.className()] = 0;
            }
            MIButton* btn = dynamic_cast<MIButton*>(*it);
            if (btn != NULL && btn->titleLabel()->getText().find(text) != string::npos) {
                ret.push_back((*it)->class_name.className()+"["+Utility::BuiltInTypeToString(class_count[(*it)->class_name.className()])+"]");
            }
        }
    }
    
    class_count.clear();
    for (it = view->m_subviews.begin(); it != view->m_subviews.end(); ++it) {
        assert((*it) != NULL);
        string class_name = (*it)->class_name.className();
        auto fit = class_count.find(class_name);
        if (fit != class_count.end()) {
            class_count[class_name] += 1;
        } else {
            class_count[class_name] = 0;
        }
        deque<string> sub_pathes = pathForText(text, *it);
        for (auto sub_it = sub_pathes.begin(); sub_it != sub_pathes.end(); ++sub_it) {
            string output = class_name+"["+Utility::BuiltInTypeToString(class_count[class_name])+"]."+(*sub_it);
            ret.push_back(output);
        }
    }
    
    view->unlock();
    
    return ret;
}

#pragma mark - MIAutoTap -

MIAutoTap::MIAutoTap()
{
    interval_time = 0;
    tap_point = MGIntPointMake(0, 0);
}

MIAutoTap::~MIAutoTap()
{
    
}

bool MIAutoTap::execute()
{
    bool ret = false;
    MIView* object_view = objectView();
    if (object_view != NULL) {
        ret = true;
    } else {
        return ret;
    }
    
    MGRect rect = object_view->getBounds();
    object_view->transAndScaleClipRect(rect);
    
    MIEvent* b_event = new MIEvent();
    MIEvent* e_event = new MIEvent();
    b_event->_event_type = CONTROL_PURE_TOUCHES_EVENT;
    e_event->_event_type = CONTROL_PURE_TOUCHES_EVENT;
    
    MITouch* touch = new MITouch();
    touch->phase = MITouchPhaseBegan;
    touch->point = MGPointMake(rect.width*0.5+rect.x, rect.height*0.5+rect.y);
    //touch->maintained_view = object_view;
    b_event->touches_in_event.insert(touch);
    GuiContainer::container->preserveEvent(b_event);
    //============================================//
    if (interval_time > 0)
        MGSleep(interval_time);
    //============================================//
    touch = new MITouch();
    touch->phase = MITouchPhaseEnded;
    touch->point = MGPointMake(rect.width*0.5+rect.x, rect.height*0.5+rect.y);
    //touch->maintained_view = object_view;
    e_event->touches_in_event.insert(touch);
    GuiContainer::container->preserveEvent(e_event);
    
    return ret;
}

string MIAutoTap::actionScript()
{
    string ret = "";
    for (auto it = object_chain.begin(); it != object_chain.end(); ++it) {
        ret.append(it->class_name);
        if (it->index > 0) {
            ret.append("[");
            ret.append(Utility::BuiltInTypeToString(it->index));
            ret.append("]");
        }
        ret.append(".");
    }
    ret.append("tap");
    if (retry_num > 0) {
        ret.append(" retry "+Utility::BuiltInTypeToString(retry_num));
    }
    return ret;
}

#pragma mark - MIAutoHide -

MIAutoHide::MIAutoHide()
{
    
}

MIAutoHide::~MIAutoHide()
{
    
}

bool MIAutoHide::execute()
{
    bool ret = false;
    MIView* object_view = objectView();
    if (object_view != NULL) {
        ret = true;
    } else {
        return ret;
    }
    
    object_view->hide();
    
    return ret;
}

string MIAutoHide::actionScript()
{
    string ret = "";
    for (auto it = object_chain.begin(); it != object_chain.end(); ++it) {
        ret.append(it->class_name);
        if (it->index > 0) {
            ret.append("[");
            ret.append(Utility::BuiltInTypeToString(it->index));
            ret.append("]");
        }
        ret.append(".");
    }
    ret.append("hide");
    if (retry_num > 0) {
        ret.append(" retry "+Utility::BuiltInTypeToString(retry_num));
    }
    return ret;
}

#pragma mark - MIAutoShow -

MIAutoShow::MIAutoShow()
{
    
}

MIAutoShow::~MIAutoShow()
{
    
}

bool MIAutoShow::execute()
{
    bool ret = false;
    MIView* object_view = objectView();
    if (object_view != NULL) {
        ret = true;
    } else {
        return ret;
    }
    
    object_view->show();
    
    return ret;
}

string MIAutoShow::actionScript()
{
    string ret = "";
    for (auto it = object_chain.begin(); it != object_chain.end(); ++it) {
        ret.append(it->class_name);
        if (it->index > 0) {
            ret.append("[");
            ret.append(Utility::BuiltInTypeToString(it->index));
            ret.append("]");
        }
        ret.append(".");
    }
    ret.append("show");
    if (retry_num > 0) {
        ret.append(" retry "+Utility::BuiltInTypeToString(retry_num));
    }
    return ret;
}

#pragma mark - MIAutoScroll -

MIAutoScroll::MIAutoScroll()
{
    scroll_x = 0;
    scroll_y = 0;
    interval_time = 0;
    init_point = MGIntPointMake(0, 0);
}

MIAutoScroll::MIAutoScroll(int x, int y):scroll_x(x),scroll_y(y)
{
    
}

MIAutoScroll::~MIAutoScroll()
{
    
}

bool MIAutoScroll::execute()
{
    bool ret = false;
    MIView* object_view = objectView();
    if (object_view != NULL) {
        ret = true;
    } else {
        return ret;
    }
    
    MGRect rect = object_view->getBounds();
    object_view->transAndScaleClipRect(rect);
    
    MIEvent* b_event = new MIEvent();
    MIEvent* m_event = new MIEvent();
    MIEvent* e_event = new MIEvent();
    b_event->_event_type = CONTROL_PURE_TOUCHES_EVENT;
    m_event->_event_type = CONTROL_PURE_TOUCHES_EVENT;
    e_event->_event_type = CONTROL_PURE_TOUCHES_EVENT;
    
    MITouch* touch = new MITouch();
    touch->phase = MITouchPhaseBegan;
    touch->point = MGPointMake(rect.width*0.5+rect.x, rect.height*0.5+rect.y);
    //touch->maintained_view = object_view;
    b_event->touches_in_event.insert(touch);
    GuiContainer::container->preserveEvent(b_event);
    //============================================//
    if (interval_time > 0) {
        int_32 t = interval_time / 200;
        int_32 extra = interval_time % 200;
        if (extra == 0) {
            t--;
        }
        int_32 time = interval_time;
        for (int_32 idx = 0; idx < t; ++idx) {
            MGSleep(200);
            time -= 200;
            touch = new MITouch();
            touch->phase = MITouchPhaseMoved;
            touch->point = MGPointMake(rect.width*0.5+rect.x+(scroll_x/(t+1)), rect.height*0.5+rect.y+(scroll_y/(t+1)));
            //touch->maintained_view = object_view;
            m_event->touches_in_event.insert(touch);
            GuiContainer::container->preserveEvent(m_event);
        }
        if (time > 0) {
            MGSleep(time);
        }
    } else {
        MGSleep(200);
        touch = new MITouch();
        touch->phase = MITouchPhaseMoved;
        touch->point = MGPointMake(rect.width*0.5+rect.x+scroll_x*0.5, rect.height*0.5+rect.y+scroll_y*0.5);
        //touch->maintained_view = object_view;
        m_event->touches_in_event.insert(touch);
        GuiContainer::container->preserveEvent(m_event);
        MGSleep(200);
    }
    //============================================//
    touch = new MITouch();
    touch->phase = MITouchPhaseEnded;
    touch->point = MGPointMake(rect.width*0.5+rect.x+scroll_x, rect.height*0.5+rect.y+scroll_y);
    //touch->maintained_view = object_view;
    e_event->touches_in_event.insert(touch);
    GuiContainer::container->preserveEvent(e_event);
    
    return ret;
}

string MIAutoScroll::actionScript()
{
    string ret = "";
    for (auto it = object_chain.begin(); it != object_chain.end(); ++it) {
        ret.append(it->class_name);
        if (it->index > 0) {
            ret.append("[");
            ret.append(Utility::BuiltInTypeToString(it->index));
            ret.append("]");
        }
        ret.append(".");
    }
    ret.append("scroll(");
    ret.append(Utility::BuiltInTypeToString(scroll_x));
    ret.append(",");
    ret.append(Utility::BuiltInTypeToString(scroll_y));
    ret.append(")");
    if (retry_num > 0) {
        ret.append(" retry "+Utility::BuiltInTypeToString(retry_num));
    }
    return ret;
}

#pragma mark - MIAutoTextIn -

MIAutoTextIn::MIAutoTextIn()
{
    text_in = "";
}

MIAutoTextIn::~MIAutoTextIn()
{
    
}

bool MIAutoTextIn::execute()
{
    bool ret = false;
    MIView* object_view = objectView();
    if (object_view != NULL) {
        ret = true;
    } else {
        return ret;
    }
    
    if (object_view->isObjectOfClass("MITextView"))
    {
        MITextView* text_view = dynamic_cast<MITextView*>(object_view);
        assert(text_view != NULL);
        //text_view->setText(text_in);
//        MGRect rect = object_view->getBounds();
//        object_view->transAndScaleClipRect(rect);
//        
//        MIEvent* b_event = new MIEvent();
//        MIEvent* e_event = new MIEvent();
//        b_event->_event_type = CONTROL_PURE_TOUCHES_EVENT;
//        e_event->_event_type = CONTROL_PURE_TOUCHES_EVENT;
//        
//        MITouch* touch = new MITouch();
//        touch->phase = MITouchPhaseBegan;
//        touch->point = MGPointMake(rect.width*0.5+rect.x, rect.height*0.5+rect.y);
//        touch->maintained_view = object_view;
//        b_event->touches_in_event.insert(touch);
//        //============================================//
//        touch = new MITouch();
//        touch->phase = MITouchPhaseEnded;
//        touch->point = MGPointMake(rect.width*0.5+rect.x, rect.height*0.5+rect.y);
//        touch->maintained_view = object_view;
//        e_event->touches_in_event.insert(touch);
//        
//        GuiContainer::container->preserveEvent(b_event);
//        GuiContainer::container->preserveEvent(e_event);
        
        text_view->setText("");
        MIEvent* event = new MIEvent();
        MGTextContent* content = new MGTextContent();
        content->text_content = text_in;
        event->_event_type = KEYBOARD_INPUT;
        event->_content = content;
        event->_responder = text_view;
        GuiContainer::container->preserveEvent(event);
    } else if (object_view->isObjectOfClass("MITextField"))
    {
        MITextField* text_field = dynamic_cast<MITextField*>(object_view);
        assert(text_field != NULL);
        //text_field->setText(text_in);
//        MGRect rect = object_view->getBounds();
//        object_view->transAndScaleClipRect(rect);
//        
//        MIEvent* b_event = new MIEvent();
//        MIEvent* e_event = new MIEvent();
//        b_event->_event_type = CONTROL_PURE_TOUCHES_EVENT;
//        e_event->_event_type = CONTROL_PURE_TOUCHES_EVENT;
//        
//        MITouch* touch = new MITouch();
//        touch->phase = MITouchPhaseBegan;
//        touch->point = MGPointMake(rect.width*0.5+rect.x, rect.height*0.5+rect.y);
//        touch->maintained_view = object_view;
//        b_event->touches_in_event.insert(touch);
//        //============================================//
//        touch = new MITouch();
//        touch->phase = MITouchPhaseEnded;
//        touch->point = MGPointMake(rect.width*0.5+rect.x, rect.height*0.5+rect.y);
//        touch->maintained_view = object_view;
//        e_event->touches_in_event.insert(touch);
//        
//        GuiContainer::container->preserveEvent(b_event);
//        GuiContainer::container->preserveEvent(e_event);
        
        text_field->setText("");
        MIEvent* event = new MIEvent();
        MGTextContent* content = new MGTextContent();
        content->text_content = text_in;
        event->_event_type = KEYBOARD_INPUT;
        event->_content = content;
        event->_responder = text_field;
        GuiContainer::container->preserveEvent(event);
    } else {
        ret = false;
    }
    
    return ret;
}

string MIAutoTextIn::actionScript()
{
    string ret = "";
    for (auto it = object_chain.begin(); it != object_chain.end(); ++it) {
        ret.append(it->class_name);
        if (it->index > 0) {
            ret.append("[");
            ret.append(Utility::BuiltInTypeToString(it->index));
            ret.append("]");
        }
        ret.append(".");
    }
    ret.append("text_in(");
    ret.append(text_in);
    ret.append(")");
    if (retry_num > 0) {
        ret.append(" retry "+Utility::BuiltInTypeToString(retry_num));
    }
    return ret;
}

#pragma mark - MIAutoWait -

MIAutoWait::MIAutoWait()
{
    wait_time = 0;
}

MIAutoWait::~MIAutoWait()
{
    
}

bool MIAutoWait::execute()
{
    bool ret = false;
    if (wait_time < 0) {
        return ret;
    }
    MGSleep(wait_time);
    ret = true;
    return ret;
}

string MIAutoWait::actionScript()
{
    string ret = "";
    ret.append("wait(");
    ret.append(Utility::BuiltInTypeToString(wait_time));
    ret.append(")");
    if (retry_num > 0) {
        ret.append(" retry "+Utility::BuiltInTypeToString(retry_num));
    }
    return ret;
}

#pragma mark - MIAutoUITree -

MIAutoUITree::MIAutoUITree()
{
    ui_tree = "";
}

MIAutoUITree::~MIAutoUITree()
{
    
}

bool MIAutoUITree::execute()
{
    bool ret = false;
    MIView* object_view = objectView();
    if (object_view != NULL) {
        ret = true;
    } else {
        return ret;
    }
    
    MIViewTreeInfo* info = object_view->viewTree();
    ui_tree = info->toUITree();
    delete_and_set_null(info);
    
    return ret;
}

string MIAutoUITree::actionScript()
{
    string ret = "";
    for (auto it = object_chain.begin(); it != object_chain.end(); ++it) {
        ret.append(it->class_name);
        if (it->index > 0) {
            ret.append("[");
            ret.append(Utility::BuiltInTypeToString(it->index));
            ret.append("]");
        }
        ret.append(".");
    }
    ret.append("ui_tree");
    if (retry_num > 0) {
        ret.append(" retry "+Utility::BuiltInTypeToString(retry_num));
    }
    return ret;
}

#pragma mark - MIAutoUITreeJson -

MIAutoUITreeJson::MIAutoUITreeJson()
{
    ui_tree = "";
}

MIAutoUITreeJson::~MIAutoUITreeJson()
{
    
}

void toUITreeJsonStr(MIViewTreeInfo* info, string& str)
{
    if (info == NULL) {
        return;
    }
    str.append("\"background\": \""+info->background+"\",");
    str.append("\"bound\": \""+Utility::BuiltInTypeToString(info->bound.x)+","+Utility::BuiltInTypeToString(info->bound.y)+","+Utility::BuiltInTypeToString(info->bound.width)+","+Utility::BuiltInTypeToString(info->bound.height)+"\",");
    str.append("\"class_name\": \""+info->class_name+"\",");
    str.append("\"place_holder\": \""+info->extra_content+"\",");
    str.append("\"sub_transition\": \""+Utility::BuiltInTypeToString(info->sub_transition.axis_x)+","+Utility::BuiltInTypeToString(info->sub_transition.axis_y)+","+Utility::BuiltInTypeToString(info->sub_transition.axis_z)+"\",");
    if (info->subviews.size() > 0)
    {
        str.append("\"sub_views\": [");
        for (auto it = info->subviews.begin(); it != info->subviews.end(); ++it) {
            if (it != info->subviews.begin()) {
                str.append(",");
            }
            string sub_str = "{";
            toUITreeJsonStr(*it, sub_str);
            sub_str.append("}");
            str.append(sub_str);
        }
        str.append("],");
    }
    str.append("\"text\": \""+info->text+"\",");
    str.append("\"transition\": \""+Utility::BuiltInTypeToString(info->transition.axis_x)+","+Utility::BuiltInTypeToString(info->transition.axis_y)+","+Utility::BuiltInTypeToString(info->transition.axis_z)+"\"");
}

void toUITreeJson(MIViewTreeInfo* info, Json::Value& value)
{
    if (info == NULL) {
        return;
    }
    value["class_name"] = info->class_name;
    value["background"] = info->background;
    value["text"] = info->text;
    value["place_holder"] = info->extra_content;
    value["bound"] = Utility::BuiltInTypeToString(info->bound.x)+","+Utility::BuiltInTypeToString(info->bound.y)+","+Utility::BuiltInTypeToString(info->bound.width)+","+Utility::BuiltInTypeToString(info->bound.height);
    value["transition"] = Utility::BuiltInTypeToString(info->transition.axis_x)+","+Utility::BuiltInTypeToString(info->transition.axis_y)+","+Utility::BuiltInTypeToString(info->transition.axis_z);
    value["sub_transition"] = Utility::BuiltInTypeToString(info->sub_transition.axis_x)+","+Utility::BuiltInTypeToString(info->sub_transition.axis_y)+","+Utility::BuiltInTypeToString(info->sub_transition.axis_z);
    
    for (auto it = info->subviews.begin(); it != info->subviews.end(); ++it) {
        Json::Value sub_value;
        toUITreeJson(*it, sub_value);
        value["sub_views"].append(sub_value);
    }
}

bool MIAutoUITreeJson::execute()
{
    bool ret = false;
    MIView* object_view = objectView();
    if (object_view != NULL) {
        ret = true;
    } else {
        return ret;
    }
    
    MIViewTreeInfo* info = object_view->viewTree();
//    Json::FastWriter writer;
//    Json::Value json;
//    toUITreeJson(info, json);
//    ui_tree = writer.write(json);
    /* 使用Json写复杂结构会导致效率过低，改为直接拼接字符串 */
    ui_tree = "{";
    toUITreeJsonStr(info, ui_tree);
    ui_tree.append("}");
    
    delete_and_set_null(info);
    
    return ret;
}

string MIAutoUITreeJson::actionScript()
{
    string ret = "";
    for (auto it = object_chain.begin(); it != object_chain.end(); ++it) {
        ret.append(it->class_name);
        if (it->index > 0) {
            ret.append("[");
            ret.append(Utility::BuiltInTypeToString(it->index));
            ret.append("]");
        }
        ret.append(".");
    }
    ret.append("ui_tree_json");
    if (retry_num > 0) {
        ret.append(" retry "+Utility::BuiltInTypeToString(retry_num));
    }
    return ret;
}

#pragma mark - MIAutoUIFind -

MIAutoUIFind::MIAutoUIFind()
{
    ui_pathes = "";
}

MIAutoUIFind::~MIAutoUIFind()
{
    
}

bool MIAutoUIFind::execute()
{
    bool ret = false;
    deque<string> views = findViews();
    if (views.size() != 0) {
        ret = true;
    } else {
        return ret;
    }
    
    for (auto it = views.begin(); it != views.end(); ++it) {
        ui_pathes.append(*it+"\n");
    }
    
    return ret;
}

string MIAutoUIFind::actionScript()
{
    string ret = "";
    for (auto it = object_chain.begin(); it != object_chain.end(); ++it) {
        ret.append(it->class_name);
        if (it->index > 0) {
            ret.append("[");
            ret.append(Utility::BuiltInTypeToString(it->index));
            ret.append("]");
        }
        ret.append(".");
    }
    ret.append("ui_find");
    if (retry_num > 0) {
        ret.append(" retry "+Utility::BuiltInTypeToString(retry_num));
    }
    return ret;
}

#pragma mark - MIAutoTextOut -

MIAutoTextOut::MIAutoTextOut()
{
    text_out = "";
}

MIAutoTextOut::~MIAutoTextOut()
{
    
}

bool MIAutoTextOut::execute()
{
    bool ret = false;
    MIView* object_view = objectView();
    if (object_view != NULL) {
        ret = true;
    } else {
        return ret;
    }
    
    if (object_view->isObjectOfClass("MITextView"))
    {
        MITextView* text_view = dynamic_cast<MITextView*>(object_view);
        assert(text_view != NULL);
        text_out = text_view->getText();
    } else if (object_view->isObjectOfClass("MITextField"))
    {
        MITextField* text_field = dynamic_cast<MITextField*>(object_view);
        assert(text_field != NULL);
        text_out = text_field->getText();
    } else if (object_view->isObjectOfClass("MIButton"))
    {
        MIButton* btn = dynamic_cast<MIButton*>(object_view);
        assert(btn != NULL);
        text_out = btn->titleLabel()->getText();
    } else if (object_view->isObjectOfClass("MILabel"))
    {
        MILabel* label = dynamic_cast<MILabel*>(object_view);
        assert(label != NULL);
        text_out = label->getText();
    } else {
        ret = false;
    }
    
    return ret;
}

string MIAutoTextOut::actionScript()
{
    string ret = "";
    for (auto it = object_chain.begin(); it != object_chain.end(); ++it) {
        ret.append(it->class_name);
        if (it->index > 0) {
            ret.append("[");
            ret.append(Utility::BuiltInTypeToString(it->index));
            ret.append("]");
        }
        ret.append(".");
    }
    ret.append("text_out");
    if (retry_num > 0) {
        ret.append(" retry "+Utility::BuiltInTypeToString(retry_num));
    }
    return ret;
}

#pragma mark - MIAutoExist -

MIAutoExist::MIAutoExist()
{
    
}

MIAutoExist::~MIAutoExist()
{
    
}

bool MIAutoExist::execute()
{
    bool ret = false;
    MIView* object_view = objectView();
    if (object_view != NULL) {
        ret = true;
    }
    
    return ret;
}

string MIAutoExist::actionScript()
{
    string ret = "";
    for (auto it = object_chain.begin(); it != object_chain.end(); ++it) {
        ret.append(it->class_name);
        if (it->index > 0) {
            ret.append("[");
            ret.append(Utility::BuiltInTypeToString(it->index));
            ret.append("]");
        }
        ret.append(".");
    }
    ret.append("exist");
    if (retry_num > 0) {
        ret.append(" retry "+Utility::BuiltInTypeToString(retry_num));
    }
    return ret;
}

#pragma mark - MIAutoEnabled -

MIAutoEnabled::MIAutoEnabled()
{
    
}

MIAutoEnabled::~MIAutoEnabled()
{
    
}

bool MIAutoEnabled::execute()
{
    bool ret = true;
    MIView* object_view = objectView();
    if (object_view == NULL) {
        ret = false;
    } else {
        while (object_view != NULL) {
            if (object_view->_view_flags.user_interaction_enabled == 0) {
                ret = false;
                break;
            }
            object_view = object_view->parent();
        }
    }
    
    return ret;
}

string MIAutoEnabled::actionScript()
{
    string ret = "";
    for (auto it = object_chain.begin(); it != object_chain.end(); ++it) {
        ret.append(it->class_name);
        if (it->index > 0) {
            ret.append("[");
            ret.append(Utility::BuiltInTypeToString(it->index));
            ret.append("]");
        }
        ret.append(".");
    }
    ret.append("enabled");
    if (retry_num > 0) {
        ret.append(" retry "+Utility::BuiltInTypeToString(retry_num));
    }
    return ret;
}

#pragma mark - MIAutoLock -

MIAutoLock::MIAutoLock()
{
    
}

MIAutoLock::~MIAutoLock()
{
    
}

bool MIAutoLock::execute()
{
    bool ret = false;
    if (GuiContainer::container == NULL) {
        return ret;
    }
    GuiContainer::lockScreenWhenIdle();
    ret = true;
    
    return ret;
}

string MIAutoLock::actionScript()
{
    string ret = "";
    
    ret.append("lock");
    if (retry_num > 0) {
        ret.append(" retry "+Utility::BuiltInTypeToString(retry_num));
    }
    return ret;
}

#pragma mark - MIAutoUnlock -

MIAutoUnlock::MIAutoUnlock()
{
    
}

MIAutoUnlock::~MIAutoUnlock()
{
    
}

bool MIAutoUnlock::execute()
{
    bool ret = false;
    if (GuiContainer::container == NULL) {
        return ret;
    }
    GuiContainer::unlockScreenWhenIdle();
    ret = true;
    
    return ret;
}

string MIAutoUnlock::actionScript()
{
    string ret = "";
    
    ret.append("unlock");
    if (retry_num > 0) {
        ret.append(" retry "+Utility::BuiltInTypeToString(retry_num));
    }
    return ret;
}

#pragma mark - MIAutoURL -

MIAutoURL::MIAutoURL()
{
    url = "";
}

MIAutoURL::~MIAutoURL()
{
    
}

bool MIAutoURL::execute()
{
    bool ret = false;
    url = MGWebViewManager::current_url;
    
    if (url.size() > 0) {
        ret = true;
    }
    
    return ret;
}

string MIAutoURL::actionScript()
{
    string ret = "";
    
    ret.append("h5_url");
    if (retry_num > 0) {
        ret.append(" retry "+Utility::BuiltInTypeToString(retry_num));
    }
    return ret;
}

#pragma mark - MIAutoKeyboardShown -

MIAutoKeyboardShown::MIAutoKeyboardShown()
{
    shown = "";
}

MIAutoKeyboardShown::~MIAutoKeyboardShown()
{
    
}

bool MIAutoKeyboardShown::execute()
{
    bool ret = true;
    
    if (GuiContainer::keyboard_shown) {
        shown = "true";
    } else {
        shown = "false";
    }
    
    return ret;
}

string MIAutoKeyboardShown::actionScript()
{
    string ret = "";
    
    ret.append("keyboard_shown");
    if (retry_num > 0) {
        ret.append(" retry "+Utility::BuiltInTypeToString(retry_num));
    }
    return ret;
}

#pragma mark - MIAutoInfo -

MIAutoInfo::MIAutoInfo()
{
    info = "";
}

MIAutoInfo::~MIAutoInfo()
{
    
}

bool MIAutoInfo::execute()
{
    string title = "\r\n**********************************************************************";
    title +=       "\r\n* OS : "+MGDevice::getDeviceOSType()+" "+MGDevice::getDeviceOSVersion()+"\r\n"+
                       "* Server : MIAutomation 0.1.1\r\n"+
                       "* Support Script: info ui_tree ui_tree_json ui_find tap() scroll() show hide\r\n"+
                       "* text_in() text_out * *.text() wait() exist enabled lock unlock h5_url keyboard_shown\r\n"+
                       "* screen_shot";
    title +=       "\r\n**********************************************************************";
    info = title;
    return true;
}

string MIAutoInfo::actionScript()
{
    string ret = "";
    ret.append("info");
    if (retry_num > 0) {
        ret.append(" retry "+Utility::BuiltInTypeToString(retry_num));
    }
    return ret;
}

#pragma mark - MIAutoScreenShot -

class AutomationScreenShotView : public MIView
{
public:
    AutomationScreenShotView()
    {
        start_capture = false;
        data = NULL;
    }
    
    virtual ~AutomationScreenShotView()
    {
        delete_and_set_null(data);
    }
    
    virtual void drawself()
    {
        MIView::drawself();
        if(start_capture)
        {
            screen_width = MGDevice::getScreenWidth();
            screen_height = MGDevice::getScreenHeight();
            
            int line_width = screen_width*4;
            int max_size = line_width*screen_height;
            data = new unsigned char[max_size];
            glReadPixels(0, 0, screen_width, screen_height, GL_RGBA, GL_UNSIGNED_BYTE, data);
            start_capture = false;
        }
    }
    
public:
    bool start_capture;
    int screen_width;
    int screen_height;
    unsigned char* data;
};

MIAutoScreenShot::MIAutoScreenShot()
{
    img.clear();
}

MIAutoScreenShot::~MIAutoScreenShot()
{
    
}

bool MIAutoScreenShot::execute()
{
    AutomationScreenShotView* shot_view = new AutomationScreenShotView();
    shot_view->autoClean();
    shot_view->initWithFrame(MGRectMake(0, 0, MGDevice::getScreenWidth(), MGDevice::getScreenHeight()));
    GuiContainer::container->gui_root->view->addSubview(shot_view);
    shot_view->start_capture = true;
    
    int frame_time = 10;
    bool shot_success = false;
    while(frame_time-- > 0)
    {
        if (shot_view->start_capture == false) {
            shot_success = true;
            break;
        }
        MGSleep(200);
    }
    if (shot_success == false) {
        GuiContainer::container->gui_root->view->removeSubview(shot_view);
        return shot_success;
    }
    
    int capture_width = shot_view->screen_width / 2;
    int capture_height = shot_view->screen_height / 2;
    
    img.push_back(capture_width);
    img.push_back(capture_width >> 8);
    img.push_back(capture_height);
    img.push_back(capture_height >> 8);
    
    int line_width = shot_view->screen_width*4;
    img.reserve(4+capture_width*3*capture_height);
    unsigned char* data = shot_view->data;
    
    for (int idy = 0; idy < capture_height; ++idy)
    {
        for (int idx = 0; idx < capture_width; ++idx) {
            int pos = idy * 2 * line_width + (idx * 8);
            unsigned int r = data[pos]+data[pos+4]+data[pos+line_width]+data[pos+line_width+4];
            unsigned int g = data[pos+1]+data[pos+5]+data[pos+1+line_width]+data[pos+line_width+5];
            unsigned int b = data[pos+2]+data[pos+6]+data[pos+2+line_width]+data[pos+line_width+6];
            //MGLogD("pos %d, %d: %d, %d, %d\r\n", idx, idy, r, g, b);
            img.push_back(r/4);
            img.push_back(g/4);
            img.push_back(b/4);
        }
    }
    
    GuiContainer::container->gui_root->view->removeSubview(shot_view);
    
    return true;
}

string MIAutoScreenShot::actionScript()
{
    string ret = "";
    ret.append("screen_shot");
    if (retry_num > 0) {
        ret.append(" retry "+Utility::BuiltInTypeToString(retry_num));
    }
    return ret;
}

#pragma mark - MIAutoUnknown -

MIAutoUnknown::MIAutoUnknown()
{
    
}

MIAutoUnknown::~MIAutoUnknown()
{
    
}

bool MIAutoUnknown::execute()
{
    return true;
}

string MIAutoUnknown::actionScript()
{
    string ret = "";
    ret.append("unknown script command.");
    return ret;
}
