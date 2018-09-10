//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (BitmapStructDefination.h)  
/// @brief (Bitmap结构定义)  
///  
///(Bitmap结构定义，提供Bitmap所需结构定义声明)  
///  
/// @version 0.1   (版本声明)  
/// @author        (Gu RongFang)  
/// @date          (11-07-22)  
///  
///  
///    修订说明：最初版本  
////////////////////////////////////////////////////////////////////////// 
#ifndef _BITMAPSTRUCTDEFINATION_H_
#define _BITMAPSTRUCTDEFINATION_H_

#include "../CommonBase/CommonDef.h"
namespace z11
{
#if defined(_WIN32) || defined(_WIN64)	

#ifndef _WINGDI_H_ //if has include wingdi.h

#pragma pack (1)
	typedef struct _tagBITMAPFILEHEADER {
		z11::uint_16    bfType;
		uint_32   bfSize;
		z11::uint_16    bfReserved1;
		z11::uint_16    bfReserved2;
		uint_32   bfOffBits;
	}BITMAPFILEHEADER, *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;
#pragma pack ()

	typedef struct tagBITMAPINFOHEADER {
		uint_32      biSize;
		long       biWidth;
		long       biHeight;
		z11::uint_16       biPlanes;
		z11::uint_16       biBitCount;
		uint_32      biCompression;
		uint_32      biSizeImage;
		long       biXPelsPerMeter;
		long       biYPelsPerMeter;
		uint_32      biClrUsed;
		uint_32      biClrImportant;
	} BITMAPINFOHEADER,  *LPBITMAPINFOHEADER, *PBITMAPINFOHEADER;

	typedef struct tagRGBQUAD {
		byte_8    rgbBlue;
		byte_8    rgbGreen;
		byte_8    rgbRed;
		byte_8    rgbReserved;
	} RGBQUAD,  *LPRGBQUAD;

	typedef struct tagBITMAPINFO {
		BITMAPINFOHEADER    bmiHeader;
		RGBQUAD             bmiColors[1];
	} BITMAPINFO, *LPBITMAPINFO, *PBITMAPINFO;
#ifndef BI_RGB
#define BI_RGB				0
#endif
#define BI_BITFIELDS  3L


#endif //_WINGDI_H_

#pragma pack (1)
	struct UnionBitmapStruct
	{
		BITMAPFILEHEADER bf; //指向位图文件头结构
		BITMAPINFOHEADER bi; //指向位图信息头结构
	};
#pragma pack ()


#else //linux


	typedef struct _tagBITMAPFILEHEADER {
		z11::uint_16    bfType;
		uint_32   bfSize;
		z11::uint_16    bfReserved1;
		z11::uint_16    bfReserved2;
		uint_32   bfOffBits;
	}__attribute__((packed)) BITMAPFILEHEADER, *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;

	typedef struct tagBITMAPINFOHEADER {
		uint_32      biSize;
		long       biWidth;
		long       biHeight;
		z11::uint_16       biPlanes;
		z11::uint_16       biBitCount;
		uint_32      biCompression;
		uint_32      biSizeImage;
		long       biXPelsPerMeter;
		long       biYPelsPerMeter;
		uint_32      biClrUsed;
		uint_32      biClrImportant;
	}__attribute__((packed)) BITMAPINFOHEADER,  *LPBITMAPINFOHEADER, *PBITMAPINFOHEADER;

	typedef struct tagRGBQUAD {
		byte_8    rgbBlue;
		byte_8    rgbGreen;
		byte_8    rgbRed;
		byte_8    rgbReserved;
	}__attribute__((packed)) RGBQUAD,  *LPRGBQUAD;

	typedef struct tagBITMAPINFO {
		BITMAPINFOHEADER    bmiHeader;
		RGBQUAD             bmiColors[1];
	}__attribute__((packed)) BITMAPINFO, *LPBITMAPINFO, *PBITMAPINFO;

#define BI_RGB				0
#define BI_BITFIELDS  3L

	struct UnionBitmapStruct
	{
		BITMAPFILEHEADER bf;
		BITMAPINFOHEADER bi;
	}__attribute__((packed));

#endif

}



#endif //_BITMAPSTRUCTDEFINATION_H_
