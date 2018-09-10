//
//  MGWebViewManager.h
//  System
//
//  Created by King Asdy on 11-10-19.
//  Copyright (c) 2011年 Zhejiang University. All rights reserved.
//

#pragma once

#include "../../CommonBase/MGObject.h"
#include "../Shared/Syncable.h"
#include "../../CommonBase/CommonDef.h"
#include "../../CommonBase/MGStructure.h"
#include <set>
#include <map>
#include <string>

using namespace std;

namespace z11 {

    class MGWebViewManagerDelegate;
    class MGWebViewManager : public MGObject
    {
    public:
        MGWebViewManager();
        virtual ~MGWebViewManager();
    protected:
        static set<MGWebViewManager*> webview_pool;
        static Syncable webview_pool_locker;
    public:
        void openWebViewWithURL(const string& url);
        void openWebViewWithURLInRect(const string& url, MGRect rect);
        void closeWebView();
        void showWebView();
        bool canGoBack();
        bool canGoForward();
        void setDelegate(MGWebViewManagerDelegate* delegate);
        MGWebViewManagerDelegate* pullDelegate();
        void pushDelegate();
        static bool shouldStartLoadWithRequest(const string& request);
        static void didStartLoad();
        static void didFinishLoad();
        static void didFailLoadWithError(int err_code, const string& description);
		static void didReceivedTitle(const string& title);
        
        static void setCanGoBack(bool is_back);
        static void setCanGoForward(bool is_forward);
    protected:
        static map<MGWebViewManager*, MGWebViewManagerDelegate*> delegate_pool;
    public:
        bool zoom_enabled;
        bool pan_enabled;
        bool is_post;
        string post_body;
        bool is_active;
        
        static bool can_go_back;
        static bool can_go_forward;
        static string current_url;
    };
    typedef set<MGWebViewManager*>::iterator WebViewPoolIter;
    typedef map<MGWebViewManager*, MGWebViewManagerDelegate*>::iterator WebDelegatePoolIter;
    MIINTERFACE MGWebViewManagerDelegate
    {
    public:
        virtual bool shouldStartLoadWithRequest(const string& request) = 0;
        virtual void didStartLoad() = 0;
        virtual void didFinishLoad() = 0;
        virtual void didFailLoadWithError(int err_code, const string& description) = 0;
		virtual void didReceivedTitle(const string& title) = 0;
    };
#define MGWEBVIEWMANAGER_DELEGAGE_DECLARE \
bool shouldStartLoadWithRequest(const string& request); \
    virtual void didStartLoad(); \
    virtual void didFinishLoad(); \
    virtual void didFailLoadWithError(int err_code, const string& description); \
	virtual void didReceivedTitle(const string& title);
} //namespace z11
