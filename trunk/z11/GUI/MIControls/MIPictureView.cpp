//
//  MIPictureView.h
//  Gui
//
//  Created by Hu DanYuan on 11-6-22
//  Copyright 2011Äê Zhejiang University. All rights reserved.
//

#include <assert.h>
#include "MIPictureView.h"
#include "MIPictureViewCell.h"
#include "../../GraphicCore/MGImage.h"

namespace z11
{
	MIPictureView::MIPictureView()
	{
        class_name = "MIPictureView";
		m_picture_view_cell = new MIPictureViewCell();
		_view_flags.clip_subviews = 1;
	}

	MIPictureView::~MIPictureView()
	{
	}

	void MIPictureView::drawself()
	{
		MIView::drawself();
	}

	void MIPictureView::setBackgroundColor(MGColor *_color)
	{
		MIView::setBackgroundColor(MGColor::fromRGBA(255, 255, 255, 0));
		if (m_picture_view_cell != NULL) 
		{
			m_picture_view_cell->setBackgroundColor(_color);
		}
	}

	void MIPictureView::initWithFrame(MGRect frame)
	{
		MIView::initWithFrame(frame);
		if (m_picture_view_cell != NULL) {
			MGRect rect = MGRectMake(0.0f, 0.0f, frame.width, frame.height);
			m_picture_view_cell->initWithFrame(rect);
			addSubview(m_picture_view_cell);
		}
	}

	void MIPictureView::setScaleEnable(bool _flag)
	{
		m_picture_view_cell->enable_scale = _flag;
	}

	void MIPictureView::setHorizontalMoveEnable(bool _flag)
	{
		m_picture_view_cell->enable_horizontal = _flag;
	}

	void MIPictureView::setVerticalMoveEnable(bool _flag)
	{
		m_picture_view_cell->enable_vertical = _flag;
	}

	MGPoint MIPictureView::getCurrOffset()
	{
		return m_picture_view_cell->m_picture_current_offset;
	}

	float MIPictureView::getCurrScaling()
	{
		return m_picture_view_cell->m_picture_current_scaling;
	}

	void MIPictureView::setAniDuration(float _duration)
	{
		m_picture_view_cell->ani_duration = _duration;
	}

	void MIPictureView::setMovedResponderSEL(MIResponderSEL _sel)
	{
		m_picture_view_cell->moved_sel = _sel;
	}
}