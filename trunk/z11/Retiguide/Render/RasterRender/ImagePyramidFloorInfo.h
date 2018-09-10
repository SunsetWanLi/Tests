#ifndef _RASTERIMAGE_PYRAMID_FLOOR_INFO_H_ //Constructor_BMP_  
#define _RASTERIMAGE_PYRAMID_FLOOR_INFO_H_ //_Constructor_BMP  

#include "PyramidScaler.h"
#include "Retiguide/Common/crs/basic_gm_type.h"
namespace GIS_Embedded
{

//	namespace Constructor_BMP
//	{
		struct ImagePyramidFloorInfo
		{
			z11::uint_32 number;		//层编号  
			MapScale width , height;	// 层的宽和高  
			z11::uint_32 regular_tile_number_per_row; // 规则瓦块数  
			z11::uint_32 non_regular_tile_number_per_row; //
			z11::uint_32 row_number;     //该层行数  

			z11::uint_32 tiles_count_per_file;//zhaoling  
		};
//	}

}

#endif
