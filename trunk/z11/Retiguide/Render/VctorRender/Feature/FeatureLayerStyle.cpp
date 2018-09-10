#include "FeatureLayerStyle.h"
using namespace std;

namespace GIS_Embedded
{
	int FeatureLayerStyle::initGmStyleByScale(int scale)
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
		if(scale > m_ceiline_scale_styles[i].first/* && i>0*/)
			i--;
	
		if (i >= 0)
		{
			m_current_style = &m_ceiline_scale_styles[i].second;
			return 0;
		}

		return -1;
	}


	//void FeatureLayerStyle::addGmLevelRender(double scale,GmStyleRender &object_level_render)
	//{
	//	_level_group_scale_index.push_back(scale);
	//	_object_level_render_group.push_back(&object_level_render);
	//}

	//void FeatureLayerStyle::addGmStyleRender(double scale,GmStyleRender &object_style_render)
	//{
	//	_style_group_scale_index.push_back(scale);
	//	_object_sytle_render_group.push_back(&object_style_render);

	//}

	//GmStyleRender* FeatureLayerStyle::getGmStyleRender(double scale)
	//{
	//	if(_object_level_render_group.size() !=0)
	//	{
	//		int i = 0, sum = _level_group_scale_index.size();
	//		while(i<sum && scale < _level_group_scale_index[i])
	//			i++;

	//		if(i>=sum)
	//			i = sum-1;
	//		return _object_level_render_group[i];

	//	}
	//	else
	//		return _object_sytle_render_group[0];

	//}
}