#include "AnnotationLayerStyle.h"

namespace GIS_Embedded
{
	int AnnotationLayerStyle::initGmStyleByScale(int scale)
	{
		m_current_style = NULL;

		int i=0,sum=m_ceiline_scale_styles.size();
		if(sum <= 0)
			return -1;

		while(i<sum && scale < m_ceiline_scale_styles[i].first)
			i++;

		if(i>=sum) //all ceiling > scale
			i = sum-1;
		
		// scale maybe > the ceiling[i], assert scale <= ceiling[0]
		if(scale > m_ceiline_scale_styles[i].first && i>0)
			i--;
		
		m_current_style = &m_ceiline_scale_styles[i].second;

		return 0;
	}
}

//void AnnotationLayerStyle::setLabelStyleRender(LabelStyleRender& label_style_render)
//{
//	_label_style_render = &label_style_render;
//}

//LabelStyleRender* AnnotationLayerStyle::getLabelStyleRender()
//{
//	return _label_style_render;
//
//}