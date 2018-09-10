#include "AnnotationLayer.h"
#include "../Render/LabelRender/GeoLabelPosTrans.h"
#include "Retiguide/Common/Types/Settings.h"

using namespace GIS_Embedded;

namespace GIS_Embedded
{
	int AnnotationLayer::initStyleByScale(int scale)
	{
		return m_annogation_layer_style.initGmStyleByScale(scale);
	}

	int AnnotationLayer::checkGeoLabelPos(const z11::MGPoint* ctrl_pos,const int ctrl_num, const std::string& label,ScreenPixMap& screen_map, const TextLanguage& language)
	{
		static z11::MGPoint left_top;
		if(m_layer_type ==FeatureLayer::t_POINT)
		{
			GeoLabelPosTrans::transGeometryPointPos(&m_label_lfTp,&m_labels,&m_escape,
									label,m_annogation_layer_style.m_current_style->m_font_size,ctrl_pos[0],screen_map);
		}
		else if(m_layer_type ==FeatureLayer::t_LINE)
		{
			//if(language == TXT_ARABIC|| !label.empty() && static_cast<const unsigned char>(label[0])<128)//english or arabic
			//{

				GeoLabelPosTrans::transGeometryLinePos(/*&m_test,*/&m_label_lfTp,&m_labels,&m_escape,
									label,m_annogation_layer_style.m_current_style->m_font_size,ctrl_pos,ctrl_num,screen_map);
			//}
			//else //chinese
			//{
			//	GeoLabelPosTrans::transGeometryLinePosChs(&m_char_lfTp,&m_chars,
			//						label,m_annogation_layer_style.m_current_style->m_font_size,ctrl_pos,ctrl_num,screen_map);
			//}

		}
		else if (m_layer_type == FeatureLayer::t_POLYGON)
		{
			//if(language == TXT_ARABIC|| !label.empty() && label[0]<127)//english or arabic
			//{
				
				GeoLabelPosTrans::transGeometryPolygonPos(&m_label_lfTp,&m_labels,&m_escape,
					label,m_annogation_layer_style.m_current_style->m_font_size,ctrl_pos,ctrl_num,screen_map);
			//else //chinese
			//{
				//GeoLabelPosTrans::transGeometryLinePos(&m_char_lfTp,&m_chars,
				//	label,m_annogation_layer_style.m_current_style->m_font_size,ctrl_pos,ctrl_num,screen_map);
			//}
		}
		return 0;
	}
}

//AnnotationLayer::AnnotationLayer()
//	 {
//
//	 }
//
//AnnotationLayer::~AnnotationLayer()
//{
//	delete _annotation_layer_style;
//
//}
//
//void AnnotationLayer::setLayerId(wstring layer_id)
//	 {
//	  _layer_id = layer_id;
//	 }
//
//void AnnotationLayer::setAnnotationRenderStyle(AnnotationLayerStyle& annotation_layer_style)
//	 {
//	 _annotation_layer_style = &annotation_layer_style;
//	 }
//
//void AnnotationLayer::needNoRender()
//	 {
//	  _need_render = false;
//	 }
//
//void AnnotationLayer::needRender()
//	 {
//	   _need_render = true;
//	 }
//
//bool AnnotationLayer::getRenderFlag()
//	    {
//	        return _need_render;
//	    }
//
//wstring AnnotationLayer::getLayerId()
//	    {
//	        return _layer_id;
//	    }
//
//AnnotationLayerStyle* AnnotationLayer::getAnnotationLayerRenderStyle()
//	    {
//	        return _annotation_layer_style;
//	    }

