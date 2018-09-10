#include "ImagePyramidUpload.h"
#include "PyramidSettings.h"
#include "ImagePyramidConfig.h"

#include <sstream>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <iostream>
#include <ExternalLib/xml/rapidxml.hpp>
#include <ExternalLib/xml/rapidxml_utils.hpp>
#include <ExternalLib/xml/rapidxml_print.hpp>
using namespace std;

namespace Raster_GIS
{
;

int ImagePyramidUpload::getImagePyramidConfig(const std::string& map_name, const std::string& map_root_path, ImagePyramidConfig* image_pyramid_config)
{
	return getImagePyramidConfig(map_root_path + "/" +map_name, image_pyramid_config);
}

int ImagePyramidUpload::getImagePyramidConfig(const std::string& rooted_map_name, ImagePyramidConfig* image_pyramid_config)
{
	int ret = -1;
	try
	{
		ret = getPyramidConfig(rooted_map_name, image_pyramid_config);
	}
	catch(...)
	{
		return -1;
	}
	if(ret < 0)
		return ret;
	try
	{
		ret = getImageCRSConfig(rooted_map_name, image_pyramid_config);
	}
	catch(...)
	{
		return -1;
	}
	return ret;
}

int ImagePyramidUpload::getPyramidConfig(const std::string& pyramid_config_name__, ImagePyramidConfig* pyramid_config)
{	
	string pyramid_config_name = pyramid_config_name__ + ".conf";
	if(NULL == pyramid_config)
		return -1;
	std::string config_name = pyramid_config_name;
	rapidxml::file<> fdoc(config_name.c_str());
	rapidxml::xml_document<>  doc;
	doc.parse<0>(fdoc.data());

	//! 获取根节点  
	rapidxml::xml_node<>* root = doc.first_node();
	//! 获取根节点第一个节点  
	rapidxml::xml_node<>* global_conf = NULL;
	if (root != NULL)
		global_conf = root->first_node("global_conf");

	if (global_conf != NULL)
	{
		rapidxml::xml_node<>* raster_name = global_conf->first_node("raster_name");
		rapidxml::xml_node<>* raster_type = global_conf->first_node("raster_type");
		rapidxml::xml_node<>* repo_name = global_conf->first_node("repo_name");
		rapidxml::xml_node<>* repo_type = global_conf->first_node("repo_type");
		rapidxml::xml_node<>* regular_tile_size = global_conf->first_node("regular_tile_size");
		rapidxml::xml_node<>* multiple_between_floor_level = global_conf->first_node("multiple_between_floor_level");
		rapidxml::xml_node<>* width = global_conf->first_node("width");
		rapidxml::xml_node<>* height = global_conf->first_node("height");
		rapidxml::xml_node<>* x_start = global_conf->first_node("x_start");
		rapidxml::xml_node<>* x_end = global_conf->first_node("x_end");
		rapidxml::xml_node<>* y_start = global_conf->first_node("y_start");
		rapidxml::xml_node<>* y_end = global_conf->first_node("y_end");
		rapidxml::xml_node<>* resolution = global_conf->first_node("resolution");
		rapidxml::xml_node<>* floor_number = global_conf->first_node("floor_number");
		rapidxml::xml_node<>* tiles_count_per_file = global_conf->first_node("tiles_count_per_file");

		if (strcmp(raster_type->value() ,"RT_PNG") == 0)
		{
			pyramid_config->m_raster_type = Raster_GIS::RT_PNG;
			pyramid_config->m_file_ext = ".png";
		}
		//else if (strcmp(raster_type->value() ,"RT_GRID") == 0)
		//{
		//	pyramid_config->m_raster_type = Raster_GIS::RT_GRID;
		//}
		else if (strcmp(raster_type->value() ,"RT_BMP") == 0)
		{
			pyramid_config->m_raster_type = Raster_GIS::RT_BMP;
			pyramid_config->m_file_ext = ".bmp";
		}
		else if (strcmp(raster_type->value() ,"RT_JPG") == 0)
		{
			pyramid_config->m_raster_type = Raster_GIS::RT_JPG;
			pyramid_config->m_file_ext = ".jpg";
		}
		//else if (strcmp(raster_type->value() ,"RT_TIFF") == 0)
		//{
		//	pyramid_config->m_raster_type = Raster_GIS::RT_TIFF;
		//}

		//if (strcmp(repo_type->value() ,"REPO_FILES") == 0)
		//{
		//	pyramid_config->repo_type = Raster_GIS::REPO_FILES;
		//}
		//else if(strcmp(repo_type->value() ,"REPO_DATABASE") == 0)
		//{
		//	pyramid_config->repo_type = Raster_GIS::REPO_DATABASE;
		//}
		//string tmp_raster_name = raster_name->value();;
		//string tmp_repo_name = repo_name->value();;
		//pyramid_config->raster_name = tmp_raster_name;
		//pyramid_config->repo_name = tmp_repo_name;

		sscanf(regular_tile_size->value(), "%d", &(pyramid_config->m_tile_size));
		sscanf(multiple_between_floor_level->value(), "%d", &(pyramid_config->m_level_factor));
		sscanf(width->value(), "%d", &(pyramid_config->m_image_width));
		sscanf(height->value(), "%d", &(pyramid_config->m_image_height));
		sscanf(x_start->value(), "%d", &(pyramid_config->m_x_start));
		sscanf(x_end->value(), "%d", &(pyramid_config->m_x_end));
		sscanf(y_start->value(), "%d", &(pyramid_config->m_y_start));
		sscanf(y_end->value(), "%d", &(pyramid_config->m_y_end));
		sscanf(resolution->value(), "%lf", &(pyramid_config->m_resolution));
		sscanf(floor_number->value(), "%d", &(pyramid_config->m_floor_num));
		//sscanf(tiles_count_per_file->value(), "%ud", &(pyramid_config->m_tiles_count_per_file));

	}

	// read floors' information
	//cfg.m_floors.resize(cfg.m_floor_num);	//resize 
	//! 获取根节点第二个节点  
	rapidxml::xml_node<>* floor = NULL;
	if (root != NULL)
		floor = root->first_node("floor");
	pyramid_config->m_floors.clear();
	pyramid_config->m_floors.reserve(pyramid_config->m_floor_num);
	for(rapidxml::xml_node<>* floor_index=floor->first_node(); floor_index!=NULL; floor_index=floor_index->next_sibling())
	{
		rapidxml::xml_node<>* image_width = floor_index->first_node("image_width");
		rapidxml::xml_node<>* image_height = floor_index->first_node("image_height");
		int img_width = 0;
		int img_height = 0;
		sscanf(image_width->value(), "%d", &img_width);
		sscanf(image_height->value(), "%d", &img_height);
		pyramid_config->m_floors.push_back(std::make_pair(img_width, img_height));
	}
	return 0;
}

int ImagePyramidUpload::getImageCRSConfig(const std::string& image_crs_name__, ImagePyramidConfig* image_crs_config)
{
	string image_crs_name = image_crs_name__ + ".crs";
	if(NULL == image_crs_config)
		return -1;
	std::string config_name = image_crs_name;
	rapidxml::file<> fdoc(config_name.c_str());
	rapidxml::xml_document<>  doc;
	doc.parse<0>(fdoc.data());

	//! 获取根节点  
	rapidxml::xml_node<>* root = doc.first_node();
	if (root != NULL)
	{
		rapidxml::xml_node<>* crs_type = root->first_node("crs_type");
		rapidxml::xml_node<>* geo_type = root->first_node("geo_type");
		rapidxml::xml_node<>* gcs_type = root->first_node("gcs_type");
		rapidxml::xml_node<>* prj_type = root->first_node("prj_type");
		rapidxml::xml_node<>* x_start = root->first_node("x_start");
		rapidxml::xml_node<>* x_end = root->first_node("x_end");
		rapidxml::xml_node<>* y_start = root->first_node("y_start");
		rapidxml::xml_node<>* y_end = root->first_node("y_end");

		if (strcmp(crs_type->value() ,"GEOGRAPHY") == 0)
		{
			image_crs_config->m_crs_type = 0;
			sscanf(geo_type->value(), "%d", &(image_crs_config->m_geo_type));
		}
		else if (strcmp(crs_type->value() ,"PROJECTION") == 0)
		{
			image_crs_config->m_crs_type = 1;
			sscanf(gcs_type->value(), "%d", &(image_crs_config->m_gcs_type));
			sscanf(prj_type->value(), "%d", &(image_crs_config->m_prj_type));
		}
		sscanf(x_start->value(), "%lf", &(image_crs_config->m_left));
		sscanf(x_end->value(), "%lf", &(image_crs_config->m_right));
		sscanf(y_start->value(), "%lf", &(image_crs_config->m_bottom));
		sscanf(y_end->value(), "%lf", &(image_crs_config->m_top));

	}
	return 0;
}

void ImagePyramidUpload::getFloorInfo(const ImagePyramidConfig& image_pyramid_config, vector<ImagePyramidFloorInfo>* floors)
{
	floors->clear();
	ImagePyramidFloorInfo info;
	int regular_tile_size = image_pyramid_config.m_tile_size;
	info.number = 0;
	info.width = image_pyramid_config.m_image_width;
	info.height = image_pyramid_config.m_image_height;
	info.regular_tile_number_per_row = info.width / regular_tile_size;
	info.non_regular_tile_number_per_row = (0 == info.width % regular_tile_size)?0:1;
	info.row_number = info.height / regular_tile_size;
	if( info.height % regular_tile_size != 0 )
		info.row_number++;
	floors->push_back(info);
	while( (info.width > regular_tile_size) 
		|| (info.height > regular_tile_size) )
	{
		info.number ++;
		info.width = static_cast<unsigned int>(ceil(1.0 * info.width / image_pyramid_config.m_level_factor));
		info.height = static_cast<unsigned int>(ceil(1.0 * info.height / image_pyramid_config.m_level_factor));
		info.regular_tile_number_per_row = info.width / regular_tile_size;
		info.non_regular_tile_number_per_row = (0 == info.width % regular_tile_size)?0:1;
		info.row_number = info.height / regular_tile_size;
		if( info.height % regular_tile_size != 0 )
			info.row_number++;
		floors->push_back(info);
	}
	return ;
}

std::string ImagePyramidUpload::getTileFileName(const vector<ImagePyramidFloorInfo>& floors, const std::string& map_name, int floor, int cur_upward_row, int cur_col)
{
	if(floors.size() < floor)
		return "";
	int total_rows = floors[floor].row_number, 
		total_cols = floors[floor].regular_tile_number_per_row + floors[floor].non_regular_tile_number_per_row;
	return getTileFileName(map_name, floor, cur_upward_row, cur_col, total_rows, total_cols);
}

std::string ImagePyramidUpload::getTileFileName(const ImagePyramidFloorInfo& floor_info, const std::string& map_name, int cur_upward_row, int cur_col)
{
	int total_rows = floor_info.row_number, 
		total_cols = floor_info.regular_tile_number_per_row + floor_info.non_regular_tile_number_per_row;
	return getTileFileName(map_name, floor_info.number, cur_upward_row, cur_col, total_rows, total_cols);
}


std::string ImagePyramidUpload::getTileFileName(const std::string& map_name, int floor, int cur_upward_row, int cur_col, int rows, int cols)
{
	int downward_row = rows - 1 - cur_upward_row;
	int tile_per_row = cols;
	int index =  downward_row * tile_per_row  + cur_col;
	uint_32 tiles_count_per_file = 500;//MAX_TILES_COUNT_PER_FILE

	stringstream grand_parent_filename;
	grand_parent_filename<<map_name<<"_"<<floor;
	stringstream parent_filename;
	parent_filename<<"/"<<map_name<<"_"<<floor<<"_"<<index / tiles_count_per_file;
	stringstream sub_filename;	
	sub_filename<<"/"<<map_name<<"_"<<floor<<"_"<<index;
	string tile_path = grand_parent_filename.str() + parent_filename.str() + sub_filename.str();

	return tile_path;
	//std::stringstream filename;	
	////filename.imbue(locale("C"));
	//filename<<map_name<<"_"<<floor<<"_"<<index/*row<<L"_"<<col*/;
	//return filename.str();
}
}

