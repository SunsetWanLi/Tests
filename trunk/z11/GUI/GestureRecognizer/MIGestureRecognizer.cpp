//
//  MIGestureRecognizer.cpp
//  Gui
//
//  Created by King Asdy on 12-3-23.
//  Copyright (c) 2012年 Zhejiang University. All rights reserved.
//

#include "MIGestureRecognizer.h"
#include "../../System/Shared/MGLog.h"
namespace z11 {
	
	MIGestureRecognizer::MIGestureRecognizer():m_view(NULL)
    {
        m_state = MIGestureRecognizerStatePossible;
        m_view = NULL;
        m_failure_requirments.clear();
        m_failure_dependents.clear();
        queriedFailureRequirement = 0;
    }
	MIGestureRecognizer::~MIGestureRecognizer()
	{
        m_targets.clear();
        m_view = NULL;
        clearRequirementsAndDepends();
	}
	void MIGestureRecognizer::initWithTarget(MIResponder *responder,SELHandler action )
	{
		MIResponderSEL target;
		target.responder = responder;
		target.action = action;
		m_targets.push_back(target);
	}
    void MIGestureRecognizer::addTarget(MIResponder* responder, SELHandler action)
    {
        MIResponderSEL target;
		target.responder = responder;
		target.action = action;
		m_targets.push_back(target);
    }
    void MIGestureRecognizer::removeTarget(MIResponder* responder, SELHandler action)
    {
        MIResponderSEL target;
		target.responder = responder;
		target.action = action;
        TargetIter it;
        for (it = m_targets.begin(); it != m_targets.end();) {
            if (*it == target) {
                it = m_targets.erase(it);
            }else
                ++it;
        }
    }

	void MIGestureRecognizer::touchesBegan( const std::set<MITouch*> &touches, const MIEvent* event)
	{
		
	}

	void MIGestureRecognizer::touchesMoved( const std::set<MITouch*> &touches, const MIEvent* event)
	{
		
	}

	void MIGestureRecognizer::touchesEnded( const std::set<MITouch*> &touches, const MIEvent* event)
	{
		
	}
	void MIGestureRecognizer::touchesCancelled(const std::set<MITouch *> & touches, const MIEvent* event)
	{
		
	}
    
    void MIGestureRecognizer::reset()
    {
        
    }
    
    void MIGestureRecognizer::trigger()
    {
        if (m_failure_dependents.size() > 0 && (m_state == MIGestureRecognizerStateRecognized || m_state == MIGestureRecognizerStateBegan)) {
            // 只要有specified gesture，都直接返回，需要在dependent gesture的下个状态来判断是否要trigger //
            queriedFailureRequirement = m_failure_dependents.size();
            return;
        }
        
        for (size_t i=0;i<m_targets.size();++i)
        {
            MIResponder *target_response=m_targets[i].responder;
            SELHandler target_action=m_targets[i].action;
            SELFunc handler=target_action.sel_func;
            (target_response->*handler)(target_action.sender);
        }
        if (state() == MIGestureRecognizerStateRecognized || state() == MIGestureRecognizerStateEnded || state() == MIGestureRecognizerStateCancelled)
            setState(MIGestureRecognizerStatePossible);
    }
    
    void MIGestureRecognizer::requireGestureRecognizerToFail( MIGestureRecognizer *otherGestureRecognizer )
	{
        assert(m_view == otherGestureRecognizer->m_view);
        if (m_view != otherGestureRecognizer->m_view) {
            return;
        }
        m_failure_dependents.insert(otherGestureRecognizer);
        otherGestureRecognizer->m_failure_requirments.insert(this);
	}

	z11::MIGestureRecognizerState MIGestureRecognizer::state()
	{
		return m_state;
	}
    
    void MIGestureRecognizer::setState(MIGestureRecognizerState s)
    {
        if (s != MIGestureRecognizerStateChanged && m_state == s) {
            return;
        }
        m_state = s;
        if (m_state == MIGestureRecognizerStateFailed) {
            for (GestureIter it = m_failure_requirments.begin(); it != m_failure_requirments.end(); ++it) {
                if ((*it)->queriedFailureRequirement > 0)
                {
                    (*it)->receivingOhterGestureRecognizerFail();
                }
            }
            return;
        }
        if (m_state == MIGestureRecognizerStateRecognized || m_state == MIGestureRecognizerStateBegan) {
            for (GestureIter it = m_failure_requirments.begin(); it != m_failure_requirments.end(); ++it) {
                (*it)->receivingOhterGestureRecognizerSuccess();
            }
            trigger();
            return;
        }
        if (m_state == MIGestureRecognizerStateChanged || m_state == MIGestureRecognizerStateCancelled) {
            trigger();
            return;
        }
    }
    
    void MIGestureRecognizer::receivingOhterGestureRecognizerFail()
    {
        --queriedFailureRequirement;
        if (queriedFailureRequirement > 0) {
            return;
        }
        for (size_t i=0;i<m_targets.size();++i)
        {
            MIResponder *target_response=m_targets[i].responder;
            SELHandler target_action=m_targets[i].action;
            SELFunc handler=target_action.sel_func;
            (target_response->*handler)(target_action.sender);
        }
        if (state() == MIGestureRecognizerStateRecognized || state() == MIGestureRecognizerStateEnded)
            setState(MIGestureRecognizerStatePossible);
        queriedFailureRequirement = 0;
    }
    
    void MIGestureRecognizer::receivingOhterGestureRecognizerSuccess()
    {
        queriedFailureRequirement = 0;
        reset();
        setState(MIGestureRecognizerStateFailed);
    }
    
    void MIGestureRecognizer::clearRequirementsAndDepends()
    {
        GestureIter it;
        for (it = m_failure_dependents.begin(); it != m_failure_dependents.end(); ++it) {
            (*it)->m_failure_requirments.erase(this);
        }
        m_failure_dependents.clear();
        for (it = m_failure_requirments.begin(); it != m_failure_requirments.end(); ++it) {
            (*it)->m_failure_dependents.erase(this);
        }
        m_failure_requirments.clear();
    }

}   //namespace z11
