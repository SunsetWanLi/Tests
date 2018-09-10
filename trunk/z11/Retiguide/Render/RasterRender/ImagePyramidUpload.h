#ifndef _RASTERIMAGE_PYRAMID_UPLOAD_H_
#define _RASTERIMAGE_PYRAMID_UPLOAD_H_

#include "./PyramidSettings.h"
#include "ImagePyramidFloorInfo.h"
#include "Retiguide/Common/crs/basic_gm_type.h"
#include <string>
#include <fstream>
#include <vector>

namespace GIS_Embedded
{
	struct ImagePyramidConfig;
	struct ImagePyramidFloorInfo;
	class ImagePyramidUpload
	{		
	public:						
		static int getImagePyramidConfig(const std::string& rooted_map_name, ImagePyramidConfig* image_pyramid_config);		
		static void getFloorInfo(const ImagePyramidConfig& image_pyramid_config, std::vector<ImagePyramidFloorInfo>* floors);
		static std::string getTileFileName(const std::string& map_name, int floor, int cur_upward_row, int cur_col, int rows, int cols, z11::uint_32 tiles_count_per_file);		
		static std::string getTileFileName(const std::vector<ImagePyramidFloorInfo>& floors, const std::string& map_name, int floor, int cur_upward_row, int cur_col);		
		static std::string getTileFileName(const ImagePyramidFloorInfo& floor_info, const std::string& map_name, int cur_upward_row, int cur_col);		
	public:
		static int getPyramidConfig(const std::string& pyramid_config_name, ImagePyramidConfig* image_pyramid_config);
		static int getImagePyramidConfig(const std::string& map_name, const std::string& map_root_path, ImagePyramidConfig* image_pyramid_config);		
		static int getImageCRSConfig(const std::string& image_crs_name, ImagePyramidConfig* image_pyramid_config);	
	};
}

#endif