#include "AnnotationLayerRender.h"
#include "../Render/LabelRender/GeoLabelPosTrans.h"

namespace GIS_Embedded
{
	void AnnotationLayerRender::renderAnnotationLayer(StyleRender &mg,AnnotationLayer& annotation_layer)
	{
		LabelStyleRender* label_style_render = annotation_layer.m_annogation_layer_style.m_current_style;
		if(label_style_render == NULL)
			return;

		mg.setLabelStyleRender(label_style_render);

		if(annotation_layer.m_layer_type == FeatureLayer::t_POINT)
		{
			int sum = annotation_layer.m_labels.size();
			for(int i=0;i<sum;i++)
			{
				mg.drawString(annotation_layer.m_label_lfTp[i].x,annotation_layer.m_label_lfTp[i].y,0,annotation_layer.m_labels[i]);
			}
		}
		else if(annotation_layer.m_layer_type == FeatureLayer::t_LINE)
		{
			int sum = annotation_layer.m_labels.size();
			for(int i=0;i<sum;i++)
			{
				mg.drawString(annotation_layer.m_label_lfTp[i].x,annotation_layer.m_label_lfTp[i].y,annotation_layer.m_escape[i], annotation_layer.m_labels[i]);
			}

			sum = annotation_layer.m_chars.size();
			for(int i=0;i<sum;i++)
			{
 				mg.drawCharacter(annotation_layer.m_char_lfTp[i].x,annotation_layer.m_char_lfTp[i].y,0,annotation_layer.m_chars[i]);
			}
		}
		//lw
		else if (annotation_layer.m_layer_type == FeatureLayer::t_POLYGON)
		{
			int sum = annotation_layer.m_labels.size();

			for(int i = 0 ;i < sum ; i++)
			{
				mg.drawStringCenterXY(annotation_layer.m_label_lfTp[i].x,annotation_layer.m_label_lfTp[i].y,annotation_layer.m_escape[i],annotation_layer.m_labels[i]);
			}
		}
		//GRF
		//annotation_layer.clearGeoLabelPos();

	}
}
