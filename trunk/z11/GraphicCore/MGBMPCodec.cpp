#include "MGBMPCodec.h"
#include "MGPersistenceImage.h"

#include "../CommonBase/CommonDef.h"
#include "../System/Shared/MGLog.h"
#include <stdio.h>
#include "BitmapStructDefination.h"
#include <stdlib.h>
#include <string.h>
namespace z11
{
	MGBMPCodec::MGBMPCodec(void)
{
}

MGBMPCodec::~MGBMPCodec(void)
{
}

MGImage* MGBMPCodec::decode( const std::string& filepath )
{
	BITMAPFILEHEADER	*pbmfh = NULL;
	BITMAPINFO			*pbmi = NULL;
	byte_8              *pBits = NULL;

	MGData *input=MGData::dataWithContentsOfFile(filepath);
	if (input == NULL) {
		MGLogD("Can't open input file: %s", filepath.c_str());
		return NULL;
	}

	pbmfh = (BITMAPFILEHEADER*)input->bytes();
	if (pbmfh == NULL)
	{
		MGLogD("Can't allocate memory in size:. Maybe low memory.");
		return NULL;
	}

	if ( (pbmfh->bfType != * (z11::uint_16 *) "BM") )
	{
		free (pbmfh);
		MGLogD("File content is not BMP or read error!");
		return NULL;
	}

	pbmi = (BITMAPINFO *)((z11::byte_8*)(pbmfh)+14);//android »áËÄ×Ö½Ú¶ÔÆë£¬¹Êsizeof(BITMAPINFO)=16
	pBits = (z11::byte_8 *)pbmfh + pbmfh->bfOffBits;

#pragma pack(1)

	MGImage *ret=MGImage::fromBlank(pbmi->bmiHeader.biBitCount,pbmi->bmiHeader.biWidth,pbmi->bmiHeader.biHeight);

#pragma pack()

	//*this=*ret;
	//ret->content_data=NULL;
	//delete_and_set_null(ret);

	const int content_data_bytes=ret->content_info.adapter_width*ret->content_info.bit_count/8;
	const int pBits_bytes=BYTEOFLINE(ret->content_info.width_in_pixel,ret->content_info.bit_count);
	byte_8 *pDest=static_cast<byte_8 *>(ret->getContentData());
	byte_8 *pSrc=pBits;
	for (int i=0;i<ret->content_info.height_in_pixel;i++)
	{
		memcpy(pDest,pSrc,pBits_bytes);
		pDest+=content_data_bytes;
		pSrc+=pBits_bytes;
	}
	if (ret->content_info.format==PF_RGBA8880)
	{
		ret->content_info.format=PF_BGRA8880;
	}
	//memcpy(content_data, pBits, content_info.image_size);

	delete_and_set_null(input);
	//MGLogD("Load Bitmap: %s OK", filepath.c_str());


	ret->convertPixelFormat(PF_RGBA8880);
	return ret;
}

bool MGBMPCodec::decode( const unsigned char* input, size_t input_size, ColorFormat format, std::vector<unsigned char>* output, int* w, int* h )
{
	//FILE	*hFile = NULL;
	BITMAPFILEHEADER	*pbmfh = NULL;
	BITMAPINFO			*pbmi = NULL;
	byte_8              *pBits = NULL;

	pbmfh=(BITMAPFILEHEADER*)input;

	pbmi = (BITMAPINFO *)((z11::byte_8*)(pbmfh)+14);//android »áËÄ×Ö½Ú¶ÔÆë£¬¹Êsizeof(BITMAPINFO)=16
	pBits = (z11::byte_8 *)pbmfh + pbmfh->bfOffBits;

	int bit_count=pbmi->bmiHeader.biBitCount;
	int image_width=pbmi->bmiHeader.biWidth;
	int image_height=pbmi->bmiHeader.biHeight;

	const int content_data_bytes=image_width*bit_count/8;
	const int image_size=content_data_bytes*image_height;
	*w=image_width;
	*h=image_height;
	output->resize(image_size);
	byte_8 *pDest=static_cast<byte_8 *>(&(output->at(0)));
	byte_8 *pSrc=pBits;
	const int pBits_bytes=BYTEOFLINE(image_width,bit_count);
	for (int i=image_height-1; i>=0; i--)
	//for (int i=0;i<ret->content_info.height_in_pixel;i++)
	{
		memcpy(pDest,pSrc,content_data_bytes);
		pDest+=content_data_bytes;
		pSrc=pBits+i*pBits_bytes;
	}

	if (bit_count==24)
	{
		pDest=static_cast<byte_8 *>(&(output->at(0)));
		for (int i=0;i<image_size;i+=3,pDest+=3)
		{
			byte_8 temp=pDest[0];
			pDest[0]=pDest[2];
			pDest[2]= temp;
		}
	}

	return true;
}

MGImage * MGBMPCodec::decode( const unsigned char* input, size_t input_size,ColorFormat format )
{
	BITMAPFILEHEADER	*pbmfh = NULL;
	BITMAPINFO			*pbmi = NULL;
	byte_8              *pBits = NULL;


	pbmfh=(BITMAPFILEHEADER*)input;

	pbmi = (BITMAPINFO *)((z11::byte_8*)(pbmfh)+14);//android »áËÄ×Ö½Ú¶ÔÆë£¬¹Êsizeof(BITMAPINFO)=16
	pBits = (z11::byte_8 *)pbmfh + pbmfh->bfOffBits;

#pragma pack(1)

	MGImage *ret=MGPersistenceImage::fromBlank(pbmi->bmiHeader.biBitCount,pbmi->bmiHeader.biWidth,pbmi->bmiHeader.biHeight);

#pragma pack()

	byte_8 *pDest=static_cast<byte_8 *>(ret->getContentData());
	byte_8 *pSrc=pBits;
	const int content_data_bytes=ret->content_info.adapter_width*ret->content_info.bit_count/8;
	const int pBits_bytes=BYTEOFLINE(ret->content_info.width_in_pixel,ret->content_info.bit_count);
	for (int i=0;i<ret->content_info.height_in_pixel;i++)
	{
		memcpy(pDest,pSrc,content_data_bytes);
		pDest+=content_data_bytes;
		pSrc+=pBits_bytes;
	}
	if (ret->content_info.format==PF_RGBA8880)
	{
		ret->content_info.format=PF_BGRA8880;
	}

	ret->convertPixelFormat(PF_RGBA8880);
	return ret;
}

bool MGBMPCodec::decode(const unsigned char* input, size_t input_size,
						ColorFormat format, unsigned char*& output,size_t &output_size,
						int& w, int& h )
{
	BITMAPFILEHEADER	*pbmfh = NULL;
	BITMAPINFO			*pbmi = NULL;
	byte_8              *pBits = NULL;

	pbmfh=(BITMAPFILEHEADER*)input;

	pbmi = (BITMAPINFO *)((z11::byte_8*)(pbmfh)+14);//android »áËÄ×Ö½Ú¶ÔÆë£¬¹Êsizeof(BITMAPINFO)=16
	pBits = (z11::byte_8 *)pbmfh + pbmfh->bfOffBits;

	int bit_count=pbmi->bmiHeader.biBitCount;
	int image_width=pbmi->bmiHeader.biWidth;
	int image_height=pbmi->bmiHeader.biHeight;

	const int content_data_bytes=image_width*bit_count/8;
	const int image_size=content_data_bytes*image_height;
	w=image_width;
	h=image_height;
	output=new unsigned char[image_size];
	byte_8 *pDest=output;
	byte_8 *pSrc=pBits;
	const int pBits_bytes=BYTEOFLINE(image_width,bit_count);
	for (int i=0;i<image_height;i++)
	{
		memcpy(pDest,pSrc,content_data_bytes);
		pDest+=content_data_bytes;
		pSrc+=pBits_bytes;
	}

	if (bit_count==24)
	{
		pDest=output;
		for (int i=0;i<image_size;i+=3,pDest+=3)
		{
			byte_8 temp=pDest[0];
			pDest[0]=pDest[2];
			pDest[2]= temp;
		}
	}

	return true;
}

}
