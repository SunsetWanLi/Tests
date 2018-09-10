//
//  ToastView.h
//  WaterMobileGIS
//
//  Created by MacOSDevelop on 12-11-28.
//  Copyright (c) 2012年 Zhejiang University. All rights reserved.
//
#ifndef __WaterMobileGIS__ToastView__
#define __WaterMobileGIS__ToastView__

#include "GUI/Base/MIView.h"

namespace z11
{
    class MITextView;
    class ImageResizableButton;
    
    class ToastView : public MIView
    {
    protected:
        ToastView();
        virtual ~ToastView();
        virtual void initWithFrame(MGRect frame);
        virtual void drawself();
        
    public:
        static void showText(const string &text, float show_second = LENGTH_SHORT, bool time_forced = false);
        static void hide();
        static void setFontSize(uint_32 _font_size);
        static bool isHidden();
    private:
        int_32 getRadious();
        void init();
        void hideAnimation();
        
    public:
        const static float LENGTH_SHORT;
        const static float LENGTH_LONG;

    private:
        bool is_animating;
		bool is_time_forced;
        float show_second;
        MGTimeTicks begin_ticks;
        ImageResizableButton* bkg_view;
        MITextView* text_view;
        
        static Syncable toast_locker;
        static uint_32 toast_font_size;
        static float text_view_fixed_size;
        static ToastView* toast_view;
    };
}

#endif /* defined(__WaterMobileGIS__ToastView__) */
