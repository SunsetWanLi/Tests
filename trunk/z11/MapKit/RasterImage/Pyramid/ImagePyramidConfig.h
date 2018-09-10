#ifndef __RASTERIMAGE_IMAGE_PYRAMID_CONFIG_H__
#define __RASTERIMAGE_IMAGE_PYRAMID_CONFIG_H__
//#include "PyramidSettings.h"
//#include "PyramidRequest.h"
#include "../Common/basic_types.h"
//#include "../../Common/basic_types.h"
#include <vector>
#include <utility>	//for pair

namespace Raster_GIS
{
	//namespace Pyramid_Space
	//{
	//;		
enum {
	RT_BMP = 0,
	RT_JPG = 1,
	RT_PNG = 2
};
typedef int_32 RasterType;

struct ImagePyramidConfig
	{
		ImagePyramidConfig()
		{
			m_floor_num = -1;
		}
		//PyramidConfig
		std::string   PyramidConfigName;
		std::string   Pyramid_Config_path;
		int	m_image_type;
		bool m_is_single_file;
		//bool m_is_load_at_first;

		std::string m_file_ext;
		RasterType m_raster_type;
		int	m_tile_size;
		int	m_level_factor;
		int m_image_width, m_image_height;	
		int m_x_start, m_x_end, m_y_start, m_y_end;
		double m_resolution;
		int m_floor_num;
		std::vector<std::pair<int,int> > m_floors;	//width_height pair	
		//ImageCRSConfig, eliminate the duplicate image width & height
		int m_crs_type;	//0:Geography, 1:Projection	
		int m_geo_type;	//if Geography
		int m_gcs_type, m_prj_type;	//if Projection
		double m_left, m_bottom, m_right, m_top;	//coordinate range		
	};

	//struct PyramidConfig	
	//{
	//	int	m_image_type;
	//	bool m_is_single_file;
	//	bool m_is_load_at_first;
	//	int	m_tile_size;
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

	//}//namespace Pyramid_Space
}

#endif