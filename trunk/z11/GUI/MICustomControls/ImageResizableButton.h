//
//  ImageResizableButton.h
//  MultidimensionBeijing
//
//  Created by slj on 13-4-1.
//  Copyright (c) 2013年 Zhejiang University. All rights reserved.
//
#pragma once
#ifndef __MultidimensionBeijing__ImageResizableButton__
#define __MultidimensionBeijing__ImageResizableButton__

#include "GUI/MIControls/MIButton.h"

namespace z11
{
    class ImageResizableButton : public MIButton
    {
    public:
        ImageResizableButton();
        virtual ~ImageResizableButton();
        virtual void initWithFrame(MGRect frame);
        
        MGPROPERTY_BOOL(bool, edge_scale_enabled, EdgeScaleEnabled);
		MGPROPERTY(DrawRoundRectMode, m_draw_round_mode, DrawRoundMode);
        void resizableWithCapInsets(const MGEdgeInsets &non_scale_edge, const MGEdgeInsets &draw_edge = _UIEdgeInsets());

    protected:
        virtual void drawself();
        
    private:
        bool isZero(float value);
        
    private:
        MGEdgeInsets non_scale_edge;
        MGEdgeInsets draw_edge;
    };
}

#endif /* defined(__MultidimensionBeijing__ImageResizableButton__) */
