#ifndef _CORE_RENDERENGINE_FEATURELAYERSTYLE_H
#define _CORE_RENDERENGINE_FEATURELAYERSTYLE_H


#include<vector>
#include "../Render/GmRender/GeoGmRender.h"

namespace GIS_Embedded{
class FeatureLayerStyle
{

public:
	FeatureLayerStyle()
	{
		m_current_style = NULL;
	}

	int initGmStyleByScale(int scale);

public:
	vector<pair<int,GmStyleRender> > m_ceiline_scale_styles;//_CEILING_SCALE_STYLES; //初始化时先放大的scale值
	GmStyleRender* m_current_style;//_CURRENT_STYLE;
    
};
}
#endif