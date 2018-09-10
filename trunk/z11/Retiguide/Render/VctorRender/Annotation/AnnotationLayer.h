#ifndef _CORE_RENDERENGINE_ANNOTATIONLAYER_H
#define _CORE_RENDERENGINE_ANNOTATIONLAYER_H

#include <string>
#include "AnnotationLayerStyle.h"
#include "../Feature/FeatureLayer.h"
#include "../Map/ScreenPixMap.h"
#include "Retiguide/Common/Types/Settings.h"
#include "CommonBase/MGWString.h"

using namespace z11;
using namespace std;

namespace GIS_Embedded{

	class AnnotationLayer 
	{

	public:
		AnnotationLayer() 
		{
		}
		int initStyleByScale(int scale);

		int clearGeoLabelPos()
		{
			m_labels.clear();
			m_label_lfTp.clear();
			m_escape.clear();

			m_chars.clear();
			m_char_lfTp.clear();
			return 0;
		}

int checkGeoLabelPos(const z11::MGPoint* ctrl_pos,const int ctrl_num, const std::string& label,ScreenPixMap& screen_map, const TextLanguage& language);

	public:
		int m_layer_global_index;
		z11::MGWString m_layer_id;

		AnnotationLayerStyle m_annogation_layer_style;
		FeatureLayer::FeatureType m_layer_type;

		bool m_need_render;
		int m_zoom_max;
		int m_zomm_min;

	public:
		vector<std::string> m_labels;
		vector<z11::MGPoint> m_label_lfTp;
		vector<int> m_escape;
		vector<std::string> m_chars;
		vector<z11::MGPoint> m_char_lfTp;
	};


}

#endif