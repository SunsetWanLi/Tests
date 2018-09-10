//
//  MITextButton.h
//  Gui
//
//  Created by Hu DanYuan on 11-7-13
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#pragma once

#include "MIControl.h"
#include "MIButton.h"
#include "MIBaseTextView.h"
#include "../../System/Shared/Utility.h"
#include <string>

namespace z11
{
	class MITextButton : public MIButton, public MIBaseTextView
	{
	public:
		MITextButton();
		~MITextButton();

	public:
		MIRESPONDER_TOUCH_DECLARE
		MIVIEW_DECLARE
		MIBASETEXTVIEW_DECLARE

	};
}