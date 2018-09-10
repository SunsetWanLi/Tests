//
//  MITextAreaCell.h
//  Gui
//
//  Created by Hu DanYuan on 11-9-9
//  Copyright 2011ƒÍ Zhejiang University. All rights reserved.
//

#pragma once

#include "MIControl.h"
#include "MIEditableTextView.h"
#include "MIBaseTextView.h"
#include "../../System/Shared/Utility.h"
#include <string>

namespace z11
{
	class MITextAreaCell : public MIControl, public MIBaseTextView, public MIEditableTextView
	{
	public:
		MITextAreaCell();
		virtual ~MITextAreaCell();
	public:
		MIRESPONDER_TOUCH_DECLARE;
		MIEDITABLETEXTVIEW_DECLARE;
		MIBASETEXTVIEW_DECLARE;
		MIVIEW_DECLARE;
    protected:
        std::string m_preinput_text;    // 中文等输入法的预输入字母 //
	protected:
		void drawBeforeSubviews();
		void adjustArea();

	private:
		float m_init_height;		//≥ı º∏ﬂ∂»
	};
}