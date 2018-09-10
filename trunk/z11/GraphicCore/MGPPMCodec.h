/*
 * =====================================================================================
 *
 *       Filename:  MGPPMCodec.h
 *
 *    Description:  ppm codec
 *
 *        Version:  1.0
 *        Created:  2012年02月20日 13时43分00秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#pragma  once
#include <string>
#include <vector>
#include "MGImage.h"
namespace z11
{
	class MGPPMCodec
	{
		enum ColorFormat
		{
			// 3 bytes per pixel (packed), in RGB order regardless of endianness.
			// This is the native JPEG format.
			FORMAT_RGB,

			// 4 bytes per pixel, in RGBA order in memory regardless of endianness.
			FORMAT_RGBA,

			// 4 bytes per pixel, in BGRA order in mem regardless of endianness.
			// This is the default Windows DIB order.
			FORMAT_BGRA,
		};
		public:
			static MGImage *decode(const std::string & filepath);
			static bool decode(const std::string & filepath,MGData *&data,MGImageInfo *info);
			static MGImage *decode(const unsigned char* input, size_t input_size );
			static bool encode( const MGImage *image, std::vector<unsigned char>& output );
			static MGData *encode( const MGImage *image);
			static MGData * encode(MGData *input,const MGImageInfo &info);
	private:
		//please use " bool encode( const MGImage *image, std::vector<unsigned char>& output );"
			static bool encode(const byte_8*input,const MGImageInfo &info,std::vector<unsigned char>& output);
	};
}
