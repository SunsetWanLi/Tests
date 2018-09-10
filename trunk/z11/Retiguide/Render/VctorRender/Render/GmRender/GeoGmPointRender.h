#ifndef _CORE_RENDERENGINE_GEOGMPOINTRENDER_H
#define _CORE_RENDERENGINE_GEOGMPOINTRENDER_H

#include "GeoGmRender.h"
using namespace std;

namespace GIS_Embedded{

class GeoGmPointRender:public GeoGmRender
{

public:

	GeoGmPointRender() {}

    int render(StyleRender& mg);
};

}

#endif