#ifndef _CORE_WORKSPACE_WSLABELSTYLE_H
#define _CORE_WORKSPACE_WSLABELSTYLE_H
#include <string>
#include "WSPolygonStyle.h"
#include "CommonBase/MGWString.h"

using namespace std;
using namespace z11;


namespace GIS_Embedded{

struct WSLabelStyle {

	z11::MGWString FONT_NAME;
	short FONT_SIZE;
	short FONT_WEIGHT;
	int FONT_COLOR;
	int FONT_BG_COLOR;
	
};
}
#endif