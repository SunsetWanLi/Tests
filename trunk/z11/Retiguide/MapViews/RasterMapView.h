//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (RasterMapView.h)  
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
#ifndef _MIRASTER_IMAGE_H_
#define _MIRASTER_IMAGE_H_
#pragma once

#include "GUI/MIControls/MIControl.h"
#include "System/Timer/MGTimer.h"
#include <string>
#include <vector>
#include <list>
using namespace z11;
namespace Raster_GIS
{
	class ImagePyramid;
}
namespace GIS_Embedded
{
	typedef enum
	{
		MIPointCollectorBegan,
		MIPointCollectorPaused,
		MIPointCollectorEnded,
		MIPointCollectorCancelled,
	}MIPointCollectorPhase;
	typedef enum
	{
		MIPointCollecting,
		MILineCollecting,
		MIRegionCollecting,
	}MICollectingElement;
	const z11::uint_32 RASTER_IMAGE_MAX_IMAGE_NUM = 100;//
	const z11::uint_32 RASTER_IMAGE_MAX_IMAGE_SIZE = 10240;//
	const z11::uint_32 RASTER_IMAGE_MAX_IMAGE_REPLACE_SIZE = 2560;

	typedef struct _RIRequest
	{
		z11::uint_32 release_num;
		std::vector<std::string> v_path;
	}RIRequest;
	typedef struct _ReplaceImage
	{
		string path_name;
		z11::MGTime image_time;
		z11::int_32 times;
	}ReplaceImage;
	class RasterMapView : public MIControl
	{
	public:
		RasterMapView(const std::string& map_name, const std::string& map_root_path, 
			z11::MGRect draw_rect=MGRectMake(121.3116f, 31.1730f, 0.004f, 0.004f));
		~RasterMapView(void);
		void zoomIn();
		void zoomOut();
		void moveXY(float offset_x, float offset_y);
		void testGeo(float offset_x,float offset_y);
		void setHost(float geo_x,float geo_y);
		void getScaleWidthHeight(double *scale_width,double *scale_height);
		void getScrRect(float *left,float *right,float *bottom,float *top);
		void startLineCollecting();
		void stopLineCollecting();
		void startRegionCollecting();
		void stopRegionCollecting();
		double getMeasuredDistance();
		double getMeasuredAcreage();
		void animateTo(const z11::MGPoint &point);//low accuracy!!!!!!!!!!!!!!!!!!!!!!!!!!
		void setZoom(z11::int_32 zoom_level);

		void drawPolyLines(const vector<z11::MGPoint> &poly_points);
		void drawPolyLoopLines(const vector<z11::MGPoint> &poly_points);
		void drawPoint(const z11::MGPoint &poly_points, std::string image_path);

		static bool isEqualZero(double value);
		static double rad(double d);
		static double getMeterFromGeo(double lat1, double lng1, double lat2, double lng2);
	public:
		MIRESPONDER_TOUCH_DECLARE;
		MIVIEW_DECLARE;
	//private:
		void init(const std::string& map_name, const std::string& map_root_path);
		int_32 getImage(const z11::MGRect &draw_rect, z11::int_32 resolution_width, z11::int_32 resolution_height,std::vector<std::string>& m_image_path);
		MGRect getPyramidRegion();
		int_32 getTotalFloorNum();
		int requestForEntry(RIRequest *request);
		int releaseFreeEntry(int_32 release_num, const std::vector<std::string> &v_path);
		void releaseFreeEntry(const std::vector<std::string> &v_path);//lw for p-lru
		void lrureleaseFreeEntry(const std::vector<std::string> &v_path);//lw for lru

		void adjustDrawRect();
		void firstAdjustDrawRect();
		void adaptToScreen();
		MGRect intersectRect(const z11::MGRect &left, const z11::MGRect &right);
		void pixelToGeo(z11::MGPoint &dest, const z11::MGPoint &src);
		void geoToPixel(z11::MGPoint &dest, const z11::MGPoint &src);
		double geoToMeter(double geo);
		double meterToGeo(double geo);
		double getDistance(const std::vector<z11::MGPoint>& points);
		double getAcreage(const std::vector<z11::MGPoint>& points);
		void getCenterPoint(const std::set<z11::MITouch*>& touches, z11::MGPoint &point);
		void getRadiusFromTouches(const std::set<z11::MITouch*>& touches, float &radius);
		MIEventHandleState handleWithControlEvent(const MIControlEvents &event);

		static bool lessFirst(ReplaceImage& lhs, ReplaceImage& rhs);

	private:
		void imageSort(vector<string>& cur_images,vector<std::string>& replace_image);
		void imageSort(int cur_level,vector<string>& cur_images,vector<std::string>& replace_image);
		void updateReplace(std::list<ReplaceImage>& each_level,ReplaceImage image_path);
		bool isErase(string path_name, std::vector<string>& cur_level);
		z11::int_32 getBufferSize(std::map<std::string,pair<z11::MGPoint,z11::MGColor *> >& image_buffer);

	private:
		z11::MGPoint m_image_curr_offset;			//当前触摸位移  
		float	m_image_curr_scaling;			//当前触摸缩放比例  
		z11::MGPoint m_image_real_offset;			//图片实际位移  
		float	m_image_real_scaling;			//图片实际缩放比例  
		z11::MGRect	m_geo_rect;						//需要绘制的地理坐标
		z11::MGPoint m_center_point;//lw

		bool	m_image_move_zoom_switch_on;		//图片移动缩放的开关（默认打开）  
		bool	enable_scale;
		bool	enable_move;
		z11::uint_32	m_curr_touches_num;					//前一次触摸的个数  
		std::set<MITouch*> m_curr_touches;			//记录当前的touches  
		float	m_pre_radius;						//前一次触摸的等效半径  
		z11::MGPoint m_pre_centra_point;					//前一次触摸的等效几何中心  

		MICollectingElement m_collect_element;
		MIPointCollectorPhase m_collect_phase;
		std::vector<z11::MGPoint> m_point_collector;
		double m_measured_length;
		double m_measured_acreage;
		z11::MGColor *m_draw_point_color;
		z11::MGPoint m_draw_point;

		Raster_GIS::ImagePyramid *m_image_pyramid;
		std::map<std::string,pair<z11::MGPoint,z11::MGColor *> > m_path_color_map;
		z11::int_32 m_path_color_map_size;

		std::map<std::string,pair<z11::MGPoint,z11::MGColor *> > m_lru_path_color_map;
		z11::int_32 m_lru_path_color_map_size;

		typedef std::map<std::string,pair<z11::MGPoint,z11::MGColor *> >::iterator ColorMapIterator;
		std::vector<std::list<ReplaceImage> > m_image_replace;
		std::list<ReplaceImage> m_lru_image_replace;
// 		std::map<std::string, MGColor *> m_path_color_map;	//用来负责图片的加载与释放  
// 		typedef std::map<std::string, MGColor *>::iterator ColorMapIterator;
// 		std::vector<MGColor *> m_image_vector;
		std::vector<std::string> m_image_path;
		z11::MGRect m_pyramid_region;
		z11::int_32 m_curr_floor_num;
		z11::int_32 m_total_floor_num;

	public:
		static z11::int_32 _replace_times;
		static z11::int_32 _lack_times;
		static z11::int_32 _lru_replace_times;
		static z11::int_32 _lru_lack_times;

	};
}

#endif //_MIRASTER_IMAGE_H_  
