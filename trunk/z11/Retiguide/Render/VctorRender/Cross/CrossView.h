#ifndef _RENDER_CROSSVIEW_H
#define _RENDER_CROSSVIEW_H

#include "Retiguide/Common/Types/basic_types.h"
#include "../Map/MapFrame.h"
#include "Retiguide/Common/Share/Scope.h"
#include "Retiguide/Common/Util/Transformer.h"
#include "../Feature/FeatureLayerRender.h"
#include "Retiguide/RenderEngine/GmRender/LineStyleRender.h"
#include "Retiguide/RenderEngine/GmRender/PolygonStyleRender.h"
#include "Retiguide/Common/Util/Algorithmic.h"
#include "../../../Analysis/CrossTrans.h"

#include "GUI/MIControls/MIControl.h"
#include "GraphicCore/MGColor.h"

using namespace z11;

namespace GIS_Embedded
{

	class CrossView : public MIControl
	{

	public:
		CrossView(int scope_width):m_scope_meter_width(scope_width)
		{
			m_scope_pix_width = 1;
			m_scope_pix_height = 1;

			m_offset_angle = 0;	
			m_auto_rotate = false;

			m_arrow_direction = true;//lw
			m_arrow_direction_bitmap = "";

			m_map_frame = NULL;

			m_center_line_render.setLineColor(0xFFFFFF);//m_line_color = RGB(255,255,255);
			m_center_line_render.m_pixel_width = 2;
			m_center_line_render.m_line_style = SOLID;

			m_dash_line_render.setLineColor(0xFFFFFF);//._LINE_COLOR = RGB(255,255,255);;
			m_dash_line_render.m_pixel_width = 1;
			m_dash_line_render.m_line_style = DASH;

			m_line_face_render.setFaceColor(0x646464);// = RGB(100,100,100);

			m_arrow_face_render.setFaceColor(0x0000FF);// = RGB(0,0,255);

		}

		virtual ~CrossView(){}

		void setMapFrame(const MapFrame* map_frame)
		{
			m_map_frame = map_frame;
			m_arrow_width = map_frame->m_arrow_width;
			m_arrow_face_render.setFaceColor(map_frame->m_arrow_color);
			m_bg_clor = map_frame->m_cross_bg_color;
		}

		void setScopePix(int width,int height);

		void setOffsetAngle(double offset_angle)
		{
			m_offset_angle = -offset_angle;
		}
		void resetPathRemain()//lw
		{
			//zj//CrossTrans::resetPathRemain();
		}

		void setCrossCenter(const Pos2DEx& cross_center)
		{
			m_view_scope.m_left = cross_center.d0 - m_scope_meter_width;
			m_view_scope.m_right = cross_center.d0 + m_scope_meter_width;
			double scope_height = m_scope_meter_width*m_scope_pix_height/m_scope_pix_width;
			m_view_scope.m_top = cross_center.d1 + scope_height;
			m_view_scope.m_bottom = cross_center.d1 - scope_height;

			m_scope_adjust.setScope(m_view_scope);
			m_scope_adjust.rotateAdjust(m_auto_rotate,m_scope_pix_width,m_scope_pix_height);
		}

		double getOffsetAngle()const
		{
			return m_offset_angle;
		}

		string getCrossBitmap()
		{
			return m_arrow_direction_bitmap;
		}
		
		void setAutoRotate(bool is_auto)
		{
			m_auto_rotate = is_auto;
		}

		bool getAutoRotate()const
		{
			return m_auto_rotate;
		}
		//lw
		bool isEaqual(z11::MGPoint a,z11::MGPoint b)
		{
			if (a.x == b.x && a.y == b.y)
			{
				return true;
			}
			return false;
		}

		void min_max_X(vector<z11::MGPoint>& points,int number,z11::MGPoint& minPoint,z11::MGPoint& maxPoint);
		void min_max_Y(vector<z11::MGPoint>& points,int number,z11::MGPoint& minPoint,z11::MGPoint& maxPoint);
		void min_max_Y(vector<z11::MGPoint>& points,int number,z11::MGPoint& minPoint,z11::MGPoint& maxPoint,int &addr_minY,int &addr_maxY);
		z11::MGPoint min_Point(vector<z11::MGPoint>& points,int number);
		z11::MGPoint max_Point(vector<z11::MGPoint>& points,int number);

		int setPathLine(vector<Pos2DEx>& path);

		int transGeotoScreen(int_32 geo_x,int_32 geo_y,long *screen_x,long *screen_y);

		int render(int offset_x,int offset_y);

	private:
		int getRoadsFromTopoData(vector<CrossRoad*>* cross_roads);

		int renderCross(int offset_x,int offset_y);

		int renderAntiAlias();


	public:
		int m_scope_pix_width;
		int m_scope_pix_height;
		Scope m_view_scope;
		Scope m_scope_adjust;
		double m_offset_angle;	
		bool m_auto_rotate;

		bool m_arrow_direction;//lw

	private:
		const MapFrame *m_map_frame;
		Transformer m_transformer;
		LineStyleRender m_center_line_render;
		LineStyleRender m_dash_line_render;
		PolygonStyleRender m_line_face_render;
		PolygonStyleRender m_arrow_face_render;

		int m_scope_meter_width;
		RGBAColorRef m_bg_clor;
		int m_arrow_width;//07-9  lw

	private:
		vector<Pos2DEx> m_arrow;
		string m_arrow_direction_bitmap;

	};
}
#endif