/*
 * =====================================================================================
 *
 *       Filename:  MGCanvas.h
 *
 *    Description:  Canvas for map
 *
 *        Version:  1.0
 *        Created:  03/20/2012 12:25:48 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#pragma once

#include "MGPath.h"
#include "MGPaint.h"
#include "MGPersistenceImage.h"
#include "../CommonBase/Memory/MGRefObject.h"
#include "Canvas/CGCanvas.h"
namespace z11
{
	typedef Ref<MGImage> MGImageRef;
	typedef Ref<CGCanvas> CGCanvasRef;	
	class MGCanvas
	{
	public:
		MGCanvas(unsigned int width,unsigned int height,unsigned int bits=16);
		MGCanvas(const Ref<MGImage> &buffer_image=NULLREF);
		void setBufferImage(const Ref<MGImage> &buffer_image);
		 Ref<MGImage>  image();
		~MGCanvas();

		void drawPath(const MGGraphicPath & path,const MGPaint &paint);
		void drawSweep(const MGRect&rect, float angle);
		void drawFuzzyRect(const MGRect&rect,float rx,float ry,const RGBAColorRef&ref);
		void drawGradientRoundedRect(const MGRect& rect,float rx,float ry,MGPoint pts[2],const vector<RGBAColorRef> &colors,const vector<float> &pos);
		void drawRoundedRect(const MGRect& rect,float rx,float ry,const RGBAColorRef & colors);
		void drawTriangle(const MGPoint& first , const MGPoint& second , const MGPoint& third , const RGBAColorRef & colors);
		void drawBubble(const MGRect&rect,float alpha = 0.9412, const RGBAColorRef &bubble_color = RGBAColorRef::black());
		void drawDisclosureButton(const MGRect&rect);
		void drawStrokeRoundedRect(const MGRect& rect,float rx,float ry,float stroke_width,const RGBAColorRef & colors);
		void drawCircle(float center_x,float center_y, float radius,const RGBAColorRef & colors,bool isFilled,float stroke_width=0.0);
        void drawCircleSector(MGRect rect, float radius,float start_angle,float end_angle,const RGBAColorRef & colors,bool isFilled,float stroke_width=0.0);
		void writeToFile(const std::string & path, bool reverse = false);
		//draw//
		void drawArcSector(MGRect rect, float edge,float start_angle,float angle,const RGBAColorRef & colors,bool isFilled,float stroke_width=0.0);
		//void drawShaderCircleSector(MGRect rect, float radius,float start_angle,float end_angle,bool isFilled,float stroke_width);

		//Temporary public
		float measureText(const std::string &text,float font_width,MGRect *bounds,float scale=0) const;
		void drawText(const std::string &text,const MGRect &rect,MGFont *font,MITextAlignment alignment=MITextAlignmentLeftTop);
		void PolygonFill(const vector<MGPoint> &points , const RGBAColorRef& color);
		void PolygonFillAA(const vector<MGPoint> &points , const RGBAColorRef& color);
		void FastPolygonFillAA(const vector<MGPoint> &points , const RGBAColorRef& color);
		void PolygonAA(const vector<MGPoint> &points , const RGBAColorRef& color,float size);
		void PolylineAA(const vector<MGPoint> &points , const RGBAColorRef& color,float size );
		void FastPolylineAA(const vector<MGPoint> &points , const RGBAColorRef& color,float size );

		void Line(const MGPoint & first,const MGPoint &second, const RGBAColorRef& color,float size );
		////反走样直线  //
        void drawDashLine(const MGPoint &first, const MGPoint &second, const vector<float> &intervals, float size = 1.0f, const RGBAColorRef &color = RGBAColorMake(255, 255, 255), float dash_phase = 0.0f);
		void AlphaBlendPixel(uint_16* pt, uint_16 color, uint_16 wAlpha);
		void LineAA(const MGPoint & first,const MGPoint &second,const RGBAColorRef& color,float size);
		void Rect( const MGRect& rect,RGBAColorRef color = RGBAColorMake(255,255,255),float size=1);
		void RectFill(const MGRect& rect,RGBAColorRef color_ref = RGBAColorMake(255,255,255));

		////两像素反走样的直线  //
		void LineAA_2(const MGPoint & first,const MGPoint &second,const RGBAColorRef& color_ref,float size);
		//virtual void LineAA_two(MyDC hDC, int x1, int y1, int x2, int y2);
		void drawImage( MGImage* image, const MGRect& rect,bool reverse=true);

        void drawRoundImgae(MGImage *image);
        void drawBezierCurve(const vector<MGPoint> &points, const RGBAColorRef &color, float size, bool is_fill=false);
        void drawSinCurve(const vector<MGPoint> &points, const RGBAColorRef &color, float size, bool is_fill=false);
        
        void drawImageNew(MGImage *image, const MGRect &rect);
        
		void draw(bool isUpdate);
		void draw(bool isUpdate,const MGRect &rect);
		void draw();
		void clear();
		Ref<CGCanvas> sk_canvas;
	private:
		Ref<MGImage> m_buffer_image;
		size_t canvas_width;
		size_t canvas_height;
		size_t canvas_pitch;
		MGRect2 canvas_clip;
	};
}
