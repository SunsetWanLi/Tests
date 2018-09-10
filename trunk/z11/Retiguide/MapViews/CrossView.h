#ifndef _RENDER_CROSSVIEW_H
#define _RENDER_CROSSVIEW_H

#include "Retiguide/Common/Types/basic_types.h"
#include "Retiguide/Render/VctorRender/Map/MapFrame.h"
#include "Retiguide/Common/Share/Scope.h"
#include "Retiguide/Common/Util/Transformer.h"
#include "Retiguide/Render/VctorRender/Feature/FeatureLayerRender.h"
#include "Retiguide/RenderEngine/GmRender/LineStyleRender.h"
#include "Retiguide/RenderEngine/GmRender/PolygonStyleRender.h"
#include "Retiguide/Common/Util/Algorithmic.h"
#include "Retiguide/Analysis/CrossTrans.h"
#include "Retiguide/Render/VctorRender/Map/Navigation.h"


#include "GUI/MIControls/MIControl.h"
#include "GUI/MIControls/MILabel.h"
#include "GraphicCore/MGColor.h"

using namespace z11;

namespace GIS_Embedded
{
	class CrossView : public MIControl
	{

	public:
		CrossView(int width,int height,int scope_width);

		virtual ~CrossView();

		void setMapFrame(const MapFrame* map_frame);
		void setCrossCenter(const Pos2DEx& cross_center);

		int setPathLine(vector<Pos2DEx>& path);
		bool getCrossData();
		int transGeotoScreen(int_32 geo_x,int_32 geo_y,long *screen_x,long *screen_y);
		void render(int offset_x,int offset_y);

	private:
		int getRoadsFromTopoData(vector<CrossRoad*>* cross_roads);
		void renderCross(int offset_x,int offset_y);
		void renderStatic();
		void renderFloat();

	public:
		int m_scope_pix_width;
		int m_scope_pix_height;
		Scope m_view_scope;
		Scope m_scope_adjust;
		double m_offset_angle;	
		bool m_auto_rotate;
		int m_scope_meter_width;
		RGBAColorRef m_bg_clor;
		int m_arrow_width;
		int m_headInfo_pixel_height,m_rightInfor_pixel_width;

		MGImage* m_traffic_light;
		MGImage* m_host_bitmap[12];
		MGImage* m_right_bar_bg;

		CrossInfo m_cross_info;

	private:
		LineStyleRender m_scale_line_render;
		PolygonStyleRender m_blackBg_render;
		PolygonStyleRender m_scale_poly_render;
		LabelStyleRender m_big_label_render;
		LabelStyleRender m_small_label_render;
		ImageStyleRender m_image_render;

		const MapFrame *m_map_frame;
		Transformer m_transformer;
		LineStyleRender m_center_line_render;
		LineStyleRender m_dash_line_render;
		PolygonStyleRender m_line_face_render;
		PolygonStyleRender m_arrow_face_render;

		vector<Pos2DEx> m_arrow;
		MILabel *m_cross_info_label;

	public:
		static CrossView *_instance;

		MIVIEW_DECLARE;
		//friend class MapViewController;
	};
}
#endif