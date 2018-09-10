//
//  MIEvent.cpp
//  Gui
//
//  Created by Wang ZhiPeng on 11-5-22.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#include "MIEvent.h"
#include "../../System/Shared/MGLog.h"

namespace z11 {
    MIEvent::MIEvent()
    {
        _event_type = 0;
        _content = NULL;
        _responder = NULL;
        touches_in_event.clear();
    }
    MIEvent::MIEvent(const MIEvent& event)
    {
        _event_type = event._event_type;
        _responder = event._responder;
        _content = event._content;
        _handler = event._handler;
    }
    MIEvent::~MIEvent()
    {
        if (_content != NULL) {
            delete _content;
            _content = NULL;
        }
        if (touches_in_event.size() > 0) {
            for(TouchesInEventIt it = touches_in_event.begin(); it != touches_in_event.end(); ++it)
            {
                if ((*it) != NULL) {
                    delete (*it);
                }
            }
            touches_in_event.clear();
        }
    }
    bool MIEvent::isSysCallbackEvent()
    {
        return _event_type > SYS_CALLBACK_EVENT && _event_type < SYS_CALLBACK_EVENT_END;
    }
    bool MIEvent::isControlRetransTouchesEvent()
    {
        return _event_type >= CONTROL_RETRANS_PURE_TOUCHES_EVENT && _event_type <= CONTROL_PURE_TOUCHES_DELAY_EVENT;
    }
    
    bool MIEvent::isImmediatellyHandleEvent()
    {
        bool ret = false;
        switch (_event_type) {
            case INVALIDATE_A_TIMER_EVENT:
            case GET_VIDEO_PREVIEW_EVENT:
			case GET_AUDIO_DURATION_EVENT:
            case UIWEBVIEW_CAN_BACK_EVENT:
            case UIWEBVIEW_CAN_FORWARD_EVENT:
            case FOREIGNER_SDK_API_IMMEDIATELY_CALL_EVENT:
				ret = true;
				break;
            default:
                ret = isControlRetransTouchesEvent();
                break;
        }
        return ret;
    }
    
    //MGTextContent//
    MGTextContent::MGTextContent()
    {
        
    }
    MGTextContent::~MGTextContent()
    {
        
    }
    
    //MGTextContent//
    MGRemoteAppContent::MGRemoteAppContent()
    {
        
    }
    MGRemoteAppContent::~MGRemoteAppContent()
    {
        
    }
    
    //MGKeyDownContent//
    MGKeyDownContent::MGKeyDownContent()
    {
        
    }
    MGKeyDownContent::~MGKeyDownContent()
    {
        
    }
    
    //MGPointContent//
    MGPointContent::MGPointContent()
    {
        
    }
    MGPointContent::~MGPointContent()
    {
        
    }
    
    //MGIntervalContent//
    MGIntervalContent::MGIntervalContent()
    {
        interval = 0.0f;
    }
    MGIntervalContent::~MGIntervalContent()
    {
        
    }
    
    //MGWebViewContent//
    MGWebViewContent::MGWebViewContent()
    {
        
    }
    MGWebViewContent::~MGWebViewContent()
    {
        
    }
}
