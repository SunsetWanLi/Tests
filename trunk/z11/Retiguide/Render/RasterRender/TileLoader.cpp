#include "TileLoader.h"
#include "PyramidTileBuffer.h"
#include "ImagePyramidFloorInfo.h"
#include "ImagePyramidUpload.h"
#include "ImagePyramid.h"
#include "PyramidTileIndex.h"
#include "GraphicCore/MGColor.h"
#include "GraphicCore/MGImage.h"
#include <assert.h>
#include <string>
#include <sstream>
using namespace z11;
using namespace std;
namespace GIS_Embedded
{

TileLoader::TileLoader(enum RasterType raster_type, enum RepositoryType repo_type, const string& repo, std::vector<Floor> floor_info)
{
	m_raster_type = raster_type;
	m_repo_type = repo_type;
	m_repo = repo;
	m_floor_info = floor_info;

	switch(m_raster_type)
	{
	case RT_GRID:
		assert(0);
		break;
	case RT_BMP:
		m_file_ext = ".bmp";
		break;
	case RT_JPG:
		m_file_ext = ".jpg";
		break;
	case RT_TIFF:
		m_file_ext = ".tiff";
		break;
	}
}

TileLoader::~TileLoader()
{

}

bool TileLoader::loadTile(const string& map_name, z11::uint_32 floor, z11::uint_32 row, z11::uint_32 col, ImageBufferEntry* entry)
{
	if(NULL == entry)
		return false;

	if(m_floor_info.size() < floor)
		return false;
	int total_rows = m_floor_info[floor].row_number, 
		total_cols = m_floor_info[floor].regular_per_row + m_floor_info[floor].non_regular_per_row;
	z11::uint_32 tiles_count_per_file = 500;//zhaoling

	int downward_row = total_rows - 1 - row;
	int tile_per_row = total_cols;
	int index =  downward_row * tile_per_row  + col;

	stringstream grand_parent_filename;
	grand_parent_filename<<m_repo<<map_name<<"_"<<floor;

	stringstream parent_filename;
	parent_filename<<"/"<<map_name<<"_"<<floor<<"_"<<index / tiles_count_per_file;/*MAX_TILES_COUNT_PER_FILE;*/

	stringstream sub_filename;	
	sub_filename<<"/"<<map_name<<"_"<<floor<<"_"<<index/*row<<L"_"<<col*/;

	string tile_path = grand_parent_filename.str() + parent_filename.str() + sub_filename.str();
	tile_path.append(m_file_ext);

	//string tile_path;
	//wss>>tile_path;
	//tile_path.append(m_file_ext);

	if (entry->color != NULL)
		delete_and_set_null(entry->color);
	entry->color = z11::MGColor::fromImage(tile_path);
	entry ->ready_for_read = true;
	entry->ready_bytes = entry->color->m_image->content_info.image_size;

	entry->image_height = entry->color->m_image->getHeight();
	entry->image_width = entry->color->m_image->getWidth();
	entry->buffer_size = entry->color->m_image->content_info.image_size;
	
	return true;
}


RasterType TileLoader::getRasterType()
{
	return m_raster_type;
}
RepositoryType TileLoader::getRepositoryType()
{
	return m_repo_type;
}

}