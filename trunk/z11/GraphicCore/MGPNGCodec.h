//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (MGPNGCodec.h)  
/// @brief (PNG解码器)  
///  
///(PNG解码器，用于解析PNG文件)  
///  
/// @version 0.1   (版本声明)  
/// @author        (Gu RongFang)  
/// @date          (11-07-22)  
///  
///  
///    修订说明：最初版本  
////////////////////////////////////////////////////////////////////////// 
#ifndef _MGPNGCODEC_H_
#define _MGPNGCODEC_H_
#pragma once
#include "MGImage.h"
#include <string>
#include <vector>
namespace z11
{
	class MGImage;
	class MGPNGCodec
	{
	public :
		enum ColorFormat
		{
            FORMAT_GRAY,
            FORMAT_GRAYA,
			// 3 bytes per pixel (packed), in RGB order regardless of endianness.
			// This is the native JPEG format.
			FORMAT_RGB,
			// 4 bytes per pixel, in RGBA order in memory regardless of endianness.
			FORMAT_RGBA,
			FORMAT_ALPHA,
			FORMAT_DEFAULT = FORMAT_ALPHA,
		};

		// Decodes the PNG data directly into the passed in SkBitmap. This is
		// significantly faster than the vector<unsigned char> version of Decode()
		// above when dealing with PNG files that are >500K, which a lot of theme
		// images are. (There are a lot of themes that have a NTP image of about ~1
		// megabyte, and those require a 7-10 megabyte side buffer.)
		//
		// Returns true if data is non-null and can be decoded as a png, false
		// otherwise.  
		static MGImage* decode(const std::string& input);

		static bool decode(const unsigned char* input, size_t input_size,ColorFormat& format, std::vector<unsigned char>* output, int* w, int* h);
		static bool decode(const std::string & filepath,MGData *&data,MGImageInfo *info);
		static MGImage *decode(const unsigned char* input, size_t input_size,ColorFormat format );
		static bool encode(const MGImage *image, std::vector<unsigned char>& output,int quality);
		static bool encode(const byte_8*input,const MGImageInfo &info,std::vector<unsigned char>& output,int quality,bool reverse=true);

	private:
		MGPNGCodec(void);
		~MGPNGCodec(void);
	};

}


#endif //_MGPNGCODEC_H_
