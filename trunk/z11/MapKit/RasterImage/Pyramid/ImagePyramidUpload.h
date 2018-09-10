#ifndef __RASTERIMAGE_IMAGE_PYRAMID_UPLOAD_H__
#define __RASTERIMAGE_IMAGE_PYRAMID_UPLOAD_H__

#include "PyramidScaler.h"

#include <string>
#include <fstream>
#include <vector>
//#include "PyramidSettings.h"
//#include "ImagePyramidConfig.h"
#include "ImagePyramidFloorInfo.h"
using namespace std;
namespace Raster_GIS
{

struct ImagePyramidConfig;
using namespace Pyramid_Space;
//using namespace Pyramid_Space::Scale_Pixel_Pixel;
//using namespace Pyramid_Space ::BMP_File_Structure;
//using namespace Pyramid_Space::Buffer;
//using namespace Pyramid_Space::Buffer_NEW; 
//using namespace Pyramid_Space::Constructor_BMP;

class ImagePyramidUpload
{		
public:						
	static int getImagePyramidConfig(const std::string& rooted_map_name, ImagePyramidConfig* image_pyramid_config);		
	static void getFloorInfo(const ImagePyramidConfig& image_pyramid_config, vector<ImagePyramidFloorInfo>* floors);
	static std::string getTileFileName(const std::string& map_name, int floor, int cur_upward_row, int cur_col, int rows, int cols);		
	static std::string getTileFileName(const vector<ImagePyramidFloorInfo>& floors, const std::string& map_name, int floor, int cur_upward_row, int cur_col);		
	static std::string getTileFileName(const ImagePyramidFloorInfo& floor_info, const std::string& map_name, int cur_upward_row, int cur_col);		
public:
	static int getPyramidConfig(const std::string& pyramid_config_name, ImagePyramidConfig* image_pyramid_config);
	static int getImagePyramidConfig(const std::string& map_name, const std::string& map_root_path, ImagePyramidConfig* image_pyramid_config);		
	static int getImageCRSConfig(const std::string& image_crs_name, ImagePyramidConfig* image_pyramid_config);	
};

}

#endif