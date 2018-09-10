//
//  MICursor.h
//  Gui
//
//  Created by Hu DanYuan on 11-6-22
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#pragma once

#include "MIControl.h"
#include "MIEditableTextView.h"
#include "MIBaseTextView.h"
#include "../../System/Shared/Utility.h"
#include <string>

namespace z11
{
	class MICursor : public MIView
	{
	public:
		MICursor();
		virtual ~MICursor();
        MGPROPERTY(uint_32, m_location, Location);
		void animate(bool animated);
        
	public:
		virtual void moveToPos(const MGPoint &point);
		virtual void moveToFirst();
		virtual void moveToUp();
		virtual void moveToDown();
		virtual void moveToLeft();
		virtual void moveToRight();
	};
}