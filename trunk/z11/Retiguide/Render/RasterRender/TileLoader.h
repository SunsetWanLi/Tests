#ifndef _RASTERIMAGE_TILE_LOADER_H_
#define _RASTERIMAGE_TILE_LOADER_H_
#pragma once

//#include "../CommonInclude.h"
//#include "../RasterType.h"
//#include "../RepositoryType.h"
#include "Retiguide/Common/crs/basic_gm_type.h"


namespace GIS_Embedded
{

struct ImageBufferEntry;
struct Floor;

class TileLoader
{
public:
	TileLoader(enum RasterType raster_type, enum RepositoryType repo_type, const std::string& repo, std::vector<Floor> floor_info);
	~TileLoader();
	virtual RasterType getRasterType();
	virtual RepositoryType getRepositoryType();
	virtual bool loadTile(const std::string& map_name, z11::uint_32 floor, z11::uint_32 row, z11::uint_32 col, ImageBufferEntry* entry);

private:
	RasterType m_raster_type;
	RepositoryType m_repo_type;
	std::string m_repo;
	std::string m_file_ext;
	std::vector<Floor> m_floor_info;
};

}
#endif