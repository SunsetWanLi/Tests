//
//  MIAnimationAlgorithm.cpp
//  Gui
//
//  Created by Wang ZhiPeng on 11-6-10.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#include "MIAnimationAlgorithm.h"
#include "../Base/MIView.h"
#include <math.h>
#include "../../System/Shared/MGLog.h"
#ifndef M_PI
#define M_PI        3.14159265358979323846264338327950288
#endif

namespace z11
{
    MIAnimationAlgorithm::MIAnimationAlgorithm()
    {
        animation_curve = MIViewAnimationOptionCurveLinear;
    }
    MIAnimationAlgorithm::~MIAnimationAlgorithm()
    {
        
    }
    float MIAnimationAlgorithm::calculateIntervalInStep(float src_val, float dst_val, int_16 current_step, int_16 all_step, float current_val)
	{
        float ret = current_val;
        switch (animation_curve) {
            case MIViewAnimationOptionCurveEaseInOut:
            {
                ret = calculateIntervalWithCurveEaseInOut(src_val, dst_val, current_step, all_step, current_val);
                break;
            }
            case MIViewAnimationOptionCurveFastInOut:
            {
                ret = calculateIntervalWithCurveFastInOut(src_val, dst_val, current_step, all_step, current_val);
                break;
            }
            case MIViewAnimationOptionCurveEaseIn:
            {
                ret = calculateIntervalWithCurveEaseIn(src_val, dst_val, current_step, all_step, current_val);
                break;
            }
            case MIViewAnimationOptionCurveEaseOut:
            {
                ret = calculateIntervalWithCurveEaseOut(src_val, dst_val, current_step, all_step, current_val);
                break;
            }
            case MIViewAnimationOptionCurveBounce:
            {
                ret = calculateIntervalWithCurveBounce(src_val, dst_val, current_step, all_step, current_val);
                break;
            }
            case MIViewAnimationOptionCurveDamping:
            {
                ret = calculateIntervalWithCurveDamping(src_val, dst_val, current_step, all_step, current_val);
                break;
            }
			case MIViewAnimationOptionCurveDampingOnce:
			{
				ret = calculateIntervalWithCurveDampingOnce(src_val, dst_val, current_step, all_step, current_val);
				break;
			}
            case MIViewAnimationOptionCurveInertia:
            {
                ret = calculateIntervalWithCurveInertia(src_val, dst_val, current_step, all_step, current_val);
                break;
            }
            case MIViewAnimationOptionCurveMomentum:
            {
                ret = calculateIntervalWithCurveMomentum(src_val, dst_val, current_step, all_step, current_val);
                break;
            }
			case MIViewAnimationOptionCurveLinearSwing:
			{
				ret=calculateIntervalWithCurveLinearSwing(src_val,dst_val,current_step,all_step, current_val);
				break;
			}
			case MIViewAnimationOptionCurvePendulum:
			{
				ret=calculateIntervalWithCurvePendulum(src_val,dst_val,current_step,all_step, current_val);
				break;
			}
            case MIViewAnimationOptionCurveCursor:
			{
				ret=calculateIntervalWithCurveCursor(src_val,dst_val,current_step,all_step, current_val);
				break;
			}
            case MIViewAnimationOptionCurveLinear:
            {
                // Do the same as Default.
            }
            default:
            {
                ret = calculateIntervalWithCurveLinear(src_val, dst_val, current_step, all_step, current_val);
                break;
            }
        }
        return ret;
    }
//动画曲线函数 //
    /*************************       //
     匀速变化摆动的动画曲线函数   //
     y= x<0.5 ? 2x : x(1-x)   //
     src_val:参数当前值   //
     dst_val:参数最终值   //
     current_step:当前步数   //
     all_step:总步数   //
     ************************/			//
    float MIAnimationAlgorithm::calculateIntervalWithCurveLinearSwing(float src_val, float dst_val, int_16 current_step, int_16 all_step, float current_val)
    {
        float ret = current_val;
        current_step = all_step - current_step + 1;
        if (all_step < current_step) {
            return ret;
        }
        float x = current_step * 1.0 / all_step;
		if(x<0.5)
		{
			x*=2;
		}
		else
		{
			x=1-x;
			x*=2;
		}
        ret = x * (dst_val - src_val);
        ret += src_val;
        return ret;
    }
    /*************************       //
     匀速变化的动画曲线函数   //
     y=x   //
     src_val:参数当前值   ///
     dst_val:参数最终值   //
     current_step:当前步数   //
     all_step:总步数   //
     ************************/		//
    float MIAnimationAlgorithm::calculateIntervalWithCurveLinear(float src_val, float dst_val, int_16 current_step, int_16 all_step, float current_val)
    {
        float ret = current_val;
        current_step = all_step - current_step + 1;
        if (all_step < current_step) {
            return ret;
        }
        float x = current_step * 1.0 / all_step;
        ret = x * (dst_val - src_val);
        ret += src_val;
        return ret;
    }
    /*************************       //
     变化先快中间慢后快的动画曲线函数  //
     y=arcsin(2*x-1)/pi+0.5            //
     src_val:参数当前值                //
     dst_val:参数最终值                //
     current_step:当前步数            //
     all_step:总步数            //
     ************************/
    float MIAnimationAlgorithm::calculateIntervalWithCurveFastInOut(float src_val, float dst_val, int_16 current_step, int_16 all_step,float  current_val)
    {
        float ret = current_val;
        current_step = all_step - current_step + 1;
        if (all_step < current_step) {
            return ret;
        }
        float x = current_step * 1.0 / all_step;
        float y = (float)asin(2*x-1)/M_PI + 0.5f;
        ret = y * (dst_val - src_val);
        ret += src_val;
        return ret;
    }
    /*************************       //
     变化先慢中间快后慢的动画曲线函数        //
     y=0.5*sin(pi*x-pi/2)+0.5       //
     src_val:参数当前值   //
     dst_val:参数最终值   //
     current_step:当前步数   //
     all_step:总步数   //
     ************************/
    float MIAnimationAlgorithm::calculateIntervalWithCurveEaseInOut(float src_val, float dst_val, int_16 current_step, int_16 all_step, float current_val)
    {
        float ret = current_val;
        current_step = all_step - current_step + 1;
        if (all_step < current_step) {
            return ret;
        }
        float x = current_step * 1.0 / all_step;
        float y = 0.5f*(float)sin(M_PI*x-0.5*M_PI)+0.5f;
        ret = y * (dst_val - src_val);
        ret += src_val;
        return ret;
    }
    /*************************       //
     变化先快后慢的动画曲线函数   //
     y=sqrt(1-pow((1-x),2));   //
     src_val:参数当前值   //
     dst_val:参数最终值   //
     current_step:当前步数    //
     all_step:总步数   //
     ************************/
    float MIAnimationAlgorithm::calculateIntervalWithCurveEaseOut(float src_val, float dst_val, int_16 current_step, int_16 all_step, float current_val)
    {
        float ret = current_val;
        current_step = all_step - current_step + 1;
        if (all_step < current_step) {
            return ret;
        }
        float x = current_step * 1.0 / all_step;
        float y = sqrt(1-pow((1-x), 2));
        ret = y * (dst_val - src_val);
        ret += src_val;
        return ret;
    }
    /*************************   //
     变化先慢后快的动画曲线函数    //
     y=1-sqrt(1-pow(x,2));   //
     src_val:参数当前值   //
     dst_val:参数最终值   //
     current_step:当前步数   //
     all_step:总步数   //
     ************************///
    float MIAnimationAlgorithm::calculateIntervalWithCurveEaseIn(float src_val, float dst_val, int_16 current_step, int_16 all_step, float current_val)
    {
        float ret = current_val;
        current_step = all_step - current_step + 1;
        if (all_step < current_step) {
            return ret;
        }
        float x = current_step * 1.0 / all_step;
        float y = 1-sqrt(1-pow(x, 2));
        ret = y * (dst_val - src_val);
        ret += src_val;
        return ret;
    }
    /*************************          //
     弹跳动画曲线函数 变化始终在目标值之上或者是目标值之下    //
     类比小球弹地，最终停在水平线         //
     y=?x       //
     src_val:参数当前值       //
     dst_val:参数最终值       //
     current_step:当前步数       //
     all_step:总步数       //
     ************************/       //
    float MIAnimationAlgorithm::calculateIntervalWithCurveBounce(float src_val, float dst_val, int_16 current_step, int_16 all_step, float current_val)
    {
        float ret = current_val;
        current_step = all_step - current_step + 1;
        if (all_step < current_step) {
            return src_val;
        }
        // TODO: implement func
		//just test by GRF
		vector<float> ret_vec;
		int t=1;
		int steps=100;
		float deltas=dst_val-src_val;
		float delta=steps*steps*5/deltas;
		for (int j=1;j<=5;j++)
		{
			int tmax=steps/t;
			float max=5*tmax*tmax;
			for (int i=0;i<=tmax;i++)
			{
				ret_vec.push_back(src_val+(tmax*10*i-5*i*i)/delta);
			}
			for (int i=0;i<=tmax;i++)
			{
				ret_vec.push_back(src_val+(max-5*i*i)/delta);
			}
			t=t*2;

		}
		int current=1.0f*current_step/all_step*ret_vec.size();
		if (current<1)
		{
			current=1;
		}
		return ret_vec[current-1]-0.1;

        return ret;
    }
    /*************************       //
     阻尼动画曲线函数 变化在目标值之上和之下周期变动       //
     类比摆钟，左右摆动最后趋于静止       //
     Wn=(2*k+1)*pi;       //
     Wd=Wn*sqrt(1-sigma*sigma);       //
     y=exp(-1*Wn*sigma*x)*cos(Wd*x+pi)+1       //
     src_val:参数当前值       //
     dst_val:参数最终值       //
     current_step:当前步数       //
     all_step:总步数       //       //
     ************************/       //
    float MIAnimationAlgorithm::calculateIntervalWithCurveDamping(float src_val, float dst_val, int_16 current_step, int_16 all_step, float current_val)
	{
        float ret = current_val;
        current_step = all_step - current_step + 1;
        if (all_step < current_step) {
            return ret;
        }
        float x = current_step * 1.0 / all_step;
        int_16 k=10;
        float sigma=0.1f;
        float Wn=(2*k+1.0f)*M_PI;
        float Wd=Wn*(float)sqrt(1-sigma*sigma);
        float y = (float)exp(-1.0f*Wn*sigma*x)*(float)cos(Wd*x+M_PI)+1.0f;
        ret = y * (dst_val - src_val);
        ret += src_val;
        return ret;
    }
    /*************************       //
	 只是把阻尼运动的系数改大
	 */
	float MIAnimationAlgorithm::calculateIntervalWithCurveDampingOnce( float src_val, float dst_val, int_16 current_step, int_16 all_step, float current_val )
	{
        
		float ret = current_val;
		current_step = all_step - current_step + 1;
		if (all_step < current_step) {
			return ret;
		}
		float x = current_step * 1.0 / all_step;
		int_16 k=3;
		float sigma=0.333f;
		float Wn=(2*k+1.0f)*M_PI;
		float Wd=Wn*(float)sqrt(1-sigma*sigma);
		float y = (float)exp(-1.0f*Wn*sigma*x)*(float)cos(Wd*x+M_PI)+1.0f;
		ret = y * (dst_val - src_val);
		ret += src_val;
		return ret;
	}
    /*************************       //
     惯性动画曲线函数       //
     y=sqrt(1-(1-x)*(1-x))       //
     src_val:参数当前值       //
     dst_val:参数最终值       //
     current_step:当前步数       //
     all_step:总步数       //
     ************************/       //
    float MIAnimationAlgorithm::calculateIntervalWithCurveInertia(float src_val, float dst_val, int_16 current_step, int_16 all_step, float current_val)
    {
        float ret = current_val;
        current_step = all_step - current_step + 1;
        if (all_step < current_step) {
            return ret;
        }
        float x = current_step * 1.0 / all_step;
        float y = (float)sqrt(1 - (1 - x) * (1 - x));
        ret = y * (dst_val - src_val);
        ret += src_val;
        return ret;
    }
	/*************************       //
     动力动画曲线函数       //
     y=1.001-pow(10,-2.5x);       //
     src_val:参数当前值       //
     dst_val:参数最终值       //
     current_step:当前步数       //
     all_step:总步数       //
     ************************/       //
    float MIAnimationAlgorithm::calculateIntervalWithCurveMomentum(float src_val, float dst_val, int_16 current_step, int_16 all_step, float current_val)
    {
        float ret = current_val;
        current_step = all_step - current_step + 1;
        if (all_step < current_step) {
            return ret;
        }
        float x = current_step * 1.0 / all_step;
        float y = 1.001-pow(10, -2.5*x);
        ret = y * (dst_val - src_val);
        ret += src_val;
        return ret;
    }

	/*************************       //
	模拟摆动曲线       //
            8*pow(x,2)          x<0.25
    y = {   1-8*pow(0.5-x,2)    0.25<=x<0.75
            8*pow(1-x,2)        x>=0.75
	src_val:参数当前值       //
	dst_val:参数最终值       //
	current_step:当前步数       //
	all_step:总步数       //
	************************/       //
	float MIAnimationAlgorithm::calculateIntervalWithCurvePendulum(float src_val, float dst_val, int_16 current_step, int_16 all_step, float current_val)
	{
		float ret = current_val;
		current_step = all_step - current_step + 1;
		if (all_step < current_step) {
			return ret;
		}
		float x = current_step * 1.0 / all_step;
		float y;
		if(x<0.25)
		{
			y=8*x*x;
		}
		else if(x<0.75)
		{
			y=1.0-8*(0.5-x)*(0.5-x);
		}
		else 
		{
			y=8*(1-x)*(1-x);
		}
		ret = y * (dst_val - src_val);
		ret += src_val;
		return ret;
	}
    
    /*************************       //
     模拟摆动曲线       //
             0      x<0.2
             10x-2  0.2<=x<0.3
     y = {   1      0.3<=x<=0.7
             8-10x  0.7<x<=0.8
             0      x>0.8
     src_val:参数当前值       //
     dst_val:参数最终值       //
     current_step:当前步数       //
     all_step:总步数       //
     ************************/       //
	float MIAnimationAlgorithm::calculateIntervalWithCurveCursor(float src_val, float dst_val, int_16 current_step, int_16 all_step, float current_val)
	{
		float ret = current_val;
		current_step = all_step - current_step + 1;
		if (all_step < current_step) {
			return ret;
		}
		float x = current_step * 1.0 / all_step;
		float y;
		if(x<0.2)
		{
			y=0;
		}
        else if(x>=0.2 && x<0.3)
		{
			y=10*x-2;
		}
		else if(x<=0.7)
		{
			y=1;
		}
        else if(x>0.7 && x<=0.8)
		{
			y=8-10*x;
		}
		else
		{
			y=0;
		}
		ret = y * (dst_val - src_val);
		ret += src_val;
		return ret;
	}

} //namespace z11
