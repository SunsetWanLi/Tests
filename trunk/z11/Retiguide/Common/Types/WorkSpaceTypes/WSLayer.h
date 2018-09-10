#ifndef _CORE_WORKSPACE_WSLAYER_H
#define _CORE_WORKSPACE_WSLAYER_H

#include<string>
#include<vector>
#include"../../Share/Scope.h"
#include"WSLayerStyle.h"
#include "WSRenderLevel.h"
#include "CommonBase/MGWString.h"

using namespace std;
using namespace z11;


namespace GIS_Embedded

{
	struct WSLayer
	{
		z11::MGWString LAYER_ID;
		short LAYER_TYPE; //点，线，面, 图片, 1,2,3,4
		int ZOOM_MAX_GM;
		int ZOOM_MIN_GM;
		int ZOOM_MAX_LABEL;
		int ZOOM_MIN_LABEL;
		WSLayerStyle LAYER_STYLE;
	};

	struct WSCrossLayer
	{
		int FRC;
		int WIDTH;
		int GRADE;
	};
}

#endif