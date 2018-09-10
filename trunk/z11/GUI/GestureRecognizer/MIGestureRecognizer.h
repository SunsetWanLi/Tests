//
//  MIGestureRecognizer.h
//  Gui
//
//  Created by King Asdy on 12-3-23.
//  Copyright (c) 2012年 Zhejiang University. All rights reserved.
//

#pragma once

#include <set>
#include <map>
#include "../Base/MIObject.h"
#include "../Events/MITouch.h"
#include "../Base/MIView.h"
namespace z11 {

    // all gestures
    /* Possible --> Failed */
    
    // discrete gestures
    /* Possible --> Recognized */
    
    // continuous gestures
    /* Possible --> Began --> Changed --> Ended */
    /* Possible --> Began --> Changed --> Cancelled */

	typedef enum {
		MIGestureRecognizerStatePossible,
        
		MIGestureRecognizerStateBegan,
		MIGestureRecognizerStateChanged,
		MIGestureRecognizerStateEnded,
		MIGestureRecognizerStateCancelled,

		MIGestureRecognizerStateFailed,

		MIGestureRecognizerStateRecognized = MIGestureRecognizerStateEnded
	} MIGestureRecognizerState;
    
    

    class MIGestureRecognizer : public MIObject
    {
    public:
        MIGestureRecognizer();
        virtual ~MIGestureRecognizer() = 0;
		void initWithTarget(MIResponder *responser, SELHandler action);
        void addTarget(MIResponder* responder, SELHandler action);
        void removeTarget(MIResponder* responder, SELHandler action);
        virtual void reset();
		MIGestureRecognizerState state();
    protected:
        // implementation by subclass //
		virtual void touchesBegan(const std::set<MITouch*> &touches, const MIEvent* event);
		virtual void touchesMoved(const std::set<MITouch*> &touches, const MIEvent* event);
		virtual void touchesEnded(const std::set<MITouch*> &touches, const MIEvent* event);
		virtual void touchesCancelled(const std::set<MITouch*> &touches, const MIEvent* event);
        void setState(MIGestureRecognizerState s);
        void receivingOhterGestureRecognizerFail();
        void receivingOhterGestureRecognizerSuccess();
        void trigger();
        void clearRequirementsAndDepends();
    public:
		void requireGestureRecognizerToFail(MIGestureRecognizer *otherGestureRecognizer);
    protected:
		MIView *m_view;
	protected:
		std::vector<MIResponderSEL> m_targets;
        std::set<MIGestureRecognizer*> m_failure_requirments;
        std::set<MIGestureRecognizer*> m_failure_dependents;
    private:
		MIGestureRecognizerState m_state;
        uint_32 queriedFailureRequirement;

		friend class MIView;
    };
    typedef std::vector<MIResponderSEL>::iterator TargetIter;
    typedef std::set<MIGestureRecognizer*>::iterator GestureIter;
#define MIGestureRecognizerTouchesHandle \
protected: \
virtual void touchesBegan(const std::set<MITouch*> &touches, const MIEvent* event); \
virtual void touchesMoved(const std::set<MITouch*> &touches, const MIEvent* event); \
virtual void touchesEnded(const std::set<MITouch*> &touches, const MIEvent* event); \
virtual void touchesCancelled(const std::set<MITouch*> &touches, const MIEvent* event); \
virtual void reset();

}   //namespace z11
