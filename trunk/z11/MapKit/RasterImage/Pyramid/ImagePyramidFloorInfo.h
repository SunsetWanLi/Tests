#ifndef __RASTERIMAGE_PYRAMID_SPACE_FLOOR_INFO_H__ //Constructor_BMP_
#define __RASTERIMAGE_PYRAMID_SPACE_FLOOR_INFO_H__ //_Constructor_BMP

namespace Raster_GIS
{
namespace Pyramid_Space
{
//	namespace Constructor_BMP
//	{
		struct ImagePyramidFloorInfo
		{
			unsigned int number;		//层编号
			MapScale width , height;	// 层的宽和高
			unsigned int regular_tile_number_per_row; // 规则瓦块数
			unsigned int non_regular_tile_number_per_row; //
			unsigned int row_number;     //该层行数
		};
//	}
}
}

#endif
