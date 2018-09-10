#ifndef _RASTERIMAGE_PYRAMID_CONFIG_H_
#define _RASTERIMAGE_PYRAMID_CONFIG_H_

#include "Retiguide/Common/crs/basic_gm_type.h"
#include <vector>
#include <utility>	//for pair

namespace GIS_Embedded
{
	//namespace PyramidSpace
	//{
	//;		
	struct ImagePyramidConfig
	{
		//PyramidConfig
		RasterType		raster_type;			//栅格类型  
		std::string		raster_name;			//栅格名称  
		RepositoryType	repo_type;				//存储类型  
		std::string		repo_name;				//存储位置（db：表名；file：根目录）  

		//std::string   PyramidConfigName;
		//std::string   Pyramid_Config_path;
		//int_32 m_image_type;
		//bool m_is_single_file;
		//bool m_is_load_at_first;
		z11::int_32 m_regular_tile_size;				//规则瓦片大小  
		z11::int_32 m_level_factor;					//金字塔层数  
		z11::int_32 m_image_width, m_image_height;	//第0层像素矩阵大小  
		z11::int_32 m_x_start, m_x_end, m_y_start, m_y_end;
		double m_resolution;					//初始分辨率 [单位像素表示的实际距离 meter / pixel]  
		z11::int_32 m_floor_num;						//金字塔层数  

		z11::uint_32 m_tiles_count_per_file;//zhaoling

		std::vector<std::pair<z11::int_32, z11::int_32> > m_floors;	//width_height pair	
		//ImageCRSConfig, eliminate the duplicate image width & height
		z11::int_32 m_crs_type;	//0:Geography, 1:Projection	
		z11::int_32 m_geo_type;	//if Geography
		z11::int_32 m_gcs_type, m_prj_type;	//if Projection
		double m_left, m_bottom, m_right, m_top;	//coordinate range		
	};

	//struct PyramidConfig	
	//{
	//	int	m_image_type;
	//	bool m_is_single_file;
	//	bool m_is_load_at_first;
	//	int	m_regular_tile_size;
	//	int	m_level_factor;
	//	int m_image_width, m_image_height;	
	//	int m_x_start, m_x_end, m_y_start, m_y_end;
	//	double m_resolution;
	//	int m_floor_num;
	//	std::vector<std::pair<int,int> > m_floors;	//width_height pair	
	//};

	//struct ImageCRSConfig	
	//{
	//	int m_crs_type;	//0:Geography, 1:Projection	
	//	int m_geo_type;	//if Geography
	//	int m_gcs_type, m_prj_type;	//if Projection
	//	double m_left, m_bottom, m_right, m_top;	//coordinate range
	//};

	//}//namespace PyramidSpace
}

#endif