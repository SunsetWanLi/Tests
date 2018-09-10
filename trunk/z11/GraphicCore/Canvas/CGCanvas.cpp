#include "../../CommonBase/CommonDef.h"
#include "../MGImage.h"
#include "../MGPersistenceImage.h"
#include "../MGPNGCodec.h"

#include "SkCanvas.h"
#include "SkGraphics.h"
#include "SkImage.h"
#include "SkImageEncoder.h"
#include "SkImageDecoder.h"
#include "SkStream.h"
#include "SkString.h"
#include "SkTemplates.h"
#include "SkTypeface.h"
#include "CGCanvas.h"
#include <assert.h>
#include "../../System/Shared/MGLog.h"
#ifdef WIN32
#pragma comment(lib, "usp10.lib")
#endif
namespace z11
{
	extern  SkColor RGBAColorRefToSkColor(const RGBAColorRef &ref);
	SkAutoGraphics* CGCanvas::auto_graphics = NULL;
	CGCanvas::CGCanvas()
	{
		m_bitmap=NULL;
		impl=new SkCanvas();
	}

	CGCanvas::CGCanvas( const CGBitmap &bitmap )
	{
		impl=new SkCanvas(*bitmap.impl);
		m_bitmap=const_cast<CGBitmap *>(&bitmap);
		//impl->drawColor(RGBAColorRefToSkColor(RGBAColorRef::black()),SkXfermode::kClear_Mode);
	}
	CGCanvas::~CGCanvas()
	{
		delete_and_set_null(m_bitmap);
		delete_and_set_null(impl);
	}
    
	void CGCanvas::createSkAutoGraphics()
	{
		if (auto_graphics == NULL)
			auto_graphics = new SkAutoGraphics();
	}

	void CGCanvas::releaseSkAutoGraphics()
	{
		if (auto_graphics != NULL)
			delete_and_set_null(auto_graphics);
	}
    
    MGImage* CGCanvas::toImage(bool reverse)
    {
        assert(m_bitmap != NULL);
        
		SkBitmap &bitmap = *(m_bitmap->impl);
        bitmap.lockPixels();
        
        // init params
		int width = m_bitmap->getRealWidth();
		int height = m_bitmap->getRealHeight();
		unsigned char* data = (unsigned char*)bitmap.getPixels();
        PixelFormat pf;
        byte_8 bit_count;
        switch (bitmap.config())
        {
            case SkBitmap::kRGB_565_Config:
            {
                bit_count = 16;
                pf = PF_RGBA5650;
                break;
            }
            case SkBitmap::kARGB_8888_Config:
            {
                bit_count = 32;
                pf = PF_RGBA8888;
                break;
            }
            case SkBitmap::kA8_Config:
            {
                bit_count = 8;
                pf = PF_RGBA0008;
                break;
            }
            default:
            {
                assert(0);
                break;
            }
        }
        //unsigned long image_size = sizeof(z11::byte_8)*(width*height*bit_count/8);
        // init image
        MGImage* ret = MGPersistenceImage::fromBlank(bit_count, width, height, pf);
        byte_8 *pWriteData = const_cast<byte_8*>(ret->getContentData());
        const int content_data_bytes = ret->content_info.adapter_width*ret->content_info.bit_count/8;
        if (reverse) {
            for (int i = 0; i < ret->content_info.height_in_pixel; ++i)
            {
                if (pf == PF_RGBA8888) {
                    int height_offset = i * content_data_bytes;
                    for (int j = 0; j < ret->content_info.width_in_pixel; ++j) {
                        int width_offset = j * 4;
#ifdef WIN32
                        pWriteData[0] = *(data+height_offset+width_offset + 2);      // R
                        pWriteData[1] = *(data+height_offset+width_offset + 1);      // G
                        pWriteData[2] = *(data+height_offset+width_offset + 0);      // B
                        pWriteData[3] = *(data+height_offset+width_offset + 3);      // A
#else
                        pWriteData[0] = *(data+height_offset+width_offset + 0);      // R
                        pWriteData[1] = *(data+height_offset+width_offset + 1);      // G
                        pWriteData[2] = *(data+height_offset+width_offset + 2);      // B
                        pWriteData[3] = *(data+height_offset+width_offset + 3);      // A
#endif
                        pWriteData += 4;
                    }
                    pWriteData += content_data_bytes - (ret->content_info.width_in_pixel*4);
                } else {
                    memcpy(pWriteData, data+(i*content_data_bytes), content_data_bytes);
                    pWriteData += content_data_bytes;
                }
            }
        } else {
            for (int i = ret->content_info.height_in_pixel - 1; i >= 0; --i)
            {
                if (pf == PF_RGBA8888) {
                    int height_offset = i * content_data_bytes;
                    for (int j = 0; j < ret->content_info.width_in_pixel; ++j) {
                        int width_offset = j * 4;
#ifdef WIN32
                        pWriteData[0] = *(data+height_offset+width_offset + 2);      // R
                        pWriteData[1] = *(data+height_offset+width_offset + 1);      // G
                        pWriteData[2] = *(data+height_offset+width_offset + 0);      // B
                        pWriteData[3] = *(data+height_offset+width_offset + 3);      // A
#else
                        pWriteData[0] = *(data+height_offset+width_offset + 0);      // R
                        pWriteData[1] = *(data+height_offset+width_offset + 1);      // G
                        pWriteData[2] = *(data+height_offset+width_offset + 2);      // B
                        pWriteData[3] = *(data+height_offset+width_offset + 3);      // A
#endif
                        pWriteData += 4;
                    }
                    pWriteData += content_data_bytes - (ret->content_info.width_in_pixel*4);
                } else {
                    memcpy(pWriteData, data+(i*content_data_bytes), content_data_bytes);
                    pWriteData += content_data_bytes;
                }
            }
        }
        
		bitmap.unlockPixels();
        return ret;
    }
    
	void CGCanvas::writeToFile( const std::string &path , MGImage* buffer_image, bool reverse)
	{
        
        SkImageEncoder* enc=NULL;
        string path_ext ;
        if (path.size() >4 ) {
            path_ext= path.substr(path.size()-4);
        }
        if (path_ext==".png") {
            enc = SkImageEncoder::Create(SkImageEncoder::kPNG_Type);
        }
        else if(path_ext == ".jpg")
        {
            enc = SkImageEncoder::Create(SkImageEncoder::kJPEG_Type);
        }
		if (enc == NULL)
		{
			enc =SkImageEncoder::Create(SkImageEncoder::kJPEG_Type);
		}
		if(enc==NULL)
		{
			MGLogD(" SkImageEncoder::Create kJPEG_Type failed");
		}
        //allocate SkBitmap & SkCanvas
        CGBitmap *bitmap=CGBitmap::bitmapWithMGImageWithoutAdapter(buffer_image, reverse);
        
		if(!enc->encodeFile(path.c_str(), *(bitmap->impl), 100))
		{
			MGLogD(" CGCanvas::writeToFile failed");
		}
        delete_and_set_null(enc);
        delete_and_set_null(bitmap);
		return ;
	}

	void CGCanvas::makePolyline( SkPath &path,const vector<MGPoint>&points )
	{
		if (!points.empty())
		{
			path.reset();
			path.moveTo(points[0].x,points[0].y);
			for (size_t i=1;i<points.size();i++)
			{
				path.lineTo(points[i].x,points[i].y);
			}
		}
	}

	void CGCanvas::makePolygon( SkPath &path,const vector<MGPoint>&points )
	{
		makePolyline(path,points);
		path.close();
	}

	bool convertImage(std::string to_image,std::string from_image)
	{
		SkBitmap image;
		//image.setConfig(SkBitmap::kRGB_565_Config,256,256);
		//image.allocPixels();
		memset(image.getPixels(),255,image.getSize());
		if(SkImageDecoder::DecodeFile(from_image.c_str(),&image))
		{
			//image.setConfig(SkBitmap::kRGB_565_Config,256,256);
			//image.allocPixels();
			return SkImageEncoder::EncodeFile(to_image.c_str(),image,SkImageEncoder::kJPEG_Type,80);
		}
		return false;
	}
}

