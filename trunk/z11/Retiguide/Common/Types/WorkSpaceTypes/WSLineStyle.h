#ifndef _CORE_WORKSPACE_WSLINESTYLE_H
#define _CORE_WORKSPACE_WSLINESTYLE_H

#include "WSPointStyle.h"

namespace GIS_Embedded{
struct WSLineStyle
{
	short TYPE; //实线，虚线，道路:1,2,3
	short WIDTH;
	int CURVE_COLOR;
	int FACE_COLOR;
	short COLOR_ALPHA;
	short FILTER_PARA;
	bool ANTIALIAS;
//
//public:
//
//	WSPointStyle POINT;
//    int COLOR;
//    int COLOR_ALPHA;
//    int LINE_WIDTH;
//    int LINE_STYLE;
//
//    WSLineStyle()
//    {
//    	COLOR=0x00;
//    	LINE_WIDTH=1;
//		LINE_STYLE = 0;
//    }
};
}
#endif