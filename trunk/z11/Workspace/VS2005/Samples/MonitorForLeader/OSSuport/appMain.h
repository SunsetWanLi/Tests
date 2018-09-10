//
//  AppMain.h
//  iOSApp
//
//  Created by Wang ZhiPeng on 11-5-24.
//  Copyright 2011年Zhejiang University. All rights reserved.
//
#pragma once

#include "../../../../../GUI/GuiContainer/GuiContainer.h"

namespace z11
{
	class AppMain : public GuiContainer
	{
	public:
		AppMain();
		virtual ~AppMain();
	public:
		void initialise(WndHandle);
		void setProjectionMode();
		void CreateFrameBuffer();
		void CreateTexture();
		void AllocateAndAttachDepthBuffer();
		void TestBufferCompleteness();
		virtual void preserveEvent(z11::MIEvent *event);
	public:
		GUICONTAINER_DECLARE;
	};

}
