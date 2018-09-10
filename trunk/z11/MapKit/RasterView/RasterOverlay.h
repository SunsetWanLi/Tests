//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (RasterOverlay.h)  
/// @brief (本文件实现的功能的简述)  
///  
///(本文件实现的功能的详述)  
///  
/// @version 0.1   (版本声明)  
/// @author        (Hu DanYuan)  
/// @date          (11-12-21)  
///  
///  
///    修订说明：最初版本  
////////////////////////////////////////////////////////////////////////// 

#pragma once

#include "CommonStruct.h"
#include "GUI/MIControls/MIControl.h"
#include "../../GraphicCore/MGImage.h"

namespace z11 {

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
		MINoneCollecting,
	}MICollectingElement;
	typedef enum
	{
		MIPointDrawing,
		MILineDrawing,
		MIRegionDrawing,
		MINoneDrawing,
	}MIDrawType;

	typedef struct _MGGeoPoint MGGeoPoint;
	class RasterViewController;
	class RasterOverlay : public MIControl
	{
	public:
		RasterOverlay();
		virtual ~RasterOverlay();
		virtual void drawself();

		RasterViewController *getController()	{	return m_controller;	}
		void setDelegate(RasterViewController *delegate)	{	m_controller = delegate;	}

	private:
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
		void drawFilledPolyLoopLines(const vector<MGGeoPoint> &poly_points, RGBAColorRef color_ref=RGBAColorMake(255, 0, 0, 128));
		void drawPoint(const std::vector<MGGeoPoint> &poly_points, std::vector<std::string> &image_path);
		std::vector<MGPoint> transPointsToSrc(const std::vector<MGGeoPoint> &v_points);

		void transView(const MGPoint &offset, const float &scale);
		friend class RasterViewController;
	public:
		MIRESPONDER_TOUCH_DECLARE;

	private:
		RasterViewController *m_controller;

	private:
		double m_measured_length;
		double m_measured_acreage;
		Syncable m_collect_point_locker;
		bool m_curr_collect_cancel;
		MIPointCollectorPhase m_collect_phase;
		MICollectingElement m_collect_element;
		std::vector<MGGeoPoint> m_point_collector;

		Syncable m_draw_point_locaker;
		std::vector<MGGeoPoint> m_draw_points;
		std::vector<MGColor *> m_draw_point_colors;

		typedef struct _RegionColor
		{
			MGRegion region;
			RGBAColorRef color;
		}RegionColor;
		Syncable m_compare_region_locker;
		std::vector<RegionColor *> m_compare_regions;
		typedef std::vector<RegionColor *> RegionColorVector;
		MIDrawType m_draw_type;
	};
} //namespace z11