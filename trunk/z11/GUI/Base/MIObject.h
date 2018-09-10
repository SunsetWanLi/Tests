//
//  MIObject.h
//  Gui
#if defined(_WIN32) || defined(_WIN64)
#pragma warning (disable:4819)
#endif
//
//  Created by Wang ZhiPeng on 11-5-21.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//
/*
 所有GUI模块内类的基类
 */

#pragma once

#include "../../CommonBase/MGObject.h"
#include "../../CommonBase/CommonDef.h"

namespace z11
{
    class MIObject : public MGObject
    {
    public:
        MIObject(){}
        virtual ~MIObject() = 0;
    };


} //namespace z11
