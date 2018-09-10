//
//  MGGraphicEngine.h
//  GraphicCore
//
//  Created by Wang ZhiPeng on 11-6-20.
//  Copyright 2011年Zhejiang University. All rights reserved.
//
#pragma once

#include "../CommonBase/MGObject.h"
#include "../CommonBase/MGStructure.h"
#include "../System/Shared/Syncable.h"
#include "GraphicStructure.h"
#include <vector>
#include <string>

#include "../Platform/MGGL.h"

#include "MGGraphicEngineContext.h"

using namespace std;

#ifndef _DEFAULT_ROUND_RATE_
#define _DEFAULT_ROUND_RATE_ 10.0f
#endif //_DEFAULT_ROUND_RATE_

namespace z11
{
    class MGFont;
    class MGImage;

	// 当需要显示的文字内容超出显示区域的时候，文字的显示策略 //
    enum
	{
		MILineBreakModeWordWrap = 0,    // 单词换行，主要针对英文 //
		MILineBreakModeCharacterWrap =1,    // 字符换行 //
		MILineBreakModeClip=0x2,    // 区域裁剪，向输入方向的反向平移 //
		MILineBreakModeHeadTruncation=0x4,  // 字符头部截断，比如头部显示... //
        MILineBreakModeTailTruncation=0x8,  // 字符尾部截断，比如尾部显示... //
		MILineBreakModeMiddleTruncation=0x10,   // 字符中间截断，比如中间显示... //
    };
	typedef uint_32 MILineBreakMode;
	enum {
		MITextAlignmentLeft=0x10,
		MITextAlignmentCenter=0x20,
		MITextAlignmentRight=0x40,
		MITextAlignmentTop=0x1,
		MITextAlignmentMiddle=0x2,
		MITextAlignmentBottom=0x4,
		MITextAlignmentLeftTop		= MITextAlignmentLeft | MITextAlignmentTop,
		MITextAlignmentLeftMiddle	= MITextAlignmentLeft | MITextAlignmentMiddle,
		MITextAlignmentLeftBottom	= MITextAlignmentLeft | MITextAlignmentBottom,
		MITextAlignmentCenterTop	= MITextAlignmentCenter | MITextAlignmentTop,
		MITextAlignmentCenterMiddle	= MITextAlignmentCenter | MITextAlignmentMiddle,
		MITextAlignmentCenterBottom	= MITextAlignmentCenter | MITextAlignmentBottom,
		MITextAlignmentRightTop		= MITextAlignmentRight | MITextAlignmentTop,
		MITextAlignmentRightMiddle	= MITextAlignmentRight | MITextAlignmentMiddle,
		MITextAlignmentRightBottom	= MITextAlignmentRight | MITextAlignmentBottom,
	};
	typedef uint_32 MITextAlignment;
    enum
    {
        ImageScaleToFill,   // 图像缩放以填满绘图区域 //
        ImageAspectFill,    // 图像根据自身比例填满绘图区域 //
        ImageAspectFit,     // 图像根据自身比例在绘图区域完全显示 //
        ImageCustomFit,     // 在绘图区域显示图像的指定部分 //
        ImageCustomOriginalCenter,   // 自定义原本大小显示图像指定部分 //
        ImageRepeat,        // 图像保持自身大小平铺，平铺图像大小只能是2的幂次 //
        ImageMirrorRepeat,  // 图像保持自身大小进行镜像平铺，平铺图像大小只能是2的幂次 //
        ImageClamp          // 图像边缘进行拉伸以填充绘图区域，图像大小只能是2的幂次 //
    };
    typedef uint_32 DrawImageMode;
	enum {
        RoundRectNone = 0x00,
		RoundRectLeftTop=0x01,
		RoundRectLeftBottom=0x02,
		RoundRectRightBottom=0x04,
		RoundRectRightTop=0x08,
		RoundRectLeft	= RoundRectLeftTop | RoundRectLeftBottom,
		RoundRectBottom	= RoundRectLeftBottom | RoundRectRightBottom,
		RoundRectRight	= RoundRectRightTop | RoundRectRightBottom,
		RoundRectTop	= RoundRectLeftTop | RoundRectRightTop,
		RoundRectAll	= RoundRectLeftTop | RoundRectLeftBottom | RoundRectRightTop | RoundRectRightBottom,
	};
	typedef uint_32 DrawRoundRectMode;
    class MGGraphicEngine : public MGObject,Syncable
    {
    protected:
        MGGraphicEngine();
//        static int matrix_stack_ref;
    public:
        virtual ~MGGraphicEngine();
        static MGGraphicEngine* sharedGraphicEngine();
    private:
        static MGGraphicEngine* singleton;
    public:
        // Interface for GUI
		virtual void drawLine(const MGPoint& first, const MGPoint& second, RGBAColorRef color = RGBAColorMake(255,255,255), float size = 1.0f, RGBAColorRef blend = RGBAColorMake(255,255,255,255));
		virtual void drawDashLine(const MGPoint& first, const MGPoint& second, RGBAColorRef color = RGBAColorMake(255,255,255), float size = 1.0f,const vector<float>&lengths=vector<float>(), float dash_phase=0.0f,RGBAColorRef blend = RGBAColorMake(255,255,255,255));
		virtual void drawPolyLines(const vector<MGPoint>& points,RGBAColorRef color = RGBAColorMake(255,255,255), float size = 1.0f, RGBAColorRef blend = RGBAColorMake(255,255,255,255));
		virtual void drawDashPolyLines(const vector<MGPoint>& points,RGBAColorRef color = RGBAColorMake(255,255,255), float size = 1.0f,const vector<float>&lengths=vector<float>(),float dash_phase=0.0f, RGBAColorRef blend = RGBAColorMake(255,255,255,255));
        virtual void drawPolyLoopLines(const vector<MGPoint>& points,RGBAColorRef color = RGBAColorMake(255,255,255), float size = 1.0f, RGBAColorRef blend = RGBAColorMake(255,255,255,255));
        virtual void drawFilledPolyLoopLines(const vector<MGPoint>& points,RGBAColorRef color = RGBAColorMake(255,255,255), float size = 1.0f, RGBAColorRef blend = RGBAColorMake(255,255,255,255));
        virtual void drawPoint(const MGPoint& point,RGBAColorRef color = RGBAColorMake(255,255,255), float size = 1.0f, RGBAColorRef blend = RGBAColorMake(255,255,255,255));
        virtual void drawPoints(const vector<MGPoint>& points,RGBAColorRef color = RGBAColorMake(255,255,255), float size = 1.0f, RGBAColorRef blend = RGBAColorMake(255,255,255,255));
        virtual void drawRect(const MGRect& rect, RGBAColorRef color = RGBAColorMake(255,255,255), float size = 1.0f, RGBAColorRef blend = RGBAColorMake(255,255,255,255));
        virtual void drawRoundRect(const MGRect& rect, RGBAColorRef color = RGBAColorMake(255,255,255), float round_rate = _DEFAULT_ROUND_RATE_, float size = 1.0f, RGBAColorRef blend = RGBAColorMake(255,255,255,255), DrawRoundRectMode round_mode=RoundRectAll);
		virtual void drawRoundAbovedRect(const MGRect& rect, RGBAColorRef color = RGBAColorMake(255,255,255), float round_rate = _DEFAULT_ROUND_RATE_, float size = 1.0f, RGBAColorRef blend = RGBAColorMake(255,255,255,255));
		virtual void drawRoundBelowedRect(const MGRect& rect, RGBAColorRef color = RGBAColorMake(255,255,255), float round_rate = _DEFAULT_ROUND_RATE_, float size = 1.0f, RGBAColorRef blend = RGBAColorMake(255,255,255,255));
        virtual void drawFilledRect(const MGRect& rect,RGBAColorRef color = RGBAColorMake(255,255,255), RGBAColorRef blend = RGBAColorMake(255,255,255,255));
        virtual void drawFilledRoundRect(const MGRect& rect,RGBAColorRef color = RGBAColorMake(255,255,255), float round_rate = _DEFAULT_ROUND_RATE_, RGBAColorRef blend = RGBAColorMake(255,255,255,255), DrawRoundRectMode round_mode=RoundRectAll);
		virtual void drawFilledRoundAbovedRect(const MGRect& rect,RGBAColorRef color = RGBAColorMake(255,255,255), float round_rate = _DEFAULT_ROUND_RATE_, RGBAColorRef blend = RGBAColorMake(255,255,255,255));
		virtual void drawFilledRoundBelowedRect(const MGRect& rect,RGBAColorRef color = RGBAColorMake(255,255,255), float round_rate = _DEFAULT_ROUND_RATE_, RGBAColorRef blend = RGBAColorMake(255,255,255,255));
		virtual void drawFilledLightedRect(const MGRect& rect,RGBAColorRef color = RGBAColorMake(255,255,255),float start_alpha=0,float end_alpha=180);
		virtual void drawFilledLightedRoundRect(const MGRect& rect,RGBAColorRef color = RGBAColorMake(255,255,255),float round_rate = _DEFAULT_ROUND_RATE_,float start_alpha=0,float end_alpha=180, DrawRoundRectMode round_mode=RoundRectAll, RGBAColorRef blend = RGBAColorMake(255,255,255,255));
		virtual void drawFilledGradientRect(const MGRect &rect,const std::vector<RGBAColorRef>& colors,RGBAColorRef blend=RGBAColorMake(255,255,255));
        void drawFilledCylinderRect( const MGRect &rect,RGBAColorRef colors,RGBAColorRef light = RGBAColorMake(255,255,255), bool vertical = true);
		virtual void drawImage(const MGImage* const image, const MGRect& rect, RGBAColorRef blend = RGBAColorMake(255,255,255,255,false),  DrawImageMode mode = ImageAspectFit, const MGRect& image_rect = MGRectMake(0,0,0,0),int order=0);
        virtual void drawText(const string& text, const MGFont*  font, const MGRect& rect,MILineBreakMode mode = MILineBreakModeClip, MITextAlignment text_alignment=MITextAlignmentLeftMiddle, RGBAColorRef color = RGBAColorMake(255,255,255), RGBAColorRef blend = RGBAColorMake(255,255,255,255),MGImage *cachedImage=NULL);
        virtual void rotate(const MGPoint& point = MGPointMake(0,0), MGRectRotate rotate = MGRectRotateMake(0,0,0,0));
        virtual void transition(MGRectTransition transition = MGRectTransitionMake(0,0,0));
        virtual void scale(const MGPoint& point = MGPointMake(0,0), MGRectScale scale = MGRectScaleMake(1,1,1));
        virtual void drawTriangles(vector<MGVertex> vertics,int segments,RGBAColorRef color,RGBAColorRef blend = RGBAColorRef::white());
        // Must use in pair
        virtual void enableClip(const MGRect& rect);
        virtual void disableClip();
        virtual void rectClip(MGRect& clip_rect, const MGRect& last_rect);
        // Must use in pair
        void beginDraw();
        void endDraw();

		//clear objects
        void initContext();
		void cleanContext();
		void pushContext(const MGGraphicEngineContext & _context);
	private:
		vector<MGGraphicEngineContext> vtContext;
        vector<MGRect> clip_stack;
    };
} //namespace z11
