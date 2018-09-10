#ifndef _CORE_RENDERSTYLE_ANNOTATIONLAYERSTYLE_H
#define _CORE_RENDERSTYLE_ANNOTATIONLAYERSTYLE_H

#include<string>
#include<vector>
#include "Retiguide/RenderEngine/LabelRender/LabelStyleRender.h"

using namespace std;

namespace GIS_Embedded{
class AnnotationLayerStyle
{

public:

	AnnotationLayerStyle()
	{
		m_current_style = NULL;
	}

	int initGmStyleByScale(int scale);


public:
	vector<pair<int,LabelStyleRender> > m_ceiline_scale_styles;//_CEILING_SCALE_STYLES;
	LabelStyleRender* m_current_style;//_CURRENT_STYLE;
    
};
}
#endif