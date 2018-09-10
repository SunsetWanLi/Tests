#pragma once
#include "../../CommonBase/CommonDef.h"
class SkBitmap;
class SkColorTable;
namespace z11
{
	class MGImage ;
	class CGBitmap
	{
	public:
		enum Config {
			kNo_Config,         //!< bitmap has not been configured
			/**
			*  1-bit per pixel, (0 is transparent, 1 is opaque)
			*  Valid as a destination (target of a canvas), but not valid as a src.
			*  i.e. you can draw into a 1-bit bitmap, but you cannot draw from one.
			*/
			kA1_Config,
			kA8_Config,         //!< 8-bits per pixel, with only alpha specified (0 is transparent, 0xFF is opaque)
			kIndex8_Config,     //!< 8-bits per pixel, using SkColorTable to specify the colors
			kRGB_565_Config,    //!< 16-bits per pixel, (see SkColorPriv.h for packing)
			kARGB_4444_Config,  //!< 16-bits per pixel, (see SkColorPriv.h for packing)
			kARGB_8888_Config,  //!< 32-bits per pixel, (see SkColorPriv.h for packing)
			/**
			*  Custom compressed format, not supported on all platforms.
			*  Cannot be used as a destination (target of a canvas).
			*  i.e. you may be able to draw from one, but you cannot draw into one.
			*/
			kRLE_Index8_Config,

			kConfigCount
		};
	public:
		CGBitmap(void);
		~CGBitmap(void);
		static CGBitmap *bitmapWithMGImage(MGImage* buffer_image);
        static CGBitmap *bitmapWithMGImageWithoutAdapter(MGImage* buffer_image, bool reverse = false);
		void setConfig(Config _config, int width, int height, int rowBytes = 0);
		bool allocPixels();
		void setPixels(void* p, SkColorTable* ctable = 0);
        int getRealWidth(){return real_width;}
        int getRealHeight(){return real_height;}
        
        SkBitmap *getImpl(){return impl;};
	private:
		SkBitmap *impl;
        byte_8* data_cache;
		friend class CGCanvas;
        int real_width;
        int real_height;
	};

}