#ifndef _CORE_WORKSPACE_WSFEATURELAYER_H
#define _CORE_WORKSPACE_WSFEATURELAYER_H

#include<string>
#include<vector>
#include"../../Share/Scope.h"
#include"WSFeatureLayerStyle.h"
#include "WSRenderLevel.h"
#include "CommonBase/MGWString.h"

using namespace std;
using namespace z11;

namespace GIS_Embedded{
class WSFeatureLayer
{

public:

	z11::MGWString LAYER_ID;
    z11::MGWString LAYER_NAME;
    int INDEX;

	vector<WSRenderLevel*>	RENDER_LEVEL_GROUP; //loujt,08-06-04,依照框架实现，引入此参数来配置不同显示比例下的样式
	vector<WSFeatureLayerStyle*> FEATURE_STYLE_GROUP;//第一个为默认样式
    Scope EXTENSION;
    int COL;
    int ROW;
    double MIN_DISPLAY_RATIO;
    double MAX_DISPLAY_RATIO;
    int DEFAULT_FLAG;
    int FLOAT_FLAG;
    int EDITABLE_FLAG;
    int VISIBLE_FLAG;
    int SHOWABLE_FLAG;
    
    WSFeatureLayer()
    {

    }

	~WSFeatureLayer()
	{
		unsigned int i,sum;
		for(i=0,sum=FEATURE_STYLE_GROUP.size();i<sum;i++)
		{
			WSFeatureLayerStyle* feature_layer_style = FEATURE_STYLE_GROUP[i];
			delete feature_layer_style;

		}

		for(i=0,sum=RENDER_LEVEL_GROUP.size();i<sum;i++)
		{
			WSRenderLevel* render_level = RENDER_LEVEL_GROUP[i];
			delete render_level;

		}

	}
};
}

#endif