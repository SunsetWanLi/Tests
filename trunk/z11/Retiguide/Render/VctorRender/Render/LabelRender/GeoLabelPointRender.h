#ifndef _CORE_RENDERENGINE_GEOLABELPOINTRENDER_H
#define _CORE_RENDERENGINE_GEOLABELPOINTRENDER_H
#include "GeoLabelRender.h"
//#include "../../../../Common/Types/GeoTypes/GeographyObject.h"
#include "../StyleRender.h"
#include "../../Map/ScreenPixMap.h"
namespace GIS_Embedded
{
class GeoLabelPointRender:public GeoLabelRender
{

private:
	long _screen_x;
    long _screen_y;

public:
	GeoLabelPointRender(){}    
    void render(StyleRender& mg, ScreenPixMap& screen_pix_map);
    void getLabelCtrlPoints();
};
}
#endif