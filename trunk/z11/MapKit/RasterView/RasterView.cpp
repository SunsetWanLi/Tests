#include "RasterView.h"
#include "GraphicCore/MGImage.h"
#include "System/Shared/MGLog.h"
#include "System/Shared/Utility.h"
#include "DataCalculator.h"
#include "RasterOverlay.h"
#include "RasterViewController.h"
#include "MapKit/RasterImage/Pyramid/ImagePyramid.h"
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#ifndef RASTERVIEW_ESPLON
#define RASTERVIEW_ESPLON 1000000
#endif
namespace z11
{
	RasterView::RasterView()
	{
		m_floor_scale = 1.0f;
		//manualClean();
		_view_flags.clip_subviews = 1;
		m_config_initialized = false;
		m_image_pyramid = NULL;
		m_path_color_map.clear();
		m_controller = NULL;
		
		m_controller = new RasterViewController();
		m_raster_overlay = new RasterOverlay();
		if (m_controller != NULL) {
			if (m_raster_overlay != NULL) {
				m_raster_overlay->delegate = m_controller;
				m_raster_overlay->setDelegate(m_controller);
				m_controller->m_raster_overlay = m_raster_overlay;
			}
			m_controller->m_raster_view = this;
			this->delegate = m_controller;
		}
	}
	RasterView::~RasterView(void)
	{
		ColorMapIterator map_it;
		std::vector<std::string>::const_iterator path_it;
		for(map_it=m_path_color_map.begin(); map_it!=m_path_color_map.end();) {
			MGColor *release_color = map_it->second;
			m_path_color_map.erase(map_it++);
			delete_and_set_null(release_color);
		}
		delete_and_set_null(m_image_pyramid);
		delete_and_set_null(m_controller);
	}
	void RasterView::init(const std::string& map_name, const std::string& map_root_path)
	{
		m_config_initialized = true;
		m_image_pyramid = new Raster_GIS::ImagePyramid();
		if (m_image_pyramid != NULL) {
			m_image_pyramid->init(map_name, map_root_path);
			m_level_factor = m_image_pyramid->getLevelFactor();
			m_regular_tile_size = m_image_pyramid->getRegularTileSize();
		}
		m_total_floor_num = getTotalFloorNum();
		m_pyramid_region = getPyramidRegion();
	}
	void RasterView::initWithFrame(MGRect frame)
	{
		assert(m_config_initialized);
		MIControl::initWithFrame(frame);
		if (m_raster_overlay != NULL) {
			m_raster_overlay->initWithFrame(MGRectMake(0.0f, 0.0f, frame.width, frame.height));
			addSubview(m_raster_overlay);
		}
		m_min_floor_num = 0;
		m_max_floor_num = getAvailableMaxFloor();
		adjustToCenter();
		//renderImage();
	}
	void RasterView::drawself()
	{
		MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();
		if (graphic_engine == NULL) {
			return;
		}
		RGBAColorRef color_ref = RGBAColorRef(255, 255, 255, 255);

		if (_view_flags.clip_subviews != 0) {
			graphic_engine->enableClip(m_bounds);
		}

		graphic_engine->drawFilledRect(m_bounds, color_ref);

		int i = 0;
		std::vector<MGPoint>::const_iterator const_it;
		m_pyramid_image_locker.lock();
		for (const_it=m_point_vector.begin(); const_it!=m_point_vector.end(); const_it++,i++) {
			MGImage *image = m_image_vector[i]->m_image;
			
			if (image != NULL) {
				PixelInt width = image->getWidth();
				PixelInt height = image->getHeight();
				MGRect rect = MGRectMake(m_bounds.x+(*const_it).x, m_bounds.y+(*const_it).y, float(width), float(height));
				
				image->sendDataToContext();
				graphic_engine->drawImage(image, rect, color_ref, draw_image_mode);
			} else {
				graphic_engine->drawFilledRoundRect(m_bounds, color_ref);
			}
		}
		m_pyramid_image_locker.unlock();

		if (_view_flags.clip_subviews != 0) {
			graphic_engine->disableClip();
		}
	}

	//触摸事件的响应 
	MIEventHandleState RasterView::touchesBegan(const std::set<MITouch*>& touches,
		const MIEvent* const event)
	{
		MIEventHandleState ret = MIControl::touchesBegan(touches, event);
		return ret;
	}
	MIEventHandleState RasterView::touchesMoved(const std::set<MITouch*>& touches,
		const MIEvent* const event)
	{
		MIEventHandleState ret = MIControl::touchesMoved(touches, event);
		MGLogD("=============RasterView::touchesMoved=============");
		return ret;
	}
	MIEventHandleState RasterView::touchesEnded(const std::set<MITouch*>& touches,
		const MIEvent* const event)
	{
		MIEventHandleState ret = MIControl::touchesEnded(touches, event);
		return ret;
	}
	MIEventHandleState RasterView::touchesCancelled(const std::set<MITouch*>& touches,
		const MIEvent* const event)
	{
		MIEventHandleState ret = MIControl::touchesCancelled(touches, event);
		return ret;
	}
	//事件响应   
	MIEventHandleState RasterView::handleEvent(const MIEvent* const event)
	{
		MIEventHandleState ret = MIControl::handleEvent(event);
		return ret;
	}

	MIEventHandleState RasterView::handleWithControlEvent(const MIControlEvents &event)
	{
		MIEventHandleState ret = MIEventHandleStatePass;
		EventsMapIterator it = events_dispatch_map.find(event);
		if (it == events_dispatch_map.end()) {
			ret = MIEventHandleStateNormal;
		} else {
			assert(it->second.view_controller != NULL);
			assert(it->second.event_func != NULL);
			EventFunc func = it->second.event_func;
			((it->second.view_controller)->*func)(this);
			ret = MIEventHandleStateDone;
		}
		return ret;
	}

	void RasterView::transView(const MGPoint &offset, const float &scale)
	{
		double scale_width = m_image_pyramid->getWidthScalePixelGeo(m_curr_floor_num);
		double scale_height = m_image_pyramid->getHeightScalePixelGeo(m_curr_floor_num);
		if (!DataCalculator::isEqualZero(scale_width) && !DataCalculator::isEqualZero(scale_height)) {
			m_geo_rect.x -= offset.x / scale_width * DataCalculator::GEO_EXPANSIVITY;
			m_geo_rect.y += offset.y / scale_height * DataCalculator::GEO_EXPANSIVITY;

			m_floor_offset.x += offset.x;
			m_floor_offset.y += offset.y;
			if (!DataCalculator::isEqualZero(offset.x) && !DataCalculator::isEqualZero(offset.y)) {
				renderImage();
			} else {
				//int i_scale = (int)(scale*getRectScale().axis_x*RASTERVIEW_ESPLON);
				//int i_floor_scale = (int)(m_floor_scale*RASTERVIEW_ESPLON);
				if ((scale < 1.0) && ((m_curr_floor_num < m_max_floor_num) || (m_floor_scale > 1.0))) {
					//zoom out 
					m_floor_scale = m_floor_scale * scale;
					//MGLogD("***************zoomOut m_floor_scale*********************%f", m_floor_scale);
					//int_32 increase_width = m_geo_rect.width - ceil(m_geo_rect.width*scale);
					//int_32 increase_height = m_geo_rect.height - ceil(m_geo_rect.height*scale);
					//m_geo_rect.x -= (increase_width / 2.0f);
					//m_geo_rect.y -= (increase_height / 2.0f);
					//m_geo_rect.width += increase_width;
					//m_geo_rect.height += increase_height;
				} else if ((scale > 1.0) && ((m_curr_floor_num > m_min_floor_num) || (m_floor_scale < 1.0))) {
					//zoom in 
					m_floor_scale = m_floor_scale * scale;
					//MGLogD("***************zoomIn m_floor_scale*********************%f", m_floor_scale);
				}
				scaleView();
			}
		}
	}

	void RasterView::scaleView()
	{
		m_floor_offset = MGPointMake(0.0, 0.0);
		float set_scale = m_floor_scale;
		int_32 curr_floor = m_curr_floor_num;
		if (m_floor_scale < 1.0) {
			float floor_scale = m_floor_scale;
			curr_floor = m_curr_floor_num;
			while (floor_scale < 1.0) {
				m_floor_scale = m_floor_scale * m_level_factor;
				if (curr_floor < m_max_floor_num) {
					m_geo_rect.x = m_geo_rect.x - ceil(m_geo_rect.width/2.0f);// / 4;
					m_geo_rect.y = m_geo_rect.y - ceil(m_geo_rect.height/2.0f);// * 3 / 4;
					m_geo_rect.width = m_geo_rect.width * 2;
					m_geo_rect.height = m_geo_rect.height * 2;
				} else {
					m_floor_scale = 1.0;
					break;
				}
				curr_floor = tryToGetImage(m_geo_rect, (int_32)m_bounds.width, (int_32)m_bounds.height);
				floor_scale = m_floor_scale;
			}
		}
		else if (m_floor_scale > (m_level_factor*1.0)) {
			float floor_scale = m_floor_scale / (m_level_factor * 1.0);
			curr_floor = m_curr_floor_num;
			while (floor_scale > 1.0) {
				m_floor_scale = floor_scale;
				if (curr_floor > m_min_floor_num) {
					m_geo_rect.x = m_geo_rect.x + ceil(m_geo_rect.width/4.0f);// / 8;
					m_geo_rect.y = m_geo_rect.y + ceil(m_geo_rect.height/4.0f);// * 5 / 8;
					m_geo_rect.width = ceil(m_geo_rect.width / 2.0f);
					m_geo_rect.height = ceil(m_geo_rect.height / 2.0f);
				} else {
					m_floor_scale = 1.0;
					break;
				}
				curr_floor = tryToGetImage(m_geo_rect, (int_32)m_bounds.width, (int_32)m_bounds.height);
				floor_scale = m_floor_scale / (m_level_factor * 1.0);
			}
		}
		else {
			if (m_curr_floor_num == m_min_floor_num) {
				m_floor_scale = 1.0;
			}
		}
		renderImage();
		setRectScale(MGRectScaleMake(m_floor_scale, m_floor_scale, 1.0));
	}

	void RasterView::zoomIn()
	{
		if (m_curr_floor_num > m_min_floor_num) {
			m_geo_rect.x = m_geo_rect.x + ceil(m_geo_rect.width/4.0f);// / 8;
			m_geo_rect.y = m_geo_rect.y + ceil(m_geo_rect.height/4.0f);// * 5 / 8;
			m_geo_rect.width = ceil(m_geo_rect.width / 2.0f);
			m_geo_rect.height = ceil(m_geo_rect.height / 2.0f);
			renderImage();
		}
	}
	
	void RasterView::zoomOut()
	{
		if (m_curr_floor_num < m_max_floor_num) {
			m_geo_rect.x = m_geo_rect.x - ceil(m_geo_rect.width/2.0f);// / 4;
			m_geo_rect.y = m_geo_rect.y - ceil(m_geo_rect.height/2.0f);// * 3 / 4;
			m_geo_rect.width = m_geo_rect.width * 2;
			m_geo_rect.height = m_geo_rect.height * 2;
			renderImage();
		}
	}
	

	void RasterView::animateTo(const MGGeoPoint &point)
	{
		m_geo_rect.x = point.x - m_geo_rect.width /2;/// 4;
		m_geo_rect.y = point.y - m_geo_rect.height /2;//* 3 / 4;
		renderImage();
	}

	void RasterView::setZoom(int_32 zoom_level)
	{
		m_curr_floor_num = zoom_level;
		adaptToScreen(m_curr_floor_num);
		renderImage();
	}
	void RasterView::setDrawRect(MGGeoRect draw_rect)
	{
		m_geo_rect = draw_rect;
		renderImage();
	}
	
	double RasterView::getWidthScalePixelGeo()
	{
		return m_image_pyramid->getWidthScalePixelGeo(m_curr_floor_num);
	}
	double RasterView::getHeightScalePixelGeo()
	{
		return m_image_pyramid->getHeightScalePixelGeo(m_curr_floor_num);
	}

	void RasterView::pixelToGeo(MGGeoPoint &dest, const MGPoint &src)
	{
		if (m_image_pyramid != NULL) {
			double scale_width = m_image_pyramid->getWidthScalePixelGeo(m_curr_floor_num);
			double scale_height = m_image_pyramid->getHeightScalePixelGeo(m_curr_floor_num);
			if (DataCalculator::isEqualZero(scale_height) || DataCalculator::isEqualZero(scale_width)) {
				return;
			}
			dest.x = int_32(m_geo_rect.x + src.x / scale_width *DataCalculator::GEO_EXPANSIVITY);
			dest.y = int_32(m_geo_rect.y + (m_geo_rect.height - src.y / scale_height *DataCalculator::GEO_EXPANSIVITY));
		}
	}

	void RasterView::geoToPixel(MGPoint &dest, const MGGeoPoint &src)
	{
		if (m_image_pyramid != NULL) {
			double scale_width = m_image_pyramid->getWidthScalePixelGeo(m_curr_floor_num);
			double scale_height = m_image_pyramid->getHeightScalePixelGeo(m_curr_floor_num);
			dest.x = (src.x - m_geo_rect.x) * scale_width /DataCalculator::GEO_EXPANSIVITY;
			dest.y = (m_geo_rect.y+m_geo_rect.height-src.y) * scale_height /DataCalculator::GEO_EXPANSIVITY;
		}
	}

	void RasterView::renderImage()
	{
		adjustDrawRect(m_geo_rect);
		int_32 curr_floor_num = tryToGetImage(m_geo_rect, (int_32)m_bounds.width, (int_32)m_bounds.height);
		//if (m_curr_floor_num != curr_floor_num)
		//	m_floor_scale = 1.0f;
		adaptToScreen(curr_floor_num);
		m_curr_floor_num = getImage(m_geo_rect, (int_32)m_bounds.width, (int_32)m_bounds.height, m_image_vector, m_point_vector);
	}

	void RasterView::adaptToScreen(int_32 floor_num)
	{
		if (m_image_pyramid != NULL) {
			double scale_width = m_image_pyramid->getWidthScalePixelGeo(floor_num);
			double scale_height = m_image_pyramid->getHeightScalePixelGeo(floor_num);
			if (DataCalculator::isEqualZero(scale_height) || DataCalculator::isEqualZero(scale_width)) {
				return;
			}
			MGGeoPoint center_point = MGGeoPointMake(m_geo_rect.x+m_geo_rect.width/2, m_geo_rect.y+m_geo_rect.height/2);
			m_geo_rect.width = ceil(/*2**/DataCalculator::GEO_EXPANSIVITY* m_bounds.width / scale_width);
			m_geo_rect.height = ceil(/*2**/DataCalculator::GEO_EXPANSIVITY* m_bounds.height / scale_height);
			m_geo_rect.x = center_point.x - m_geo_rect.width / 2;
			m_geo_rect.y = center_point.y - m_geo_rect.height / 2;
		}
	}

	void RasterView::adjustToCenter()
	{
		m_curr_floor_num = m_total_floor_num / 2;
		adaptToScreen(m_curr_floor_num);
		MGGeoPoint center = MGGeoPointMake(m_pyramid_region.x+m_pyramid_region.width/2, m_pyramid_region.y+m_pyramid_region.height/2);
		animateTo(center);
	}

	//获取能取到的最上层  
	int_32 RasterView::getAvailableMaxFloor()
	{
		//adaptToScreen(m_total_floor_num-1);
		////MGGeoPoint center = MGGeoPointMake(m_pyramid_region.x+m_pyramid_region.width/2, m_pyramid_region.y+m_pyramid_region.height/2);
		//adjustDrawRect(m_geo_rect);
		int max_floor_num = tryToGetImage(m_pyramid_region, (int_32)m_bounds.width, (int_32)m_bounds.height);
		return max_floor_num;
	}

	void RasterView::adjustDrawRect(MGGeoRect &draw_rect)
	{
		draw_rect.width = MINVALUE(draw_rect.width, m_pyramid_region.width);
		draw_rect.height = MINVALUE(draw_rect.height, m_pyramid_region.height);
		if (draw_rect.x < m_pyramid_region.x) {
			draw_rect.x = m_pyramid_region.x;
		}
		if (draw_rect.y < m_pyramid_region.y/*-draw_rect.height/2*/) {
			draw_rect.y = m_pyramid_region.y/*-draw_rect.height/2*/;
		}
		if ( (draw_rect.x+draw_rect.width) > (m_pyramid_region.x+m_pyramid_region.width/*+draw_rect.width/2*/) ) {
			draw_rect.x = (m_pyramid_region.x+m_pyramid_region.width) - draw_rect.width/*+draw_rect.width/2*/;
		}
		if ( (draw_rect.y+draw_rect.height) > (m_pyramid_region.y+m_pyramid_region.height) ) {
			draw_rect.y = (m_pyramid_region.y+m_pyramid_region.height) - draw_rect.height;
		}
	}
	
	int_32 RasterView::tryToGetImage(const MGGeoRect &draw_rect, int_32 resolution_width, int_32 resolution_height)
	{
		int_32 ret = -1;
		Raster_GIS::Rect2D rect;
		
		rect.left_low.d0 = (double)(draw_rect.x*1.0/DataCalculator::GEO_EXPANSIVITY);
		rect.left_low.d1 = (double)(draw_rect.y*1.0/DataCalculator::GEO_EXPANSIVITY);
		rect.right_top.d0 = (double)((draw_rect.x + draw_rect.width)*1.0/DataCalculator::GEO_EXPANSIVITY);
		rect.right_top.d1 = (double)((draw_rect.y + draw_rect.height)*1.0/DataCalculator::GEO_EXPANSIVITY);
		if (m_image_pyramid != NULL) {
			ret = m_image_pyramid->tryToGetImage(rect, (const long)resolution_width, (const long)resolution_height);
		}
		return ret;
	}

	int_32 RasterView::getImage(const MGGeoRect &draw_rect, int_32 resolution_width, int_32 resolution_height, 
		std::vector<MGColor *> &image_vector, std::vector<MGPoint> &point_vector)
	{
		int_32 ret = -1;
		Raster_GIS::Rect2D rect;
		std::vector<std::string> image_path_vector;
		std::vector<Raster_GIS::LeftTop> left_top_vector;
		std::vector<Raster_GIS::LeftTop>::const_iterator point_it;
		long col;
		
		rect.left_low.d0 = (double)(draw_rect.x*1.0/DataCalculator::GEO_EXPANSIVITY);
		rect.left_low.d1 = (double)(draw_rect.y*1.0/DataCalculator::GEO_EXPANSIVITY);
		rect.right_top.d0 = (double)((draw_rect.x + draw_rect.width)*1.0/DataCalculator::GEO_EXPANSIVITY);
		rect.right_top.d1 = (double)((draw_rect.y + draw_rect.height)*1.0/DataCalculator::GEO_EXPANSIVITY);
		image_path_vector.clear();
		left_top_vector.clear();
		if (m_image_pyramid!=NULL && m_total_floor_num>0) {
			ret = m_image_pyramid->getImage(rect, (const long)resolution_width, 
				(const long)resolution_height, image_path_vector, left_top_vector, col);
		}
		
		m_pyramid_image_locker.lock();	
		image_vector.clear();
		point_vector.clear();
		m_pyramid_image_locker.unlock();	
		int i = 0;
		ColorMapIterator map_it;
		MGColor *color = NULL;
		bool is_request = false;

		MGPoint first_point;
		MGPoint last_point;
		MGPoint curr_point;
		MGColor *last_color = NULL;
		int curr_col = 0;
		if (!left_top_vector.empty()) {
			first_point = MGPointMake(left_top_vector[0].left, left_top_vector[0].top);
			curr_point = first_point;
		}
		m_pyramid_image_locker.lock();
		for (point_it=left_top_vector.begin(); point_it!=left_top_vector.end(); point_it++,i++) {
			map_it = m_path_color_map.find(image_path_vector[i]);
			if (map_it == m_path_color_map.end()) {	//not exist 
				if (m_path_color_map.size() >= RASTER_IMAGE_MAX_IMAGE_NUM
					&& is_request == false)	{	//map is full and not request 
					is_request = true;

					//待加入线程取图操作  
					//std::vector<std::string>::const_iterator tmp_image_it;
					//RIRequest *request = new RIRequest;
					//for(tmp_image_it=image_path_vector.begin(); tmp_image_it!=image_path_vector.end(); tmp_image_it++)
					//	request->v_path.push_back(*tmp_image_it);
					//request->release_num = RASTER_IMAGE_MAX_IMAGE_NUM-image_path_vector.size();
					//requestForEntry(request);

					releaseFreeEntry(RASTER_IMAGE_MAX_IMAGE_NUM-image_path_vector.size(), image_path_vector);
				}
				color = MGColor::fromImage(image_path_vector[i]);
				//m_path_color_map[image_path_vector[i]] = color;//这样每次先把空对象插入map,再赋值,效率低  
				m_path_color_map.insert(std::map<std::string, MGColor *>::value_type(image_path_vector[i], color));
			} else {
				color = map_it->second;
			}

			if (last_color!=NULL && last_color->m_image!=NULL) {
				//按照图片大小来调整，以免拼图时出现白线  
				curr_point.x += last_color->m_image->getWidth();
			} else {
				curr_point.x = left_top_vector[i].left;
			}
			if ( curr_col++ == col ) {
				if (last_color!=NULL && last_color->m_image!=NULL) {
					//按照图片大小来调整，以免拼图时出现白线  
					curr_point.y += last_color->m_image->getHeight();
				} else {
					curr_point.y = left_top_vector[i].top;
				}
				//curr_point.x = first_point.x;
				curr_point.x = left_top_vector[i].left;
				curr_col = 1;
			}
			last_color = color;
			MGPoint point = curr_point;

			//MGPoint point = MGPointMake(left_top_vector[i].left, left_top_vector[i].top);
			//m_pyramid_image_locker.lock();
			image_vector.push_back(color);
			point_vector.push_back(point);
			//m_pyramid_image_locker.unlock();
			
		}
		m_pyramid_image_locker.unlock();
		return ret;
	}

	int RasterView::requestForEntry(RIRequest *request)
	{
		int ret = 0;
		releaseFreeEntry(request->release_num, request->v_path);
		return ret;
	}

	int RasterView::releaseFreeEntry(int_32 release_num, const std::vector<std::string> &v_path)
	{
		int ret = 0;
		ColorMapIterator map_it;
		std::vector<std::string>::const_iterator path_it;
		for(map_it=m_path_color_map.begin(); map_it!=m_path_color_map.end();) {
			ColorMapIterator tmp_map_it;
			for (path_it=v_path.begin(); path_it!=v_path.end(); path_it++) {
				if (!strcmp(map_it->first.c_str(), (*path_it).c_str()))	{	//is in vector
					break;
				}
			}
			if (path_it == v_path.end()) {
				MGColor *release_color = map_it->second;
				m_path_color_map.erase(map_it++);
				delete_and_set_null(release_color);
				ret++;
				if (ret == release_num) {
					break;
				}
				continue;
			}
			map_it++;
		}
		return ret;
	}

	MGGeoRect RasterView::getPyramidRegion()
	{
		Raster_GIS::Rect2D rect;
		if (m_image_pyramid != NULL)
			rect = m_image_pyramid->getPyramidRegion();
		MGGeoRect ret;
		ret.x = int_32(rect.left_low.d0*DataCalculator::GEO_EXPANSIVITY);
		ret.y = int_32(rect.left_low.d1*DataCalculator::GEO_EXPANSIVITY);
		ret.width = int_32((rect.right_top.d0 - rect.left_low.d0)*DataCalculator::GEO_EXPANSIVITY);
		ret.height = int_32((rect.right_top.d1 - rect.left_low.d1)*DataCalculator::GEO_EXPANSIVITY);
		return ret;
	}

	int_32 RasterView::getTotalFloorNum()
	{
		int_32 ret = 0;
		if (m_image_pyramid != NULL) {
			ret = m_image_pyramid->getTotalFloorNum();
		}
		return ret;
	}

}
