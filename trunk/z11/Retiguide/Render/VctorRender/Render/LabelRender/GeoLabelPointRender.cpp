#include "GeoLabelPointRender.h"
#include "Retiguide/Common/Share/ScreenPos2D.h"
#include "MapKit/Embedded/BasicType/Point.h"
#include "System/Shared/Utility.h"
#include "CommonBase/MGWString.h"
#include "Retiguide/Common/String_Tools.h"
using namespace z11;
using namespace Embedded;
using namespace GIS_Embedded;

void GeoLabelPointRender::render(StyleRender& mg, ScreenPixMap& screen_pix_map)
{
	std::string label = Utility::wstr2ustr( _geo->getName());

    if(label == "")
        return;
    _screen_x = -1;
    _screen_y = -1;
    getLabelCtrlPoints();
    if(_screen_x < 0 || _screen_y < 0)
        return;

	mg.drawStringCenterXY(_screen_x,_screen_y,0,label);
    
// 	_screen_x += 15;
// 	_screen_y -= 5;
// 
//     UserScreenRect rect;
// 	mg.getStringBound(label,&rect);
//     rect._left += _screen_x;
// 	rect._right += _screen_x;
//     rect._top += _screen_y;
//     rect._bottom += _screen_y;

//     if(screen_pix_map.testDirty(rect))
//     {
//         return;
//     } 
//     else
//     {
//         screen_pix_map.setDirty(rect);
//         
// 		mg.drawString(rect._left, rect._top, 0, label);
//         return;
//    }

}

void GeoLabelPointRender::getLabelCtrlPoints()
{
	Embedded::Point* gm_point = dynamic_cast< Embedded::Point*>(_geo);
	Embedded::ScreenPos2D pos;
	gm_point->getPoint(&pos);
 	_transformer->transfromMapToScr(pos.d0,pos.d1,&_screen_x,&_screen_y);
}