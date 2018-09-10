#include "FeatureLayer.h"

namespace GIS_Embedded
{
	int FeatureLayer::initStyleByScale(int scale)
	{
		m_is_path_layer = false;

		if(m_feature_layer_style.initGmStyleByScale(scale)<0)
			return -1;

		if(m_layer_type == t_LINE)
		{
			if(m_feature_layer_style.m_current_style->m_line_render->m_line_style == PATH)
			m_is_path_layer = true;
		}

		return 0;
	}
}