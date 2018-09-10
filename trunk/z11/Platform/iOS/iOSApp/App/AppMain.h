//
//  AppMain.h
//  iOSApp
//
//  Created by Wang ZhiPeng on 11-5-24.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//
#pragma once

#include "MIKit.h"
#include <QuartzCore/QuartzCore.h>
#include <OpenGLES/EAGL.h>
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>

using namespace z11;

class AppMain : public GuiContainer
{
public:
    AppMain();
    virtual ~AppMain();
public:
    virtual void initialise(WndHandle);
    virtual void preserveEvent(MIEvent* event);
public:
    virtual void setProjectionMode();
protected:
    void createFrameBuffer();
    void createRenderBuffer();
    void createTexture();
    void allocateAndAttachDepthBuffer();
    void testBufferCompleteness();
public:
    GUICONTAINER_DECLARE;
public:
    GLint framebufferWidth,framebufferHeight;
    GLuint defaultFramebuffer, colorRenderbuffer, depthRenderbuffer;
    GLuint textureBuffer;
    EAGLContext* maintain_context;
};