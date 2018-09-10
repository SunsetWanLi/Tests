#include "Pyramid.h"
#include "PyramidTileIndex.h"
#include "PyramidSettings.h"
#include "ImagePyramid.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include "ImagePyramidConfig.h"
#include "ImagePyramidFloorInfo.h"
#include "ImagePyramidUpload.h"
#include <assert.h>

namespace Raster_GIS
{
namespace Pyramid_Space
{

	using namespace std;
//	using namespace Buffer;
//	using namespace Index;
//	using namespace Constructor_BMP;

Pyramid::Pyramid(const string map_name)
	: m_map_name(map_name) , m_ref_count(1)
{

	if (initialize() != 0)
		return;
	// Construct the index tree
	m_index = new IndexTree(m_floor_info,m_pyramid_info.region,m_pyramid_info.level,m_pyramid_info.tile_size);
} // Pyramid constructor


Pyramid::~Pyramid()
{
	delete m_index; m_index = NULL;
	delete [] m_floor_info; m_floor_info = NULL;

	return ;
} // ~Pyramid()

int Pyramid::initialize()
{
//	using namespace Index;
	int ret = -1;
	m_pyramid_info.sampMethod = 0; // Not used in this edition.

	ImagePyramidConfig* image_pyramid_config = new ImagePyramidConfig();;
	ret = ImagePyramidUpload::getImagePyramidConfig(m_map_name, image_pyramid_config);
	if (ret != 0)
	{
		return ret;
	}
	//ret = data_ex->getImagePyramidConfig(m_map_name, &image_pyramid_config);

	m_pyramid_info.image_type = (PYRAMID_IMAGE_TYPE)image_pyramid_config->m_image_type;
	//m_is_single_file = image_pyramid_config->m_is_single_file;
	//m_is_load_at_first = image_pyramid_config->m_is_load_at_first;
	m_pyramid_info.tile_size = image_pyramid_config->m_tile_size;
	m_pyramid_info.sampMulti = image_pyramid_config->m_level_factor;
	m_pyramid_info.orig_width = image_pyramid_config->m_image_width;
	m_pyramid_info.orig_height = image_pyramid_config->m_image_height;
	m_pyramid_info.region.x_start = image_pyramid_config->m_x_start;
	m_pyramid_info.region.x_end = image_pyramid_config->m_x_end;
	m_pyramid_info.region.y_start = image_pyramid_config->m_y_start;
	m_pyramid_info.region.y_end = image_pyramid_config->m_y_end;
	m_pyramid_info.orig_resolution = image_pyramid_config->m_resolution;
	m_pyramid_info.level = image_pyramid_config->m_floor_num;
	assert(m_pyramid_info.level == image_pyramid_config->m_floors.size());
	// read floors' information
	m_floor_info = new Floor[m_pyramid_info.level];			
	for(int i = 0; i < image_pyramid_config->m_floors.size(); ++i)
	{
		m_floor_info[i].width = image_pyramid_config->m_floors[i].first;
		m_floor_info[i].height = image_pyramid_config->m_floors[i].second;
		// calculate other info of the floor
		m_floor_info[i].regular_per_row = m_floor_info[i].width / m_pyramid_info.tile_size;
		m_floor_info[i].non_regular_per_row = 0 < (m_floor_info[i].width % m_pyramid_info.tile_size);
		m_floor_info[i].row_number = (m_floor_info[i].height / m_pyramid_info.tile_size);
        unsigned int last_row = ( (0 == (m_floor_info[i].height % m_pyramid_info.tile_size)) ? 0 : 1) ;
        m_floor_info[i].row_number += last_row;
		m_floor_info[i].resolution = m_pyramid_info.orig_resolution * max(ceil(1.0 * m_pyramid_info.orig_width / m_floor_info[i].width), ceil(1.0 * m_pyramid_info.orig_height /m_floor_info[i].height));
	}

	delete image_pyramid_config, image_pyramid_config = 0;		
	return 0;

} // initialize()

Tiles Pyramid::indexTiles(const Request & request)
{
	return m_index ->getTiles(request.region,request.resolution);
}// indexTiles(const Request &)

Result * Pyramid::getMap(const Request & request,bool single_thread)
{
	assert(0);
	return 0;
} // getMap(const Request & , bool)


Result * Pyramid::getTile(unsigned int floor_number, unsigned int row_number, unsigned int col_number)
{
	Result * result = new Result;
	result ->block_count = 1;
	result ->from = m_index;
	result ->blocks = new TileBlock[result ->block_count]; 
	
	Floor  current_floor = m_floor_info[floor_number];
	ImagePyramidFloorInfo floor_info;
	floor_info.number = floor_number;
	floor_info.row_number = current_floor.row_number;
	floor_info.regular_tile_number_per_row = current_floor.regular_per_row;
	floor_info.non_regular_tile_number_per_row = current_floor.non_regular_per_row;
	Index_Tree_Node * tile = m_index ->getTile(floor_number,row_number,col_number); //attention 2008-08-13
	result ->blocks[0].tile = tile;
	return result;
} // getTile(unsigned int, unsigned int, unsigned int)


void Pyramid::reserveMap(const Request & request,/*MapRequest::*/Priority priority)
{
	assert(0);
} // reserveMap(const Request & , MapRequest::Priority )

void Pyramid::reserveTile(unsigned int floor_number,unsigned int row_number,unsigned int col_number,/*MapRequest::*/Priority priority)
{
	Tile_Request * tile_req = new Tile_Request;
	tile_req ->tile = m_index ->getTile(floor_number,row_number,col_number);
					
	Floor  current_floor = m_floor_info[floor_number];
	ImagePyramidFloorInfo floor_info;
	floor_info.number = floor_number;
	floor_info.row_number = current_floor.row_number;
	floor_info.regular_tile_number_per_row = current_floor.regular_per_row;
	floor_info.non_regular_tile_number_per_row = current_floor.non_regular_per_row;
	//tile_req ->file_name = m_pyramid_root_path + L"/" + PyramidConstructorImpl::getTileFileName(
	//	floor_info, row_number,col_number,m_map_name);
	tile_req ->file_name = m_map_name;
	tile_req->floor = floor_number, tile_req->row = row_number, tile_req->col = col_number;
	//tile_req ->prio = /*Thread::*/getThreadPriority();

	m_requests[priority].push(tile_req);
} // reserveTile(unsigned int , unsigned int ,unsigned int , MapRequest::Priority )

void Pyramid::refer()
{
	m_ref_count ++;
	if(m_ref_count < 0) 
		m_ref_count --;
} // refer()

int Pyramid::leave()
{
	m_ref_count --;
	if(m_ref_count < 0 )m_ref_count = 0;
	return m_ref_count;
} // leave()
int Pyramid::getImageWidth( )const
{
	return m_pyramid_info.orig_width;
}
int Pyramid::getImageHeight()const
{
	return m_pyramid_info.orig_height;
}

} // end of namespace Pyramid_Space
} // end of namespace Raster_GIS
