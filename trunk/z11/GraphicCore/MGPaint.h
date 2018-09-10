/*
 * =====================================================================================
 *
 *       Filename:  MGPaint.h
 *
 *    Description:  Paint for Canvas
 *
 *        Version:  1.0
 *        Created:  03/20/2012 12:27:13 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#pragma once

#include "GraphicStructure.h"
namespace z11
{
	enum MGPaintStyle
	{
		MGPaintStyleFill,
		MGPaintStyleStoke
	};
	class MGPaint
	{
	public:
		void setStyle(MGPaintStyle style);

		MGPaintStyle getStyle();
		
		void setRGBA(int r,int g,int b,int a);

		void setAlpha(int a);

		void setColorRef(const RGBAColorRef& ref);

		RGBAColorRef getColorRef();

		void setStrokeWidth(float width);

		float getStrokeWidth();
	private:
		float m_stroke_width;
		MGPaintStyle m_style;
		RGBAColorRef m_color;
	};
}
