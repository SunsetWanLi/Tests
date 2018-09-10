/*
* =====================================================================================
*
*       Filename:  MKOverlayPathView.h
*
*    Description:  The MKOverlayPathView class represents a generic 
*    overlay that draws its contents using a CGPathRef data type
*
*        Version:  1.0
*        Created:  04/12/2012 02:08:12 PM
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  Lucifer (GRF), gurongfang@gmail.com
*        Company:  ZJU
*
* =====================================================================================
*/
#pragma once
#include "MKOverlayView.h"
#include "GraphicCore/MGPath.h"
namespace z11
{
	class MGContextRef;
	class MKOverlayPathView:public MKOverlayView
	{
	public:
		//Accessing the Drawing Attributes
		MGPROPERTY(RGBAColorRef,m_fillColor,FillColor);
		MGPROPERTY(RGBAColorRef,m_strokeColor,StrokeColor);
		MGPROPERTY(float,m_lineWidth,LineWidth);
		MGPROPERTY(MGLineJoin,m_lineJoin,LineJoin);
		MGPROPERTY(MGLineCap,m_lineCap,LineCap);
		MGPROPERTY(float,m_miterLimit,MiterLimit);
		MGPROPERTY(float,m_lineDashPhase,LineDashPhase);
		void setLineDashPattern(const vector<float> &pattern);
		MKOverlayPathView();

		//Creating and Managing the Path
		MGPROPERTY(MGGraphicPath *,m_path,Path);
		virtual void createPath();
		void invalidatePath();

		void applyStrokePropertiesToContextAtZoomScale(MGContextRef *context,MKZoomScale zoomScale);
		void applyFillPropertiesToContextAtZoomScale(MGContextRef *context,MKZoomScale zoomScale);
		void strokePathInContext(MGGraphicPath *path,MGContextRef *context);
		void fillPathInContext(MGGraphicPath *path,MGContextRef *context);
		virtual ~MKOverlayPathView()
		{

		}
	protected:
		vector<float> m_lineDashPattern;
	};
}
