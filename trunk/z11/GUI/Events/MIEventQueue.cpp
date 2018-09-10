//
//  MIEventQueue.cpp
//  Gui
//
//  Created by Wang ZhiPeng on 11-5-24.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#include "MIEventQueue.h"
#include "MIEvent.h"
#include <assert.h>

namespace z11
{
    MIEventQueue::MIEventQueue()
    {
        
    }
    MIEventQueue::~MIEventQueue()
    {
        AUTO_LOCK_THIS;
        MIEVentItrator it;
		std::deque<MIEvent*> tmp_que;
        for (it = m_event_queue.begin(); it != m_event_queue.end(); ++it) {
            assert((*it) != NULL);
			tmp_que.push_back(*it);
        }
        m_event_queue.clear();

		for (it = tmp_que.begin(); it != tmp_que.end(); ++it) {
            assert((*it) != NULL);
			delete(*it);
        }
    }
    void MIEventQueue::eventEnque(MIEvent *event)
    {
        AUTO_LOCK_THIS;
        if(event == NULL)
        {
            return;
        }
        m_event_queue.push_back(event);
    }
    MIEvent* MIEventQueue::eventDeque()
    {
        AUTO_LOCK_THIS;
        MIEvent* ret = NULL;
        do{
            if (m_event_queue.size() <= 0) {
                break; //while(0)
            }
            ret = m_event_queue.front();
            m_event_queue.pop_front();
        }while(0);
        return ret;
    }
    void MIEventQueue::clearEventWithObject(MGObject* object)
    {
		if (object == this)
			return;
        AUTO_LOCK_THIS;
        MIEVentItrator it;
        for (it = m_event_queue.begin(); it != m_event_queue.end(); ++it) {
            assert((*it) != NULL);
            if (object == (MGObject*)(*it)->_responder) {
                (*it)->_responder = NULL;
            }
            if (object == (*it)->_handler.sender) {
                (*it)->_handler.sender = NULL;
            }
            if (object == (*it)->_content) {
                (*it)->_content = NULL;
            }
            for (TouchesInEventIt t_it = (*it)->touches_in_event.begin();
                 t_it != (*it)->touches_in_event.end();) {
                if ((MGObject*)(*t_it)->maintained_view == object) {
                    (*it)->touches_in_event.erase(t_it++);
                }else {
                    ++t_it;
                }
            }
        }
    }
}