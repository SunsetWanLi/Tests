#pragma once
#include "../Common/Types/GeoTypes/Types.h"
#include "../Common/Types/GeoTypes/GuiderGeoPointTrigger.h"
#include "../Common/Types/basic_types.h"
#include "DataReader.h"

namespace GIS_Embedded{
	class GuiderGeoPointAdapter{
	private:
		static int languages;
		static int getGuiderGeoPointNoTrigger(DataReader &reader,GuiderGeoPoint**ggp);
		static int getGuiderGeoPointTrigger(DataReader &reader,GuiderGeoPoint**ggp);
	public:
		static int translate(GGPTypeT type,byte_8 *data,int size, vector<GuiderGeoPoint*> **gg_points);
		static int translateTrigerArea(byte_8 *data,int size,vector<PolygonArea> **ploygon_areas);
		static int translateFileInfos(byte_8 *data, int size,FileInfos** file_infos);
	};
}