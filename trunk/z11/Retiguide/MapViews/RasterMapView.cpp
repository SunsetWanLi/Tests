#include "RasterMapView.h"
#include "GraphicCore/MGImage.h"
#include "System/Shared/MGLog.h"
#include "System/Shared/Utility.h"
#include "MapKit/RasterImage/Pyramid/ImagePyramid.h"
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#define RASTER_IMAGE_MIN_MOVE_VALUE 0.1f
#define RASTER_IMAGE_MIN_ZOOM_VALUE 0.00f
#define RASTER_IMAGE_MIN_SCALING_VALVE 0.25f
#define RASTER_IMAGE_MAX_SCALING_VALVE 4.0f
#define RASTER_IMAGE_SCALING_BETWEEN_GEOGRAPHIC_AND_SCREEN_VALVE 0.0000029f
#define RASTER_IMAGE_MAX_TOUCH_NUM 2
#define RASTER_IMAGE_ESPLON 0.000005f

const double EARTH_RADIUS = 6378137.0f; //meter
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288 
#endif

namespace GIS_Embedded
{
	z11::int_32 RasterMapView::_replace_times = 0;
	z11::int_32 RasterMapView::_lack_times = 0;
	z11::int_32 RasterMapView::_lru_replace_times = 0;
	z11::int_32 RasterMapView::_lru_lack_times = 0;

	bool RasterMapView::lessFirst(ReplaceImage& lhs, ReplaceImage& rhs)
	{
		bool flag = false;
		if (lhs.image_time == rhs.image_time)
		{
			if (lhs.times < rhs.times)
				flag = true;
		}
		else if(lhs.image_time < rhs.image_time)
		{
			if (lhs.times < rhs.times)
				flag =  true;
		}
		else
			flag = false;

		return flag;
	}

	double RasterMapView::rad(double d)
	{
		return d * M_PI / 180.0;
	} 

	double RasterMapView::getMeterFromGeo(double lat1, double lng1, double lat2, double lng2)
	{
		double radLat1 = rad(lat1);
		double radLat2 = rad(lat2);
		double a = radLat1 - radLat2;
		double b = rad(lng1) - rad(lng2);

		double s = 2 * asin(sqrt(pow(sin(a/2),2) +
			cos(radLat1)*cos(radLat2)*pow(sin(b/2),2)));
		s = s * EARTH_RADIUS;
		s = int(s * 1000 + 0.5f) / 1000.0f;
		return s;
	}
	RasterMapView::RasterMapView(const std::string& map_name, const std::string& map_root_path, MGRect draw_rect)
	{
		m_collect_phase = MIPointCollectorCancelled;
		m_image_real_scaling = 1.0f;
		m_curr_touches_num = 0;
		_view_flags.clip_subviews = 1;
		m_geo_rect = draw_rect;
		m_curr_floor_num = 0;
		init(map_name, map_root_path);
	}
	RasterMapView::~RasterMapView(void)
	{
	}
	void RasterMapView::init(const std::string& map_name, const std::string& map_root_path)
	{
		m_image_pyramid = new Raster_GIS::ImagePyramid();
		if (m_image_pyramid != NULL)
			m_image_pyramid->init(map_name, map_root_path);
		m_total_floor_num = getTotalFloorNum();
		m_pyramid_region = getPyramidRegion();
		firstAdjustDrawRect();

		for (int i=0 ;i <m_total_floor_num ; i++)
		{
			std::list<ReplaceImage> tmp_img;
			m_image_replace.push_back(tmp_img);
		}
		m_path_color_map_size = 0;
		m_lru_path_color_map_size = 0;
	}
	void RasterMapView::initWithFrame(MGRect frame)
	{
		MIControl::initWithFrame(frame);
		m_curr_floor_num = getImage(m_geo_rect, m_frame.width, m_frame.height,m_image_path);
		adaptToScreen();
// 		//MGLogD("m_curr_floor_num= %d", m_curr_floor_num);
	}
	void RasterMapView::drawself()
	{
		MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();
		if (graphic_engine == NULL) 
			return;
		RGBAColorRef color_ref = RGBAColorRef(255, 255, 255, 255);

		if (_view_flags.clip_subviews != 0)
			graphic_engine->enableClip(m_bounds);

		graphic_engine->drawFilledRect(m_bounds, color_ref);

		int i = 0;
		std::vector<std::string>::const_iterator str_const_it;
		for (str_const_it=m_image_path.begin(); str_const_it!=m_image_path.end(); str_const_it++)
		{
			ColorMapIterator map_it = m_path_color_map.find(*str_const_it);
			MGImage *image = map_it->second.second->m_image;
			MGPoint image_point = map_it->second.first;
			
			if (image != NULL)
			{
				PixelInt width = image->getWidth();
				PixelInt height = image->getHeight();
				MGRect rect = MGRectMake(m_bounds.x+image_point.x, m_bounds.y+image_point.y, float(width), float(height));
				
				image->sendDataToContext();
				graphic_engine->drawImage(image, rect, color_ref, draw_image_mode);
			}
			else
			{
				graphic_engine->drawFilledRoundRect(m_bounds, color_ref);
			}
		}
		std::vector<MGPoint> point_vector;
		std::vector<MGPoint>::const_iterator const_it;
		for (const_it=m_point_collector.begin(); const_it!=m_point_collector.end(); const_it++,i++)
		{
			MGPoint tmp_point;
			geoToPixel(tmp_point, (*const_it));
			point_vector.push_back(tmp_point);
		}
		if (m_collect_element == MILineCollecting)
			graphic_engine->drawPolyLines(point_vector, RGBAColorMake(255, 0, 0), 2);
		else if (m_collect_element == MIRegionCollecting)
			graphic_engine->drawFilledPolyLoopLines(point_vector, RGBAColorMake(255, 0, 0, 128));
		else if (m_collect_element == MIPointCollecting)
		{
			if (m_draw_point_color!=NULL && m_draw_point_color->m_image!=NULL)
			{
				MGPoint tmp_point;
				geoToPixel(tmp_point, m_draw_point);
				PixelInt width = m_draw_point_color->m_image->getWidth();
				PixelInt height = m_draw_point_color->m_image->getHeight();
				MGRect location_rect = MGRectMake(tmp_point.x-width/2, tmp_point.y-height, width, height);
				m_draw_point_color->m_image->sendDataToContext();
				graphic_engine->drawImage(m_draw_point_color->m_image, location_rect, color_ref, draw_image_mode);
			}
		}
		if (_view_flags.clip_subviews != 0)
			graphic_engine->disableClip();

		//MGPoint text_point;
		//pixelToGeo(text_point, MGPointMake(m_bounds.width, m_bounds.height));
		////MGLogD("text_point=(%f,%f)", text_point.x, text_point.y);
		////MGLogD("text_point=(%f,%f)", m_geo_rect.x+m_geo_rect.width/4, m_geo_rect.y+m_geo_rect.height*3/4);
	}

	//触摸事件的响应 
	MIEventHandleState RasterMapView::touchesBegan(const std::set<MITouch*>& touches,
		const MIEvent* const event)
	{
		MIEventHandleState ret = MIControl::touchesBegan(touches, event);
		std::set<MITouch*> tmp_touches;
		std::set<MITouch*>::const_iterator touch_it;
		for (touch_it=touches.begin(); touch_it!=touches.end() && m_curr_touches_num!=RASTER_IMAGE_MAX_TOUCH_NUM; ++touch_it) {
			if ((*touch_it) == NULL)
				continue;
			m_curr_touches.insert(*touch_it);
			m_curr_touches_num++;
		}

		//collect
		for (touch_it=touches.begin(); touch_it!=touches.end(); ++touch_it) {
			if (m_collect_phase == MIPointCollectorBegan)
			{
				if ((*touch_it) == NULL)
					continue;
				MGPoint point;
				pixelToGeo(point, (*touch_it)->point);
				m_point_collector.push_back(point);
			}
			else
			{
				break;
			}
		}
		

		//获得起始中心点 
		this->getCenterPoint(m_curr_touches, m_pre_centra_point);
		MGPoint tmp_point;
		pixelToGeo(tmp_point, m_pre_centra_point);
		//MGLogD("m_pre_geo_centra_point=(%f,%f)", tmp_point.x, tmp_point.y);
		pixelToGeo(tmp_point, MGPointMake(160, 215));
		//MGLogD("m_pre_geo_centra_point=(%f,%f)", tmp_point.x, tmp_point.y);

		//获得起始等效半径 
		this->getRadiusFromTouches(m_curr_touches, m_pre_radius);
		return ret;
	}
	MIEventHandleState RasterMapView::touchesMoved(const std::set<MITouch*>& touches,
		const MIEvent* const event)
	{
		MIEventHandleState ret = MIControl::touchesMoved(touches, event);
		bool is_moved = false;
		bool is_zoomed = false;
		bool is_need_redraw = false;
		std::set<MITouch*> tmp_touches;
		std::set<MITouch*>::const_iterator touch_it;
		int tmp_touches_num = 0;
		for (touch_it=touches.begin(); touch_it!=touches.end() && tmp_touches_num!=RASTER_IMAGE_MAX_TOUCH_NUM; ++touch_it) {
			if ((*touch_it) == NULL)
				continue;
			tmp_touches.insert(*touch_it);
			tmp_touches_num++;
		}

		MGPoint tmp_point;
		float tmp_radius = 0;
		MGPoint tmp_offset;
		float tmp_scaling = 1.0;
		this->getCenterPoint(tmp_touches, tmp_point);
		this->getRadiusFromTouches(tmp_touches, tmp_radius);

		m_image_curr_scaling = 1.0f;
		if (tmp_touches_num!=1 && m_curr_touches_num!=1)
		{
			tmp_scaling = tmp_radius / m_pre_radius;
			m_pre_radius = tmp_radius;
			if (fabsf(tmp_scaling - 1.0f) > RASTER_IMAGE_MIN_ZOOM_VALUE)
			{
				m_image_curr_scaling = tmp_scaling;
				is_zoomed = true;
			}
		}

		m_image_curr_offset = MGPointMake(0.0f, 0.0f);
		tmp_offset.x = tmp_point.x - m_pre_centra_point.x;
		tmp_offset.y = tmp_point.y - m_pre_centra_point.y;
		m_pre_centra_point = tmp_point;
		if (fabsf(tmp_offset.x) > RASTER_IMAGE_MIN_MOVE_VALUE
			|| fabsf(tmp_offset.y) > RASTER_IMAGE_MIN_MOVE_VALUE)
		{
			m_image_curr_offset = tmp_offset;
			is_moved = true;
		}
		//if (m_image_move_zoom_switch_on)
		//	if (is_moved || is_zoomed)
		//	{
		//		//MGLogD("m_image_curr_offset(%f, %f)", m_image_curr_offset.x, m_image_curr_offset.y);
		//		//MGLogD("m_image_curr_scaling=%f", m_image_curr_scaling);
		//		m_image_view_cell->adjustFrame(m_image_curr_offset, m_image_curr_scaling);
		//	}
		//MGRect draw_rect = m_geo_rect;
		if (enable_move && is_moved)
		{
			//m_image_real_offset.x += m_image_curr_offset.x;
			//m_image_real_offset.y += m_image_curr_offset.y;
			////MGLogD("m_image_curr_offset=(%f,%f)", m_image_curr_offset.x, m_image_curr_offset.y);
			//
			//m_geo_rect.x -= m_image_curr_offset.x * RASTER_IMAGE_SCALING_BETWEEN_GEOGRAPHIC_AND_SCREEN_VALVE * (1<<m_curr_floor_num);
			//m_geo_rect.y += m_image_curr_offset.y * RASTER_IMAGE_SCALING_BETWEEN_GEOGRAPHIC_AND_SCREEN_VALVE * (1<<m_curr_floor_num);

			////draw_rect.x -= m_image_real_offset.x * RASTER_IMAGE_SCALING_BETWEEN_GEOGRAPHIC_AND_SCREEN_VALVE * (m_curr_floor_num+1);
			////draw_rect.y += m_image_real_offset.y * RASTER_IMAGE_SCALING_BETWEEN_GEOGRAPHIC_AND_SCREEN_VALVE * (m_curr_floor_num+1);
			//is_need_redraw = true;
		}
		if(enable_scale && is_zoomed)
		{
			m_image_real_scaling *= m_image_curr_scaling;
			;
			float increase_width = m_geo_rect.width*m_image_curr_scaling - m_geo_rect.width;
			float increase_height = m_geo_rect.height*m_image_curr_scaling - m_geo_rect.height;
			m_geo_rect.x += (increase_width / 2.0f);
			m_geo_rect.y += (increase_height / 2.0f);
			m_geo_rect.width -= increase_width;
			m_geo_rect.height -= increase_height;
			is_need_redraw = true;
		}

		if (is_need_redraw)
		{
			//MGLogD("m_pyramid_region(%f,%f,%f,%f)",m_pyramid_region.x,m_pyramid_region.y,m_pyramid_region.width,m_pyramid_region.height);
			//MGLogD("before m_geo_rect(%f,%f,%f,%f)",m_geo_rect.x,m_geo_rect.y,m_geo_rect.width,m_geo_rect.height);
			adjustDrawRect();
			//MGLogD("after  m_geo_rect(%f,%f,%f,%f)",m_geo_rect.x,m_geo_rect.y,m_geo_rect.width,m_geo_rect.height);
			m_curr_floor_num = getImage(m_geo_rect, (int_32)m_frame.width, (int_32)m_frame.height,m_image_path);
			//MGLogD("m_curr_floor_num=%d", m_curr_floor_num);
			//std::vector<MGPoint>::const_iterator it;
			//int i=0;
			//for (it=m_point_vector.begin(); it!=m_point_vector.end(); it++)
			//{
			//	//MGLogD("getImage: %d%d%d%d%d%d%d%d",i,i,i,i,i,i,i,i);
			//	//MGLogD("getImage: point= (%f, %f)", m_point_vector[i].x, m_point_vector[i].y);
			//	//MGLogD("getImage: color= %s", m_image_vector[i]->m_image->getFileName().c_str());
			//}
		}

		if (is_zoomed)
		{
			//zoom
			//ret = handleWithControlEvent(MIPictureEventZoom);
		}

		if (is_moved)
		{
			//move image
			//ret = handleWithControlEvent(MIPictureEventMove);
		}
		return ret;
	}
	MIEventHandleState RasterMapView::touchesEnded(const std::set<MITouch*>& touches,
		const MIEvent* const event)
	{
		MIEventHandleState ret = MIControl::touchesEnded(touches, event);
		std::set<MITouch*> tmp_touches;
		std::set<MITouch*>::const_iterator touch_it;
		int touches_num = 0;

		//从m_curr_touches中删除ended的touch
		for (touch_it=touches.begin(); touch_it!=touches.end(); ++touch_it) {
			assert((*touch_it) != NULL);
			std::set<MITouch*>::const_iterator tmp_touch_it;
			for (tmp_touch_it=m_curr_touches.begin(); tmp_touch_it!=m_curr_touches.end(); ++tmp_touch_it) {
				assert((*tmp_touch_it) != NULL);
				if((*tmp_touch_it)->tag == (*touch_it)->tag)
				{
					m_curr_touches.erase(*tmp_touch_it);
					m_curr_touches_num--;
					//MGLogD("m_curr_touches.erase");
					break;
				}
			}
		}
		//更新当前touches的几何中心和等效半径 
		//for (touch_it=m_curr_touches.begin(); touch_it!=m_curr_touches.end() && m_curr_touches_num!=2; ++touch_it) {
		//	if ((*touch_it) == NULL)
		//		continue;
		//	tmp_touches.insert(*touch_it);
		//	m_curr_touches_num++;
		//}
		this->getCenterPoint(m_curr_touches, m_pre_centra_point);
		//MGLogD("m_curr_touches_num=%d", m_curr_touches_num);
		//MGLogD("m_pre_centra_point=(%f,%f)", m_pre_centra_point.x, m_pre_centra_point.y);
		//MGLogD("end===========end");
		this->getRadiusFromTouches(m_curr_touches, m_pre_radius);
		return ret;
	}
	MIEventHandleState RasterMapView::touchesCancelled(const std::set<MITouch*>& touches,
		const MIEvent* const event)
	{
		MIEventHandleState ret = MIControl::touchesCancelled(touches, event);
		m_curr_touches.clear();
		m_curr_touches_num = 0;
		return ret;
	}
	//事件响应   
	MIEventHandleState RasterMapView::handleEvent(const MIEvent* const event)
	{
		MIEventHandleState ret = MIControl::handleEvent(event);
		return ret;
	}

	MIEventHandleState RasterMapView::handleWithControlEvent(const MIControlEvents &event)
	{
		MIEventHandleState ret = MIEventHandleStatePass;
		EventsMapIterator it = events_dispatch_map.find(event);
		if (it == events_dispatch_map.end()) {
			ret = MIEventHandleStateNormal;
		}else{
			assert(it->second.view_controller != NULL);
			assert(it->second.event_func != NULL);
			EventFunc func = it->second.event_func;
			((it->second.view_controller)->*func)(this);
			ret = MIEventHandleStateDone;
		}
		return ret;
	}

	void RasterMapView::moveXY(float offset_x, float offset_y)
	{
		if (fabsf(offset_x) > RASTER_IMAGE_MIN_MOVE_VALUE
			|| fabsf(offset_y) > RASTER_IMAGE_MIN_MOVE_VALUE)
		{
			//MGLogD("offsetX = %f,offsetY = %f",offset_x,offset_y);

			double scale_width = m_image_pyramid->getWidthScalePixelGeo(m_curr_floor_num);
			double scale_height = m_image_pyramid->getHeightScalePixelGeo(m_curr_floor_num);

			m_geo_rect.x -= offset_x / scale_width;//RASTER_IMAGE_SCALING_BETWEEN_GEOGRAPHIC_AND_SCREEN_VALVE * (1<<m_curr_floor_num);
			m_geo_rect.y += offset_y / scale_height;//RASTER_IMAGE_SCALING_BETWEEN_GEOGRAPHIC_AND_SCREEN_VALVE * (1<<m_curr_floor_num);
		}
		adjustDrawRect();
		/*m_curr_floor_num = */getImage(m_geo_rect, (int_32)m_frame.width, (int_32)m_frame.height,m_image_path);

	}

	void RasterMapView::testGeo(float offset_x,float offset_y)
	{
		double scale_width = m_image_pyramid->getWidthScalePixelGeo(m_curr_floor_num);
		double scale_height = m_image_pyramid->getHeightScalePixelGeo(m_curr_floor_num);

		float dest_x =m_geo_rect.x - offset_x / scale_width;//RASTER_IMAGE_SCALING_BETWEEN_GEOGRAPHIC_AND_SCREEN_VALVE * (1<<m_curr_floor_num);
		float dest_y =m_geo_rect.y + offset_y / scale_height;//RASTER_IMAGE_SCALING_BETWEEN_GEOGRAPHIC_AND_SCREEN_VALVE * (1<<m_curr_floor_num);

	}
	void RasterMapView::setHost(float geo_x,float geo_y)
	{
		m_geo_rect.x = geo_x - m_geo_rect.width/2;
		m_geo_rect.y = geo_y - m_geo_rect.height/2;
		/*m_curr_floor_num = */getImage(m_geo_rect, (int_32)m_frame.width, (int_32)m_frame.height,m_image_path);

	}
	void RasterMapView::getScrRect(float *left,float *right,float *bottom,float *top)
	{
		*left = m_geo_rect.x;
		*right = m_geo_rect.x + m_geo_rect.width;
		*bottom = m_geo_rect.y;
		*top = m_geo_rect.y + m_geo_rect.height;
	}
	void RasterMapView::getScaleWidthHeight(double *scale_width,double *scale_height)
	{
		*scale_width = m_image_pyramid->getWidthScalePixelGeo(m_curr_floor_num);
		*scale_height = m_image_pyramid->getHeightScalePixelGeo(m_curr_floor_num);
	}
	void RasterMapView::zoomIn()
	{			//MGLogD("zoomIn before m_geo_rect(%f,%f,%f,%f)",m_geo_rect.x,m_geo_rect.y,m_geo_rect.width,m_geo_rect.height);

		if (m_curr_floor_num > 0)
		{
			m_curr_floor_num--;
			m_geo_rect.x = m_geo_rect.x + m_geo_rect.width/4;// / 8;
			m_geo_rect.y = m_geo_rect.y + m_geo_rect.height/4;// * 3 / 8;
			adaptToScreen();

			//m_geo_rect.x = m_geo_rect.x + m_geo_rect.width/4;// / 8;
			//m_geo_rect.y = m_geo_rect.y + m_geo_rect.height/4;// * 3 / 8;
			//m_geo_rect.width = m_geo_rect.width / 2;
			//m_geo_rect.height = m_geo_rect.height / 2;
			adjustDrawRect();
			//MGLogD("zoomIn after  m_geo_rect(%f,%f,%f,%f)",m_geo_rect.x,m_geo_rect.y,m_geo_rect.width,m_geo_rect.height);
			/*m_curr_floor_num = */getImage(m_geo_rect, (int_32)m_frame.width, (int_32)m_frame.height,m_image_path);
			//MGLogD("zoomIn floor_num=%d", m_curr_floor_num);
		}
	}
	
	void RasterMapView::zoomOut()
	{
		if (m_curr_floor_num < m_total_floor_num-1)
		{			//MGLogD("adjust before m_geo_rect(%f,%f,%f,%f)",m_geo_rect.x,m_geo_rect.y,m_geo_rect.width,m_geo_rect.height);

			m_curr_floor_num++;
			m_geo_rect.x = m_geo_rect.x - m_geo_rect.width / 2;//4;
			m_geo_rect.y = m_geo_rect.y - m_geo_rect.height /2;//* 3 / 4;
			adaptToScreen();
			////MGLogD("zoomOut before m_geo_rect(%f,%f,%f,%f)",m_geo_rect.x,m_geo_rect.y,m_geo_rect.width,m_geo_rect.height);
			//m_geo_rect.x = m_geo_rect.x - m_geo_rect.width / 2;//4;
			//m_geo_rect.y = m_geo_rect.y - m_geo_rect.height /2;//* 3 / 4;
			//m_geo_rect.width = m_geo_rect.width * 2;
			//m_geo_rect.height = m_geo_rect.height * 2;
			adjustDrawRect();
			//MGLogD("adjust after  m_geo_rect(%f,%f,%f,%f)",m_geo_rect.x,m_geo_rect.y,m_geo_rect.width,m_geo_rect.height);
			/*m_curr_floor_num = */getImage(m_geo_rect, (int_32)m_frame.width, (int_32)m_frame.height,m_image_path);
			//MGLogD("zoomIn floor_num=%d", m_curr_floor_num);
		}
	}
	double RasterMapView::getMeasuredDistance()
	{
		double ret = 0.0f;
		if (m_collect_phase == MIPointCollectorEnded)
			ret = m_measured_length;
		return ret;
	}
	double RasterMapView::getMeasuredAcreage()
	{
		double ret = 0.0f;
		if (m_collect_phase == MIPointCollectorEnded)
			ret = m_measured_acreage;
		return ret;
	}

	void RasterMapView::animateTo(const MGPoint &point)
	{
		m_geo_rect.x = point.x - m_geo_rect.width / 4;
		m_geo_rect.y = point.y - m_geo_rect.height * 3 / 4;
		adjustDrawRect();
		m_curr_floor_num = getImage(m_geo_rect, (int_32)m_frame.width, (int_32)m_frame.height,m_image_path);
	}

	void RasterMapView::setZoom(int_32 zoom_level)
	{
		
	}

	void RasterMapView::startLineCollecting()
	{
		m_collect_element = MILineCollecting;
		m_collect_phase = MIPointCollectorBegan;
		m_point_collector.clear();
	}

	void RasterMapView::stopLineCollecting()
	{
		m_collect_phase = MIPointCollectorEnded;
		m_measured_length = getDistance(m_point_collector)*getMeterFromGeo(0,0,1,0);
		m_point_collector.clear();
	}

	void RasterMapView::startRegionCollecting()
	{
		m_collect_element = MIRegionCollecting;
		m_collect_phase = MIPointCollectorBegan;
		m_point_collector.clear();
	}

	void RasterMapView::stopRegionCollecting()
	{
		m_collect_phase = MIPointCollectorEnded;
		m_measured_length = getDistance(m_point_collector)*getMeterFromGeo(0,0,1,0);
		m_measured_acreage = getAcreage(m_point_collector)*getMeterFromGeo(0,0,1,0)*getMeterFromGeo(0,0,1,0);
		m_point_collector.clear();
	}

	double RasterMapView::geoToMeter(double geo)
	{
		return 0.0f;
	}
	double RasterMapView::meterToGeo(double geo)
	{
		return 0.0f;
	}

	void RasterMapView::drawPolyLines(const vector<MGPoint> &poly_points)
	{
		m_collect_element = MILineCollecting;
		m_point_collector.clear();
		std::vector<MGPoint>::const_iterator it;
		for (it=poly_points.begin(); it!=poly_points.end(); it++)
			m_point_collector.push_back(*it);
	}
	void RasterMapView::drawPolyLoopLines(const vector<MGPoint> &poly_points)
	{
		m_collect_element = MIRegionCollecting;
		m_point_collector.clear();
		std::vector<MGPoint>::const_iterator it;
		for (it=poly_points.begin(); it!=poly_points.end(); it++)
			m_point_collector.push_back(*it);
	}
	void RasterMapView::drawPoint(const MGPoint &poly_points, std::string image_path)
	{
		m_collect_element = MIPointCollecting;
		m_draw_point = poly_points;
		m_draw_point_color = MGColor::fromImage(image_path);
	}

	double RasterMapView::getDistance(const std::vector<MGPoint>& points)
	{
		int_32 size = points.size();
		double ret = 0.0f;
		if (size==0 || size==1)
			return 0.0f;
		std::vector<MGPoint>::const_iterator pre_it;
		std::vector<MGPoint>::const_iterator curr_it;
		for (curr_it=points.begin(),pre_it=curr_it; curr_it!=points.end(); pre_it=curr_it++) {
			if (pre_it == curr_it)
				continue;
			ret += sqrt(pow(((*curr_it).x - (*pre_it).x) , 2) + pow(((*curr_it).y - (*pre_it).y), 2));
		}
		return ret;
	}

	double RasterMapView::getAcreage(const std::vector<MGPoint>& points)
	{
		double ret = Utility::calPolygonArea(points);
		return ret;
	}

	//获得touches的几何中心    
	void RasterMapView::getCenterPoint(const std::set<MITouch*>& touches, MGPoint &point)
	{
		std::set<MITouch*>::const_iterator touch_it;
		point.x = point.y = 0;
		if (touches.size() == 0)
			return;
		for (touch_it = touches.begin(); touch_it != touches.end(); ++touch_it) {
			assert((*touch_it) != NULL);
			point.x += (*touch_it)->point.x;
			point.y += (*touch_it)->point.y;
		}
		point.x = point.x / touches.size();
		point.y = point.y / touches.size();
	}

	//获得touches的等效半径 
	void RasterMapView::getRadiusFromTouches(const std::set<MITouch*>& touches, float &radius)
	{
		radius = 0.0f;
		if (touches.size() == 0)
			return;
		MGPoint centra_point;
		getCenterPoint(touches, centra_point);
		std::set<MITouch*>::const_iterator touch_it;
		for (touch_it = touches.begin(); touch_it != touches.end(); ++touch_it) {
			assert((*touch_it) != NULL);
			radius += sqrtf((float)pow((centra_point.x - (*touch_it)->point.x) , 2) + (float)pow((centra_point.y - (*touch_it)->point.y), 2));
		}
		radius = radius / touches.size();
	}

	//获取两个Rect的相交部分  
	MGRect RasterMapView::intersectRect(const MGRect &left, const MGRect &right)
	{
		MGRect rect;

		rect.x = left.x > right.x ? left.x : right.x ;
		rect.y = left.y > right.y ? left.y : right.y ;
		rect.width = ((left.x+left.width) < (right.x+right.width) ? (left.x+left.width) : (right.x+right.width)) - rect.x;
		rect.height = ((left.y+left.height) < (right.y+right.height) ? (left.y+left.height) : (right.y+right.height)) - rect.y;
		if (rect.width<0.0f || rect.height<0.0f)
			rect = MGRectMake(0.0f, 0.0f, 0.0f, 0.0f);
		return rect;
	}

	void RasterMapView::pixelToGeo(MGPoint &dest, const MGPoint &src)
	{
		if (m_image_pyramid != NULL)
		{
			double scale_width = m_image_pyramid->getWidthScalePixelGeo(m_curr_floor_num);
			double scale_height = m_image_pyramid->getHeightScalePixelGeo(m_curr_floor_num);
			if (isEqualZero(scale_height) || isEqualZero(scale_width))
				return;
			dest.x = m_geo_rect.x + src.x / scale_width;
			dest.y = m_geo_rect.y + (m_geo_rect.height - src.y / scale_height);
		}
	}

	void RasterMapView::geoToPixel(MGPoint &dest, const MGPoint &src)
	{
		if (m_image_pyramid != NULL)
		{
			double scale_width = m_image_pyramid->getWidthScalePixelGeo(m_curr_floor_num);
			double scale_height = m_image_pyramid->getHeightScalePixelGeo(m_curr_floor_num);
			dest.x = (src.x - m_geo_rect.x) * scale_width;
			dest.y = (m_geo_rect.y+m_geo_rect.height-src.y) * scale_height;
		}
	}


	void RasterMapView::adaptToScreen()
	{
		if (m_image_pyramid != NULL)
		{
			double scale_width = m_image_pyramid->getWidthScalePixelGeo(m_curr_floor_num);
			double scale_height = m_image_pyramid->getHeightScalePixelGeo(m_curr_floor_num);
			if (isEqualZero(scale_height) || isEqualZero(scale_width))
				return;
			//has problem
			m_geo_rect.width = /*2**/m_frame.width / scale_width/* + 0.002f*/;
			m_geo_rect.height = /*2**/m_frame.height / scale_height/* + 0.002f*/;
		}
	}

	void RasterMapView::firstAdjustDrawRect()
	{
		m_geo_rect = intersectRect(m_geo_rect, m_pyramid_region);
	}

	void RasterMapView::adjustDrawRect()
	{
#ifndef DEBUG
		//m_geo_rect.width = MINVALUE(m_geo_rect.width, m_pyramid_region.width);
		//m_geo_rect.height = MINVALUE(m_geo_rect.height, m_pyramid_region.height);
		////float width_scale = m_geo_rect.width / m_pyramid_region.width;
		////float height_scale = m_geo_rect.height / m_pyramid_region.height;
		////if (width_scale>1.0f && height_scale>1.0f) {
		////	if (width_scale > height_scale)
		////		m_geo_rect.height = m_pyramid_region.height;
		////	else
		////		m_geo_rect.width = m_pyramid_region.width;
		////}
		//if (m_geo_rect.x < m_pyramid_region.x) {
		//	m_geo_rect.x = m_pyramid_region.x;
		//}
		////has problem
		//if (m_geo_rect.y < m_pyramid_region.y/*-m_geo_rect.height/2*/) {
		//	m_geo_rect.y = m_pyramid_region.y/*-m_geo_rect.height/2*/;
		//}
		////has problem
		//if ( (m_geo_rect.x/*+m_geo_rect.width*/) > (m_pyramid_region.x+m_pyramid_region.width/*+m_geo_rect.width/2*/) ) {
		//	m_geo_rect.x = (m_pyramid_region.x+m_pyramid_region.width)/* - m_geo_rect.width+m_geo_rect.width/2*/;
		//}
		//if ( (m_geo_rect.y/*+m_geo_rect.height*/) > (m_pyramid_region.y+m_pyramid_region.height) ) {
		//	m_geo_rect.y = (m_pyramid_region.y+m_pyramid_region.height)/* - m_geo_rect.height*/;
		//}
#endif
	}

	void RasterMapView::updateReplace(std::list<ReplaceImage>& each_level,ReplaceImage image_path)
	{
//  		//MGLogD("begin updateReplace:size = %d",each_level.size());
		std::list<ReplaceImage>::iterator list_it;
		for (list_it = each_level.begin(); list_it != each_level.end(); list_it++)
		{
			if ((*list_it).path_name == image_path.path_name)
			{
				(*list_it).image_time = image_path.image_time;
				(*list_it).times++;
				return;
			}
		}
		each_level.push_back(image_path);
// 		//MGLogD("end updateReplace:size = %d",each_level.size());
	}

	bool RasterMapView::isErase(string path_name, std::vector<string>& cur_level)
	{
		std::vector<string>::iterator _it;
		for (_it = cur_level.begin(); _it != cur_level.end(); _it++)
		{
			if ((*_it) == path_name)
				return true;
		}
		return false;
	}

	void RasterMapView::imageSort(vector<string>& cur_images,vector<std::string>& replace_image)
	{
		float buffer_size = 0;
		std::list<ReplaceImage>::iterator _it;
		ColorMapIterator lru_map_it;
		MGColor* color;

		m_lru_image_replace.sort(lessFirst);
		_it = m_lru_image_replace.begin();

		while(buffer_size < RASTER_IMAGE_MAX_IMAGE_REPLACE_SIZE)
		{	
			std::list<ReplaceImage>::iterator tmp_it= _it;
			_it++;

			if (!isErase((*tmp_it).path_name,cur_images))
			{
				replace_image.push_back((*tmp_it).path_name);
				lru_map_it = m_lru_path_color_map.find((*tmp_it).path_name);
				color = lru_map_it->second.second;
				buffer_size += color->m_image->getImageSize();
				m_lru_image_replace.erase(tmp_it);
			}
		}
	}

	void RasterMapView::imageSort(int cur_level,vector<string>& cur_images,vector<std::string>& replace_image)
	{
		z11::int_32 buffer_size = 0;
		std::list<ReplaceImage>::iterator _it;
		std::list<ReplaceImage>::iterator  tmp_it;
		ColorMapIterator map_it;
		MGColor* color;

		for (int i=0; i<=2 && (buffer_size < RASTER_IMAGE_MAX_IMAGE_REPLACE_SIZE); i++)
		{	
			if (i == cur_level)
				continue;
			m_image_replace[i].sort(lessFirst);

			for (_it = m_image_replace[i].begin(); _it != m_image_replace[i].end(); )
			{
				tmp_it= _it;
				_it++;

				if (!isErase((*tmp_it).path_name,cur_images))
				{
					replace_image.push_back((*tmp_it).path_name);
					map_it = m_path_color_map.find((*tmp_it).path_name);
					color = map_it->second.second;
					buffer_size += color->m_image->getImageSize();
					m_image_replace[i].erase(tmp_it);
				}

				if (buffer_size > RASTER_IMAGE_MAX_IMAGE_REPLACE_SIZE)
					break;
			}
		}

		if (buffer_size < RASTER_IMAGE_MAX_IMAGE_REPLACE_SIZE)
		{
			m_image_replace[cur_level].sort(lessFirst);

			for (_it = m_image_replace[cur_level].begin(); _it != m_image_replace[cur_level].end(); )
			{
				tmp_it= _it;
				_it++;

				if (!isErase((*tmp_it).path_name,cur_images))
				{
					replace_image.push_back((*tmp_it).path_name);
					map_it = m_path_color_map.find((*tmp_it).path_name);
					color = map_it->second.second;
					buffer_size += color->m_image->getImageSize();
					m_image_replace[cur_level].erase(tmp_it);
				}

				if (buffer_size > RASTER_IMAGE_MAX_IMAGE_REPLACE_SIZE)
					break;
			}
		}
	}

	
	int_32 RasterMapView::getImage(const z11::MGRect &draw_rect, int_32 resolution_width, int_32 resolution_height,	std::vector<std::string>& image_path_vector)
	{
		int_32 ret = -1;
		Raster_GIS::Rect2D rect;
		std::vector<Raster_GIS::LeftTop> left_top_vector;
		std::vector<Raster_GIS::LeftTop>::const_iterator point_it;
		long col;
		
		rect.left_low.d0 = (double)(draw_rect.x);
		rect.left_low.d1 = (double)(draw_rect.y);
		rect.right_top.d0 = (double)(draw_rect.x + draw_rect.width);
		rect.right_top.d1 = (double)(draw_rect.y + draw_rect.height);
		image_path_vector.clear();
		left_top_vector.clear();
// 		//MGLogD("getImage m_geo_rect(%f,%f,%f,%f)",m_geo_rect.x,m_geo_rect.y,m_geo_rect.width,m_geo_rect.height);

		if (m_image_pyramid!=NULL && m_curr_floor_num<m_total_floor_num && m_curr_floor_num>=0)
			ret = m_image_pyramid->getImage(rect, (const long)resolution_width, (const long)resolution_height, image_path_vector, left_top_vector, col);
		
		int i = 0;
		//lru
		ColorMapIterator lru_map_it;
		MGColor *lru_color = NULL;
		bool lru_is_request = false;

		//for p-lru
		ColorMapIterator map_it;
		MGColor *color = NULL;

		ReplaceImage tmp_replace_image;
		tmp_replace_image.times = 0;

		for (point_it=left_top_vector.begin(); point_it!=left_top_vector.end(); point_it++,i++)
		{
			z11::MGTime cur_time = z11::MGTime::Now();
			tmp_replace_image.image_time = cur_time;
			tmp_replace_image.path_name = image_path_vector[i];

			//for lru
			lru_map_it = m_lru_path_color_map.find(image_path_vector[i]);
			updateReplace(m_lru_image_replace,tmp_replace_image);


			if (lru_map_it == m_lru_path_color_map.end())	//not exist
			{
				_lru_lack_times++;
				lru_color = MGColor::fromImage(image_path_vector[i]);
				m_lru_path_color_map_size += lru_color->m_image->getImageSize();

				if (!m_lru_path_color_map.empty())
				{//getBufferSize(m_lru_path_color_map)
					if (m_lru_path_color_map_size >= RASTER_IMAGE_MAX_IMAGE_SIZE)	//map is full and not request
					{
						_lru_replace_times++;

						std::vector<string> lru_replace_images;
						lru_replace_images.clear();

						MGLogD("LRU Release lru_size = %d. replacetimes = %d, lackTimes = %d !",m_lru_path_color_map_size,_lru_replace_times,_lru_lack_times);

						//lru
						imageSort(image_path_vector,lru_replace_images);
						lrureleaseFreeEntry(lru_replace_images);

						MGLogD("after LRU Release lru_size = %d.!",m_lru_path_color_map_size);

					}
				}
			}
			else
			{
				lru_color = lru_map_it->second.second;
			}
			MGPoint lru_point = MGPointMake(left_top_vector[i].left, left_top_vector[i].top);
			m_lru_path_color_map[image_path_vector[i]] = make_pair(lru_point,lru_color);

			//for p-lru
			map_it = m_path_color_map.find(image_path_vector[i]);
			updateReplace(m_image_replace[ret],tmp_replace_image);

			if (map_it == m_path_color_map.end())	//not exist
			{
				_lack_times++;
				color = MGColor::fromImage(image_path_vector[i]);
				m_path_color_map_size += color->m_image->getImageSize();

				if (!m_path_color_map.empty())
				{//getBufferSize(m_path_color_map)
					if (m_path_color_map_size >= RASTER_IMAGE_MAX_IMAGE_SIZE)	//map is full and not request
					{
						_replace_times++;
						std::vector<string> replace_images;
						replace_images.clear();

						MGLogD("P---LRU Release size = %d. replacetimes = %d , lacktimes = %d !",m_path_color_map_size,_replace_times,_lack_times);

						//lw p_LRU
						imageSort(ret,image_path_vector,replace_images);
						releaseFreeEntry(replace_images);
						MGLogD("after P---LRU Release size = %d.!",m_path_color_map_size);


					}
				}
			}
			else
			{
				color = map_it->second.second;
			}
			MGPoint point = MGPointMake(left_top_vector[i].left, left_top_vector[i].top);
			m_path_color_map[image_path_vector[i]] = make_pair(point,color);

		}

		return ret;

	}

	z11::int_32 RasterMapView::getBufferSize(std::map<std::string,pair<MGPoint,MGColor *> >& image_buffer)
	{
		z11::int_32 ret = -1;
		z11::int_32 buffer_size = 0;
		ColorMapIterator map_it;
		MGColor* color;
		for (map_it = image_buffer.begin(); map_it != image_buffer.end(); map_it++)
		{
			color = map_it->second.second;
			buffer_size += color->m_image->getImageSize();
		}
		ret = buffer_size>>10;
		return ret<0 ? 0 :ret;
	}

	int RasterMapView::requestForEntry(RIRequest *request)
	{
		int ret = 0;
		releaseFreeEntry(request->release_num, request->v_path);
		return ret;
	}

	void RasterMapView::releaseFreeEntry(const std::vector<std::string> &v_path)
	{//删除v_path里的图片
		ColorMapIterator map_it;
		for (int i=0 ; i<v_path.size(); i++)
		{
			map_it = m_path_color_map.find(v_path[i]);
			MGColor *release_color = map_it->second.second;
			m_path_color_map_size -= release_color->m_image->getImageSize();
			m_path_color_map.erase(map_it++);
			delete_and_set_null(release_color);
		}
		MGLogD("in P---LRU release!");
	}

	void RasterMapView::lrureleaseFreeEntry(const std::vector<std::string> &v_path)
	{
		ColorMapIterator lru_map_it;
		for (int i=0 ; i<v_path.size(); i++)
		{
			lru_map_it = m_lru_path_color_map.find(v_path[i]);
			MGColor *lru_release_color = lru_map_it->second.second;
			m_lru_path_color_map_size -= lru_release_color->m_image->getImageSize();
			m_lru_path_color_map.erase(lru_map_it++);
			delete_and_set_null(lru_release_color);
		}
		MGLogD("in LRU release!");

	}

	int RasterMapView::releaseFreeEntry(int_32 release_num, const std::vector<std::string> &v_path)
	{//删除m_path_color_map中不在v_path里的图片
		int ret = 0;
		ColorMapIterator map_it;
		std::vector<std::string>::const_iterator path_it;
		for(map_it=m_path_color_map.begin(); map_it!=m_path_color_map.end();)
		{
			ColorMapIterator tmp_map_it;
			for (path_it=v_path.begin(); path_it!=v_path.end(); path_it++)
			{
				if (!strcmp(map_it->first.c_str(), (*path_it).c_str()))	//is in vector
					break;
			}
			if (path_it == v_path.end())
			{
				MGColor *release_color = map_it->second.second;
				m_path_color_map.erase(map_it++);
				delete_and_set_null(release_color);
				ret++;
				if (ret == release_num)
					break;
				continue;
			}
			map_it++;
		}
		return ret;
	}


	MGRect RasterMapView::getPyramidRegion()
	{
		Raster_GIS::Rect2D rect;
		if (m_image_pyramid != NULL)
			rect = m_image_pyramid->getPyramidRegion();
		MGRect ret;
		ret.x = float(rect.left_low.d0);
		ret.y = float(rect.left_low.d1);
		ret.width = float(rect.right_top.d0 - rect.left_low.d0);
		ret.height = float(rect.right_top.d1 - rect.left_low.d1);
		return ret;
	}

	int_32 RasterMapView::getTotalFloorNum()
	{
		int_32 ret = 0;
		if (m_image_pyramid != NULL)
			ret = m_image_pyramid->getTotalFloorNum();
		return ret;
	}

	bool RasterMapView::isEqualZero(double value)
	{
		bool ret = false;
		if (value > RASTER_IMAGE_ESPLON*(-1.0) &&
			value < RASTER_IMAGE_ESPLON)
			ret = true;
		return ret;
	}
}
