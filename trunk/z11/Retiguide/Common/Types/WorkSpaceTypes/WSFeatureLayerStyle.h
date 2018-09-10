#ifndef _CORE_WORKSPACE_WSFEATURELAYERSTYLE_H
#define _CORE_WORKSPACE_WSFEATURELAYERSTYLE_H
#include <vector>
#include "WSPointStyle.h"
#include "WSLineStyle.h"
#include "WSPolygonStyle.h"
#include "../basic_types.h"
using namespace std;

namespace GIS_Embedded{
class WSFeatureLayerStyle
{

public:
    WSPointStyle POINT;
    WSLineStyle LINE;
    WSPolygonStyle POLYGON;
	vector<GO_ID> GOID_VECTOR;
	int MAX_SCALE;

    WSFeatureLayerStyle()
    {
    	
    }
};
}

#endif