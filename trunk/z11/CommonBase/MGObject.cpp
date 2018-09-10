//
//  MGObject.cpp
//  CommonBase
//
//  Created by Wang ZhiPeng on 11-5-24.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#include "MGObject.h"
#include "../GUI/GuiContainer/GuiContainer.h"

namespace z11
{
    MGObject::~MGObject()
    {
		if (GuiContainer::container!=this)
		{
			if (GuiContainer::container != NULL)
				GuiContainer::container->clearEventWithObject(this);
		}
		
    }

	bool MGObject::isEqual( MGObject * rhs)
	{
		return this==rhs;
	}

}