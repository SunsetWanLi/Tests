/*
* =====================================================================================
*
*       Filename:  MGCanvas.cpp
*
*    Description:  Canvas for map
*
*        Version:  1.0
*        Created:  03/20/2012 12:25:37 PM
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  Lucifer (GRF), gurongfang@gmail.com
*        Company:  ZJU
*
* =====================================================================================
*/

#include "../ExternalLib/wykobi/wykobi.hpp"
#include "../ExternalLib/wykobi/wykobi_algorithm.hpp"
#include "MGCanvas.h"
#include "MGGraphicEngine.h"
#include "../System/Shared/MGLog.h"
#include "MGFont.h"

#include "SkBitmap.h"
#include "SkCanvas.h"
#include "SkTypeface.h"
#include "SkShader.h"
#include "SkDevice.h"
#include "SkGradientShader.h"
#include "SkBlurMaskFilter.h"
#include "Canvas/CGRegionClipper.h"
#include "SkDashPathEffect.h"
#include "SkDrawFilter.h"
#include "../System/IO/MGFileManager.h"

#include <cmath>
#include <stdlib.h>
#include <cstring>
#include <algorithm>
#ifndef M_PI
#define M_PI        3.14159265358979323846264338327950288
#endif
using namespace z11;
#define INDEX_FORWORD(index)	index = (index + 1) % num;
#define INDEX_BACKWORD(index)	index = (index - 1 + num) % num;
#define DEVICE_USE_565
#define  RGB565(r, g, b)   (((uint_16)r>>3<<11)|((uint_16)g>>2<<5)|((uint_16)b>>3))
#define GetRByte(p) ((byte_8)((p)>>11<<3))
#define GetGByte(p) ((byte_8)((p)>>5<<2))
#define GetBByte(p) ((byte_8)((p)<<3))
//a 0~255
//Find this implement in FTEfficientGlyph.cpp 
extern void AlphaBlend565(uint_16 *p,byte_8 r,byte_8 g,byte_8 b,byte_8 a) ;

inline void AlphaBlend34(byte_8 dst[],byte_8 src[])
{
    dst[0]+=(((int)(src[0]-dst[0]))*src[3])>>8;
    dst[1]+=(((int)(src[1]-dst[1]))*src[3])>>8;
    dst[2]+=(((int)(src[2]-dst[2]))*src[3])>>8;
}

inline void AlphaBlend43(byte_8 dst[],byte_8 src[])
{
	dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
}

inline void AlphaBlend33(byte_8 dst[],byte_8 src[])
{
	dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
}

inline void AlphaBlend44(byte_8 dst[],byte_8 src[])
{
	dst[0]+=(((int)(src[0]-dst[0]))*src[3])>>8;
    dst[1]+=(((int)(src[1]-dst[1]))*src[3])>>8;
    dst[2]+=(((int)(src[2]-dst[2]))*src[3])>>8;
    dst[3]=src[3]>dst[3]?src[3]:dst[3];
}

inline void RGB565TORGB8880(byte_8 dst[], uint_16 rgb565)
{
    dst[0] = GetRByte(rgb565);
    dst[1] = GetGByte(rgb565);
    dst[2] = GetBByte(rgb565);
}

inline void RGB565TORGB8888(byte_8 dst[], uint_16 rgb565)
{
    dst[0] = GetRByte(rgb565);
    dst[1] = GetGByte(rgb565);
    dst[2] = GetBByte(rgb565);
    dst[3] = 255;
}

namespace z11
{
	SkColor RGBAColorRefToSkColor(const RGBAColorRef &ref)
	{
        return SkColorSetARGBInline(ref.m_alpha_component,ref.m_red_component,ref.m_green_component,ref.m_blue_component);
	}
	z11::MGCanvas::MGCanvas(unsigned int width,unsigned int height,unsigned int bits)
	{
		m_buffer_image = MGImageRef ( MGPersistenceImage::fromBlank(bits,width , height ));
		if (m_buffer_image == NULL)
		{
			MGLogD("MGCanvas: create buffer image (width=%d,height=%d,bits=%d) failed.",width,height,bits);
		}
		m_buffer_image->release();

		setBufferImage(m_buffer_image);



	}

	MGCanvas::MGCanvas(const Ref<MGImage> &buffer_image )
	{
		setBufferImage(buffer_image);
	}

	z11::MGCanvas::~MGCanvas()
	{
	}

	void z11::MGCanvas::drawPath( const MGGraphicPath & path,const MGPaint &paint )
	{

	}

	void MGCanvas::PolygonFill( const vector<MGPoint> &points , const RGBAColorRef& color_ref )
	{
		PolygonFillAA(points,color_ref);
	}

	void MGCanvas::draw(bool isUpdate)
	{
		if (m_buffer_image==NULL)
		{
			return ;
		}
		if (isUpdate)
		{
			static_cast<MGPersistenceImage *>(m_buffer_image.object_)->dirty();
			m_buffer_image->sendDataToContext();
		}

		MGGraphicEngine *engine=MGGraphicEngine::sharedGraphicEngine();
		engine->drawImage(m_buffer_image,MGRectMake(0,0,m_buffer_image->getWidth(),m_buffer_image->getHeight()), RGBAColorMake(255,255,255,255,false),ImageAspectFit, MGRectMake(0,0,0,0),1);
	}

	void MGCanvas::draw()
	{
		if (m_buffer_image==NULL)
		{
			return ;
		}
		m_buffer_image->sendDataToContext();

		MGGraphicEngine *engine=MGGraphicEngine::sharedGraphicEngine();
		engine->drawImage(m_buffer_image,MGRectMake(0,0,m_buffer_image->getWidth(),m_buffer_image->getHeight()),RGBAColorMake(255,255,255,255,false),  ImageAspectFit, MGRectMake(0,0,0,0),1);
	}

	void MGCanvas::draw( bool isUpdate,const MGRect &rect )
	{
		if (m_buffer_image==NULL)
		{
			return ;
		}
		if (isUpdate)
		{
			static_cast<MGPersistenceImage *>(m_buffer_image.object_)->dirty();
			m_buffer_image->sendDataToContext();
		}
		//m_buffer_image->sendDataToContext();

		MGGraphicEngine *engine=MGGraphicEngine::sharedGraphicEngine();
		engine->drawImage(m_buffer_image,MGRectMake(rect.x,rect.y,m_buffer_image->getWidth(),m_buffer_image->getHeight()),RGBAColorMake(255,255,255,255,false),  ImageAspectFit, MGRectMake(0,0,0,0),1);

	}

	void MGCanvas::LineAA( const MGPoint & first,const MGPoint &second,const RGBAColorRef& color_ref,float size )
	{
		vector<MGPoint> polyline;
		polyline.push_back(first);
		polyline.push_back(second);
		PolylineAA(polyline,color_ref,size);
	}

	void MGCanvas::PolygonFillAA( const vector<MGPoint> &points , const RGBAColorRef& color_ref )
	{
		if (points.empty())
		{
			return ;
		}
		SkPaint paint;
		paint.setAntiAlias(true);
		paint.setStyle(SkPaint::kFill_Style);
		//paint.setColor(SkColorSetARGBInline(color.m_alpha_component,color.m_red_component,color.m_green_component,color.m_blue_component));
		paint.setColor(RGBAColorRefToSkColor(color_ref));
		//paint.setStrokeWidth(size);
		SkPath path;
		sk_canvas->makePolygon(path,points);
		sk_canvas->impl->drawPath(path,paint);
	}

	void MGCanvas::PolygonAA( const vector<MGPoint> &points , const RGBAColorRef& color_ref,float size )
	{
		SkPaint paint;
		paint.setAntiAlias(true);
		paint.setStyle(SkPaint::kStroke_Style);
		//paint.setColor(SkColorSetARGBInline(color.m_alpha_component,color.m_red_component,color.m_green_component,color.m_blue_component));
		paint.setColor(RGBAColorRefToSkColor(color_ref));
		//paint.setStrokeWidth(size);
		SkPath path;
		sk_canvas->makePolygon(path,points);
		sk_canvas->impl->drawPath(path,paint);
	}

	void MGCanvas::PolylineAA( const vector<MGPoint> &points , const RGBAColorRef& color,float size )
	{
		vector<vector<MGPoint> > clip_points;
		if (points.size()>CGRegionClipper::CUT_NUM)
		{

			clip_points=CGRegionClipper::clipPolyline(points,MGRectMake(canvas_clip.left,canvas_clip.top,canvas_clip.right-canvas_clip.left,canvas_clip.bottom-canvas_clip.top));
		}
		else
		{
			clip_points.push_back(points);
		}

		SkPaint paint;
		paint.setAntiAlias(true);
		paint.setStyle(SkPaint::kStroke_Style);
		//paint.setColor(SkColorSetARGBInline(color.m_alpha_component,color.m_red_component,color.m_green_component,color.m_blue_component));
		paint.setColor(RGBAColorRefToSkColor(color));
		paint.setStrokeWidth(size);
		for (size_t i=0;i<clip_points.size();i++)
		{
			SkPath path;
			sk_canvas->makePolyline(path,clip_points[i]);
			sk_canvas->impl->drawPath(path,paint);
		}
	}

	void MGCanvas::Line( const MGPoint & first,const MGPoint &second, const RGBAColorRef& color_ref,float size )
	{
		LineAA(first,second,color_ref,size);
	}
    
#define        DASH_LINE_INTERVAL_MAX_COUNT            256
    void MGCanvas::drawDashLine(const MGPoint &first, const MGPoint &second, const vector<float> &intervals, float size/* = 1.0f*/, const RGBAColorRef &color/* = RGBAColorMake(255, 255, 255)*/, float dash_phase/* = 0.0f*/)
    {
		assert(!intervals.empty() && intervals.size() < DASH_LINE_INTERVAL_MAX_COUNT);
        SkScalar scalars[DASH_LINE_INTERVAL_MAX_COUNT];
        for(size_t index = 0; index < intervals.size(); ++index)
        {
            scalars[index] = intervals[index];
        }
		SkDashPathEffect dash(scalars, intervals.size(), 0);
        
        SkPath path;
        path.moveTo(first.x, first.y);
        path.lineTo(second.x, second.y);
        
        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setStyle(SkPaint::kStroke_Style);
        paint.setPathEffect(&dash);
        paint.setColor(RGBAColorRefToSkColor(color));
        paint.setStrokeWidth(size);
        
        sk_canvas->impl->drawPath(path, paint);
    }

	void MGCanvas::clear()
	{
		memset(m_buffer_image->getContentData(),128,m_buffer_image->getImageSize());
	}

	void MGCanvas::Rect( const MGRect& rect,RGBAColorRef color_ref /*= RGBAColorMake(255,255,255)*/,float size/*=1*/ )
	{
		vector<MGPoint> pts;
		pts.push_back(MGPointMake( rect.x, rect.y ));
		pts.push_back(MGPointMake(  rect.x+rect.width, rect.y ));
		pts.push_back(MGPointMake(   rect.x+rect.width, rect.y+rect.height ));
		pts.push_back(MGPointMake( rect.x, rect.y+rect.height ));
		PolygonAA(pts,color_ref,size);
	}

#define EPS 1e-6
	void MGCanvas::RectFill( const MGRect& rect,RGBAColorRef color_ref /*= RGBAColorMake(255,255,255)*/ )
	{
		if (rect.width+rect.x>canvas_width-EPS&&rect.height+rect.y>canvas_height-EPS
			&&rect.x<EPS&&rect.y<EPS)
		{
			sk_canvas->impl->clear(RGBAColorRefToSkColor(color_ref));
		}

		vector<MGPoint> pts;
		pts.push_back(MGPointMake( rect.x, rect.y ));
		pts.push_back(MGPointMake(  rect.x+rect.width, rect.y ));
		pts.push_back(MGPointMake(   rect.x+rect.width, rect.y+rect.height ));
		pts.push_back(MGPointMake( rect.x, rect.y+rect.height ));
		PolygonFillAA(pts,color_ref);
	}

	void MGCanvas::drawImage(  MGImage* image, const MGRect& rect,bool reverse)
	{
		if (image == NULL || m_buffer_image==NULL||m_buffer_image->getContentData()==NULL)
		{
			MGLogD("MGCanvas::drawImage failed");
			return ;
		}
		if (canvas_clip.right-canvas_clip.left==0)
		{
			return ;
		}
		if (m_buffer_image->content_info.format==PF_RGBA5650)
		{
            uint_16 *pData=(uint_16 *)m_buffer_image->getContentData();
            uint_16 *p=pData;
			if (image->content_info.format==PF_RGBA8880)
			{
                uint_8 *pSrcStart=image->getContentData();
                uint_8 *pSrc=pSrcStart;
                if (pData == NULL || pSrcStart==NULL) {
                    MGLogD("MGCanvas drawimage failed,pData == NULL || pSrcStart==NULL");
                    return ;
                }
				for (int i=0;i<image->getHeight();i++)
                {
                    p = pData+canvas_pitch*(i);
                    pSrc = pSrcStart+image->getAdapterWidth()*i*3;
                    for (int j=0;j<image->getWidth();j++)
                    {
                        if (reverse){
                            uint_8 tmp[3];
                            tmp[0] = pSrc[0];
                            tmp[1] = pSrc[1];
                            tmp[2] = pSrc[2];
                            RGB565TORGB8880(tmp ,*p);
                            *p = RGB565(tmp[0],tmp[1],tmp[2]);
                        }else
                            *p = RGB565(pSrc[0],pSrc[1],pSrc[2]);
                        p++;
                        pSrc+=3;
                    }
                }
			}
			else if (image->content_info.format==PF_RGBA8888)
			{
                uint_8 *pSrcStart=image->getContentData();
                uint_8 *pSrc=pSrcStart;
                if (pData == NULL || pSrcStart==NULL) {
                    MGLogD("MGCanvas drawimage failed,pData == NULL || pSrcStart==NULL");
                    return ;
                }
				for (int i=0;i<image->getHeight();i++)
				{
					p = pData+canvas_pitch*(i);
					pSrc = pSrcStart+image->getAdapterWidth()*i*4;
					for (int j=0;j<image->getWidth();j++)
					{
						if (!reverse){
                            uint_8 tmp[4];
                            tmp[0] = pSrc[0];
                            tmp[1] = pSrc[1];
                            tmp[2] = pSrc[2];
                            tmp[3] = pSrc[3];
                            RGB565TORGB8888(tmp ,*p);
                            AlphaBlend565(p,tmp[0],tmp[1],tmp[2],tmp[3]);
                        }else
                            AlphaBlend565(p,pSrc[0],pSrc[1],pSrc[2],pSrc[3]);
						p++;
						pSrc+=4;
					}
				}
			}
            else if (image->content_info.format==PF_RGBA5650)
			{
                uint_16 *pSrcStart=(uint_16 *)image->getContentData();
                uint_16 *pSrc=pSrcStart;
                if (pData == NULL || pSrcStart==NULL) {
                    MGLogD("MGCanvas drawimage failed,pData == NULL || pSrcStart==NULL");
                    return ;
                }
				for (int i=0;i<image->getHeight();i++)
				{
					p = pData+canvas_pitch*(i);
					pSrc = pSrcStart+image->getAdapterWidth()*i;
					for (int j=0;j<image->getWidth();j++)
					{
						if (reverse)
                            *p = *pSrc;
						p++;
						pSrc++;
					}
				}
			}
		}
		else if(m_buffer_image->content_info.format==PF_RGBA8880)
		{
            uint_8 *pData=(uint_8 *)m_buffer_image->getContentData();
            uint_8 *p=pData;
            
			if (image->content_info.format==PF_RGBA8888)
			{
                uint_8 *pSrcStart=image->getContentData();
                uint_8 *pSrc=pSrcStart;
                if (pData == NULL || pSrcStart==NULL) {
                    MGLogD("MGCanvas drawimage failed,pData == NULL || pSrcStart==NULL");
                    return ;
                }
				for (int i=0;i<image->getHeight();i++)
				{
					p = pData+canvas_pitch*(i)*3;
					pSrc = pSrcStart+image->getAdapterWidth()*i*4;
					for (int j=0;j<image->getWidth();j++)
					{
						//MGLogD("i=%d,j=%d",i,j);
                        if (!reverse){
                            uint_8 tmp[4];
                            tmp[0] = pSrc[0];
                            tmp[1] = pSrc[1];
                            tmp[2] = pSrc[2];
                            tmp[3] = pSrc[3];
                            AlphaBlend43(tmp,p);
                            AlphaBlend34(p,tmp);
                        }else
                            AlphaBlend34(p,pSrc);
						p+=3;
						pSrc+=4;
					}
				}
			}
            if (image->content_info.format==PF_RGBA8880)
			{
                uint_8 *pSrcStart=image->getContentData();
                uint_8 *pSrc=pSrcStart;
                if (pData == NULL || pSrcStart==NULL) {
                    MGLogD("MGCanvas drawimage failed,pData == NULL || pSrcStart==NULL");
                    return ;
                }
				for (int i=0;i<image->getHeight();i++)
				{
					p = pData+canvas_pitch*(i)*3;
					pSrc = pSrcStart+image->getAdapterWidth()*i*3;
					for (int j=0;j<image->getWidth();j++)
					{
						//MGLogD("i=%d,j=%d",i,j);
                        if (reverse)
                            AlphaBlend33(p,pSrc);
						p+=3;
						pSrc+=3;
					}
				}
			}
            if (image->content_info.format==PF_RGBA5650)
			{
                uint_16 *pSrcStart=(uint_16 *)image->getContentData();
                uint_16 *pSrc=pSrcStart;
                if (pData == NULL || pSrcStart==NULL) {
                    MGLogD("MGCanvas drawimage failed,pData == NULL || pSrcStart==NULL");
                    return ;
                }
				for (int i=0;i<image->getHeight();i++)
				{
					p = pData+canvas_pitch*(i)*3;
					pSrc = pSrcStart+image->getAdapterWidth()*i;
					for (int j=0;j<image->getWidth();j++)
					{
						//MGLogD("i=%d,j=%d",i,j);
                        if (!reverse){
                            uint_16 tmp;
                            tmp = RGB565(p[0],p[1],p[2]);
                            RGB565TORGB8880(p,tmp);
                        }else
                            RGB565TORGB8880(p,*pSrc);
						p+=3;
						pSrc++;
					}
				}
			}
		}
		else if(m_buffer_image->content_info.format==PF_RGBA8888)
		{
            uint_8 *pData=(uint_8 *)m_buffer_image->getContentData();
            uint_8 *p=pData;
			if (image->content_info.format==PF_RGBA8888)
			{
                uint_8 *pSrcStart = image->getContentData();
                uint_8 *pSrc = pSrcStart;
                if (pData != NULL && pSrcStart != NULL)
                {
                    for (int canvas_index = 0; canvas_index < image->getHeight() && canvas_index < canvas_height; ++canvas_index)
                    {
                        p = pData+canvas_pitch*(canvas_index)*4;
                        if(!reverse)
                        {
                            pSrc = pSrcStart+image->getAdapterWidth()*canvas_index*4;
                            //memcpy(p, pSrc, image->getWidth()*4);
                            for (int j = 0; j < image->getWidth(); ++j)
                            {
                                AlphaBlend44(p, pSrc);
                                p += 4;
                                pSrc += 4;
                            }
                        }
                        else
                        {
                            int image_height_index = image->getHeight()-1-canvas_index;
                            pSrc = pSrcStart+image->getAdapterWidth()*image_height_index*4;
//                            memcpy(p, pSrc, image->getWidth()*4);
                            for (int j = 0; j < image->getWidth(); ++j)
                            {
                                AlphaBlend44(p, pSrc);
                                p += 4;
                                pSrc += 4;
                            }
                        }
                    }
                }
			}
            if (image->content_info.format==PF_RGBA8880)
			{
                uint_8 *pSrcStart=image->getContentData();
                uint_8 *pSrc=pSrcStart;
                if (pData == NULL || pSrcStart==NULL) {
                    MGLogD("MGCanvas drawimage failed,pData == NULL || pSrcStart==NULL");
                    return ;
                }
				for (int i=0;i<image->getHeight();i++)
				{
					p = pData+canvas_pitch*(i)*4;
					pSrc = pSrcStart+image->getAdapterWidth()*i*3;
					for (int j=0;j<image->getWidth();j++)
					{
						//MGLogD("i=%d,j=%d",i,j);
                        if (!reverse){
                            uint_8 tmp[3];
                            tmp[0] = pSrc[0];
                            tmp[1] = pSrc[1];
                            tmp[2] = pSrc[2];
                            AlphaBlend34(tmp, p);
                            AlphaBlend43(p,tmp);
                        }else
                            AlphaBlend43(p,pSrc);
						p+=4;
						pSrc+=3;
					}
				}
			}
            if (image->content_info.format==PF_RGBA5650)
			{
                uint_16 *pSrcStart=(uint_16 *)image->getContentData();
                uint_16 *pSrc=pSrcStart;
                if (pData == NULL || pSrcStart==NULL) {
                    MGLogD("MGCanvas drawimage failed,pData == NULL || pSrcStart==NULL");
                    return ;
                }
				for (int i=0;i<image->getHeight();i++)
				{
					p = pData+canvas_pitch*(i)*4;
					pSrc = pSrcStart+image->getAdapterWidth()*i;
					for (int j=0;j<image->getWidth();j++)
					{
						//MGLogD("i=%d,j=%d",i,j);
                        if (!reverse){
                            uint_16 tmp;
                            tmp = *pSrc;
                            AlphaBlend565(&tmp,p[0],p[1],p[2],p[3]);
                            RGB565TORGB8888(p ,tmp);
                        }else
                            RGB565TORGB8888(p ,*pSrc);
						p+=4;
						pSrc++;
					}
				}
			}
		}
	}

	void MGCanvas::FastPolygonFillAA( const vector<MGPoint> &points , const RGBAColorRef& color )
	{
		if (points.size()>CGRegionClipper::CUT_NUM)
		{
			vector<MGPoint> clip_points=points;
			CGRegionClipper::clipRect(clip_points,MGRectMake(canvas_clip.left,canvas_clip.top,canvas_clip.right-canvas_clip.left,canvas_clip.bottom-canvas_clip.top));

			PolygonFillAA(clip_points,color);
		}
		else
		{
			PolygonFillAA(points,color);
		}
	}

	void MGCanvas::setBufferImage(const Ref<MGImage> &buffer_image)
	{
		m_buffer_image = buffer_image;

		if (buffer_image != NULL)
		{
			canvas_width	= m_buffer_image->getWidth();
			canvas_height	= m_buffer_image->getHeight();
			canvas_pitch	= m_buffer_image->getAdapterWidth();

			//allocate SkBitmap & SkCanvas
			CGBitmap *bitmap=CGBitmap::bitmapWithMGImage(m_buffer_image);
			sk_canvas=CGCanvasRef(new CGCanvas(*bitmap));
			sk_canvas->release();

			//SkMatrix matrix;
			//matrix.setRotate(180,canvas_width/2,canvas_height/2);
			//SkCanvas *c=sk_canvas->impl;
			//c->setMatrix(matrix);
			//c->translate(canvas_width/2, 0);
			//c->rotate(89);
			//c->translate(canvas_width/2, canvas_height/2);
			//c->translate(-canvas_width/2, -canvas_height/2);
		}
		else
		{
			canvas_width= canvas_height = canvas_pitch=0;
		}
		canvas_clip.left=0;
		canvas_clip.top=0;
		canvas_clip.right=canvas_width;
		canvas_clip.bottom=canvas_height;

	}

	void MGCanvas::drawText(const std::string &text, const MGRect &_rect, MGFont *_font, MITextAlignment text_alignment)
	{
		if(_font == NULL)
		{
			return;
		}
        
		SkPaint paint;
		paint.setAntiAlias(true);
		RGBAColorRef color =_font->getColor();
		paint.setColor(RGBAColorRefToSkColor(color));
		paint.setTextSize(_font->getFontSize());
		
        std::string font_path;
        MGFileManager* file_manager = MGFileManager::defaultManager();
        if (font_path.empty())
        {
#if MG_PLATFORM_ANDROID == MG_TARGET_PLATFORM
            font_path = "/system/fonts/DroidSansFallback.ttf";
            
            if (!file_manager->fileExistsAtPath(font_path))
            {
                font_path = "/system/fonts/DFHEIA5A.ttf"; //android 5.0
            }
            if (!file_manager->fileExistsAtPath(font_path))
            {
                font_path = "/system/fonts/NotoSansHans-Regular.otf"; //android 5.0
            }
#elif MG_PLATFORM_IOS == MG_TARGET_PLATFORM
            font_path = "/System/Library/Fonts/Cache/STHeiti-Light.ttc";
            if (!file_manager->fileExistsAtPath(font_path))
            {
                font_path = "/System/Library/Fonts/STHeiti Light.ttc"; //STHeitiSC
            }
            if (!file_manager->fileExistsAtPath(font_path))
            {
                font_path = "/System/Library/Fonts/Cache/STHeitiTC-Light.ttc"; //STHeitiTC
            }
            if(!file_manager->fileExistsAtPath(font_path))
            {
                font_path = "/System/Library/Fonts/Core/STHeiti-Light.ttc";
            }
            if(!file_manager->fileExistsAtPath(font_path))
            {
                font_path = "/System/Library/Fonts/LanguageSupport/PingFang.ttc";
            }
            
#elif MG_PLATFORM_WIN32 == MG_TARGET_PLATFORM
            font_path = "C:/WINDOWS/Fonts/STZHONGS.TTF"; //win 8
            if (!file_manager->fileExistsAtPath(font_path))
            {
                font_path = "C:/WINDOWS/Fonts/msyh.ttf"; //win 7
            }
            if (!file_manager->fileExistsAtPath(font_path))
            {
                font_path = "C:/WINDOWS/Fonts/simsun.ttc"; //win xp
            }
#else
            MGLogD("MGFTGLESFont:Unknown platform");
#endif
        }
        
        SkTypeface* font = NULL;
        if(file_manager->fileExistsAtPath(MGFont::getDefaultFontPath())){
            font_path = MGFont::getDefaultFontPath();
        }
        if(file_manager->fileExistsAtPath(font_path)){
            font = SkTypeface::CreateFromFile(font_path.c_str());
            if(font == NULL){
                font = SkTypeface::CreateFromName("STHeitiSC-Medium", SkTypeface::kNormal);
            }
        }
        else{
            font = SkTypeface::CreateFromName("STHeitiSC-Medium", SkTypeface::kNormal);
        }

        
		if (font != NULL)
		{
			//MGLogD("font not null");
			paint.setTypeface(font);
		}
        
		paint.setTextAlign(SkPaint::kLeft_Align);
		SkRect bounds;
		SkScalar width = paint.measureText(text.c_str(), text.size(), &bounds);
		MGRect rect = _rect;
		rect.y += bounds.height();
        
        int offsetX = 0;
		int offsetY = 0;
		if(text_alignment & MITextAlignmentCenter)
		{
			offsetX = (_rect.width-bounds.width())/2;
		}
		if(text_alignment & MITextAlignmentRight)
		{
			offsetX = _rect.width-bounds.width();
		}
		if(text_alignment & MITextAlignmentMiddle)
		{
			offsetY = (_rect.height-bounds.height())/2;
		}
		if(text_alignment & MITextAlignmentBottom)
		{
			offsetY = _rect.height-bounds.height();
		}
		rect.x += offsetX;
		rect.y += offsetY;
        
		bounds.fLeft += rect.x;
		bounds.fRight += rect.x;
		bounds.fTop += rect.y;
		bounds.fBottom += rect.y;
		if (bounds.fLeft+bounds.width()<0||bounds.fRight-bounds.width()>canvas_width||bounds.fTop+bounds.height()<0||bounds.fBottom-bounds.height()>canvas_height)
		{
            MGLogD("MGCanvas::drawText: RECT???");
			return;
		}
		
		sk_canvas->impl->drawText(text.c_str(), text.size(), rect.x,rect.y, paint);
	}

	void MGCanvas::writeToFile( const std::string & path, bool reverse)
	{
		sk_canvas->writeToFile(path, m_buffer_image, reverse);
	}

	void MGCanvas::drawSweep(const MGRect&rect, float angle)
	{
		int width;
		int height;
		width=rect.width;
		height=rect.height;

		SkRect  r;
		SkPaint p;

		p.setAntiAlias(true);
		//    p.setDither(true);
		p.setStrokeWidth(SkIntToScalar(width/10));
		p.setStyle(SkPaint::kStroke_Style);

		r.set(0, 0, SkIntToScalar(width), SkIntToScalar(height));

		//    SkColor colors[] = { SK_ColorRED, SK_ColorBLUE, SK_ColorGREEN, SK_ColorCYAN };

		SkColor colors[] = { RGBAColorRefToSkColor(RGBAColorRef::fromARGB(0x4c737373)), 
			RGBAColorRefToSkColor(RGBAColorRef::fromARGB(0x4c737373)), 
			RGBAColorRefToSkColor(RGBAColorRef::fromARGB(0xff00d3ff))};
		SkShader* s = SkGradientShader::CreateSweep(r.centerX(), r.centerY(),
			colors, NULL, SK_ARRAY_COUNT(colors));
		p.setShader(s)->unref();

		SkCanvas *c=sk_canvas->impl;
		SkAutoCanvasRestore acr(c, true);

		c->translate(r.centerX(), r.centerY());
		c->rotate(angle);
		c->translate(-r.centerX(), -r.centerY());

		//SkRect bounds = r;
		r.inset(p.getStrokeWidth(), p.getStrokeWidth());
		//SkRect innerBounds = r;

		if (true) {
			c->drawOval(r, p);
		}
	}

	void MGCanvas::drawFuzzyRect( const MGRect&rect,float rx,float ry,const RGBAColorRef&color )
	{
		SkBlurMaskFilter::BlurStyle fStyle=SkBlurMaskFilter::kOuter_BlurStyle;

		SkPaint paint;
		paint.setAntiAlias(true);
		//paint.setTextSize(25);

		SkBlurMaskFilter::BlurFlags flags = SkBlurMaskFilter::kHighQuality_BlurFlag;
		paint.setColor(RGBAColorRefToSkColor(color));
		SkMaskFilter* mf = SkBlurMaskFilter::Create(20,fStyle,flags);
		paint.setMaskFilter(mf)->unref();

		SkRect skrect=SkRect::MakeXYWH(rect.x+30,rect.y+30,rect.width*2,rect.height*2);
		sk_canvas->impl->scale(0.5f,0.5f);
		sk_canvas->impl->drawRoundRect(skrect,rx*2,ry*2,paint);
		paint.setStyle(SkPaint::kFill_Style);
		paint.setColor(SK_ColorWHITE);
		paint.setMaskFilter(NULL);
		sk_canvas->impl->drawRoundRect(skrect,rx*2,ry*2,paint);

	}

	void MGCanvas::drawGradientRoundedRect( const MGRect& rect,float rx,float ry, MGPoint _pts[2], const vector<RGBAColorRef> &_colors, const vector<float> &pos )
	{
		if (_pts==NULL||_colors.empty()||_colors.size()!=pos.size())
		{
			return ;
		}
		SkPoint pts[2]={
			{
				_pts[0].x,_pts[0].y
			},
			{
				_pts[1].x,_pts[1].y
				}
		};
		SkColor *colors=new SkColor[_colors.size()];
		for (size_t i=0;i<_colors.size();i++ )
		{
			colors[i]=RGBAColorRefToSkColor(_colors[i]);
		}
		SkShader *shader=SkGradientShader::CreateLinear(pts,colors,&pos[0],MINVALUE(pos.size(),_colors.size()),SkShader::kClamp_TileMode);
		SkPaint paint;
		paint.setAntiAlias(true);
		paint.setDither(true);
		paint.setShader(shader)->unref();
		sk_canvas->impl->drawRoundRect(SkRect::MakeXYWH(rect.x,rect.y,rect.width,rect.height),rx,ry,paint);
		delete_array_and_set_null(colors);
	}

	void MGCanvas::drawRoundedRect( const MGRect& rect,float rx,float ry,const RGBAColorRef & colors )
	{
		SkPaint paint;
		paint.setAntiAlias(true);
		paint.setColor(RGBAColorRefToSkColor(colors));
		paint.setXfermodeMode(SkXfermode::kSrcOver_Mode);
		sk_canvas->impl->drawRoundRect(SkRect::MakeXYWH(rect.x,rect.y,rect.width,rect.height),rx,ry,paint);
	}
	void MGCanvas::drawStrokeRoundedRect(const MGRect& rect,float rx,float ry,float stroke_width,const RGBAColorRef & colors)
	{
		SkPaint paint;
		paint.setAntiAlias(true);
		paint.setColor(RGBAColorRefToSkColor(colors));
		paint.setXfermodeMode(SkXfermode::kSrcOver_Mode);
		paint.setStyle(SkPaint::kStroke_Style);
		paint.setStrokeWidth(stroke_width);
		sk_canvas->impl->drawRoundRect(SkRect::MakeXYWH(rect.x,rect.y,rect.width,rect.height),rx,ry,paint);
	}
	void MGCanvas::drawCircle(float center_x,float center_y, float radius,const RGBAColorRef & colors,bool isFilled,float stroke_width)
	{
		SkPaint paint;
		paint.setAntiAlias(true);
		paint.setColor(RGBAColorRefToSkColor(colors));
		paint.setXfermodeMode(SkXfermode::kSrcOver_Mode);
		paint.setStrokeWidth(stroke_width);
		if(isFilled)
			paint.setStyle(SkPaint::kFill_Style);
		else
			paint.setStyle(SkPaint::kStroke_Style);
		sk_canvas->impl->drawCircle(center_x,center_y,radius,paint);
	}
    void MGCanvas::drawCircleSector(MGRect rect, float radius,float start_angle,float end_angle,const RGBAColorRef & colors,bool isFilled,float stroke_width)
    {
        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setColor(RGBAColorRefToSkColor(colors));
        paint.setXfermodeMode(SkXfermode::kSrcOver_Mode);
        paint.setStrokeWidth(stroke_width);
        SkRect skrect;
        skrect.set(rect.x, rect.y, rect.x+rect.width, rect.y+rect.height);
        sk_canvas->impl->drawArc(skrect, start_angle, end_angle, isFilled, paint);
		
    }
	void MGCanvas::drawArcSector(MGRect rect, float edge,float start_angle,float angle,const RGBAColorRef & colors,bool isFilled,float stroke_width)
	{
		SkPaint paint;
        paint.setAntiAlias(true);
        paint.setColor(RGBAColorRefToSkColor(colors));
        paint.setXfermodeMode(SkXfermode::kSrcOver_Mode);
        paint.setStrokeWidth(stroke_width);
		paint.setStyle(SkPaint::kStroke_Style);
        SkRect skrect;
		skrect.set((int)(rect.x + stroke_width / 2.0 + edge), (int)(rect.y + stroke_width / 2.0 + edge), 
			(int)(rect.x+rect.width - stroke_width / 2.0 - edge), (int)(rect.y+rect.height - stroke_width / 2.0 - edge));
		sk_canvas->impl->drawArc(skrect, start_angle, angle, isFilled, paint);
	}
	//Ω•±‰//
	//void MGCanvas::drawShaderCircleSector(MGRect rect, float radius,float start_angle,float end_angle,bool isFilled,float stroke_width)
 //   {
	//	int count = 2;
	//	SkScalar pos[2] = {0, 1.0};
	//	SkColor colors[] = {0xff616000, 0xfdd48200};
	//	SkPoint pts_arr[2] = {SkPoint::Make(rect.x, rect.y), SkPoint::Make(rect.x + rect.width, rect.y + rect.height)};
	//	SkShader *shader = SkGradientShader::CreateLinear(pts_arr, colors, pos, count, SkShader::TileMode::kMirror_TileMode, NULL);
	//	
 //       SkPaint paint;
 //       paint.setAntiAlias(true);
	//	paint.setShader(shader);
 //       paint.setXfermodeMode(SkXfermode::kSrcOver_Mode);
 //       paint.setStrokeWidth(stroke_width);		
	//	paint.setStyle(SkPaint::Style::kStroke_Style);
	//	
	//	shader->ref();
 //       SkRect skrect;
	//	skrect.set(rect.x + stroke_width, rect.y + stroke_width, rect.x+rect.width - stroke_width, rect.y+rect.height - stroke_width);
 //       sk_canvas->impl->drawArc(skrect, start_angle, end_angle, isFilled, paint);
 //   }
	void MGCanvas::drawTriangle( const MGPoint& first , const MGPoint& second , const MGPoint& third , const RGBAColorRef & color )
	{
		vector<MGPoint> points;
		//points.push_back(MGPointMake(first.x*2,first.y*2));
		//points.push_back(MGPointMake(second.x*2,second.y*2));
		//points.push_back(MGPointMake(third.x*2,third.y*2));
		points.push_back(first);
		points.push_back(second);
		points.push_back(third);

		SkPaint paint;
		paint.setAntiAlias(true);
		paint.setColor(RGBAColorRefToSkColor(color));
		SkPath path;
		sk_canvas->makePolygon(path,points);

		//SkBlurMaskFilter::BlurStyle fStyle=SkBlurMaskFilter::kSolid_BlurStyle;
		//SkBlurMaskFilter::BlurFlags flags = SkBlurMaskFilter::kHighQuality_BlurFlag;
		paint.setColor(RGBAColorRefToSkColor(color));
		//SkMaskFilter* mf = SkBlurMaskFilter::Create(20,fStyle,flags);
		//paint.setMaskFilter(mf)->unref();
		//sk_canvas->impl->scale(0.5f,0.5f);

		sk_canvas->impl->drawPath(path,paint);

	}

	void MGCanvas::drawBubble( const MGRect&_rect ,float alpha, const RGBAColorRef &bubble_color)
	{
		MGRect rect=_rect;
        if (_rect.height > 126) {
            rect.height = _rect.height - 36;
        } else {
            rect.height*=(1.0/1.4);
        }
		float radius = 0;
        if (_rect.height > 126) {
            radius = 10;
        } else {
            radius = _rect.height / 12;
        }
		rect.width -= radius;
		//rect.width=165;
		//rect.height=44;

		SkPaint paint;
		paint.setAntiAlias(true);
		paint.setColor(RGBAColorRefToSkColor(RGBAColorMake(bubble_color.m_red_component, bubble_color.m_green_component, bubble_color.m_blue_component, alpha*255.0)));

		SkPath path;
		path.moveTo(0,radius);
		path.quadTo(0,0,radius,0);
		path.lineTo(rect.width-radius,0);
		path.quadTo(rect.width,0,rect.width,radius);

		SkPath bottom_path=path;
		{
			float center_x=rect.width/2;
			float center_y=rect.height;
			float triangle_height = _rect.height - rect.height;
			paint.setColor(RGBAColorRefToSkColor(RGBAColorMake(bubble_color.m_red_component, bubble_color.m_green_component, bubble_color.m_blue_component, alpha*255.0)));
			bottom_path.lineTo(rect.width,rect.height-radius);
			bottom_path.quadTo(rect.width,rect.height,rect.width-radius,rect.height);
			bottom_path.lineTo(center_x+triangle_height,center_y);
			bottom_path.lineTo(center_x,center_y+triangle_height);
			bottom_path.lineTo(center_x-triangle_height,center_y);
			bottom_path.lineTo(radius,rect.height);
			bottom_path.quadTo(0,rect.height,0,rect.height-radius);
			bottom_path.close();

			SkBlurMaskFilter::BlurStyle fStyle=SkBlurMaskFilter::kNormal_BlurStyle;
			SkBlurMaskFilter::BlurFlags flags = SkBlurMaskFilter::kHighQuality_BlurFlag;
			SkMaskFilter* mf = SkBlurMaskFilter::Create(5,fStyle,flags);
			paint.setMaskFilter(mf)->unref();
			sk_canvas->impl->translate(radius/2,radius/2);
			sk_canvas->impl->drawPath(bottom_path,paint);
			paint.setMaskFilter(NULL);
			sk_canvas->impl->translate(-radius/2,-radius/2);
			paint.setXfermodeMode(SkXfermode::kSrc_Mode);
			sk_canvas->impl->drawPath(bottom_path,paint);
		}

		SkPath top_path=path;
		{
			//paint.setColor(RGBAColorRefToSkColor(RGBAColorRef::black(240)));
			//top_path.lineTo(rect.width,rect.height-radius);
			//top_path.lineTo(0,rect.height-radius);
			//top_path.close();
			//sk_canvas->impl->drawPath(top_path,paint);
			paint.setXfermodeMode(SkXfermode::kSrcOver_Mode);
		}

		SkPath white_path=path;
		{
			paint.setColor(RGBAColorRefToSkColor(RGBAColorRef::white(16)));
			white_path.lineTo(rect.width,rect.height/2-radius/2);
			white_path.quadTo(rect.width,rect.height/2,rect.width-radius/2,rect.height/2);
			white_path.lineTo(radius/2,rect.height/2);
			white_path.quadTo(0,rect.height/2,0,rect.height/2-radius/2);
			white_path.close();
			sk_canvas->impl->drawPath(white_path,paint);
		}


	}

	void MGCanvas::drawDisclosureButton( const MGRect&rect )
	{
		float x=rect.x+rect.width/2;
		float y=rect.y+rect.height/2;
		float r=MINVALUE(rect.width,rect.height)/2;
		SkPaint paint;
		paint.setAntiAlias(true);
		r-=1;
		paint.setColor(RGBAColorRefToSkColor(RGBAColorRef::darkGray()));
		sk_canvas->impl->drawCircle(x,y,r,paint);
		r-=2;
		paint.setColor(RGBAColorRefToSkColor(RGBAColorRef::white()));
		sk_canvas->impl->drawCircle(x,y,r,paint);
		r-=5;
		paint.setColor(RGBAColorRefToSkColor(RGBAColorMake(38,106,213)));
		sk_canvas->impl->drawCircle(x,y,r,paint);

		{
			SkAutoCanvasRestore auto_canvas(sk_canvas->impl,true);
			sk_canvas->impl->translate(6,0);
			SkPath line_path;
			float alpha=135*M_PI/180;
			float radius=r-2;
			line_path.moveTo(x+radius*cos(alpha),y+radius*sin(alpha));
			line_path.lineTo(x,y);
			alpha=-135*M_PI/180;
			line_path.lineTo(x+radius*cos(alpha),y+radius*sin(alpha));
			paint.setColor(RGBAColorRefToSkColor(RGBAColorRef::white()));
			paint.setStrokeWidth(5);
			paint.setStyle(SkPaint::kStroke_Style);
			sk_canvas->impl->drawPath(line_path,paint);
		}

		{
			SkPath white_path;
			white_path.moveTo(x-r,y);
			white_path.quadTo(x-r,y-r,x,y-r);
			white_path.quadTo(x+r,y-r,x+r,y);
			white_path.close();
			paint.setColor(RGBAColorRefToSkColor(RGBAColorRef::white(32)));
			paint.setStyle(SkPaint::kFill_Style);
			sk_canvas->impl->drawPath(white_path,paint);
		}

	}

	float MGCanvas::measureText( const std::string &text,float font_width,MGRect *bounds,float scale/*=0*/ ) const
	{
		SkPaint paint;
		paint.setAntiAlias(true);
		paint.setTextSize(font_width*1.3);
		float width=0;
		if (bounds!=NULL)
		{
			SkRect rect;
			width=paint.measureText(text.c_str(),text.size(),&rect,scale);
			bounds->x=rect.x();
			bounds->y=rect.y();
			bounds->width=rect.width();
			bounds->height=rect.height();
		}
		else
		{
			width=paint.measureText(text.c_str(),text.size(),NULL,scale);
		}
		return width;
	}

	Ref<MGImage> MGCanvas::image()
	{
		if (m_buffer_image!=NULL)
		{
			return m_buffer_image;
		}
		//MGImage *ret=MGImage::fromBlank(pbmi->bmiHeader.biBitCount,pbmi->bmiHeader.biWidth,pbmi->bmiHeader.biHeight);
		return NULLREF;
	}
    
    void MGCanvas::drawRoundImgae(MGImage *image)
    {
        float width = image->getWidth();
        float height = image->getHeight();
        float roundPx;
        float left, top, right, bottom, dst_left, dst_top, dst_right, dst_bottom;
        if (width <= height) {
            roundPx = width / 2.0;
            float clip = (height - width) / 2.0;
            left = 0;
            top = clip;
            right = width;
            bottom = height-clip;
            height = width;
            dst_left = 0;
            dst_top = 0;
            dst_right = width;
            dst_bottom = width;
        } else {
            roundPx = height / 2.0;
            float clip = (width - height) / 2.0;
            left = clip;
            right = width - clip;
            top = 0;
            bottom = height;
            width = height;
            dst_left = 0;
            dst_top = 0;
            dst_right = height;
            dst_bottom = height;
        }
        
        /*
        final Paint paint = new Paint();
        paint.setAntiAlias(true);
        Bitmap target = Bitmap.createBitmap(min, min, Config.ARGB_8888);
        
        Canvas canvas = new Canvas(target);
        canvas.drawCircle(min / 2, min / 2, min / 2, paint);
        
        paint.setXfermode(new PorterDuffXfermode(PorterDuff.Mode.SRC_IN));
        canvas.drawBitmap(source, 0, 0, paint);
        */
        
        SkPaint *paint = new SkPaint();
        paint->setAntiAlias(true);
        
        SkRect src;
        src.set(left,top,right,bottom);
        SkRect dst;
        dst.set(dst_left,dst_top,dst_right,dst_bottom);
        
        sk_canvas->impl->drawARGB(0, 0, 0, 0);
        paint->setColor(SK_ColorWHITE);
        
        sk_canvas->impl->drawCircle(roundPx, roundPx, roundPx, *paint);
        
        paint->setXfermode(SkXfermode::Create(SkProcXfermode::kSrcIn_Mode));
        
        SkBitmap *bitmap = CGBitmap::bitmapWithMGImage(image)->getImpl();
        sk_canvas->impl->drawBitmapRectToRect(*bitmap, &src, dst, paint);
        
        
        delete_and_set_null(paint);
        delete_and_set_null(bitmap);
    }
    
    void MGCanvas::drawBezierCurve(const vector<MGPoint> &points, const RGBAColorRef &color, float size, bool is_fill)
    {
        if(points.size()<2){
            return;
        }
        SkPath path;
        
        path.moveTo(points[0].x, 0);
        path.lineTo(points[0].x, points[0].y);
        for(int i=0;i<points.size()-1;i++){
            float center_x = (points[i].x+points[i+1].x)/2.0;
            float center_y = (points[i].y+points[i+1].y)/2.0;
            
            //float cx1 = (center_x-points[i].x)*0.5+points[i].x;
            float cx1 = center_x;
            float cy1 = points[i].y;
            
            //float cx2 = points[i+1].x-(points[i+1].x-center_x)*0.5;
            float cx2 = center_x;
            float cy2 = points[i+1].y;
            
            path.cubicTo(cx1, cy1, cx2, cy2, points[i+1].x, points[i+1].y);
        }
        path.lineTo(points[points.size()-1].x, 0);
        
        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setStrokeWidth(size);
        paint.setColor(RGBAColorRefToSkColor(color));
        if(is_fill){
            paint.setStyle(SkPaint::kFill_Style);
        }
        else{
            paint.setStyle(SkPaint::kStroke_Style);
        }
        
        sk_canvas->impl->drawPath(path, paint);
    }
	void MGCanvas::drawSinCurve(const vector<MGPoint> &points, const RGBAColorRef &color, float size, bool is_fill)
	{
		if(points.size()<2){
            return;
        }
        SkPath path;
        
		float h = (points[1].y - points[0].y) / 2.0;
		float start = (points[1].x - points[0].x) / 2.0;
        
		int flag = 0;
		float dela = 0, dela0 = 0;
		for(int i = 0; i < points.size() - 1; ++ i) {
			if(i == 0) dela = -start;
			else dela = 0;
			if(i == points.size() - 2) dela0 = start;
			else dela0 = 0;
			for(float x = points[i].x + dela; x <= points[i + 1].x + dela0; x++)
			{
				if(x == points[0].x - start)
					path.moveTo(points[0].x - start, -h*cos((x-points[i].x) / (start * 2) * M_PI) + points[0].y + h);
				path.lineTo(x, -h*cos((x-points[i].x) / (start * 2) * M_PI + flag * M_PI) + points[0].y + h);
			}
			flag ++;
		}
        
        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setStrokeWidth(size);
        paint.setColor(RGBAColorRefToSkColor(color));
        if(is_fill){
            paint.setStyle(SkPaint::kFill_Style);
        }
        else{
            paint.setStyle(SkPaint::kStroke_Style);
        }
        
        sk_canvas->impl->drawPath(path, paint);
	}
    void MGCanvas::drawImageNew(MGImage *image, const MGRect &rect)
    {
        SkPaint *paint = new SkPaint();
        paint->setAntiAlias(true);
        
        SkRect dst;
        dst.set(rect.x, canvas_height-rect.y-rect.height, rect.x+rect.width, canvas_height-rect.y);
        SkRect src = SkRect::MakeXYWH(0, 0, image->getWidth(), image->getHeight());
        
        SkBitmap *bitmap = CGBitmap::bitmapWithMGImage(image)->getImpl();
        paint->setXfermode(SkXfermode::Create(SkProcXfermode::kDarken_Mode));
        sk_canvas->impl->drawBitmapRectToRect(*bitmap, &src, dst, paint);
        delete_and_set_null(paint);
        delete_and_set_null(bitmap);
    }

};

