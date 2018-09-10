//
//  AppMain.h
//  iOSApp
//
//  Created by Wang ZhiPeng on 11-5-24.
//  Copyright 2011骞碯hejiang University. All rights reserved.
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
		virtual void preserveEvent(z11::MIEvent *event);
	public:
		void initialise(WndHandle);
		//EAGLContext* CreateBestEAGLContext();
		void setProjectionMode();
		void CreateFrameBuffer();
		void CreateTexture();
		void AllocateAndAttachDepthBuffer();
		void TestBufferCompleteness();
	public:
		GUICONTAINER_DECLARE;
	public:
		//GLint framebufferWidth,framebufferHeight;
	};

}
