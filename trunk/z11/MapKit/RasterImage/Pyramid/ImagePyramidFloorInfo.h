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
			unsigned int number;		//����
			MapScale width , height;	// ��Ŀ�͸�
			unsigned int regular_tile_number_per_row; // �����߿���
			unsigned int non_regular_tile_number_per_row; //
			unsigned int row_number;     //�ò�����
		};
//	}
}
}

#endif
