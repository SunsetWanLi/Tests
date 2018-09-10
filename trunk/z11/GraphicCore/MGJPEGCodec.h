//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (MGJPEGCodec.h)  
/// @brief (JPEG解析器)  
///  
///(JPEG解析器，解析JPEG文件)  
///  
/// @version 0.1   (版本声明)  
/// @author        (Gu RongFang)  
/// @date          (11-07-22)  
///  
///  
///    修订说明：最初版本  
////////////////////////////////////////////////////////////////////////// 
#ifndef _MGJPEGCODEC_H_
#define _MGJPEGCODEC_H_
#pragma once
#include <string>
#include <vector>
#include "MGImage.h"
namespace z11
{
	class MGImage;
	class MGJPEGCodec
	{
	public :
		enum ColorFormat
		{
            // 1 byte per pixel.
            FORMAT_INDEX_8,
            
			// 3 bytes per pixel (packed), in RGB order regardless of endianness.
			// This is the native JPEG format.
			FORMAT_RGB_565,

			// 4 bytes per pixel, in RGBA order in memory regardless of endianness.
			FORMAT_RGBA,

			// 4 bytes per pixel, in BGRA order in mem regardless of endianness.
			// This is the default Windows DIB order.
			FORMAT_BGRA,
            
            // According JPEG file itself.
            FORMAT_DEFAULT
		};

		// Decodes the JPEG data contained in input of length input_size. The
		// decoded data will be placed in *output with the dimensions in *w and *h
		// on success (returns true). This data will be written in the'format'
		// format. On failure, the values of these output variables is undefined.
		static bool decode(const unsigned char* input, size_t input_size,
			ColorFormat format, std::vector<unsigned char>* output,
			int* w, int* h);

		// Decodes the JPEG data contained in input of length input_size. The
		// decoded data will be placed in *output with the dimensions in *w and *h
		// on success (returns true). This data will be written in the'format'
		// format. On failure, the values of these output variables is undefined.
		static MGImage* decode(const std::string &file_path, ColorFormat format=FORMAT_DEFAULT);
		static bool decode(const std::string & filepath,MGData *&data,MGImageInfo *info);
		static MGImage* decode(const unsigned char* input, size_t input_size,ColorFormat format );

	private:
		MGJPEGCodec(void);
		~MGJPEGCodec(void);
	};

}


#endif //_MGJPEGCODEC_H_
