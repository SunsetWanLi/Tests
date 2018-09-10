#include "MGImageSource.h"
#include <assert.h>
#include "../../../GraphicCore/MGImage.h"
using namespace zxing;
namespace z11
{
	#define GetRByte(p) ((byte_8)((p)>>11<<3))
#define GetGByte(p) ((byte_8)((p)>>5<<2))
#define GetBByte(p) ((byte_8)((p)<<3))
	z11::MGImageSource::MGImageSource( MGImage *image )
	{
		assert(image!=NULL);
		image_=image;
		width=image->getWidth();
		height=image->getHeight();
	}

	MGImageSource::~MGImageSource()
	{
		//delete_and_set_null(image_);
	}

	int MGImageSource::getWidth() const
	{
		return width;
	}

	int MGImageSource::getHeight() const
	{
		return height;
	}

    // Give the gray data of specified row.
	unsigned char* MGImageSource::getRow( int y, unsigned char* row )
	{
        int width = getWidth();
        
		if(image_->content_info.format == PF_RGBA8888)
		{
            int byte_width = 4;
            const unsigned char* pixel_cache = image_->getContentData() + image_->getAdapterWidth() * y * byte_width;
            if (row == NULL) {
                row = new unsigned char[width];
            }
            for (int x = 0; x < width; x++) {
                const unsigned char* p = pixel_cache + x * byte_width;
                row[x] = 0.3 *p[0]+ 0.59 *p[1] + 0.11 *p[2];
            }
		}
        else if (image_->content_info.format == PF_RGBA8880)
        {
            int byte_width = 3;
            const unsigned char* pixel_cache = image_->getContentData() + image_->getAdapterWidth() * y * byte_width;
            if (row == NULL) {
                row = new unsigned char[width];
            }
            for (int x = 0; x < width; x++) {
                const unsigned char* p = pixel_cache + x * byte_width;
                row[x] = 0.3 *p[0]+ 0.59 *p[1] + 0.11 *p[2];
            }
        }
        else
        {
            assert(0); // Other color spaces are not supported for now.
        }
		return row;
	}
	unsigned char r_table[256];
	unsigned char g_table[256];
	unsigned char b_table[256];
	bool table_init=false;
	void calcTable()
	{
		if(!table_init)
		{
			table_init=true;
			for (int i=0;i<256;i++)
			{
				r_table[i]=0.3*i;
				g_table[i]=0.59*i;
				b_table[i]=0.11*i;
			}

		}
	}
    // Give the gray data of all image.
	unsigned char* MGImageSource::getMatrix()
	{
		calcTable();
        unsigned char* matrix = NULL;
        int width = getWidth();
        int height = getHeight();
		if(image_->content_info.format == PF_RGBA8888)
		{
            int byte_width = 4;
            const unsigned char *pixel_cache = image_->getContentData();
			matrix = new unsigned char[width*height];
			unsigned char* m = matrix;
            const unsigned char* p = pixel_cache;
			for (int y = 0; y < height; y++)
			{
				p = pixel_cache + (height-y-1)*image_->getAdapterWidth() * byte_width;
                for (int x = 0; x < width; x++) {
                    *m = r_table[p[0]]+g_table[p[1] ]+ b_table[p[2]]; // 标准灰度变换 RGB -> Gray
                    m++;
                    p += byte_width;
                }
			}
		}
        else if (image_->content_info.format == PF_RGBA8880)
        {
            int byte_width = 3;
            const unsigned char *pixel_cache = image_->getContentData();
            matrix = new unsigned char[width*height];
            unsigned char* m = matrix;
            const unsigned char* p = pixel_cache;
            for (int y = 0; y < height; y++)
            {
                p = pixel_cache + (height-y-1)*image_->getAdapterWidth() * byte_width;
				for (int x = 0; x < width; x++) {
					*m = r_table[p[0]]+g_table[p[1] ]+ b_table[p[2]]; // 标准灰度变换 RGB -> Gray
                    m++;
                    p += byte_width;
                }
            }			
		}
		else if (image_->content_info.format == PF_RGBA5650)
		{
			const unsigned short *pixel_cache =(const unsigned short*)  image_->getContentData();
			matrix = new unsigned char[width*height];
			unsigned char* m = matrix;
			const unsigned short* p =pixel_cache;
			for (int y = 0; y < height; y++)
			{
				p = pixel_cache + (height-y-1)*image_->getAdapterWidth() ;
				for (int x = 0; x < width; x++) {
					*m = r_table[GetRByte(*p)]+ g_table[GetGByte(*p)] + b_table[GetBByte(*p)]; // 标准灰度变换 RGB -> Gray
					m++;
					p ++;
				}
			}
		}
        else
        {
            assert(0); // Other color spaces are not supported for now.
        }
		return matrix;
	}
}
