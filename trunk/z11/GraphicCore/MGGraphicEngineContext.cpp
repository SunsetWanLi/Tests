//
//  MGGraphicEngineContext.cpp
//  GraphicCore
//
//  Created by Wang ZhiPeng on 11-6-23.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#include "MGGraphicEngineContext.h"
#include <stdio.h>
#include <stdlib.h>
#include "../System/Shared/MGLog.h"

namespace z11 {
    // MGGraphicEngineContext
    MGGraphicEngineContext::MGGraphicEngineContext()
    {
        texture_id = 0;
        //opaque = NULL;
    }
    MGGraphicEngineContext::~MGGraphicEngineContext()
    {
        
    }
    void MGGraphicEngineContext::initWithData(byte_8*& data, PixelFormat pixel_format, uint_16 pixel_width, uint_16 pixel_height)
    {
		//LogStart();
        if (texture_id != 0) {
            MGLogD("Delete Texture ID : %d.", texture_id);
            glDeleteTextures(1, &texture_id);
			texture_id=0;
        }
		initWithDataWithoutDelete(data,pixel_format,pixel_width,pixel_height);
        
		delete_array_and_set_null(data);
		//LogEnd("MGGraphicEngineContext::initWithData");
    }

	void MGGraphicEngineContext::initWithDataWithoutDelete( byte_8*& data, PixelFormat pixel_format, uint_16 pixel_width, uint_16 pixel_height )
	{
		if (texture_id==0)
		{
			glGenTextures(1, &texture_id);
            //MGLogD("Gen Texture ID : %d.", texture_id);
		}
		glBindTexture(GL_TEXTURE_2D, texture_id);
		

		// Specify OpenGL texture image

		switch(pixel_format)
		{
		case PF_RGBA8888:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pixel_width, pixel_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			break;
		case PF_RGBA8880:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pixel_width, pixel_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			break;
		case PF_RGBA4444:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pixel_width, pixel_height, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, data);
			break;
		case PF_RGBA5551:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pixel_width, pixel_height, 0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, data);
			break;
		case PF_RGBA5650:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pixel_width, pixel_height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, data);
			break;
		case PF_RGBA0008:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, pixel_width, pixel_height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, data);
			break;
        case PF_GRAYA88:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, pixel_width, pixel_height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data);
            break;
        case PF_GRAY8:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, pixel_width, pixel_height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
            break;
		default:
			break;
		}
		GLenum err=glGetError();
		if(err!=0)
		{
			char err_msg[3][32]={"GL_INVALID_ENUM","GL_INVALID_VALUE","GL_INVALID_OPERATION"};
			err-=GL_INVALID_ENUM;
			if (err<=2)
			{
				MGLogD("glTexImage2D failed(%s) while MGImage::sendDataToContext",err_msg[err]);
			}
			
			
		}
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	void MGGraphicEngineContext::releaseContext()
	{
        if (texture_id == 0) {
            return;
        }
		glDeleteTextures(1, &texture_id);
		texture_id=0;
	}


} //namespace z11
