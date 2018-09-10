//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
#pragma once

#include "../MIControls/MIControl.h"

namespace z11
{
#ifndef TIME_PERIOD
#define TIME_PERIOD 24  // 数据时间周期 //
#endif
#ifndef ROUND_BOARDER
#define ROUND_BOARDER 20    // 标尺控件周边边距 //
#endif
#ifndef LEFT_RIGHT_BOARDER
#define LEFT_RIGHT_BOARDER 40    // 标尺控件左右边距 //
#endif
#ifndef RULER_ANOTATION_SIZE
#define RULER_ANOTATION_SIZE 5 // 标尺标注线的大小 //
#endif
#ifndef RULER_DASH_SIZE
#define RULER_DASH_SIZE 4 // 标尺标注虚线的大小 //
#endif
    
	class MGFont;

	class MIRainMonitor : public MIControl
	{
	public:
		MIRainMonitor();
		virtual ~MIRainMonitor();
    protected:
        // 时间数据 //
        float rain_fall[TIME_PERIOD+1];    // 24小时降雨量 //
        float water_pos[TIME_PERIOD+1];    // 24小时水位 //
        float flow_value[TIME_PERIOD+1];   // 24小时流量 //
    public:
        unsigned int current_hour;   // 当前时间，间隔为小时 //
        // 显示相关//
        float down_low_value;   // 下标尺的低位值 //
        float down_high_value;  // 下标尺的高位值 //
        float down_interval;      // 下标尺的标尺间隔 //
        float up_low_value;     // 上标尺的低位值 //
        float up_high_value;    // 上标尺的高位值 //
        float up_interval;      // 上标尺的标尺间隔 //
        float time_start;       // 显示的时间起点，间隔半小时，即0.5 //
        float time_end;         // 显示的时间终点，间隔半小时，即0.5 //
        float time_interval;    // 显示时间时采用的间隔，最小为半小时,即0.5 //
        RGBAColorRef cylinder_color;  // 圆柱体主色 //
        RGBAColorRef cylinder_light;  // 圆柱体光源颜色 //
        RGBAColorRef water_pos_color; // 水位线颜色 //
        RGBAColorRef flow_value_color; // 流量线颜色 //
        unsigned int font_size; // 字体大小 //
    private:
        // 私有绘图函数 //
        void drawRuler();   // 绘制底层标尺 //
        void drawAnotation();   // 绘制标注 //
        void drawRainFallCylinder();    // 绘制降雨量柱状图 //
        void drawWaterPosLine();        // 绘制水位过程线 //
        void drawFlowValueLine();       // 绘制流量过程线 //
        void drawTouchRect();
        // 获取指定时间数据下标 //
        unsigned int getStartIdxOfHour(unsigned int hour);
        unsigned int getEndIdxOfHour(unsigned int hour);
        void resetTimeRuler();
        void resetTimeInterval(float diff);
        float getReStartTime();
        float getReEndTime();
    private:
        // 操作绘制参数 //
        MGFont* text_font;  // 文本绘制字体 //
        float begin_touch;  // 开始操作触摸的x坐标 //
        float current_touch;    // 触摸过程的x坐标 //
    private:
        MGPoint up_zero, down_zero; // 上标尺和下标尺的坐标原点 //
        MGPoint up_one, down_one, up_down_center;   // 上标尺和下标尺的坐标终点 //
    public:
        void setRainFall(float rf[TIME_PERIOD+1]);
        void setWaterPos(float wp[TIME_PERIOD+1]);
        void setFlowValue(float fv[TIME_PERIOD+1]);
	public:
		MIRESPONDER_TOUCH_DECLARE;
		MIVIEW_DECLARE;
	protected:
	};
}

