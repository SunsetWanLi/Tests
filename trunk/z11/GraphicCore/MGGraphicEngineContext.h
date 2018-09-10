//
//  MGGraphicEngineContext.h
//  GraphicCore
//
//  Created by Wang ZhiPeng on 11-6-23.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#pragma once

#include "../CommonBase/MGObject.h"
#include "../CommonBase/MGStructure.h"


#include "../Platform/MGGL.h"

namespace z11
{
    enum
    {
        PF_DEFAULT,
        PF_RGBA8888,
        PF_RGBA8880,
        PF_RGBA5650,
        PF_RGBA4444,
        PF_RGBA5551,
        PF_RGBA0008,
		PF_BGRA8880,
		PF_BGRA8888,
        PF_GRAYA88,
        PF_GRAY8
    };
    typedef uint_16 PixelFormat;
    class MGGraphicEngineContext : public MGObject
    {
    public:
        MGGraphicEngineContext();
        virtual ~MGGraphicEngineContext();
    public:
		void initWithData(byte_8*& data, PixelFormat pixel_format, uint_16 pixel_width, uint_16 pixel_height);
		void initWithDataWithoutDelete(byte_8*& data, PixelFormat pixel_format, uint_16 pixel_width, uint_16 pixel_height);
		void releaseContext();
    public:
        GLuint texture_id;
        //void* opaque;
    };
} //namesapce z11
