#ifndef _WORKSPACE_ADAPTER_H_
#define _WORKSPACE_ADAPTER_H_

#include <fstream>
#include <vector>
#include "../Common/Types/DataFetcherTypes/TypeDefs.h"
#include "TypeDefs.h"

#include "../Common/Types/WorkSpaceTypes/WSLayer.h"
#include "../Common/Types/WorkSpaceTypes/WSLayerStyle.h"
#include "../Common/Types/WorkSpaceTypes/WSPointStyle.h"
#include "../Common/Types/WorkSpaceTypes/WSLineStyle.h"
#include "../Common/Types/WorkSpaceTypes/WSPolygonStyle.h"
#include "../Common/Types/WorkSpaceTypes/WSLabelStyle.h"
#include "../Common/Share/Scope.h"
#include "../Common/Types/WorkSpaceTypes/WorkSpace.h"
#include "DataReader.h"

using namespace std;

namespace GIS_Embedded
{

	class WorkSpaceAdapter
	{
	private:
		static void getLayerStyle(DataReader &read,WSLayer* layer);
		static void getPointStyle(DataReader& read,pair<int,WSPointStyle>* point_style);
		static void getLineStyle(DataReader& read,pair<int,WSLineStyle>* line_style);
		static void getPolygonStyle(DataReader& read,pair<int,WSPolygonStyle>* polygon_style);
		static void getLabelStyle(DataReader& read,pair<int,WSLabelStyle>* label_style);

		static void getCrossStyle(DataReader &read,CrossType* cross_type);
		static void getCrossLayerStyle(DataReader &read,WSCrossLayer* layer);

		static int _last_context_color;

	public:
		static int translate(byte_8 *data, int size, WorkSpace *ws);
	};

}

#endif