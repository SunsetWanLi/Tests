//
//  MIScaleView.h
//  Gui
//
//  Created by slj on 13-5-14.
//  Copyright (c) 2013年 Zhejiang University. All rights reserved.
//
#pragma once
#include "../Base/MIView.h"

namespace z11
{
    class MIImageView;
    class ImageResizableButton;
    
    class MIScaleView : public MIView
    {
    public:
        MIScaleView();
        virtual ~MIScaleView();
        virtual void initWithFrame(MGRect frame);
        virtual void showWithCaptureCenter(const MGPoint &capture_center);
    
        MGPROPERTY(float, scale_value, ScaleValue);
        void setResizableBkgWithCapInsets(MGColor* color, const MGEdgeInsets &non_scale_edge = _UIEdgeInsets());
    
    protected:
        virtual void drawself();
        void updateCaptureImage(const MGPoint &capture_center);
        
    private:
        MGImage* getCaptureImage();
    
    protected:
        ImageResizableButton* bkg_view;
        MIImageView* content_view;
        
    private:
        MGPoint capture_center;
        bool should_reload_capture_image;
    };
}

