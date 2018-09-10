//
//  MIEventQueue.h
//  Gui
//
//  Created by Wang ZhiPeng on 11-5-24.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#pragma once

#include "../Base/MIObject.h"
#include "../../System/Shared/Syncable.h"
#include <deque>

namespace z11
{
class MIEvent;
    class MIEventQueue : public MIObject, Syncable
    {
    public:
        MIEventQueue();
        virtual ~MIEventQueue();
    public:
        void eventEnque(MIEvent*);
        MIEvent* eventDeque();
        void clearEventWithObject(MGObject* object);
    protected:
        //事件队列//
        std::deque<MIEvent*> m_event_queue;
    };
    typedef std::deque<MIEvent*>::iterator MIEVentItrator;
} //namespace z11