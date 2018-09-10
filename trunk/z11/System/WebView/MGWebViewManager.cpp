//
//  MGWebViewManager.cpp
//  System
//
//  Created by King Asdy on 11-10-19.
//  Copyright (c) 2011年 Zhejiang University. All rights reserved.
//

#include "MGWebViewManager.h"
#include "../../GUI/Events/MIEventDefine.h"
#include "../../GUI/Events/MIEvent.h"
#include "../../GUI/GuiContainer/GuiContainer.h"
#include "../Shared/MGLog.h"

namespace z11 {
    set<MGWebViewManager*> MGWebViewManager::webview_pool;
    Syncable MGWebViewManager::webview_pool_locker;
    map<MGWebViewManager*, MGWebViewManagerDelegate*> MGWebViewManager::delegate_pool;
    bool MGWebViewManager::can_go_forward = false;
    bool MGWebViewManager::can_go_back = false;
    string MGWebViewManager::current_url = "";
    MGWebViewManager::MGWebViewManager()
    {
        zoom_enabled = true;
        pan_enabled = true;
        is_post = false;
        post_body = "";
        is_active = false;
    }
    
    MGWebViewManager::~MGWebViewManager()
    {
        webview_pool_locker.lock();
        WebViewPoolIter view_it = webview_pool.find(this);
        if(view_it != webview_pool.end())
            webview_pool.erase(view_it);
        WebDelegatePoolIter it = delegate_pool.find(this);
        if(it != delegate_pool.end())
            delegate_pool.erase(it);
        webview_pool_locker.unlock();
    }
    void MGWebViewManager::setDelegate(MGWebViewManagerDelegate* dlg)
    {
        webview_pool_locker.lock();
        delegate_pool[this] = dlg;
        webview_pool_locker.unlock();
    }
    void MGWebViewManager::openWebViewWithURL(const string& url)
    {
        openWebViewWithURLInRect(url, MGRectMake(0, 0, (float)MGDevice::getScreenWidth(), (float)MGDevice::getScreenHeight()));
    }
    void MGWebViewManager::openWebViewWithURLInRect(const string &url, MGRect rect)
    {
        webview_pool_locker.lock();
        MIEvent* event = new MIEvent;
        event->_event_type = UIWEBVIEW_OPEN_URL_EVENT;
        event->_responder = NULL;
        MGWebViewContent* content = new MGWebViewContent();
        content->rect = rect;
        content->url_content = url;
        content->is_post = is_post;
        content->post_body = post_body;
        content->zoom_enabled = zoom_enabled;
        content->pan_enabled = pan_enabled;
        event->_content = content;
        GuiContainer::container->preserveEvent(event);
        is_active = true;
        webview_pool.insert(this);
        current_url = url;
        webview_pool_locker.unlock();
    }
    void MGWebViewManager::closeWebView()
    {
        webview_pool_locker.lock();
        MIEvent* event = new MIEvent;
        event->_event_type = UIWEBVIEW_CLOSE_VIEW_EVENT;
        event->_responder = NULL;
        GuiContainer::container->preserveEvent(event);
        is_active = false;
        webview_pool_locker.unlock();
    }
    
    void MGWebViewManager::showWebView()
    {
        webview_pool_locker.lock();
        MIEvent* event = new MIEvent();
        event->_event_type = UIWEBVIEW_SHOW_VIEW_EVENT;
        event->_responder = NULL;
        GuiContainer::container->preserveEvent(event);
        is_active = true;
        webview_pool_locker.unlock();
    }
    
    bool MGWebViewManager::canGoBack()
    {
        MIEvent* event = new MIEvent();
        event->_event_type = UIWEBVIEW_CAN_BACK_EVENT;
        MGWebViewManager* web_manager = new MGWebViewManager();
        *web_manager = *this;
        event->_content = web_manager;
        GuiContainer::container->preserveEvent(event);
        
        return can_go_back;
    }
    bool MGWebViewManager::canGoForward()
    {
        MIEvent* event = new MIEvent();
        event->_event_type = UIWEBVIEW_CAN_FORWARD_EVENT;
        MGWebViewManager* web_manager = new MGWebViewManager();
        *web_manager = *this;
        event->_content = web_manager;
        GuiContainer::container->preserveEvent(event);
        
        return can_go_forward;
    }
    void MGWebViewManager::setCanGoBack(bool is_back)
    {
        can_go_back = is_back;
    }
    void MGWebViewManager::setCanGoForward(bool is_forward)
    {
        can_go_forward = is_forward;
    }
    
    MGWebViewManagerDelegate* MGWebViewManager::pullDelegate()
    {
        webview_pool_locker.lock();
        WebDelegatePoolIter it = delegate_pool.find(this);
        if(it == delegate_pool.end())
            return NULL;
        return delegate_pool[this];
    }
    
    void MGWebViewManager::pushDelegate()
    {
        webview_pool_locker.unlock();
    }
    
    bool MGWebViewManager::shouldStartLoadWithRequest(const string& request)
    {
		bool ret = true;
        webview_pool_locker.lock();
        WebDelegatePoolIter it;
        for(it = delegate_pool.begin(); it != delegate_pool.end(); ++it)
        {
            if (it->first->is_active == false) {
                continue;
            }
            MGWebViewManagerDelegate* delegate = it->second;
            if(delegate != NULL)
                ret = delegate->shouldStartLoadWithRequest(request);
        }
        current_url = request;
        webview_pool_locker.unlock();
        return ret;
    }
    void MGWebViewManager::didStartLoad()
    {
        webview_pool_locker.lock();
        WebDelegatePoolIter it;
        for(it = delegate_pool.begin(); it != delegate_pool.end(); ++it)
        {
            if (it->first->is_active == false) {
                continue;
            }
            MGWebViewManagerDelegate* delegate = it->second;
            if(delegate != NULL)
                delegate->didStartLoad();
        }
        webview_pool_locker.unlock();
    }
    void MGWebViewManager::didFinishLoad()
    {
        webview_pool_locker.lock();
        WebDelegatePoolIter it;
        for(it = delegate_pool.begin(); it != delegate_pool.end(); ++it)
        {
            if (it->first->is_active == false) {
                continue;
            }
            MGWebViewManagerDelegate* delegate = it->second;
            if(delegate != NULL)
                delegate->didFinishLoad();
        }
        webview_pool_locker.unlock();
    }
    void MGWebViewManager::didFailLoadWithError(int err_code, const string& description)
    {
        webview_pool_locker.lock();
        WebDelegatePoolIter it;
        for(it = delegate_pool.begin(); it != delegate_pool.end(); ++it)
        {
            if (it->first->is_active == false) {
                continue;
            }
            MGWebViewManagerDelegate* delegate = it->second;
            if(delegate != NULL)
                delegate->didFailLoadWithError(err_code,description);
        }
        webview_pool_locker.unlock();
    }
    void MGWebViewManager::didReceivedTitle(const string& title)
	{
		webview_pool_locker.lock();
		WebDelegatePoolIter it;
		for(it = delegate_pool.begin(); it != delegate_pool.end(); ++it)
		{
            if (it->first->is_active == false) {
                continue;
            }
			MGWebViewManagerDelegate* delegate = it->second;
			if(delegate != NULL)
				delegate->didReceivedTitle(title);
		}
		webview_pool_locker.unlock();
	}
} //namespace z11
