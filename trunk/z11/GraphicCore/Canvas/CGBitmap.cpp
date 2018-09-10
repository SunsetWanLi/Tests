#include "CGBitmap.h"
#include "SkBitmap.h"
#include "SkColor.h"
#include "../../CommonBase/MGStructure.h"
#include "../MGImage.h"
namespace z11
{
	CGBitmap::CGBitmap(void)
		:impl(new SkBitmap())
	{
		void *pixels = impl->getPixels();
		memset(pixels,0,impl->getSize());
        data_cache = NULL;
        real_width = 0;
        real_height = 0;
	}
    
    CGBitmap * CGBitmap::bitmapWithMGImage( MGImage* buffer_image )
	{
		CGBitmap *ret=new CGBitmap();
        
		if (buffer_image!=NULL)
		{
			CGBitmap::Config config;
			switch (buffer_image->content_info.format)
			{
                case PF_RGBA5650:
                    config=CGBitmap::kRGB_565_Config;
                    break;
                case PF_RGBA8888:
                    config=CGBitmap::kARGB_8888_Config;
                    break;
                default:
                    config=CGBitmap::kNo_Config;
                    break;
			}
            ret->real_width = buffer_image->getWidth();
            ret->real_height = buffer_image->getHeight();
			ret->setConfig(config, buffer_image->getAdapterWidth(), buffer_image->getAdapterHeight());
			ret->setPixels(buffer_image->getContentData());
		}
		return ret;
	}

	CGBitmap * CGBitmap::bitmapWithMGImageWithoutAdapter( MGImage* buffer_image, bool reverse)
	{
		CGBitmap *ret=new CGBitmap();

		if (buffer_image!=NULL)
		{
			CGBitmap::Config config;
			switch (buffer_image->content_info.format)
			{
			case PF_RGBA5650:
				config=CGBitmap::kRGB_565_Config;
				break;
			case PF_RGBA8888:
				config=CGBitmap::kARGB_8888_Config;
				break;
			default:
				config=CGBitmap::kNo_Config;
				break;
			}
            ret->real_width = buffer_image->getWidth();
            ret->real_height = buffer_image->getHeight();
			ret->setConfig(config, buffer_image->getWidth(), buffer_image->getHeight());
            long image_size = buffer_image->getWidth()*buffer_image->getHeight()*buffer_image->content_info.bit_count/8;
            ret->data_cache = new byte_8[image_size];
            memset(ret->data_cache, 0, image_size);
            byte_8* src_data = buffer_image->getContentData();
            const int adapter_width_bytes = buffer_image->getAdapterWidth()*buffer_image->content_info.bit_count/8;
            const int width_bytes = buffer_image->getWidth()*buffer_image->content_info.bit_count/8;
            for (int i = 0; i < buffer_image->getHeight(); ++i) {
                byte_8* dst_line = ret->data_cache + (i*width_bytes);
                byte_8* src_line = NULL;
                if (reverse) {
                    src_line = src_data + (i*adapter_width_bytes);
                } else {
                    src_line = src_data + ((buffer_image->getHeight()-1-i)*adapter_width_bytes);
                }
                memcpy(dst_line, src_line, width_bytes);
            }
			ret->setPixels(ret->data_cache);
		}
		return ret;
	}

	CGBitmap::~CGBitmap(void)
	{
		delete_and_set_null(impl);
        delete_and_set_null(data_cache);
	}

	void CGBitmap::setConfig( Config _config, int width, int height, int rowBytes /*= 0*/ )
	{
		impl->setConfig(SkBitmap::Config(_config),width,height,rowBytes);
	}

	bool CGBitmap::allocPixels()
	{
		return impl->allocPixels();
	}

	void CGBitmap::setPixels( void* p, SkColorTable* ctable /*= NULL*/ )
	{
		impl->setPixels(p,ctable);
	}

}