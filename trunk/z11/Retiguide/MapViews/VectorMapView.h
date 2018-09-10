#ifndef _MAPVIEWS_VECTORMAPVIEW_H
#define _MAPVIEWS_VECTORMAPVIEW_H

#include "Retiguide/Render/VctorRender/Map/MapFrame.h"
#include "Retiguide/Render/VctorRender/Render/StyleRender.h"
#include "Retiguide/Render/VctorRender/Map/ScreenPixMap.h"
#include "Retiguide/Render/VctorRender/Feature/FeatureLayerRender.h"
#include "Retiguide/Render/VctorRender/Annotation/AnnotationLayerRender.h"
#include "Retiguide/Common/Types/Settings.h"
#include "Retiguide/Common/Share/Scope.h"
#include "Retiguide/Common/Util/Transformer.h"
#include "Retiguide/RenderEngine/GmRender/LineStyleRender.h"
#include "MapKit/Embedded/Application/LayerMgr.h"
#include "CommonBase/MGWString.h"

#include "GUI/MIControls/MIControl.h"
#include "GUI/MIControls/MILabel.h"
#include "GUI/MIControls/MIButton.h"

#include "System/LocationService/MGLocation.h"

using namespace z11;
using namespace Embedded;

namespace GIS_Embedded{



	typedef enum
	{
		LOOK_DOWN,
		LOOK_AWRY
	}VISUAL_STATE;

	class VectorMapView  : public MIControl
	{
	public:
		VectorMapView(bool is_raster_map);
		virtual ~VectorMapView();

		void setScreenPix(int width,int height);
		int getScreenPixWidth() const
		{
			return m_screen_pix_width;
		}

		int getScreenPixHeight() const
		{
			return m_screen_pix_height;
		}
		void setViewScope(const Scope& view_scope)
		{
			m_view_scope.setScope(view_scope);
		}

		void setDrawPath(bool flag)
		{
			m_draw_path = flag;
		}
		void setScaleRank(int scale)
		{
			m_scale_rank = scale;
		}
		void setScale(double scale_width,double scale_height)
		{
			m_scale_width = scale_width;
			m_scale_height = scale_height;
		}
		int getScaleRank() const
		{
			return m_scale_rank;
		}

		MapFrame* getMapFrame()
		{
			return &m_map_frame;
		}
		GIS_Embedded::Pos2DEx& getHost()
		{
			return m_host;
		}
		void setCurPos(Pos2DEx cur_pos)
		{
			m_cur_pos = cur_pos;
		}
		void setIsRenderHost(bool flag)
		{
			m_is_render_host = flag;
		}
		void setIsRenderDest(bool flag)
		{
			m_is_render_dest = flag;
		}
		
		void getLayerDataInScope();

		void renderFeatureLayers();
		void renderPointFeatureLayers();
		void renderAnnotationLayers();
		void renderPath();
		void renderHost();
		void renderDest();
		void renderSrc();
		void renderCurPos();
		void renderGPSHost();

		void setPos(vector<Pos2DEx>& poses,vector<string>& images);
		void renderPos();

		void roadSearch(Pos2DEx host,Pos2DEx dest);
        void roadSearch();
		void roadReturn();

		void startNavi(MGLocation &gps);
		void startSimuNavi();
		void stopNavi();
		void setHost(GIS_Embedded::Pos2DEx hostPos);
		void setDest(GIS_Embedded::Pos2DEx destPos);
		void setSrc(GIS_Embedded::Pos2DEx srcPos);
		void setDest(float dest_x, float dest_y);
		void setGPSHost(GIS_Embedded::Pos2DEx gpsPos);
		void setGPSHostImage(string gps_image);
		void setDestImage(string dest_image);
		void setHostImage(string host_image);
		void setCurPosImage(string curpos_image);


		//rect select
		void getLayerDataInRect(Scope &scope,vector<Pos2DEx>& points,vector<std::string>& names,bool flag=true);//框选
		void getLayerDataInRect(Scope& scope,vector<int>& fcls_id,vector<Pos2DEx>& points,vector<std::string>& names,bool flag = true);//框选特定图层


	private:
		void initData();

	public:
		int m_screen_pix_width;
		int m_screen_pix_height;

		double m_scale_width;
		double m_scale_height;

		Scope m_view_scope;
		Scope m_data_scope;
		Scope m_trans_scope;

		VISUAL_STATE m_visual_angle_state;
		int m_scale_rank;
		
		bool m_need_redraw;
	private:

		MapFrame m_map_frame;
		ScreenPixMap m_screen_pix_map;
		StyleRender m_style_render;
		Transformer m_transformer;
		FeatureLayerRender m_feature_layer_render;
		AnnotationLayerRender m_annotation_layer_render;

	private:
		vector<int> m_layer_mapframe_indexs;
		vector<int> m_layer_global_ids;
		bool m_fechting_data;
		//new data
		std::vector<std::vector<Embedded::IShape*> > m_shapes;
		std::vector<std::vector<Embedded::IShape*> > m_rect_shapes;//rect data
		Embedded::LayerMgr *m_layerMgr;

		vector<pair<std::string, vector<std::string> > > m_datasrc_nodes;
		vector<std::string> m_fcls_names;
		//host dest
		Pos2DEx m_host;
		Pos2DEx m_cur_pos;
		MGImage* m_cur_image;
		bool m_is_gps_host;
		MGImage* m_host_image;
		Pos2DEx m_dest;
		MGImage* m_dest_image;
		Pos2DEx m_src;
		MGImage* m_src_image;
		Pos2DEx m_gps_host;
		MGImage* m_gps_host_image;

		bool m_is_render_host;
		bool m_is_render_dest;
		bool m_is_render_src;
		//path
		vector<vector<GIS_Embedded::Pos2DEx> >  m_path_points; //the shortest path
		vector<vector<GIS_Embedded::Pos2DEx> > m_track_points; //the path passed
		bool m_draw_path;

		//poses
		vector<Pos2DEx> m_poses;
		vector<z11::MGImage*> m_poses_images;

		bool m_is_rastermap;

		LineStyleRender m_path_line_render;
		LineStyleRender m_track_line_render;
		LineStyleRender m_dash_line_render;

		//navigate
		bool m_is_navi;
	private:
		enum
		{
			ROAD_MAP_LOCATER,
			ROAD_MAP_SETHOST,
			ROAD_MAP_SETDEST,
			ROAD_MAP_ZOOMIN,
			ROAD_MAP_ZOOMOUT,
			ROAD_MAP_RETURN,
			ROAD_MAP_ROTATE,
			ROAD_MAP_SERCH,
			ROAD_MAP_RETURNROAD,
			ROAD_MAP_NAMEQUERY,
			ROAD_MAP_STARTNAVI,
			ROAD_MAP_STARTSIMUNAVI,
			ROAD_MAP_STOP,
			ROAD_MAP_VIEW_COUNT
		};
		z11::MIButton *m_buttons[ROAD_MAP_VIEW_COUNT];//
	private:
		void centraPoint(const std::set<MITouch*>& touches, MGPoint &point);
		void endPoint(const std::set<MITouch*>& touches, MGPoint &point);
		void getRadiusFromTouches(const std::set<MITouch*>& touches, float &radius);

		MGPoint m_map_curr_offset;			//当前触摸位移
		float	m_map_curr_scaling;			//当前触摸缩放比例

		int		m_pre_touches_num;					//前一次触摸的个数
		std::set<MITouch*> m_curr_touches;			//记录当前的touches
		float	m_pre_radius;						//前一次触摸的等效半径
		MGPoint m_pre_centra_point;					//前一次触摸的等效几何中心
		
		z11::MIButton * m_host_button;

	public:
		MIRESPONDER_TOUCH_DECLARE;
		MIVIEW_DECLARE;

	public:
		static VectorMapView *_instance;
		z11::MILabel *m_hint;

		friend class VectorMapViewController;
	};
}
#endif
