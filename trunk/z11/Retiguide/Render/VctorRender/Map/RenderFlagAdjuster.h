#ifndef _SHELL_UTIL_RENDER_FLAGADJUSTER
#define _SHELL_UTIL_RENDER_FLAGADJUSTER


#include<math.h>
#include "MapFrame.h"
#include "../Feature/FeatureLayer.h"
#include "../Annotation/AnnotationLayer.h"

namespace GIS_Embedded
{
	class RenderFlagAdjuster
	{

	public:
		static void adjust(MapFrame& map_frame,int scale)
		{
			FeatureLayer* f_layer;
			int sum = (int)map_frame.m_feature_layers.size();
			for(int i = 0; i < sum; i++)
			{
				f_layer = map_frame.m_feature_layers[i];

				if(scale > f_layer->m_zoom_min && scale <= f_layer->m_zoom_max)
					f_layer->m_need_render = true;
				else 
					f_layer->m_need_render = false;
			}
	      
			AnnotationLayer* a_layer;
			sum=(int)map_frame.m_annotation_layers.size();
			for (int i=0;i<sum;i++)
			{
	      		a_layer = map_frame.m_annotation_layers[i];

				if(scale > a_layer->m_zomm_min && scale <= a_layer->m_zoom_max)
					a_layer->m_need_render = true;
				else 
					a_layer->m_need_render = false;
			}
		
		}


		//static void adjust(MapFrame& map_frame,Scope& scope)
		//{
		//	FeatureLayer* feature_layer;
		//	AnnotationLayer* annotation_layer;

		//	int sum = map_frame._FEATURE_LAYERS.size();
		//	for(int i = 0; i < sum; i++)
		//	{
		//		feature_layer = map_frame.getFeatureLayerByIndex(i);

		//		double current = sqrt(scope.getArear());
		//		double min_ratio_fits_render = feature_layer->_ZOOM_MIN;
		//		double max_ratio_fits_render = feature_layer->_ZOOM_MAX;

		//		if(max_ratio_fits_render == 0 || (current >= min_ratio_fits_render && current <= max_ratio_fits_render))
		//			feature_layer->_NEED_RENDER = true;
		//		else 
		//			feature_layer->_NEED_RENDER = false;
	 //         
		//	}
	 //     
		//	sum=map_frame.getAnnotationLayerSum();
	 //     
		//	for (int i=0;i<sum;i++)
		//	{
	 //     		annotation_layer=map_frame.getAnnotationLayerByIndex(i);
		//		feature_layer=map_frame.getFeatureLayerByIndex(i);
		//		if (feature_layer->_NEED_RENDER/* && feature_layer->getShowlabelFlag()*/)
		//			annotation_layer->_NEED_RENDER = true;
	 //     		else
	 //     			annotation_layer->_NEED_RENDER = false;
		//	}
		//
		//}

	};


}

#endif