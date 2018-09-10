#ifndef __RASTERIMAGE_PYRAMID_SETTINGS_H__
#define __RASTERIMAGE_PYRAMID_SETTINGS_H__

#include <string>

//extern unsigned int load;
//extern unsigned int hit;

namespace Raster_GIS
{
namespace Pyramid_Space
{
	enum SETTINGS
	{
		DEFAULT_PYRAMID_MULTI 					= 	2, // Don't change it in this version.
		REGULAR_TILE_SIZE						=	417, // Pixels 208 ==> 127K , 295 ==> 256K , 417 ==> 510K
	//	REGULAR_TILE_IMAGE_SIZE					=	REGULAR_TILE_SIZE * REGULAR_TILE_SIZE * 3, // bytes
		MAX_IMAGE_BUFFER_SIZE					=	static_cast<unsigned int>(33554432 * 1 / 1), // bytes ( 32 M * n)
	//	IMAGE_BUFFER_ENTRY						=	static_cast<unsigned int>(MAX_IMAGE_BUFFER_SIZE * 1.0 / REGULAR_TILE_IMAGE_SIZE), 
		DISK_ACCESS_BLOCK_SIZE					=	64 * 1024 // Bytes , only for reserving
	};
	
	const double BUFFER_SIZE_RESERVE_RATE	= -999.0;
	const double BUFFER_ENTRY_RESERVE_RATE	= -999.0;
	const double BUFFER_ENTRY_RELEASE_RATE	= 0.15;
	const bool CRITICAL_BUFFER_SIZE			= false;
	const double BUFFER_ERROR_RATE			= 0.1;
	const unsigned int MAX_SLEEP_COUNT		= 20;


	const double MIN_FLOOT_DELTA			= 0.000001;
	const double MIN_DEGREE_DELTA			= 0.000001;
	const double MIN_MINUTE_DELTA			= 0.00001;
	const double MIN_SECOND_DELTA			= 0.0001;

	const double ACCEPTABLE_RESOLUTION_LOSS_RATE =  MIN_FLOOT_DELTA;

	const bool LOW_COST_GET					= true; // LOW_COST does not mean poor efficiency.

	enum PYRAMID_IMAGE_TYPE
	{
		TYPE_BMP = 1,  
		TYPE_JPG = 2,
		TYPE_JPG_PRM = 3,
		TYPE_SINGLE_FILE = 4   //µ¥ÎÄ¼þ add by wlp 2008-08-01
	};

//	enum PYRAMID_FILE_TYPE
//	{
//		TYPE_SINGLE = 1,
///		TYPE_MUTIL = 2
//	};
	
	
	//const std::std::string PYRAMID_IMAGE_EXTENSION[] = {L"", L".bmp", L".jpg", L".prm"};
} // end of namespace Pyramid_Space
} // end of namespace Raster_GIS
#endif
