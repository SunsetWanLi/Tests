//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (RasterView.h)  
/// @brief (本文件实现的功能的简述)  
///  
///(本文件实现的功能的详述)  
///  
/// @version 0.1   (版本声明)  
/// @author        (Hu DanYuan)  
/// @date          (11-11-01)  
///  
///  
///    修订说明：最初版本  
////////////////////////////////////////////////////////////////////////// 
#ifndef _MIRASTER_VIEW_H_
#define _MIRASTER_VIEW_H_
#pragma once

#include "CommonStruct.h"
#include "GUI/MIControls/MIControl.h"
#include "CommonBase/MGStructure.h"
#include <string>
#include <vector>
namespace Raster_GIS
{
	class ImagePyramid;
}
namespace z11
{
	
	const uint_32 RASTER_IMAGE_MAX_IMAGE_NUM = 50;
	typedef struct _RIRequest
	{
		uint_32 release_num;
		std::vector<std::string> v_path;
	}RIRequest;

	class RasterOverlay;
	class RasterViewController;
	class RasterView : public MIControl
	{
	public:
		RasterView();
		~RasterView(void);

	public:
		void init(const std::string& map_name, const std::string& map_root_path);
		RasterViewController *getController()	{	return m_controller;	}
		void setDelegate(RasterViewController *delegate)	{	m_controller = delegate;	}
		//Overlay *getOverlays();

	private:
		void zoomIn();
		void zoomOut();
		void animateTo(const MGGeoPoint &point);
		void setZoom(int_32 zoom_level);
		void setDrawRect(MGGeoRect draw_rect);
		

		double getWidthScalePixelGeo();
		double getHeightScalePixelGeo();
		void pixelToGeo(MGGeoPoint &dest, const MGPoint &src);
		void geoToPixel(MGPoint &dest, const MGGeoPoint &src);

		void scaleView();
		void transView(const MGPoint &offset, const float &scale);
		friend class RasterViewController;
	public:
		MIRESPONDER_TOUCH_DECLARE;
		MIVIEW_DECLARE;
	private:
		int_32 getImage(const MGGeoRect &draw_rect, int_32 resolution_width, int_32 resolution_height, 
			std::vector<MGColor *> &image_vector, std::vector<MGPoint> &point_vector);
		int_32 tryToGetImage(const MGGeoRect &draw_rect, int_32 resolution_width, int_32 resolution_height);
		MGGeoRect getPyramidRegion();
		int_32 getTotalFloorNum();

		int requestForEntry(RIRequest *request);
		int releaseFreeEntry(int_32 release_num, const std::vector<std::string> &v_path);

		void renderImage();
		void adjustDrawRect(MGGeoRect &draw_rect);
		void adaptToScreen(int_32 floor_num);
		void adjustToCenter();
		int_32 getAvailableMaxFloor();
	
		MIEventHandleState handleWithControlEvent(const MIControlEvents &event);

	private:
		RasterViewController *m_controller;
		RasterOverlay *m_raster_overlay;
	private:
		
		bool m_config_initialized;
		
		Syncable m_pyramid_image_locker;
		std::vector<MGColor *> m_image_vector;
		std::vector<MGPoint> m_point_vector;
		std::map<std::string, MGColor *> m_path_color_map;	//用来负责图片的加载与释放  
		typedef std::map<std::string, MGColor *>::iterator ColorMapIterator;
		
		Raster_GIS::ImagePyramid *m_image_pyramid;
		MGGeoRect	m_pyramid_region;
		MGGeoRect	m_geo_rect;						//需要绘制的地理坐标  
		int_32 m_level_factor;
		int_32 m_regular_tile_size;
		int_32 m_curr_floor_num;
		int_32 m_total_floor_num;
		int_32 m_max_floor_num;
		int_32 m_min_floor_num;

		float m_floor_scale;
		MGPoint m_floor_offset;
	};
}

#endif //_MIRASTER_IMAGE_H_  
