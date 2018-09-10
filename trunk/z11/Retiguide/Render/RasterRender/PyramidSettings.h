#ifndef _RASTERIMAGE_PYRAMID_SETTINGS_H_
#define _RASTERIMAGE_PYRAMID_SETTINGS_H_	

#include <string>
#include "CommonBase/CommonDef.h"
//extern z11::uint_32 load;
//extern z11::uint_32 hit;

namespace GIS_Embedded
{

	enum SETTINGS
	{
		DEFAULT_PYRAMID_MULTI 					= 	2, // Don't change it in this version.
		REGULAR_TILE_SIZE						=	417, // Pixels 208 ==> 127K , 295 ==> 256K , 417 ==> 510K
	//	REGULAR_TILE_IMAGE_SIZE					=	REGULAR_TILE_SIZE * REGULAR_TILE_SIZE * 3, // bytes
		MAX_IMAGE_BUFFER_SIZE					=	static_cast<z11::uint_32>(33554432 * 1 / 1), // bytes ( 32 M * n)
	//	IMAGE_BUFFER_ENTRY						=	static_cast<z11::uint_32>(MAX_IMAGE_BUFFER_SIZE * 1.0 / REGULAR_TILE_IMAGE_SIZE), 
		DISK_ACCESS_BLOCK_SIZE					=	64 * 1024 // Bytes , only for reserving
	};
	
	const double BUFFER_SIZE_RESERVE_RATE	= -999.0;
	const double BUFFER_ENTRY_RESERVE_RATE	= -999.0;
	const double BUFFER_ENTRY_RELEASE_RATE	= 0.15;
	const bool CRITICAL_BUFFER_SIZE			= false;
	const double BUFFER_ERROR_RATE			= 0.1;
	const z11::uint_32 MAX_SLEEP_COUNT		= 20;


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
		TYPE_SINGLE_FILE = 4   //单文件 add by wlp 2008-08-01
	};

//	enum PYRAMID_FILE_TYPE
//	{
//		TYPE_SINGLE = 1,
///		TYPE_MUTIL = 2
//	};
	
	
	//const std::string PYRAMID_IMAGE_EXTENSION[] = {L"", L".bmp", L".jpg", L".prm"};

} // end of namespace GIS_Embedded
#endif
