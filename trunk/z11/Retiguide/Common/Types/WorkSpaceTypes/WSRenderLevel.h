#ifndef _CORE_WORKSPACE_WSRENDERLEVEL_H
#define _CORE_WORKSPACE_WSRENDERLEVEL_H

#include "WSPointStyle.h"
#include "WSLineStyle.h"
#include "WSPolygonStyle.h"

namespace GIS_Embedded{
class WSRenderLevel
{

public:
    WSPointStyle MGPoint;
    WSLineStyle LINE;
    WSPolygonStyle POLYGON;

	double SCALE;

    WSRenderLevel()
    {
    	SCALE = 0;
    }
};
}

#endif