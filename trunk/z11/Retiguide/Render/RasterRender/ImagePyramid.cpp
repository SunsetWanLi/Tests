#include "ImagePyramid.h"
#include "Pyramid.h"
#include "PyramidScaler.h"
#include "ImagePyramidConfig.h"
#include "PyramidTileIndex.h"
#include "ImagePyramidUpload.h"
#include "PyramidTileBuffer.h"
#include "GraphicCore/MGColor.h"
#include <stdlib.h>
#include <assert.h>
#include <cmath>

using namespace z11;
namespace GIS_Embedded
{

#define STD_ORIGIONAL_RESOLUTION 1.0f
	ImagePyramid::~ImagePyramid()
	{
		delete m_pyramid;
		delete m_crt_affine;
		delete m_crs_config;	
		delete m_crs_range;

	}
	ImagePyramid::ImagePyramid()
	{

	}

	void ImagePyramid::init()
	{
		pyramid_width = m_crs_config->m_image_width;
		pyramid_height = m_crs_config->m_image_height;
		initCrt();
	}
	void ImagePyramid::initCrt()
	{
		double l = m_crs_config->m_left, 
			b = m_crs_config->m_bottom, 
			r = m_crs_config->m_right, 
			t = m_crs_config->m_top,  
			width = m_crs_config->m_image_width, 
			height = m_crs_config->m_image_height;
		m_crt_affine = new CRT_Affine();
		m_crt_affine->a = width / fabs( r - l );
		m_crt_affine->c = - m_crt_affine->a * l;
		m_crt_affine->e = height / fabs(t - b );
		m_crt_affine->f = - m_crt_affine->e *b;
		m_crt_affine->b = m_crt_affine->d = 0;

		//zhaoling 2011-3-21
		m_last_tiles_region.x_start = 0;
		m_last_tiles_region.x_end = 0;
		m_last_tiles_region.y_start = 0;
		m_last_tiles_region.y_end = 0;
	}
	//得到转换比例:in在src内显示;现要得到放缩后的in'在在屏幕坐标dest中显示的位置.假设src和dest中width/height比例近似. 
	double ImagePyramid::getTransRatio(const Rect2D& src, const Rect2D& in, const Rect2D& dest, 
		SPosition* left_top, z11::uint_32* width, z11::uint_32* height)
	{
		double in_width = in.right_top.d0 - in.left_low.d0,
			in_height = in.right_top.d1 - in.left_low.d1;
		//以像素坐标的计算方法来算.同世界坐标系,y轴方向不变  
		//in的最左上角在src中left_low的偏移  
		double left_top_x_offset = in.left_low.d0 - src.left_low.d0,
			left_top_y_offset = in.right_top.d1 - src.left_low.d1;
		//assert(left_top_x_offset > -0.1 && left_top_y_offset > -0.1);
		double resolution = getResolution(src, dest);	//获得缩放比例  
		left_top->x = /*floor*/(int_32)(dest.left_low.d0 + left_top_x_offset*resolution);
		//把这一步放到下面一起做以减小误差  
		//left_top->y = ceil(dest.left_low.d1 + left_top_y_offset*resolution);	//像素坐标  
		//转换为屏幕坐标  
		double dest_height = dest.right_top.d1-dest.left_low.d1;	//屏幕坐标  
		left_top->y = /*floor*/(int_32)(dest_height - dest.left_low.d1 - left_top_y_offset*resolution);
		//assert(left_top->x >= 0 && left_top->y >= 0);  
		*width = (z11::uint_32)ceil(fabs(in_width*resolution));
		*height = (z11::uint_32)ceil(fabs(in_height*resolution));
		return resolution;
	}

	//当dest框比src小:目标缩小,分辨率(放大率)<1  
	double ImagePyramid::getResolution(const Rect2D& src, const Rect2D& dest)
	{
		double resolution = 1.0;	//放缩尺度:  
		double ratio_src = (double)(src.right_top.d0-src.left_low.d0)
			/(src.right_top.d1-src.left_low.d1),
			ratio_dest = (double)(dest.right_top.d0-dest.left_low.d0)
			/(dest.right_top.d1-dest.left_low.d1);
		//ratio_src与ratio_dest应该相差很小,不大于0.3  
		//assert(abs(ratio_dest-ratio_src)<0.3);  
		//调整dest的最终放大倍数  
		if(ratio_dest > ratio_src)	//以较短的宽width为标准放缩  
			resolution *= (dest.right_top.d0-dest.left_low.d0)/(src.right_top.d0-src.left_low.d0);
		else	//以较短的高height为标准放缩  
			resolution *= (dest.right_top.d1-dest.left_low.d1)/(src.right_top.d1-src.left_low.d1);
		return resolution;
	}

	int_32 ImagePyramid::getPixelRect(const Rect2D &rect, Rect2D * pixel_rect)
	{
		Rect2D trans_rect;
		m_crt_affine->trans(rect.left_low, &trans_rect.left_low);
		m_crt_affine->trans(rect.right_top,&trans_rect.right_top);

		//交换,使得框的范围是:left_low和right_top相对位置正确  
		if(trans_rect.left_low.d0 > trans_rect.right_top.d0)
			swap(trans_rect.left_low.d0, trans_rect.right_top.d0);
		if(trans_rect.left_low.d1 > trans_rect.right_top.d1)
			swap(trans_rect.left_low.d1, trans_rect.right_top.d1);

		*pixel_rect = trans_rect;
		return 0;	
	}

	Rect2D ImagePyramid::intersectRect( const Rect2D & left, const Rect2D &right )
	{
		Rect2D rect ;

		rect.left_low.d0 = left.left_low.d0 > right.left_low.d0 ? left.left_low.d0 : right.left_low.d0 ;
		rect.left_low.d1 = left.left_low.d1 > right.left_low.d1 ? left.left_low.d1 : right.left_low.d1 ;
		rect.right_top.d0 = left.right_top.d0 < right.right_top.d0 ? left.right_top.d0 : right.right_top.d0 ;
		rect.right_top.d1 = left.right_top.d1 < right.right_top.d1 ? left.right_top.d1 : right.right_top.d1 ;

		return rect ;
	}

	int_32 ImagePyramid::init(const string& map_name, const string& map_root_path)
	{
		m_map_name = map_name;
		m_crs_config = new ImagePyramidConfig();
		m_crs_range = new CRS_Range();
		ImagePyramidUpload::getImagePyramidConfig(map_name,map_root_path,m_crs_config);
		setCrsRange(m_crs_config);
		init();
		/*ImageBuffer *buffer = new ImageBuffer;*/
		ImagePyramidConfig* image_pyramid_config = new ImagePyramidConfig();;
		ImagePyramidUpload::getImagePyramidConfig(map_root_path + map_name, image_pyramid_config);
		TILE_PYRAMID pyramid_info;
		//pyramid_info.image_type = (PYRAMID_IMAGE_TYPE)image_pyramid_config->m_image_type;
		//m_is_single_file = image_pyramid_config->m_is_single_file;
		//m_is_load_at_first = image_pyramid_config->m_is_load_at_first;
		pyramid_info.raster_name = image_pyramid_config->raster_name;
		pyramid_info.raster_type = image_pyramid_config->raster_type;
		//pyramid_info.repo_name = image_pyramid_config->repo_name;
		pyramid_info.repo_name = map_root_path;
		pyramid_info.repo_type = image_pyramid_config->repo_type;
		pyramid_info.tile_size = image_pyramid_config->m_regular_tile_size;
		pyramid_info.samp_multi = image_pyramid_config->m_level_factor;
		pyramid_info.orig_width = image_pyramid_config->m_image_width;
		pyramid_info.orig_height = image_pyramid_config->m_image_height;
		pyramid_info.region.x_start = image_pyramid_config->m_x_start;
		pyramid_info.region.x_end = image_pyramid_config->m_x_end;
		pyramid_info.region.y_start = image_pyramid_config->m_y_start;
		pyramid_info.region.y_end = image_pyramid_config->m_y_end;
		pyramid_info.orig_resolution = image_pyramid_config->m_resolution;
		pyramid_info.level = image_pyramid_config->m_floor_num;
		assert(pyramid_info.level == image_pyramid_config->m_floors.size());
		m_pyramid = new Pyramid(pyramid_info, NULL);
		return 0;
	}
	int_32 ImagePyramid::getImage(const MGRect &draw_rect, int_32 resolution_width, int_32 resolution_height, 
		std::vector<z11::MGColor *> &image_vector, std::vector<z11::MGPoint> &point_vector)
	{
		int_32 ret = 0;
		z11::uint_32 img_width = 0;
		z11::uint_32 img_height = 0;
		SPosition left_top;
		Rect2D rect;
		rect.left_low.d0 = (double)(draw_rect.x);
		rect.left_low.d1 = (double)(draw_rect.y + draw_rect.height);
		rect.right_top.d0 = (double)(draw_rect.x + draw_rect.width);
		rect.right_top.d1 = (double)(draw_rect.y);
		Result* result = getImage(rect, (const long)resolution_width, (const long)resolution_height, &left_top, &img_width, &img_height, true);
		image_vector.clear();
		point_vector.clear();
		if (result != NULL)
		{
			for (int i=0; i<result->block_count; i++)
			{
				image_vector.push_back(result->blocks[i].color);
				point_vector.push_back(MGPointMake(float(m_bmp_lefttop[i].left), float(m_bmp_lefttop[i].top)));
			}
		}
		return ret;
	}
	Result* ImagePyramid::getImage(const Rect2D &draw_rect, const long resolution_width, const long resolution_height,
		SPosition* left_top, z11::uint_32* width, z11::uint_32* height, bool key_press)
	{
		Rect2D pixel_rect;
		if( getPixelRect( draw_rect, &pixel_rect ) >= 0 )
		{
			Rect2D virtual_screen;	//虚屏范围  
			virtual_screen.left_low = Pos2D(0,0);
			virtual_screen.right_top = Pos2D(resolution_width, resolution_height);	

			Rect2D pyramid_rect;
			pyramid_rect.left_low = Pos2D(0,0);
			pyramid_rect.right_top = Pos2D(pyramid_width, pyramid_height);

			Rect2D overlap;		//draw_rect与pyramid相交的有效区域  
			overlap = intersectRect(pixel_rect, pyramid_rect);
			if(overlap.left_low.d0 > overlap.right_top.d0	//判断是否有相交部分  
				|| overlap.left_low.d1 > overlap.right_top.d1)
				return NULL;	//无公共部分  

			z11::uint_32 show_width = 0;
			z11::uint_32 show_height = 0;//zhaoling 2011-3-21  

			double resolution = getTransRatio(pixel_rect, overlap, virtual_screen, left_top, &show_width, &show_height);

			double draw_Len = 1.0f * fabs(draw_rect.right_top.d0 - draw_rect.left_low.d0);
			double pixel_Len = 1.0f * fabs(pixel_rect.right_top.d0 - pixel_rect.left_low.d0);

			proTOscr_rate = draw_Len / pixel_Len ;
			proTOscr_rate /= resolution;

			//return getImage(overlap, resolution, width, height, key_press);

			Request req;
			req.region.x_start	= overlap.left_low.d0;
			req.region.x_end	= overlap.right_top.d0;
			//req.region.y_start	= overlap.right_top.d1;
			//req.region.y_end	= overlap.left_low.d1;
			req.region.y_start	= overlap.left_low.d1;
			req.region.y_end	= overlap.right_top.d1;
			req.resolution = STD_ORIGIONAL_RESOLUTION / resolution;
			Result* result = m_pyramid ->getMap(req);

			double magnify_ratio = STD_ORIGIONAL_RESOLUTION / result->blocks[0].tile->resolution;
			GeoScale height = m_crs_config->m_floors[result->blocks[0].tile->floor_number].second;
			GeoScale width = m_crs_config->m_floors[result->blocks[0].tile->floor_number].first;
			GeoRegion_Scale & region = req.region;
			region.x_start	= ceil(region.x_start * magnify_ratio);
			region.x_end	= ceil(region.x_end * magnify_ratio);
			region.y_start	= height - ceil(region.y_start * magnify_ratio);
			region.y_end	= height - ceil(region.y_end * magnify_ratio);
			swap(region.y_start, region.y_end);
			for (int i=0; i<result->block_count; i++)
			{
				//tile_reg 是每个小tile在本层的相对位置  
				GeoRegion_Scale tile_reg = result ->blocks[i].tile ->region;
				tile_reg.x_start	= ceil(tile_reg.x_start * magnify_ratio);
				tile_reg.x_end		= ceil(tile_reg.x_end * magnify_ratio);
				tile_reg.y_start	= height - ceil(tile_reg.y_start * magnify_ratio);
				tile_reg.y_end		= height - ceil(tile_reg.y_end * magnify_ratio);
				swap(tile_reg.y_start, tile_reg.y_end);

				////copy_reg是每个小tile中有效的那块在本层的相对位置  
				//GeoRegion_Scale     copy_reg;
				//copy_reg.x_start = max(tile_reg.x_start, region.x_start);
				//copy_reg.x_end	 = min(tile_reg.x_end, region.x_end);
				//copy_reg.y_start = max(tile_reg.y_start, region.y_start);
				//copy_reg.y_end	 = min(tile_reg.y_end, region.y_end);

				////copy_reg是每个小tile中有效的那块在所取图的相对位置  
				//GeoRegion_Scale     rel_reg;
				//rel_reg.x_start = copy_reg.x_start - region.x_start;
				//rel_reg.x_end = copy_reg.x_end - region.x_end;
				//rel_reg.y_start = copy_reg.y_start - region.y_start;
				//rel_reg.y_end = copy_reg.y_end - region.y_end;

				LeftTop left_top;
				left_top.left = tile_reg.x_start - region.x_start;
				left_top.top = tile_reg.y_start - region.y_start;
				m_bmp_lefttop.push_back(left_top);
			}
			return result;
		}
		return NULL;
	}

	int_32 ImagePyramid::getImage(const Rect2D &draw_rect, double resolution, z11::uint_32* width, z11::uint_32 * height, bool key_press)
	{
		if(resolution < 0) 
			return NULL;
		if( std::fabs(resolution) < MIN_FLOOT_DELTA )
			return NULL;
		Request req;
		req.region.x_start	= (GeoScale)draw_rect.left_low.d0;
		req.region.x_end	= req.region.x_start + (GeoScale)(draw_rect.right_top.d0 - draw_rect.left_low.d0 );
		req.region.y_start	= (GeoScale)draw_rect.left_low.d1;
		req.region.y_end	= req.region.y_start + (GeoScale)( draw_rect.right_top.d1 - draw_rect.left_low.d1 );
		req.resolution = 10 / resolution;

		Tiles tiles = m_pyramid ->indexTiles(req);

		z11::uint_32 floor_number, row_number, col_number;
		GeoRegion_Scale & region = req.region;
		double magnify_ratio = 10 / tiles.back() ->resolution;
		//************************************************  

		GeoRegion_Scale floor_region;
		floor_region.x_start = (GeoScale)(region.x_start * magnify_ratio);
		floor_region.x_end = (GeoScale)(region.x_end * magnify_ratio);
		floor_region.y_start = (GeoScale)(region.y_start * magnify_ratio);
		floor_region.y_end = (GeoScale)(region.y_end * magnify_ratio);

		if(!m_bmp_names.empty())
			m_bmp_names.clear();
		if(!m_bmp_lefttop.empty())
		{
			m_bmp_lefttop.clear();
		}
		//zhaoling 2011-3-21  
		if(floor_region.x_start >= m_last_tiles_region.x_start &&
			floor_region.x_end <= m_last_tiles_region.x_end &&
			floor_region.y_start >= m_last_tiles_region.y_start &&
			floor_region.y_end <= m_last_tiles_region.y_end
			&& !key_press)
		{
			m_need_not_fetch_tiles = true;
		}


		else
		{
			m_need_not_fetch_tiles = false;

			//************************************************  
			z11::uint_32 floor_region_width	= (z11::uint_32)ceil(fabs((double)(floor_region.x_end - floor_region.x_start)));//add ceil() by  
			z11::uint_32 floor_region_height	= (z11::uint_32)ceil(fabs((double)(floor_region.y_end - floor_region.y_start)));//hqs 2007-11-8  


			//zhaoling 2011-3-21  
				
			m_last_tiles_region.x_start = 0;
			m_last_tiles_region.x_end = 0;
			m_last_tiles_region.y_start = 0;
			m_last_tiles_region.y_end = 0;

			while( ! tiles.empty())
			{
				floor_number	= tiles.back() ->floor_number;
				row_number		= tiles.back() ->block_row_number;
				col_number		= tiles.back() ->block_col_number;

				Result * one_tile = m_pyramid ->getTile(floor_number,row_number,col_number);
				GeoRegion_Scale tile_reg = one_tile ->blocks[0].tile ->region;
				//???TODO:以下可能有问题,x_start,y_start应保持不变.by hqs 2007-7-31  


				//tile_reg.x_start	*= magnify_ratio;  
				//tile_reg.x_end		= ceil(tile_reg.x_end * magnify_ratio);	//add ceil by  
				//tile_reg.y_start	*= magnify_ratio;  
				//tile_reg.y_end		= ceil(tile_reg.y_end * magnify_ratio);	//hqs 2007-11-8	  

	
				tile_reg.x_start	= (GeoScale)(ceil(tile_reg.x_start * magnify_ratio));
				tile_reg.x_end		= (GeoScale)(floor(tile_reg.x_end * magnify_ratio));
				tile_reg.y_start	= (GeoScale)(ceil(tile_reg.y_start * magnify_ratio));
				tile_reg.y_end		= (GeoScale)(floor(tile_reg.y_end * magnify_ratio));

				//zhaoling 2011-3-21  
				if(m_last_tiles_region.x_end == 0 && m_last_tiles_region.y_end == 0)
				{
					m_last_tiles_region.x_start = tile_reg.x_start;
					m_last_tiles_region.x_end = tile_reg.x_end;
					m_last_tiles_region.y_start = tile_reg.y_start;
					m_last_tiles_region.y_end = tile_reg.y_end;
				}

				else
				{
					if(tile_reg.x_start < m_last_tiles_region.x_start)
						m_last_tiles_region.x_start = tile_reg.x_start;
					if(tile_reg.x_end > m_last_tiles_region.x_end)
						m_last_tiles_region.x_end = tile_reg.x_end;
					if(tile_reg.y_start < m_last_tiles_region.y_start)
						m_last_tiles_region.y_start = tile_reg.y_start;
					if(tile_reg.y_end > m_last_tiles_region.y_end)
						m_last_tiles_region.y_end = tile_reg.y_end;
				}


				GeoRegion_Scale    copy_reg;	//取region与tile_reg的交集:如果空呢?  
				copy_reg.x_start = max(tile_reg.x_start, region.x_start);
				copy_reg.x_end	 = min(tile_reg.x_end, region.x_end);
				copy_reg.y_start = max(tile_reg.y_start, region.y_start);
				copy_reg.y_end	 = min(tile_reg.y_end, region.y_end);
				z11::uint_32 copy_width = abs(copy_reg.x_end - copy_reg.x_start) ;
				z11::uint_32 copy_height = abs(copy_reg.y_end - copy_reg.y_start);


				string tilename = getTileName(m_map_name,floor_number,row_number,col_number);
				//***************************************zhaoling// save tilename and its x_start, y_start   
				storeBMPName(tilename);
				LeftTop lt;
				lt.left = tile_reg.x_start;
				lt.top = tile_reg.y_end;
				storeBMPLeftTop(lt);
				//****************************************end  
				delete one_tile;
				tiles.pop_back();
			}

			for(BMP_LeftTop::iterator iter = m_bmp_lefttop.begin(); iter != m_bmp_lefttop.end(); ++iter)
			{
				(*iter).left = (*iter).left - m_last_tiles_region.x_start;
				(*iter).top = m_last_tiles_region.y_end  - (*iter).top;
			}

		}//end else  

		//zhaoling 2011-3-21  
		*width = m_last_tiles_region.x_end - m_last_tiles_region.x_start;
		*height = m_last_tiles_region.y_end - m_last_tiles_region.y_start;

		m_blit_region.x_start = floor_region.x_start - m_last_tiles_region.x_start;
		m_blit_region.y_start = m_last_tiles_region.y_end - floor_region.y_end;
		m_blit_region.x_end = m_blit_region.x_start + (floor_region.x_end - floor_region.x_start);
		m_blit_region.y_end = m_blit_region.y_start + (floor_region.y_end - floor_region.y_start);

		return 0;
	}
	string ImagePyramid::getTileName(string image_name,int floor, int cur_row, int cur_col)
	{
		vector<ImagePyramidFloorInfo> floors;
		ImagePyramidUpload::getFloorInfo(*m_crs_config,&floors);
		return ImagePyramidUpload::getTileFileName(floors, image_name, floor, cur_row, cur_col);
	}
	////*********************************zhaoling  
	void ImagePyramid::storeBMPName(string BMPNames)
	{
		if("" != BMPNames)
			m_bmp_names.push_back(BMPNames);
		else
			return;
	}

	void ImagePyramid::storeBMPLeftTop(LeftTop lt)
	{
		m_bmp_lefttop.push_back(lt);
	}

	void ImagePyramid::getBMPName(string& bmpName)
	{
		if(!m_bmp_names.empty())
		{
			/*string BMPname;*/  
			bmpName = m_bmp_names.back();
			m_bmp_names.pop_back();
		}
		else 
			return;

	}

	LeftTop ImagePyramid::getBMPLeftTop()
	{
		LeftTop lt;
		lt.left = 0;
		lt.top = 0;
		if(!m_bmp_lefttop.empty())
		{
			
			lt = m_bmp_lefttop.back();
			m_bmp_lefttop.pop_back();
		}
		return lt;
	}
	bool ImagePyramid::BMPNameEmpty()
	{
		if(m_bmp_names.empty())
			return true;
		else
			return false;
	}
	bool ImagePyramid::BMPLeftTopEmpty()
	{
		if(m_bmp_lefttop.empty())
			return true;
		else
			return false;
	}
	void ImagePyramid::setCrsRange(ImagePyramidConfig* image_pyramid_config)
	{
		if(NULL == image_pyramid_config) return;
		m_crs_range->crs_left = image_pyramid_config->m_left;
		m_crs_range->crs_bottom = image_pyramid_config->m_bottom;
		m_crs_range->crs_right = image_pyramid_config->m_right;
		m_crs_range->crs_top = image_pyramid_config->m_top;
	}
	//////*************************************end  
	

}