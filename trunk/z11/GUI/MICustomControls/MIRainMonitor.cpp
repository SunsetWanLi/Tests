
//  MILabel.cpp
//  Gui

#include "MIRainMonitor.h"
#include "../../GraphicCore/MGImage.h"
#include "../../System/Shared/MGLog.h"
#include "../../System/Shared/Utility.h"
#include "../../System/Device/MGDevice.h"
#include "../../GraphicCore/MGFont.h"
#include "../../GraphicCore/MGGraphicEngine.h"
#include <assert.h>
#include<stdio.h>

#include <iostream>
using namespace std;

namespace z11
{
	MIRainMonitor::MIRainMonitor()
	{
		m_background_color=MGColor::fromRGBA(0,0,0,0);
        for (int i = 0; i <= TIME_PERIOD; ++i) {
            rain_fall[i] = 0.0f;
            water_pos[i] = 0.0f;
            flow_value[i] = 0.0f;
        }
        font_size = MGDevice::getScreenWidth() / 40;
        text_font = MGFont::create(font_size, RGBAColorMake(0, 0, 0));
        cylinder_color = RGBAColorRef::skyBlue();
        cylinder_light = RGBAColorRef::lightBlue();
        water_pos_color = RGBAColorRef::green();
        flow_value_color = RGBAColorRef::red();
	}

	MIRainMonitor::~MIRainMonitor()
	{
        if(text_font != NULL)
            delete_and_set_null(text_font);
	}
    
    void MIRainMonitor::initWithFrame(MGRect frame)
	{
		MIControl::initWithFrame(frame);
        // 计算坐标 //
        up_zero.x = m_bounds.x + LEFT_RIGHT_BOARDER;
        up_zero.y = m_bounds.y + ROUND_BOARDER;
        down_zero.x = up_zero.x;
        down_zero.y = m_bounds.y + m_bounds.height - LEFT_RIGHT_BOARDER;
        up_down_center.x = up_zero.x;
        up_down_center.y = (up_zero.y + down_zero.y) / 2.0f;
        up_one.x = m_bounds.x + m_bounds.width - ROUND_BOARDER;
        up_one.y = up_zero.y;
        down_one.x = up_one.x;
        down_one.y = down_zero.y;
	}
    
#pragma mark - draw logic

	void MIRainMonitor::drawself()
	{
		//TODO: redraw here!
        MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();
        if (graphic_engine == NULL) {
            return;
        }
        RGBAColorRef color_ref;
        if (m_background_color != NULL) {
            color_ref = m_background_color->m_color;
            color_ref.m_alpha_component = (byte_8)(m_alpha * color_ref.m_alpha_component);
        }else{
            color_ref = RGBAColorMake(0, 0, 0, 100);//(byte_8)(m_alpha * 255));
        }
        if (m_background_color != NULL && m_background_color->m_image != NULL) {
            m_background_color->m_image->sendDataToContext();
            graphic_engine->drawImage(m_background_color->m_image, m_bounds, color_ref, draw_image_mode);
        }else{
            graphic_engine->drawFilledRect(m_bounds, color_ref);
        }
        drawRuler();
        drawAnotation();
        drawRainFallCylinder();
        drawWaterPosLine();
        drawFlowValueLine();
        drawTouchRect();
	}
    
    void MIRainMonitor::drawRuler()
    {
        MGGraphicEngine* ge = MGGraphicEngine::sharedGraphicEngine();
        if (ge == NULL) {
            return;
        }
        // 绘制标尺 //
        RGBAColorRef line_color = RGBAColorRef::black();
        ge->drawLine(up_zero, up_one, line_color);
        ge->drawLine(up_zero, up_down_center, line_color);
        ge->drawLine(down_zero, down_one, line_color);
        ge->drawLine(down_zero, up_down_center, line_color);
        
        // 绘制图例及单位说明 //
        MGRect rect = MGRectMake(up_one.x + 5, up_one.y + 20, font_size, font_size);
        ge->drawFilledCylinderRect(rect, cylinder_color, cylinder_light, true);
        rect.y += font_size * 3;
        rect = MGRectMake(rect.x, rect.y, font_size*2, 10);
        ge->drawText("降 ", text_font, rect);
        rect.y += font_size;
        ge->drawText("雨 ", text_font, rect);
        rect.y += font_size;
        ge->drawText("量 ", text_font, rect);
        rect.y += font_size + 4;
        ge->drawText("毫 ", text_font, rect);
        rect.y += font_size;
        ge->drawText("米 ", text_font, rect);
        rect.y += font_size * 2;
        MGPoint begin = MGPointMake(rect.x + font_size * 0.5f, rect.y);
        rect.y += font_size * 2;
        MGPoint end = MGPointMake(rect.x + font_size * 0.5f, rect.y);
        ge->drawLine(begin, end, water_pos_color);
        rect.y += font_size*2;
        ge->drawText("水 ", text_font, rect);
        rect.y += font_size;
        ge->drawText("位 ", text_font, rect);
        rect.y += font_size + 4;
        ge->drawText("米 ", text_font, rect);
        rect.y += font_size * 2;
        begin = MGPointMake(rect.x + font_size * 0.5f, rect.y);
        rect.y += font_size * 2;
        end = MGPointMake(rect.x + font_size * 0.5f, rect.y);
        ge->drawLine(begin, end, flow_value_color);
        rect.y += font_size*2;
        ge->drawText("流 ", text_font, rect);
        rect.y += font_size;
        ge->drawText("量 ", text_font, rect);
        rect.y += font_size + 4;
        ge->drawText("米 ", text_font, rect);
    }
    
    void MIRainMonitor::drawAnotation()
    {
        MGGraphicEngine* ge = MGGraphicEngine::sharedGraphicEngine();
        if (ge == NULL) {
            return;
        }
        float diff = time_end - time_start;
        if(diff <= 0)
            diff += TIME_PERIOD;
        resetTimeInterval(diff);
        // 绘制标注 //
        char text[20];
        RGBAColorRef line_color = RGBAColorRef::black();
        RGBAColorRef dash_color = RGBAColorMake(50, 50, 50, 100);
        MGRect text_rect;
        unsigned int up_anotation_num = (up_high_value - up_low_value) / up_interval; // 上标尺标注数量 //
        unsigned int down_anotation_num = (down_high_value - down_low_value) / down_interval; // 下标尺标注数量 //
        MGPoint anotation_point;
        anotation_point.x = up_zero.x;
        vector<float> dash_style;
        dash_style.push_back(RULER_DASH_SIZE);
        dash_style.push_back(RULER_DASH_SIZE);
        for(unsigned int i = 0; i <= up_anotation_num; ++i)
        {
            anotation_point.y = up_zero.y + (up_down_center.y - up_zero.y) * i / up_anotation_num;
            MGPoint end = MGPointMake(anotation_point.x - RULER_ANOTATION_SIZE, anotation_point.y);
            ge->drawLine(anotation_point, end, line_color);
            end = MGPointMake(up_one.x, anotation_point.y);
            ge->drawDashLine(anotation_point, end, dash_color, 1.0, dash_style, 0.0, dash_color);
            float step = font_size * 0.5f;
            if(i == up_anotation_num)
                step = font_size;
            text_rect = MGRectMake(anotation_point.x - font_size * 3, anotation_point.y - step, font_size*5, 10);
            sprintf(text, "%.1f", up_low_value + (up_interval) * i);
            ge->drawText(text, text_font, text_rect);
        }
        anotation_point.x = down_zero.x;
        for(unsigned int i = 0; i <= down_anotation_num; ++i)
        {
            anotation_point.y = down_zero.y - (down_zero.y - up_down_center.y) * i / down_anotation_num;
            MGPoint end = MGPointMake(anotation_point.x - RULER_ANOTATION_SIZE, anotation_point.y);
            ge->drawLine(anotation_point, end, line_color);
            end = MGPointMake(down_one.x, anotation_point.y);
            ge->drawDashLine(anotation_point, end, dash_color, 1.0, dash_style, 0.0, dash_color);
            float step = font_size * 0.5f;
            if(i == down_anotation_num)
                step = 0;
            text_rect = MGRectMake(anotation_point.x - font_size * 3, anotation_point.y - step, font_size*5, 10);
            sprintf(text, "%.1f", down_low_value + (down_interval) * i);
            ge->drawText(text, text_font, text_rect);
        }
        unsigned int time_anotation_num; // 时间标尺标注数量 //
        if(time_end > time_start)
            time_anotation_num= (time_end - time_start) / time_interval;
        else
            time_anotation_num= (time_end - time_start + TIME_PERIOD) / time_interval;
        anotation_point.y = up_zero.y;
        for(unsigned int i = 0; i <= time_anotation_num; ++i)
        {
            anotation_point.x = up_zero.x + (up_one.x - up_zero.x) *i / time_anotation_num;
            MGPoint end = MGPointMake(anotation_point.x, anotation_point.y - RULER_ANOTATION_SIZE);
            ge->drawLine(anotation_point, end, line_color);
            end = MGPointMake(anotation_point.x, up_down_center.y);
            ge->drawDashLine(anotation_point, end, dash_color, 1.0, dash_style, 0.0, dash_color);
            text_rect = MGRectMake(anotation_point.x - font_size - 2, anotation_point.y - font_size * 1.25, font_size*5, 10);
            float time = time_start + (time_interval * i);
            if(time >= TIME_PERIOD)
                time -= TIME_PERIOD;
            unsigned int hour = (unsigned int)time;
            time = time - hour;
            bool half_hour = false;
            if(time >= 0.4 && time <= 0.6)
                half_hour = true;
            if(hour < 10)
                sprintf(text, "0%d:%s", hour, half_hour ? "30" : "00");
            else
                sprintf(text, "%d:%s", hour, half_hour ? "30" : "00");
            ge->drawText(text, text_font, text_rect);
        }
        anotation_point.y = down_zero.y;
        for(unsigned int i = 0; i <= time_anotation_num; ++i)
        {
            anotation_point.x = down_zero.x + (down_one.x - down_zero.x) *i / time_anotation_num;
            MGPoint end = MGPointMake(anotation_point.x, anotation_point.y + RULER_ANOTATION_SIZE);
            ge->drawLine(anotation_point, end, line_color);
            end = MGPointMake(anotation_point.x, up_down_center.y);
            ge->drawDashLine(anotation_point, end, dash_color, 1.0, dash_style, 0.0, dash_color);
            text_rect = MGRectMake(anotation_point.x - font_size - 2, anotation_point.y + font_size * 1.25, font_size*5, 10);
            float time = time_start + (time_interval * i);
            if(time >= TIME_PERIOD)
                time -= TIME_PERIOD;
            unsigned int hour = (unsigned int)time;
            time = time - hour;
            bool half_hour = false;
            if(time >= 0.4 && time <= 0.6)
                half_hour = true;
            if(hour < 10)
                sprintf(text, "0%d:%s", hour, half_hour ? "30" : "00");
            else
                sprintf(text, "%d:%s", hour, half_hour ? "30" : "00");
            ge->drawText(text, text_font, text_rect);
        }
    }
    
    void MIRainMonitor::drawRainFallCylinder()
    {
        MGGraphicEngine* ge = MGGraphicEngine::sharedGraphicEngine();
        if (ge == NULL) {
            return;
        }
//        unsigned int time_anotation_num; // 时间标尺标注数量 //
//        if(time_end > time_start)
//            time_anotation_num= (time_end - time_start) / time_interval;
//        else
//            time_anotation_num= (time_end - time_start + TIME_PERIOD) / time_interval;
        //vector<MGPoint> rain_fall_v;
        MGPoint point;
        unsigned int start_v = getStartIdxOfHour(time_start);
        unsigned int end_v = getEndIdxOfHour(time_end);
        int diff_v = end_v - start_v;
        if(diff_v <= 0)
            return;
        float section = (up_one.x - up_zero.x) / diff_v;
        for(int i = 0; i <= diff_v; ++i)
        {
            float height = (up_down_center.y - up_zero.y) * (rain_fall[start_v + i] - up_low_value) / (up_high_value - up_low_value);
            float x = up_zero.x + (section * i);
            float y = up_zero.y;
            //point = MGPointMake(x, y);
            //rain_fall_v.push_back(point);
            MGRect rect;
            if(i == 0)
            {
                rect = MGRectMake(x, y, section * 0.5f, height);
            }else if(i == diff_v)
            {
                x -= (section * 0.5f);
                rect = MGRectMake(x, y, section * 0.5f, height);
            }else{
                x -= (section * 0.5f);
                rect = MGRectMake(x, y, section * 1.0f, height);
            }
            //ge->drawFilledGradientRect(rect, cylinder_color);
            ge->drawFilledCylinderRect(rect, cylinder_color, cylinder_light, true);
            
			if(i != 0 && i != diff_v && diff_v < 13 && rain_fall[start_v+i] != 0.0f)
			{
				MGRect rain_value_rect = MGRectMake(x, y+height, section, 1.5f*font_size);
				char rain_value[10] = {0};
				sprintf(rain_value, "%.2f", rain_fall[start_v+i]);
				ge->drawText(rain_value, text_font, rain_value_rect, MILineBreakModeClip, MITextAlignmentCenterMiddle);
			}
        }
    }
    
    void MIRainMonitor::drawWaterPosLine()
    {
        MGGraphicEngine* ge = MGGraphicEngine::sharedGraphicEngine();
        if (ge == NULL) {
            return;
        }
//        unsigned int time_anotation_num; // 时间标尺标注数量 //
//        if(time_end > time_start)
//            time_anotation_num= (time_end - time_start) / time_interval;
//        else
//            time_anotation_num= (time_end - time_start + TIME_PERIOD) / time_interval;
        vector<MGPoint> water_pos_v;
        MGPoint point;
        unsigned int start_v = getStartIdxOfHour(time_start);
        unsigned int end_v = getEndIdxOfHour(time_end);
        int diff_v = end_v - start_v;
        if(diff_v <= 0)
            return;
        for(int i = 0; i <= diff_v; ++i)
        {
            float x = down_zero.x + (down_one.x - down_zero.x) * i / diff_v;
            float y = down_zero.y - (down_zero.y - up_down_center.y) * (water_pos[start_v + i] - down_low_value) / (down_high_value - down_low_value);
            point = MGPointMake(x, y);
            water_pos_v.push_back(point);
        }
        ge->drawPolyLines(water_pos_v, water_pos_color, 2);
    }
    
    void MIRainMonitor::drawFlowValueLine()
    {
        MGGraphicEngine* ge = MGGraphicEngine::sharedGraphicEngine();
        if (ge == NULL) {
            return;
        }
//        unsigned int time_anotation_num; // 时间标尺标注数量 //
//        if(time_end > time_start)
//            time_anotation_num= (time_end - time_start) / time_interval;
//        else
//            time_anotation_num= (time_end - time_start + TIME_PERIOD) / time_interval;
        vector<MGPoint> flow_value_v;
        MGPoint point;
        unsigned int start_v = getStartIdxOfHour(time_start);
        unsigned int end_v = getEndIdxOfHour(time_end);
        int diff_v = end_v - start_v;
        if(diff_v <= 0)
            return;
        for(int i = 0; i <= diff_v; ++i)
        {
            float x = down_zero.x + (down_one.x - down_zero.x) * i / diff_v;
            float y = down_zero.y - (down_zero.y - up_down_center.y) * (flow_value[start_v + i] - down_low_value) / (down_high_value - down_low_value);
            point = MGPointMake(x, y);
            flow_value_v.push_back(point);
        }
        ge->drawPolyLines(flow_value_v, flow_value_color, 2);
    }
    
    void MIRainMonitor::drawTouchRect()
    {
        if(begin_touch == current_touch)
            return;
        MGGraphicEngine* ge = MGGraphicEngine::sharedGraphicEngine();
        if (ge == NULL) {
            return;
        }
        RGBAColorRef color = RGBAColorMake(100, 100, 100, 100);
        MGRect rect = MGRectMake(begin_touch, up_zero.y, current_touch - begin_touch, down_zero.y - up_zero.y);
        ge->drawFilledRect(rect, color);
    }
    
    unsigned int MIRainMonitor::getStartIdxOfHour(unsigned int hour)
    {
        assert(hour < TIME_PERIOD);
        assert(current_hour < TIME_PERIOD);
        unsigned int ret = 0;
        if(current_hour > hour)
        {
            ret = hour - current_hour + TIME_PERIOD;
        }else {
            ret = hour - current_hour;
        }
        //ret = (TIME_PERIOD - 1) - ret;
        assert(ret <= TIME_PERIOD);
        return ret;
    }
    
    unsigned int MIRainMonitor::getEndIdxOfHour(unsigned int hour)
    {
        assert(hour < TIME_PERIOD);
        assert(current_hour < TIME_PERIOD);
        unsigned int ret = 0;
        if(current_hour >= hour)
        {
            ret = hour - current_hour + TIME_PERIOD;
        }else {
            ret = hour - current_hour;
        }
        assert(ret <= TIME_PERIOD);
        return ret;
    }
    
    void MIRainMonitor::resetTimeInterval(float diff)
    {
        if(diff >= 18)
        {
            time_interval = 3.0f;
        }
        else if(diff < 18 && diff >=12)
        {
            time_interval = 2.0f;
        }
        else if(diff < 12 && diff >= 6)
        {
            time_interval = 1.0f;
        }
        else if(diff < 6 && diff >= 3)
        {
            time_interval = 0.5f;
        }
        else{
            time_end = current_hour;
            time_start = time_end;
            time_interval = 3.0f;
        }
    }
    
    void MIRainMonitor::resetTimeRuler()
    {
        if(begin_touch == current_touch)
            return;
        else if(begin_touch > current_touch)
        {
            float touch = begin_touch;
            begin_touch = current_touch;
            current_touch = touch;
        }
        time_start = getReStartTime();
        time_end = getReEndTime();
        float diff;
        diff = time_end - time_start;
        if(diff <= 0)
            diff += TIME_PERIOD;
        resetTimeInterval(diff);
        begin_touch = 0;
        current_touch = 0;
    }
    
    float MIRainMonitor::getReStartTime()
    {
        float ret = 0.0f;
        do{
            if(begin_touch <= up_zero.x)
            {
                ret = time_start;
                break;
            }
            if(begin_touch >= up_one.x)
            {
                ret = time_end;
                break;
            }
            ret = time_end - time_start;
            if(ret <= 0)
                ret += TIME_PERIOD;
            ret = ret * (begin_touch - up_zero.x) / (up_one.x - up_zero.x);
            ret = time_start + ret;
            if(ret >= TIME_PERIOD)
                ret -= TIME_PERIOD;
        }while(0);
        assert(ret < TIME_PERIOD);
        return (int)ret;
    }
    
    float MIRainMonitor::getReEndTime()
    {
        float ret = 0.0f;
        do{
            if(current_touch <= up_zero.x)
            {
                ret = time_start;
                break;
            }
            if(current_touch >= up_one.x)
            {
                ret = time_end;
                break;
            }
            ret = time_end - time_start;
            if(ret <= 0)
                ret += TIME_PERIOD;
            ret = ret * (current_touch - up_zero.x) / (up_one.x - up_zero.x);
            ret = time_start + ret;
            if(ret >= TIME_PERIOD)
                ret -= TIME_PERIOD;
        }while(0);
        assert(ret < TIME_PERIOD);
        return (int)ret;
    }
    
#pragma mark - Touch Handle

	MIEventHandleState MIRainMonitor::touchesBegan(const std::set<MITouch*>& touches, const MIEvent* const event)
	{
		//...
        do{
            if(touches.size() > 1)
                break;
            MITouch* touch = *touches.begin();
            begin_touch = touch->point.x;
            current_touch = touch->point.x;
        }while(0);
		return MIEventHandleStateDone;
	}

	MIEventHandleState MIRainMonitor::touchesMoved(const std::set<MITouch*>& touches, const MIEvent* const event)
	{
		//...
        do{
            if(touches.size() > 1)
                break;
            MITouch* touch = *touches.begin();
            current_touch = touch->point.x;
        }while(0);
		return MIEventHandleStateDone;
	}

	MIEventHandleState MIRainMonitor::touchesEnded(const std::set<MITouch*>& touches, const MIEvent* const event)
	{
		//...
        do{
            if(touches.size() > 1)
                break;
            MITouch* touch = *touches.begin();
            current_touch = touch->point.x;
            resetTimeRuler();
        }while(0);
		return MIEventHandleStateDone;
	}

	MIEventHandleState MIRainMonitor::touchesCancelled(const std::set<MITouch*>& touches, const MIEvent* const event)
	{
		//...
        do{
            resetTimeRuler();
        }while(0);
		return MIEventHandleStateDone;
	}

	MIEventHandleState MIRainMonitor::handleEvent(const MIEvent* const event)
	{
		//...
		return MIEventHandleStatePass;
	}

#pragma mark - Value Setting
    
    void MIRainMonitor::setRainFall(float *rf)
    {
        for (int i = 0; i <= TIME_PERIOD; ++i) {
            rain_fall[i] = rf[i];
        }
    }
    
    void MIRainMonitor::setWaterPos(float *wp)
    {
        for (int i = 0; i <= TIME_PERIOD; ++i) {
            water_pos[i] = wp[i];
        }
    }
    
    void MIRainMonitor::setFlowValue(float *fv)
    {
        for (int i = 0; i <= TIME_PERIOD; ++i) {
            flow_value[i] = fv[i];
        }
    }
    
}
