#ifndef _CORE_RENDERENGINE_GEOLABELRENDER_H
#define _CORE_RENDERENGINE_GEOLABELRENDER_H

#include <vector>
#include "Retiguide/Common/Util/Transformer.h"
#include "Retiguide/Common/Share/UserScreenRect.h"
#include "Retiguide/Common/Share/ScreenPos2D.h"
#include "MapKit/Embedded/Tools/SpatialDataBase.h"
#include "../StyleRender.h"
#include "../../Map/ScreenPixMap.h"

using namespace Embedded;
using namespace std;

namespace GIS_Embedded{
class GeoLabelRender
{

protected:
	IShape* _geo;
    const Transformer* _transformer;   

public:
	GeoLabelRender(){}

	virtual ~GeoLabelRender(){}

     Embedded::IShape* getGeographyObject()
 	{
 		return _geo;
 	}

    void setTransformer(const Transformer*& transformer)
	{
		_transformer=transformer;
	}

    virtual void setGeo(Embedded::IShape* geo)
	{
		_geo = geo;
	}
    
    virtual void render(StyleRender& mg, ScreenPixMap& screen_pix_map) = 0;
};
}
#endif