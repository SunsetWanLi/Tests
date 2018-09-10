//
//  MIImageView.h
//  Gui
//
//  Created by Wang ZhiPeng on 11-8-18.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#pragma once

#include "MIControl.h"

namespace z11 {
    class MIImageView : public MIControl
    {
    public:
        MIImageView();
        virtual ~MIImageView();
    public:
        virtual void drawself();
        MIRESPONDER_TOUCH_DECLARE;
    };
} //namespace z11