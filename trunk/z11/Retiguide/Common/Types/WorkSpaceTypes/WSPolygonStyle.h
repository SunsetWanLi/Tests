#ifndef _CORE_WORKSPACE_WSPOLYGONSTYLE_H
#define _CORE_WORKSPACE_WSPOLYGONSTYLE_H
#include "CommonBase/MGWString.h"

using namespace std;
using namespace z11;

namespace GIS_Embedded{
struct WSPolygonStyle
{
	int FACE_COLOR;
	int CONTEXT_COLOR;
	short COLOR_ALPHA;
	int EDGE_COLOR;
	z11::MGWString PATTERN_PIC;
	short FILTER_PARA;
	bool ANTIALIAS;
	

};
}
#endif