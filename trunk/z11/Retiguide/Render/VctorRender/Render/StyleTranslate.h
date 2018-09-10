#ifndef _CORE_WORKSPACE_STYLETRANSLATE_H
#define _CORE_WORKSPACE_STYLETRANSLATE_H

#include "Retiguide/RenderEngine/GmRender/PointStyleRender.h"
#include "Retiguide/RenderEngine/GmRender/LineStyleRender.h"
#include "Retiguide/RenderEngine/GmRender/PolygonStyleRender.h"
#include "Retiguide/RenderEngine/LabelRender/LabelStyleRender.h"
#include "Retiguide/Common/Types/WorkSpaceTypes/WSPointStyle.h"
#include "Retiguide/Common/Types/WorkSpaceTypes/WSLineStyle.h"
#include "Retiguide/Common/Types/WorkSpaceTypes/WSPolygonStyle.h"
#include "Retiguide/Common/Types/WorkSpaceTypes/WSLabelStyle.h"

namespace GIS_Embedded{
class StyleTranslate {
	
public:

	static void transPointStyle(WSPointStyle& style,PointStyleRender **point_style_render);
	
	static void transLineStyle(WSLineStyle& style,LineStyleRender **line_style_render);
	
	static void transPolygonStyle(WSPolygonStyle& style,PolygonStyleRender **polygon_style_render);
	
	static void transLabelStyle(WSLabelStyle& style,LabelStyleRender *label_style_render);

// 	void releaseBitmap()
// 	{
// 		if (!m_bitmaps.empty())
// 			m_bitmaps.clear();
// 	}
// 	
// public:
// 	vector<wstring> m_bitmaps;
};
}

#endif