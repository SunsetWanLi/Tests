#ifndef _CORE_RENDERENGINE_GEOGMIMAGERENDER_H
#define _CORE_RENDERENGINE_GEOGMIMAGERENDER_H

#include "GeoGmRender.h"
using namespace std;

namespace GIS_Embedded{

class GeoGmImageRender:public GeoGmRender
{

public:

	GeoGmImageRender() {}
    int render(StyleRender& mg);

// private:
// 	HANDLE mImageHandle;
};

}

#endif