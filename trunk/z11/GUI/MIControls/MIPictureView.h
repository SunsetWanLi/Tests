//
//  MIPictureView.h
//  Gui
//
//  Created by Hu DanYuan on 11-6-22
//  Copyright 2011年 Zhejiang University. All rights reserved.
//
//	MIPictureView初始加载的图片不会进行缩放,都是原样载入  
#pragma once
#include "MIControl.h"

namespace z11
{
	class MIPictureViewCell;
	class MIPictureView : public MIControl
	{
	public:
		MIPictureView();
		virtual ~MIPictureView();

		virtual void setBackgroundColor(MGColor *_color);
		void setScaleEnable(bool _flag);
		void setHorizontalMoveEnable(bool _flag);
		void setVerticalMoveEnable(bool _flag);

		MGPoint getCurrOffset();
		float getCurrScaling();

		void setAniDuration(float _duration);
		void setMovedResponderSEL(MIResponderSEL _sel);

		MIVIEW_DECLARE

	private:
		MIPictureViewCell *m_picture_view_cell;
	};
}