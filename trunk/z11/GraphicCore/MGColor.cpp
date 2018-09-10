//
//  MGColor.cpp
//  GraphicCore
//
//  Created by Wang ZhiPeng on 11-5-25.
//  Copyright 2011å¹?Zhejiang University. All rights reserved.
//

#include "MGColor.h"
#include "MGImage.h"
#include "../System/Shared/Utility.h"
#include "../System/Shared/MGTime.h"
#include "../System/Shared/MGLog.h"
namespace z11
{
    MGColor::MGColor()
    {
        m_image = NULL;
    }

	MGColor::~MGColor()
    {
        delete_and_set_null(m_image);
    }

	MGColor * MGColor::fromImage(const string& filename)
	{
		MGColor *ret=new MGColor;
		ret->m_color=RGBAColorMake(255,255,255,255,false);
        ret->m_image=MGImage::loadImageFromFile(filename);
		return ret;
	}

	MGColor * MGColor::fromImage( MGImage * image)
	{
		MGColor *ret=new MGColor;
		ret->m_color=RGBAColorMake(255,255,255,255,false);
		ret->m_image=image;
		return ret;
	}


	MGColor * MGColor::fromRGBA(byte_8 r,byte_8 g,byte_8 b, byte_8 a)
	{
		MGColor *ret=new MGColor;
		ret->m_color=RGBAColorMake(r,g,b,a);
		ret->m_image=NULL;
		return ret;
	}
    
    MGColor* MGColor::fromColorRef(z11::RGBAColorRef ref)
    {
        MGColor *ret=new MGColor;
		ret->m_color=ref;
		ret->m_image=NULL;
		return ret;
    }

	void MGColor::setImage( const string& filename )
	{
		delete m_image;
		m_image=MGImage::loadImageFromFile(filename);
	}

	void MGColor::setImage( MGImage *image )
	{
		delete m_image;
		m_image=image;
	}


	void MGColor::setRGBA(byte_8 r,byte_8 g,byte_8 b, byte_8 a)
	{
		m_color = RGBAColorMake(r,g,b,a);
	}
    
    void MGColor::setColorRef(RGBAColorRef ref)
    {
        m_color = ref;
    }
    void MGColor::setAlphaInByte(byte_8 _alpha)
    {
        m_color.m_alpha_component = _alpha;
    }
}