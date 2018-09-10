//
//  MIAnimationAlgorithm.h
//  Gui
//
//  Created by Wang ZhiPeng on 11-6-10.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//
#pragma once

#include "../Base/MIObject.h"
#include "../../CommonBase/CommonDef.h"

namespace z11
{
    typedef uint_16 MIViewAnimationOptionCurve;
    class MIAnimationAlgorithm : public MIObject
    {
    public:
        MIAnimationAlgorithm();
        virtual ~MIAnimationAlgorithm();
    public:
        MIViewAnimationOptionCurve animation_curve;

		/**
		 * @brief calculateIntervalInStep 根据current_step 和all_step 的比例，以及区间[src_val,dst_val]计算出val
		 *
		 * @param src_val 区间左端点
		 * @param dst_val 区间右端点
		 * @param current_step 当前step
		 * @param all_step 全部step
		 * @param current_val 当前值
         * 
		 * @return 计算出的val
		 */
        virtual float calculateIntervalInStep(float src_val, float dst_val, int_16 current_step, int_16 all_step, float current_val);
    private:
        float calculateIntervalWithCurveLinear(float src_val, float dst_val, int_16 current_step, int_16 all_step, float current_val);
        float calculateIntervalWithCurveLinearSwing(float src_val, float dst_val, int_16 current_step, int_16 all_step, float current_val);
        float calculateIntervalWithCurveEaseInOut(float src_val, float dst_val, int_16 current_step, int_16 all_step, float current_val);
        float calculateIntervalWithCurveFastInOut(float src_val, float dst_val, int_16 current_step, int_16 all_step, float current_val);
        float calculateIntervalWithCurveEaseIn(float src_val, float dst_val, int_16 current_step, int_16 all_step, float current_val);
        float calculateIntervalWithCurveEaseOut(float src_val, float dst_val, int_16 current_step, int_16 all_step, float current_val);
        float calculateIntervalWithCurveBounce(float src_val, float dst_val, int_16 current_step, int_16 all_step, float current_val);
		float calculateIntervalWithCurveDamping(float src_val, float dst_val, int_16 current_step, int_16 all_step, float current_val);
		float calculateIntervalWithCurveDampingOnce(float src_val, float dst_val, int_16 current_step, int_16 all_step, float current_val);
        float calculateIntervalWithCurveInertia(float src_val, float dst_val, int_16 current_step, int_16 all_step, float current_val);
        float calculateIntervalWithCurveMomentum(float src_val, float dst_val, int_16 current_step, int_16 all_step, float current_val);
		float calculateIntervalWithCurvePendulum(float src_val, float dst_val, int_16 current_step, int_16 all_step, float current_val);
        float calculateIntervalWithCurveCursor(float src_val, float dst_val, int_16 current_step, int_16 all_step, float current_val);
    };
} //namespace z11
