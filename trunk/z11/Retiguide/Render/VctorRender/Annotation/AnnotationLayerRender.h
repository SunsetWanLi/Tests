#ifndef _CORE_RENDERENGINE_ANNOTATIONLAYERRENDER_H
#define _CORE_RENDERENGINE_ANNOTATIONLAYERRENDER_H

#include "Retiguide/Common/Util/Transformer.h"
#include "../Render/LabelRender/GeoLabelRender.h"
#include "../Render/StyleRender.h"
#include "AnnotationLayer.h"
#include "Retiguide/Common/Types/Settings.h"
#include "../Map/ScreenPixMap.h"
#include "../Render/LabelRender/GeoLabelPointRender.h"
#include "../Render/LabelRender/GeoLabelLineRender.h"
//#include <vector>

namespace GIS_Embedded{
class AnnotationLayerRender {

public:
	AnnotationLayerRender(/*Settings* &setConf*/)//:m_setting_config(setConf)
	{
		//polygon_annotation.resize(127);
	}
		
	void setTransformer(const Transformer* trans)
	{
		m_transformer = trans;
	}
	//void renderAnnotationLayer(StyleRender &mg,AnnotationLayer &annotation_layer,ScreenPixMap &screen_pix_map,vector<GeographyObject *> &layer_data);

	void renderAnnotationLayer(StyleRender &mg,AnnotationLayer& annotation_layer);
private:
	Settings* m_setting_config;

	const Transformer* m_transformer;

	GeoLabelLineRender m_line_label_render;
	GeoLabelPointRender m_point_label_render;
};
}

#endif