#ifndef _CORE_WORKSPACE_WSFEATURELAYERSTYLE_H
#define _CORE_WORKSPACE_WSFEATURELAYERSTYLE_H
#include <vector>
#include "WSPointStyle.h"
#include "WSLineStyle.h"
#include "WSPolygonStyle.h"
#include "WSLabelStyle.h"
#include "../basic_types.h"
using namespace std;

namespace GIS_Embedded{

	struct WSLayerStyle
	{
		vector<pair<int,WSPointStyle> > POINT_STYLE;
		vector<pair<int,WSLineStyle> > LINE_STYLE;
		vector<pair<int,WSPolygonStyle> > POLYGON_STYLE;
		vector<pair<int,WSLabelStyle> > LABEL_STYLE;
	};
//
//class WSFeatureLayerStyle
//{
//
//public:
//    WSPointStyle POINT;
//    WSLineStyle LINE;
//    WSPolygonStyle POLYGON;
//	vector<GO_ID> GOID_VECTOR;
//	int MAX_SCALE;
//
//    WSFeatureLayerStyle()
//    {
//    	
//    }
//};
}

#endif