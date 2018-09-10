#include "ImagePyramid.h"
#include "ImagePyramidConfig.h"
#include "Pyramid.h"
#include "../Common/CRT_Affine.h"
//#include "../../Common/CRT_Affine.h"
#include "System/Shared/MGLog.h"
#include "PyramidTileIndex.h"
#include "ImagePyramidUpload.h"
#include <cmath>
#include <vector>
namespace Raster_GIS
{
#ifndef STD_ORIGIONAL_RESOLUTION
#define STD_ORIGIONAL_RESOLUTION 1.0f
#endif
#ifndef IMAGE_PYRAMID_ESPLON
#define IMAGE_PYRAMID_ESPLON 0.000005f
#endif
using namespace z11;//temp
using namespace std;
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
}
//得到转换比例:in在src内显示;现要得到放缩后的in'在在屏幕坐标dest中显示的位置.假设src和dest中width/height比例近似.
double ImagePyramid::getTransRatio(const Rect2D& src, const Rect2D& in, const Rect2D& dest, 
							SPosition* left_top, unsigned int* width, unsigned int* height)
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
	*width = (uint_32)ceil(fabs(in_width*resolution));
	*height = (uint_32)ceil(fabs(in_height*resolution));
	return resolution;
}

//当dest框比src小:目标缩小,分辨率(放大率)<1
double ImagePyramid::getResolution(const Rect2D& src, const Rect2D& dest)
{
	double resolution = 1.0;	//放缩尺度: 
	double ratio_x = (double)(dest.right_top.d0-dest.left_low.d0)
		/(src.right_top.d0-src.left_low.d0),
		ratio_y = (double)(dest.right_top.d1-dest.left_low.d1)
		/(src.right_top.d1-src.left_low.d1);
	if(ratio_x < ratio_y)	//以较短的高height为标准放缩
		resolution *= ratio_y;
	else	//以较短的高height为标准放缩
		resolution *= ratio_x;
	//double ratio_src = (double)(src.right_top.d0-src.left_low.d0)
	//				/(src.right_top.d1-src.left_low.d1),
	//	ratio_dest = (double)(dest.right_top.d0-dest.left_low.d0)
	//				/(dest.right_top.d1-dest.left_low.d1);
	////ratio_src与ratio_dest应该相差很小,不大于0.3
	////assert(abs(ratio_dest-ratio_src)<0.3);
	////调整dest的最终放大倍数
	//if(ratio_dest > ratio_src)	//以较短的宽width为标准放缩
	//	resolution *= (dest.right_top.d0-dest.left_low.d0)/(src.right_top.d0-src.left_low.d0);
	//else	//以较短的高height为标准放缩
	//	resolution *= (dest.right_top.d1-dest.left_low.d1)/(src.right_top.d1-src.left_low.d1);
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

int ImagePyramid::init(const std::string& map_name, const std::string& map_root_path)
{
	m_map_name = map_name;
	m_map_root_path = map_root_path;
	m_crs_config = new ImagePyramidConfig();
	ImagePyramidUpload::getImagePyramidConfig(m_map_name,m_map_root_path,m_crs_config);
	init();
	m_pyramid = new Pyramid(m_map_root_path + "/" + m_map_name);
	return 0;
}

int ImagePyramid::getImage(const Rect2D &draw_rect, const long resolution_width, const long resolution_height, 
				std::vector<std::string> &image_path_vector, std::vector<LeftTop> &point_vector, long &get_col)
{
	int_32 ret = -1;
	unsigned int img_width = 0;
	unsigned int img_height = 0;
	SPosition left_top;
	std::vector<LeftTop>::const_iterator point_it;
	std::vector<std::string>::const_iterator path_it;
	image_path_vector.clear();
	point_vector.clear();

	ret = getImage(draw_rect, resolution_width, resolution_height, &left_top, &img_width, &img_height, get_col);
	while (!m_bmp_names.empty())
	{
		image_path_vector.push_back(m_map_root_path+"/"+m_bmp_names.back()+m_crs_config->m_file_ext);
		LeftTop tmp_left_top = m_bmp_lefttop.back();
		tmp_left_top.left += left_top.x; //by lw
		tmp_left_top.top += left_top.y;
		point_vector.push_back(tmp_left_top);
		m_bmp_names.pop_back();
		m_bmp_lefttop.pop_back();
	}
	//for (point_it=m_bmp_lefttop.begin(), path_it=m_bmp_names.begin(); 
	//	point_it!=m_bmp_lefttop.end(), path_it!=m_bmp_names.end(); 
	//	point_it++, path_it++)
	//{
	//	image_path_vector.push_back(m_map_root_path+"/"+(*path_it)+m_crs_config->m_file_ext);
	//	point_vector.push_back(*point_it);
	//}
	//m_bmp_names.clear();
	//m_bmp_lefttop.clear();

	return ret;
}

Rect2D ImagePyramid::getPyramidRegion()
{
	Rect2D rect;
	rect.left_low.d0 = m_crs_config->m_left;
	rect.right_top.d0 = m_crs_config->m_right;
	rect.left_low.d1 = m_crs_config->m_bottom;
	rect.right_top.d1 = m_crs_config->m_top;
	return rect;
}

double ImagePyramid::getWidthScalePixelGeo(unsigned int floor_num)
{
	return m_crt_affine->a / pow(2.0f,(int)floor_num);
	//double ret = -1.0;
	//double width = m_crs_config->m_right - m_crs_config->m_left;
	//if (!isEqualZero(width))
	//	ret = (m_crs_config->m_floors[floor_num].first / (width));
	//return ret;
}

double ImagePyramid::getHeightScalePixelGeo(unsigned int floor_num)
{
	return m_crt_affine->e / pow(2.0f,(int)floor_num);
	//double ret = -1.0;
	//double height = m_crs_config->m_top - m_crs_config->m_bottom;
	//if (!isEqualZero(height))
	//	ret = (m_crs_config->m_floors[floor_num].second / (height));
	//return ret;
}

int ImagePyramid::getTotalFloorNum()
{
	return m_crs_config->m_floor_num;
}

int ImagePyramid::getLevelFactor()
{
	return m_crs_config->m_level_factor;
}

int ImagePyramid::getRegularTileSize()
{
	return m_crs_config->m_tile_size;
}

int ImagePyramid::getImage(const Rect2D &draw_rect, const long resolution_width, const long resolution_height, 
						   SPosition* left_top, unsigned int* width, unsigned int* height, long &get_col)
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

		double resolution = getTransRatio(pixel_rect, overlap, virtual_screen, left_top, width, height);
		return getImage(overlap, resolution, get_col);
	}
	return -1;
}

int ImagePyramid::tryToGetImage(const Rect2D &draw_rect, const long resolution_width, const long resolution_height)
{
	if (m_crs_config->m_floor_num == -1)
		return -1;
	int floor_num = -1;
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

		unsigned int width = 0;
		unsigned int height = 0;
		SPosition left_top;
		double resolution = getTransRatio(pixel_rect, overlap, virtual_screen, &left_top, &width, &height);
		
		if(resolution < 0) 
			return -1;
		if( std::fabs(resolution) < Pyramid_Space::MIN_FLOOT_DELTA )
			return -1;
		Pyramid_Space::Request req;
		req.region.x_start	= overlap.left_low.d0;
		req.region.x_end	=  req.region.x_start + (overlap.right_top.d0 - overlap.left_low.d0 );
		req.region.y_start	= draw_rect.left_low.d1;
		req.region.y_end	= req.region.y_start + ( overlap.right_top.d1 - overlap.left_low.d1 );
		req.resolution = STD_ORIGIONAL_RESOLUTION / resolution;

		Pyramid_Space/*::Index*/::Tiles tiles = m_pyramid ->indexTiles(req);
		double magnify_ratio = 1.0f;
		if (tiles.size() > 0)
		{
			magnify_ratio = STD_ORIGIONAL_RESOLUTION / tiles.back() ->resolution;
			floor_num = tiles.back() ->floor_number;
		}
	}
	return floor_num;
}

int ImagePyramid::getImage(const Rect2D &draw_rect, double resolution, long &get_col)
{
	int ret = -1;
	if(resolution < 0) 
		return -1;
	if( std::fabs(resolution) < Pyramid_Space::MIN_FLOOT_DELTA )
		return -1;
	Pyramid_Space::Request req;
	req.region.x_start	= draw_rect.left_low.d0;
	req.region.x_end	=  req.region.x_start + (draw_rect.right_top.d0 - draw_rect.left_low.d0 );
	req.region.y_start	= draw_rect.left_low.d1;
	req.region.y_end	= req.region.y_start + ( draw_rect.right_top.d1 - draw_rect.left_low.d1 );
	req.resolution =  STD_ORIGIONAL_RESOLUTION /resolution;//

	Pyramid_Space/*::Index*/::Tiles tiles = m_pyramid ->indexTiles(req);
	if (tiles.empty())
		return -1;	
	unsigned int floor_number, row_number, col_number;
	double magnify_ratio = STD_ORIGIONAL_RESOLUTION / tiles.back() ->resolution;//
	floor_number	= tiles.back() ->floor_number;

	GeoScale height = m_crs_config->m_floors[floor_number].second;
	GeoScale width = m_crs_config->m_floors[floor_number].first;
	Pyramid_Space::GeoRegion_Scale & region = req.region;
	region.x_start	= ceil(region.x_start * magnify_ratio);
	region.x_end	= ceil(region.x_end * magnify_ratio);
	region.y_start	= height - ceil(region.y_start * magnify_ratio);
	region.y_end	= height - ceil(region.y_end * magnify_ratio);
	swap(region.y_start, region.y_end);

	//对tiles进行排序，以便算出相对位置   
	vector<Index_Tree_Node *>::const_iterator tiles_it;
	vector<Index_Tree_Node *>::iterator sorted_tiles_it;
	vector<Index_Tree_Node *> sorted_tiles;
	sorted_tiles.clear();
	int max_row=0, 
		min_row=0, 
		max_col=0, 
		min_col=0;
	for (tiles_it=tiles.begin(); tiles_it!=tiles.end(); tiles_it++)
	{
		max_row = min_row = (*tiles_it)->block_row_number;
		max_col = min_col = (*tiles_it)->block_col_number;
		vector<Index_Tree_Node *>::iterator insert_it = sorted_tiles.begin();
		for(sorted_tiles_it=sorted_tiles.begin(); sorted_tiles_it!=sorted_tiles.end(); sorted_tiles_it++)
		{
			if ( (*tiles_it)->block_row_number > (*sorted_tiles_it)->block_row_number )
			{
				break;
			}
			else if ((*tiles_it)->block_row_number == (*sorted_tiles_it)->block_row_number)
			{
				if ( (*tiles_it)->block_col_number <= (*sorted_tiles_it)->block_col_number )
					break;//insert_it = sorted_tiles_it;
				else
					insert_it = sorted_tiles_it + 1;
			}
			else
			{
				insert_it = sorted_tiles_it + 1;
			}
		}
		sorted_tiles.insert(insert_it, *tiles_it);
	}
	for (sorted_tiles_it=sorted_tiles.begin(); sorted_tiles_it!=sorted_tiles.end(); sorted_tiles_it++)
	{
		min_col = min_col < (*sorted_tiles_it)->block_col_number ? min_col : (*sorted_tiles_it)->block_col_number;
		min_row = min_row < (*sorted_tiles_it)->block_row_number ? min_row : (*sorted_tiles_it)->block_row_number;
		max_col = max_col > (*sorted_tiles_it)->block_col_number ? max_col : (*sorted_tiles_it)->block_col_number;
		max_row = max_row > (*sorted_tiles_it)->block_row_number ? max_row : (*sorted_tiles_it)->block_row_number;
	}
	get_col = max_col - min_col + 1;
	tiles.clear();
	
	//用地理坐标拼图会有几个像素的差距，导致屏幕上会出现一条白线  
	while( ! sorted_tiles.empty())
	{
		row_number		= sorted_tiles.back() ->block_row_number;
		col_number		= sorted_tiles.back() ->block_col_number;
		Pyramid_Space::GeoRegion_Scale tile_reg = sorted_tiles.back()->region;
		
		//Pyramid_Space::/*MapRequest::*/Result * one_tile = m_pyramid ->getTile(floor_number,row_number,col_number);
		//Pyramid_Space::GeoRegion_Scale tile_reg = one_tile ->blocks[0].tile ->region;
		tile_reg.x_start	= ceil(tile_reg.x_start * magnify_ratio);
		tile_reg.x_end		= floor(tile_reg.x_end * magnify_ratio);
		tile_reg.y_start	= height - floor(tile_reg.y_start * magnify_ratio);
		tile_reg.y_end		= height - ceil(tile_reg.y_end * magnify_ratio);
		swap(tile_reg.y_start, tile_reg.y_end);

		
		std::string tilename = getTileName(m_map_name,floor_number,row_number,col_number);
		storeBMPName(tilename);
		LeftTop left_top;
		left_top.left = tile_reg.x_start - region.x_start;//; by lw
		left_top.top = tile_reg.y_start- region.y_start; //;by lw 
		storeBMPLeftTop(left_top);

		//delete one_tile;
		sorted_tiles.pop_back();
	}
	return floor_number;
}
std::string ImagePyramid::getTileName(std::string image_name,int floor, int cur_row, int cur_col)
{
	vector<ImagePyramidFloorInfo> floors;
	ImagePyramidUpload::getFloorInfo(*m_crs_config,&floors);
	return ImagePyramidUpload::getTileFileName(floors, image_name, floor, cur_row, cur_col);
}
////*********************************zhaoling
void ImagePyramid::storeBMPName(std::string BMPNames)
{
	m_bmp_names.push_back(BMPNames);
}

void ImagePyramid::storeBMPLeftTop(LeftTop lt)
{
	m_bmp_lefttop.push_back(lt);
}

std::string ImagePyramid::getBMPName()
{
	if(!m_bmp_names.empty())
	{
		std::string BMPname;
		BMPname = m_bmp_names.back();
		m_bmp_names.pop_back();
		return BMPname;
	}
	else 
		return NULL;

}

LeftTop* ImagePyramid::getBMPLeftTop()
{
	if(!m_bmp_lefttop.empty())
	{
		LeftTop* lt = new LeftTop;
		lt->left = m_bmp_lefttop.back().left;
		lt->top = m_bmp_lefttop.back().top;
		m_bmp_lefttop.pop_back();
		return lt;
	}
	else
		return NULL;
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

bool ImagePyramid::isEqualZero(double value)
{
	bool ret = false;
	if (value > IMAGE_PYRAMID_ESPLON*(-1.0) &&
		value < IMAGE_PYRAMID_ESPLON)
		ret = true;
	return ret;
}

//////*************************************end
}