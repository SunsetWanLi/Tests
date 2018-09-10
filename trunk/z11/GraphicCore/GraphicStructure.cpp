//
//  GraphicStructure.cpp
//  GraphicCore
//
//  Created by Wang ZhiPeng on 11-6-20.
//  Copyright 2011å¹?Zhejiang University. All rights reserved.
//

#include "GraphicStructure.h"

namespace z11
{
    RGBAColorRef RGBAColorRef::white(byte_8 alpha)
    {
		return RGBAColorMake(255, 255, 255, alpha);
    }
    RGBAColorRef RGBAColorRef::black(byte_8 alpha)
    {
        return RGBAColorMake(0, 0, 0, alpha);
    }
    RGBAColorRef RGBAColorRef::red(byte_8 alpha)
    {
        return RGBAColorMake(255, 0, 0, alpha);
    }
    RGBAColorRef RGBAColorRef::green(byte_8 alpha)
    {
        return RGBAColorMake(0, 255, 0, alpha);
    }
    RGBAColorRef RGBAColorRef::blue(byte_8 alpha)
    {
        return RGBAColorMake(0, 0, 255, alpha);
    }
    RGBAColorRef RGBAColorRef::yellow(byte_8 alpha)
    {
        return RGBAColorMake(255, 255, 0, alpha);
    }
    RGBAColorRef RGBAColorRef::skyBlue(byte_8 alpha)
    {
        return RGBAColorMake(64, 64, 255, alpha);
    }
    RGBAColorRef RGBAColorRef::lightBlue(byte_8 alpha)
    {
        return RGBAColorMake(193, 210, 240, alpha);
    }
    RGBAColorRef RGBAColorRef::lightGray(byte_8 alpha)
    {
        return RGBAColorMake(217, 217, 217, alpha);
    }
    RGBAColorRef RGBAColorRef::darkGray(byte_8 alpha)
    {
        return RGBAColorMake(159, 159, 159, alpha);
    }
    RGBAColorRef RGBAColorRef::lightBlack(byte_8 alpha)
    {
        return RGBAColorMake(20, 20, 20, alpha);
    }
	z11::RGBAColorRef RGBAColorRef::darkBlue(byte_8 alpha)
	{
		return RGBAColorMake(50,79,133, alpha);
	}

	z11::uint_16 RGBAColorRef::convertToRGB565() const
	{
		return ((z11::uint_16)m_red_component>>3<<11)|((z11::uint_16)m_green_component>>2<<5)|((z11::uint_16)m_blue_component>>3);
	}

	z11::RGBAColorRef RGBAColorRef::fromRGB565( uint_16 _color )
	{
		return RGBAColorRef((byte_8)(_color>>11<<3),(byte_8)(_color>>5<<2),(byte_8)(_color<<3));
	}

	z11::RGBAColorRef RGBAColorRef::fromRGB( uint_32 _color )
	{
		return RGBAColorRef(byte_8(_color>>16),byte_8((_color>>8)&0xFF),byte_8(_color&0xFF));
	}

	z11::RGBAColorRef RGBAColorRef::fromARGB( uint_32 _color )
	{
		return RGBAColorRef(byte_8(_color>>16),byte_8((_color>>8)&0xFF),byte_8(_color&0xFF),byte_8(_color>>24));
	}

	z11::uint_32 RGBAColorRef::convertToARGB() const
	{
		return ((z11::uint_32)m_alpha_component<<24)|((z11::uint_32)m_red_component<<16)|((z11::uint_32)m_green_component<<8)|((z11::uint_32)m_blue_component);
	}



    RGBAColorRef RGBAColorMake(byte_8 r, byte_8 g, byte_8 b, byte_8 a, bool valid)
    {
        RGBAColorRef ret;
        ret.m_red_component = r;
        ret.m_green_component = g;
        ret.m_blue_component = b;
        ret.m_alpha_component = a;
        ret.valid_src = valid;
        return ret;
    }
    MGVertex MGVertexMake(float _x, float _y, float _z)
    {
        MGVertex vertex;
        vertex.x = _x;
        vertex.y = _y;
        vertex.z = _z;
        return vertex;
    }
    MGTexCoords MGTexCoordsMake(float _x, float _y)
    {
        MGTexCoords tex_coords;
        tex_coords.x = _x;
        tex_coords.y = _y;
        return tex_coords;
    }
} //namespace z11