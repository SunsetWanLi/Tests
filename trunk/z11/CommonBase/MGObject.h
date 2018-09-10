//
//  MGDevKitBase.h
//  CommonBase
//
//  Created by Wang ZhiPeng on 11-5-21.
//  Copyright 2011 Zhejiang University. All rights reserved.
//

/*
 The Base Class
 */
#pragma once

#include "CommonDef.h"
#include <string>

using namespace std;

namespace z11 {
    class MGObject
    {
    public:
        MGObject(){}
        virtual ~MGObject() = 0;
		virtual bool isEqual(MGObject *);
    };
    typedef void (MGObject::*SELFunc)(MGObject*);
    typedef struct _SELHandler
    {
        _SELHandler(){sender = NULL; sel_func = NULL;}
        MGObject* sender;
        SELFunc sel_func;
    }SELHandler;


} //namespace z11
