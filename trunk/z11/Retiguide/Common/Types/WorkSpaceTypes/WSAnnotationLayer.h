#ifndef _CORE_WORKSPACE_WSANNOTATIONLAYER_H
#define _CORE_WORKSPACE_WSANNOTATIONLAYER_H

#include<string>
#include"WSAnnotationLayerStyle.h"
#include "CommonBase/MGWString.h"

using namespace std;
using namespace z11;

namespace GIS_Embedded{
class WSAnnotationLayer
{

public:

	z11::MGWString LAYER_ID;
    z11::MGWString LAYER_NAME;
    int INDEX;
    vector<WSAnnotationLayerStyle*> ANNOTATION_STYLE_GROUP;//第一个为默认样式
    int MIN_DISPLAY_RATIO;
    int MAX_DISPLAY_RATIO;

    WSAnnotationLayer()
    {
    }

	~WSAnnotationLayer()
	{
		unsigned int i,sum;
		for(i=0,sum=ANNOTATION_STYLE_GROUP.size();i<sum;i++)
		{
			WSAnnotationLayerStyle* annotation_layer_style = ANNOTATION_STYLE_GROUP[i];
			delete annotation_layer_style;

		}

	}
};
}
#endif