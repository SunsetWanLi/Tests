/*
 * =====================================================================================
 *
 *       Filename:  MGPaint.cpp
 *
 *    Description:  Paint for Canvas
 *
 *        Version:  1.0
 *        Created:  03/20/2012 12:27:03 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */

#include "MGPaint.h"
namespace z11
{
	void MGPaint::setStyle( MGPaintStyle style )
	{
		m_style = style;
	}

	z11::MGPaintStyle MGPaint::getStyle()
	{
		return m_style;
	}

	void MGPaint::setRGBA( int r,int g,int b,int a )
	{
		m_color.m_red_component		= r;
		m_color.m_green_component	= g;
		m_color.m_blue_component	= b;
		m_color.m_alpha_component	= a;
	}

	void MGPaint::setAlpha( int a )
	{
		m_color.m_alpha_component = a;
	}

	void MGPaint::setColorRef( const RGBAColorRef& ref )
	{
		m_color = ref;
	}

	z11::RGBAColorRef MGPaint::getColorRef()
	{
		return m_color;
	}

	void MGPaint::setStrokeWidth( float width )
	{
		m_stroke_width = width;
	}

	float MGPaint::getStrokeWidth()
	{
		return m_stroke_width;
	}
}