#ifndef _CORE_RENDERENGINE_MAPVIEW_H
#define _CORE_RENDERENGINE_MAPVIEW_H

#include "VectorMapView.h"
#include "CommonBase/MGWString.h"

#include "GUI/MIControls/MIControl.h"
#include "GUI/MIControls/MILabel.h"
#include "GUI/MIControls/MIButton.h"

#include "System/LocationService/MGLocation.h"
#include "RasterMapView.h"
#include "CommonBase/Memory/MGRefObject.h"
#include <iostream>

namespace z11
{
	class MIPanGestureRecognizer;
	class MIPinchGestureRecognizer;
}
using namespace z11;
using namespace Embedded;

namespace GIS_Embedded{
	enum {
		MIMapEventZoomIn				= 1 <<  0,
		MIMapEventZoomOut				= 1 <<  1,
		MIMapEventMove					= 1 <<  2,
	};

	class MapView;
	class MapViewDelegate
	{
	public:
		MapViewDelegate(){}//fix me? maybe should't retain ;
		virtual void mapClick(MapView * map_view, GIS_Embedded::Pos2DEx& pos)=0;
		virtual void endSimuNavi() = 0;
		virtual void endRealNavi() = 0;
		virtual void afterMapMoved(MapView * map_view,const MGPoint& pre_center_point,const MGPoint &cur_center)
		{
			
		}
	};
	typedef enum {
		MIMapViewNormalResult,
		MIMapViewHostPositionInvalidError,
		MIMapViewDestPositionInvalidError,
	}MIMapViewResult;
	class MapView  : public MIControl
	{
	public:
		MapView();
		virtual ~MapView();

		//set screen pixels
		void setScreenPix(int width,int height);
		int getScreenPixWidth() const;
		int getScreenPixHeight() const;


		//set images
		void setGPSHostImage(string gps_image);
		void setDestImage(string dest_image);
		void setHostImage(string host_image);
		void setCurPosImage(string curpos_image);


		void setViewScope(const Scope& view_scope)
		{
			m_view_scope.setScope(view_scope);
		}

		void setScaleRank(int scale)
		{
			m_scale_rank = scale;			
		}			
		int getScaleRank() const
		{
			return m_scale_rank;
		}

		MapFrame* getMapFrame()
		{
			return m_vector_map->getMapFrame();
		}

		GIS_Embedded::Pos2DEx& getCurPos()
		{
			return m_last_pos;
		}

		bool getIsInNavi()
		{
			return m_is_real_navi;
		}

		void setHostFromGps(bool flag)
		{
			m_is_from_gps = flag;
		}
		void setIsRenderHost(bool flag)
		{
			m_vector_map->setIsRenderHost(flag);
		}
		void setIsRenderDest(bool flag)
		{
			m_vector_map->setIsRenderDest(flag);
		}
        void setScale(const MGRectScale &scale);


		float mapZoomIn();
		float mapZoomOut();
		void locater();
		void locaterPos(GIS_Embedded::Pos2DEx& pos);

		MIMapViewResult roadSearch();
		MIMapViewResult roadGPSSearch();
		void roadReturn();

		void startNavi(MGLocation &gps);
		void stopNavi();
		void startSimuNavi();
		void stopSimuNavi();
		void setHost();
		void setDest();
		void setSrc();
		void setDest(float dest_x, float dest_y);
		void setHost(float dest_x, float dest_y);
		void setCurPos(GIS_Embedded::Pos2DEx& point);

		bool updateHost(MGLocation &location,bool flag);
		void setPos(vector<Pos2DEx>& poses,vector<string>& images);
// 		void renderHost();
		//for test
		void moveXY(int offset_x,int offset_y);
		void centerSubView(MIView *sub_view);
		MGPoint transVectorMapPoint(const MGPoint &pt);

		//for raster map
		void setRasterHost(int_32 host_x,int_32 host_y);
		

		//rect select
		//rect select
		void getLayerDataInRect(Scope &scope,vector<Pos2DEx>& points,vector<std::string>& names,bool flag);
		void getLayerDataInRect(Scope& scope,vector<int>& fcls_id,vector<Pos2DEx>& points,vector<std::string>& names,bool flag = true);
	public:
		void setMapViewDelegate(MapViewDelegate* delegate);

	public:
		int m_screen_pix_width;
		int m_screen_pix_height;

		Scope m_view_scope;
		int m_scale_rank;
		z11::MGRect	m_geo_rect;

	private:
		Settings* m_setting_config;
		MapConfigType* m_map_config;

		MapFrame m_map_frame;
		ScreenPixMap m_screen_pix_map;
		StyleRender m_style_render;
		Transformer m_transformer;

		double m_scale_width;
		double m_scale_height;

		bool m_is_moved;
		bool m_is_from_gps;

	    //host dest
		Pos2DEx m_last_pos;
		Pos2DEx m_host;
		Pos2DEx m_dest;
		Pos2DEx m_src;
		Pos2DEx m_simu_host;
		Pos2DEx m_gps_host;
		MGImage* m_host_image;

		//navigate
		bool m_is_real_navi;
		bool m_is_simu_navi;
		bool m_is_vectormap;
		string m_raster_name;

    public:
		VectorMapView *m_vector_map;

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


	public:
		MIRESPONDER_TOUCH_DECLARE;
		MIVIEW_DECLARE;

	public:
		static MapView *_instance;
		z11::MILabel *m_hint;

	private:
		MapViewDelegate * m_mapview_delegate;

	public:


		//add by GRF
		MIPanGestureRecognizer *m_pan_recognizer;
		MIPinchGestureRecognizer *m_pinch_recognizer;
	public:

		void scaleCall(z11::MGObject* _sender);
		void panMoveCall(z11::MGObject* _sender);

	};


}
#endif
