//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2011 Zhejiang University.
///    All rights reserved.
///
/// @file    (FreeTypeDefinition.h)
/// @brief (本文件实现的功能的简述)
///
///(本文件实现的功能的详述)
///
/// @version 0.1   (版本声明)
/// @author        (Gu RongFang)
/// @date          (11-07-22)
///
///
///    修订说明：最初版本
//////////////////////////////////////////////////////////////////////////
#ifndef _FREETYPEDEFINITION_H_
#define _FREETYPEDEFINITION_H_

#include "../CommonBase/CommonDef.h"
//#include "freetype/ftoutln.h"
//#include "freetype/internal/ftobjs.h"
#include <ft2build.h>
#include "freetype/freetype.h" //FT_FREETYPE_H
#include "freetype/ftglyph.h" //FT_GLYPH_H
#include "freetype/ftoutln.h" //FT_OUTLINE_H
#include "freetype/internal/ftobjs.h"

#define FONTMAXNUM				10
#define FACESIZE				32
#define BLANKCHARWIDTH			5

////******************freetype*********************
typedef FT_Face		FONT;
typedef FT_Library	FONTLIB;
typedef struct tagLogFont
{
	z11::int_8 bShowBg;
	z11::int_8 wSize;
	z11::uint_16 wWeight;
	z11::uint_16 wEscapement;
	z11::uint_16 wColor;
	z11::uint_16 wBgColor;
	z11::int_8 sFaceName[FACESIZE];
}LogFont;

typedef struct tg_FontConf
{
	z11::int_32 nSum;
	z11::int_8 aFaceName[FONTMAXNUM][FACESIZE];
	z11::int_8 aTTFName[FONTMAXNUM][FACESIZE];
	FONT aFont[FONTMAXNUM];

	tg_FontConf()
	{
		nSum = 0;
		for (int i=0;i<FONTMAXNUM;i++)
		{
			aFaceName[i][0]=0;
			aTTFName[i][0]=0;
			aFont[i]=0;
		}
	}

	~tg_FontConf()
	{
		for(int i=0;i<nSum;i++)
		{
			FT_Done_Face(aFont[i]);
			aFont[i] = NULL;
		}
		nSum = 0;
	}
}FontConf, *PFONTCONF;

typedef PFONTCONF	FONTCONF;
//
////******************freetype*********************end
//static /*const*/
//FT_Matrix	g_Plane_Matrix= {(FT_Fixed)( 1 * 0x10000L ),
//(FT_Fixed)(0 * 0x10000L ),
//(FT_Fixed)( 0 * 0x10000L ),
//(FT_Fixed)(1 * 0x10000L )};
//static /*const*/	FT_Vector g_Plane_Vector= {0,0};


#endif //_FREETYPEDEFINITION_H_
