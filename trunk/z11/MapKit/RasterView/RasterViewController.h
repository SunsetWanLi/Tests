//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (RasterViewController.h)  
/// @brief (���ļ�ʵ�ֵĹ��ܵļ���)  
///  
///(���ļ�ʵ�ֵĹ��ܵ�����)  
///  
/// @version 0.1   (�汾����)  
/// @author        (Hu DanYuan)  
/// @date          (11-11-01)  
///  
///  
///    �޶�˵��������汾  
////////////////////////////////////////////////////////////////////////// 
#ifndef _MIRASTER_VIEW_CONTROLLER_H_
#define _MIRASTER_VIEW_CONTROLLER_H_
#pragma once

#include "CommonStruct.h"
#include "CommonBase/MGStructure.h"
#include "../../GraphicCore/MGImage.h"
#include "GUI/Base/MIInstance.h"
#include "GUI/Controllers/MIViewController.h"
#include "System/LocationService/MGLocationManager.h"
#include <vector>

namespace z11 
{
	class RasterView;
	class RasterOverlay;
	typedef struct _MGGeoPoint MGGeoPoint;
	class RasterViewController : public MIViewController, public MGLocationManager
		//,public z11::MIControllerInstance<RasterViewController>
	{
	public:
		RasterViewController();
		~RasterViewController();
		MIVIEWCONTROLLER_DECLARE;
		MIRESPONDER_TOUCH_DECLARE;
		MGLOCATIONMANAGER_DELEGAGE_DECLARE;
	public:
		void initViewsWithFrame(const MGRect &frame);
		void initRasterView(const std::string& map_name, const std::string& map_root_path);

		void enableMove()	{	enable_move=true;	}
		void enableScale()	{	enable_scale=true;	}
		void disableMove()	{	enable_move=false;	}
		void disableScale()	{	enable_scale=false;	}
		void zoomIn();
		void zoomOut();
		void animateTo(const MGGeoPoint &point);
		void setZoom(int_32 zoom_level);

		void startLineCollecting();
		void stopLineCollecting();
		void startRegionCollecting();
		void stopRegionCollecting();
		double getMeasuredDistance();
		double getMeasuredAcreage();
		vector<MGGeoPoint> getCollectedPoints();

		void clearLines();
		void clearPoints();
		void drawPolyLines(const vector<MGGeoPoint> &poly_points, RGBAColorRef color_ref=RGBAColorMake(255, 0, 0, 128));
		void drawFilledPolyLoopLines(const vector<MGGeoPoint> &poly_points, RGBAColorRef color_ref);
		void drawPoint(const std::vector<MGGeoPoint> &poly_points, std::vector<std::string> &image_path);
		void drawPolyLines(const std::vector<MGPoint> &poly_points, RGBAColorRef color_ref=RGBAColorMake(255, 0, 0, 128));
		void drawFilledPolyLoopLines(const std::vector<MGPoint> &poly_points, RGBAColorRef color_ref);
		void drawPoint(const std::vector<MGPoint> &poly_points, std::vector<std::string> &image_path);

		void pixelToGeo(MGGeoPoint &dest, const MGPoint &src);
		void geoToPixel(MGPoint &dest, const MGGeoPoint &src);

		RasterView *getRasterView();
		RasterOverlay *getOverlayView();
	private:
		void transView(const MGPoint &offset, const float &scale);
		void updateView();
		
	public:
		RasterOverlay *m_raster_overlay;
		RasterView *m_raster_view;
	private:
		bool m_frame_initialized;

		MGPoint m_image_curr_offset;			//��ǰ����λ��  
		float	m_image_curr_scaling;			//��ǰ�������ű���  
		MGPoint m_image_real_offset;			//ͼƬʵ��λ��  
		float	m_image_real_scaling;			//ͼƬʵ�����ű���  

		//bool	m_image_move_zoom_switch_on;		//ͼƬ�ƶ����ŵĿ��أ�Ĭ�ϴ򿪣�  
		bool	enable_scale;
		bool	enable_move;

		std::set<MITouch*> m_previous_touches;	//��¼��ǰ��touches  
		MGPoint m_previous_centra_point;		//ǰһ�δ����ĵ�Ч��������  
		float	m_previous_radius;				//ǰһ�δ����ĵ�Ч�뾶  
	};//RasterViewController
}//z11

#endif//_MIRASTER_VIEW_CONTROLLER_H_