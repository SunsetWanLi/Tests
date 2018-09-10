#ifndef _COMMON_TYPES_MAPCONFIGTYPE_H
#define _COMMON_TYPES_MAPCONFIGTYPE_H

#include "basic_types.h"
#include <vector>
#include <string>
#include "CommonBase/MGWString.h"

using namespace std;
using namespace z11;


namespace GIS_Embedded
{
	struct MapConfigType
	{
	public:
		string gps_com_port;
		int gps_baunrate;
		int gps_simulate;
		int gps_need_read;
		int gps_need_record;

		vector<int> scale_ranks;
		int scale_pixel_width;
		int initial_scale;
		int query_resize_scale;

		int navi_need;
		int navi_scale;

		double cross_scope_width;

		int show_3d_map;

		int grid_side_divider;
		//lw rastermap
		int raster_map;
		int vector_map;
		string raster_file;

	};

}
#endif