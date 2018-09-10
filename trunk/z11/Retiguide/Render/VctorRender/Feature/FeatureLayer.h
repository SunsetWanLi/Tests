#ifndef _CORE_RENDERENGINE_FEATURELAYER_H
#define _CORE_RENDERENGINE_FEATURELAYER_H

#include <string>
#include "FeatureLayerStyle.h"
#include "Retiguide/Common/Share/Scope.h"
#include "CommonBase/MGWString.h"
using namespace std;
using namespace z11;

namespace GIS_Embedded{


class FeatureLayer {

public:
	FeatureLayer():m_is_path_layer(false) {}

	int initStyleByScale(int scale);

public:
	enum FeatureType
	{
		t_POINT = 1,
		t_LINE,
		t_POLYGON,
		t_IMAGE
	};

	z11::MGWString m_layer_id;
	int m_layer_global_index;
	FeatureType m_layer_type;
	FeatureLayerStyle m_feature_layer_style;

	bool m_need_render;
	int m_zoom_max;
	int m_zoom_min;

	bool m_is_path_layer;

};
}
#endif