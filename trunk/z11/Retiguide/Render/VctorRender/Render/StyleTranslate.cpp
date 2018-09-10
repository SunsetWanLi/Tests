#include "StyleTranslate.h"
#include "Retiguide/DataFetcher/DataFetcher.h"
 #include "Retiguide/Common/String_Tools.h"

namespace GIS_Embedded
{
 
	void StyleTranslate::transPointStyle(WSPointStyle& style,PointStyleRender **point_style_render)
	{
		PointStyleRender* ret = new PointStyleRender();
		*point_style_render = ret;
		(*point_style_render)->m_filename=Utility::wstr2ustr(style.PIC_NAME);
		(*point_style_render)->m_bit_map=NULL;
		//(*point_style_render)->m_bit_map=MGImage::imageWithContentsOfFile(
		//	MGBundle::mainBundle()->pathWithFileName("GIS_Embedded/image/"+Utility::wstr2ustr(style.PIC_NAME)+".png"));
	}

	void StyleTranslate::transLineStyle(WSLineStyle& style,LineStyleRender **line_style_render)
	{
		LineStyleRender* ret = new LineStyleRender();

		ret->setLineColor(style.CURVE_COLOR);
		ret->setFaceColor(style.FACE_COLOR);

		if(style.TYPE == 1)
			ret->m_line_style = SOLID;
		else if(style.TYPE == 2)
			ret->m_line_style =DASH;
		else if(style.TYPE == 3)
			ret->m_line_style = PATH;

		ret->m_meter_width = style.WIDTH;
		ret->m_filter_meter = style.FILTER_PARA;
		ret->m_is_antialias = style.ANTIALIAS;

		if(ret->m_filter_meter <=0 )
			ret->m_filter_meter = 1;

		*line_style_render = ret;
	}

	void StyleTranslate::transPolygonStyle(WSPolygonStyle& style,PolygonStyleRender **polygon_style_render)
	{
		PolygonStyleRender* ret = new PolygonStyleRender();

		ret->m_is_draw_edge = false;
		ret->m_is_draw_pattern = false;
		ret->m_filter_meter = style.FILTER_PARA;
		ret->m_is_antialias = style.ANTIALIAS;
		ret->setFaceColor(style.FACE_COLOR);

		ret->setContextColor(style.CONTEXT_COLOR);

		if(style.EDGE_COLOR >=0)
		{
			ret->setEdgeColor(style.EDGE_COLOR);
			ret->m_is_draw_edge = true;
		}

		if(style.PATTERN_PIC.compare(WS(L"")) != 0)
		{
 			//DataFetcher::getInstance()->getBitmap(style.PATTERN_PIC,ret->m_patten_pic);
			ret->m_is_draw_pattern = true;
		}

		if(ret->m_filter_meter <=0 )
			ret->m_filter_meter = 1;

		*polygon_style_render = ret;
		
	}

	void StyleTranslate::transLabelStyle(WSLabelStyle& style,LabelStyleRender *label_style_render)
	{
		label_style_render->setFontSize(style.FONT_SIZE);
		label_style_render->setFontName(style.FONT_NAME); 
		label_style_render->setFontWeight(style.FONT_WEIGHT);
		label_style_render->m_is_draw_bg = false;

		label_style_render->setLabelColor(style.FONT_COLOR);
		

		if(style.FONT_BG_COLOR >=0)
		{
			label_style_render->setLabelBgColor(style.FONT_BG_COLOR);
			label_style_render->m_is_draw_bg = true;
		}
	}
}