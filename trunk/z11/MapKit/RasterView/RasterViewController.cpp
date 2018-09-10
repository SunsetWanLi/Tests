#include "RasterViewController.h"
#include "DataCalculator.h"
#include "RasterView.h"
#include "RasterOverlay.h"
#include "System/Shared/MGLog.h"
#include <math.h>

#define RASTER_VIEW_CONTROLLER_MIN_MOVE_VALUE 0.1f
#define RASTER_VIEW_CONTROLLER_MIN_ZOOM_VALUE 0.0f
#define RASTER_VIEW_CONTROLLER_MIN_SCALING_VALVE 0.25f
#define RASTER_VIEW_CONTROLLER_MAX_SCALING_VALVE 4.0f
#define RASTER_VIEW_CONTROLLER_MAX_TOUCH_NUM 2
namespace z11
{
	RasterViewController::RasterViewController()
	{
		m_frame_initialized = false;
		enable_move = true;
		enable_scale = true;
		m_raster_overlay = NULL;
		m_raster_view = NULL;
		m_image_real_scaling = 1.0f;
	}
	RasterViewController::~RasterViewController()
	{
		//delete_and_set_null(m_raster_view);
		//delete_and_set_null(m_raster_overlay);
	}

	MIViewController* RasterViewController::initWithMixNameInBundle(const string& name, const MGBundle* bundle)
	{
		return MIViewController::initWithMixNameInBundle(name,bundle);
	}
	MIViewController* RasterViewController::initManually()
	{
		MIViewController* ret = MIViewController::initManually();
		return ret;
	}
	void RasterViewController::initViewsWithFrame(const MGRect &frame)
	{

	}
	void RasterViewController::initRasterView(const std::string& map_name, const std::string& map_root_path)
	{

	}

	//触摸事件的响应 
	MIEventHandleState RasterViewController::touchesBegan(const std::set<MITouch*>& touches,
		const MIEvent* const event)
	{
		MIEventHandleState ret = MIViewController::touchesBegan(touches, event);
		std::set<MITouch*> tmp_touches;
		std::set<MITouch*>::const_iterator touch_it;
		for (touch_it=touches.begin(); touch_it!=touches.end(); ++touch_it) {
			if ((*touch_it) == NULL)
				continue;
            MGPoint loc_in_view = (*touch_it)->locationInView(m_raster_view);
			if (m_raster_view->pointInside(loc_in_view, NULL)) {
				if (m_previous_touches.size() >= RASTER_VIEW_CONTROLLER_MAX_TOUCH_NUM) {
					break;
				}
				//m_raster_overlay应改成用户自定义图层  
				//if ((*touch_it)->maintained_view == m_raster_overlay) {
					m_previous_touches.insert(*touch_it);
				//}
					//MGLogD("touchesBegan point(%f, %f);", (*touch_it)->point.x, (*touch_it)->point.y);
			}
		}
		DataCalculator::getCentrePointFromTouches(m_previous_touches, m_previous_centra_point);
		DataCalculator::getRadiusFromTouches(m_previous_touches, m_previous_radius);
		m_previous_radius = m_previous_radius * m_raster_view->getRectScale().axis_x;
		return ret;
	}
	MIEventHandleState RasterViewController::touchesMoved(const std::set<MITouch*>& touches,
		const MIEvent* const event)
	{
		MIEventHandleState ret = MIViewController::touchesMoved(touches, event);
		bool is_moved = false;
		bool is_zoomed = false;
		bool is_need_redraw = false;
		MGPoint tmp_point, tmp_offset;
		float tmp_radius = 0, tmp_scaling = 1.0;

		std::set<MITouch*> tmp_touches;
		std::set<MITouch*>::const_iterator touch_it;
		for (touch_it=touches.begin(); touch_it!=touches.end(); ++touch_it) {
			if ((*touch_it) == NULL)
				continue;
			MGPoint loc_in_view = (*touch_it)->locationInView(m_raster_view);
			if (m_raster_view->pointInside(loc_in_view, NULL)) {
				if (tmp_touches.size() >= RASTER_VIEW_CONTROLLER_MAX_TOUCH_NUM) {
					break;
				}
				//if ((*touch_it)->maintained_view == m_raster_overlay) {
					tmp_touches.insert(*touch_it);
				//}
				//MGLogD("touchesMoved point(%f, %f);", (*touch_it)->point.x, (*touch_it)->point.y);
			}
		}


		if (tmp_touches.size() != m_previous_touches.size()) {
			DataCalculator::getCentrePointFromTouches(tmp_touches, m_previous_centra_point);
			DataCalculator::getRadiusFromTouches(tmp_touches, m_previous_radius);
			m_previous_radius *= m_raster_view->getRectScale().axis_x;
		}
		DataCalculator::getCentrePointFromTouches(tmp_touches, tmp_point);
		DataCalculator::getRadiusFromTouches(tmp_touches, tmp_radius);
		tmp_radius *= m_raster_view->getRectScale().axis_x;

//MGLogD("tmp_radius=%f, m_previous_radius=%f",tmp_radius, m_previous_radius);
		//计算缩放比例  
		m_image_curr_scaling = 1.0f;
		if (tmp_touches.size()==2 && m_previous_touches.size()==2) {
			tmp_scaling = tmp_radius / m_previous_radius;
			m_previous_radius = tmp_radius;
			if (fabsf(tmp_scaling - 1.0f) > RASTER_VIEW_CONTROLLER_MIN_ZOOM_VALUE) {
				m_image_curr_scaling = tmp_scaling;
				is_zoomed = true;
			}
		}

		//计算移动距离  
		m_image_curr_offset = MGPointMake(0.0f, 0.0f);
		if (tmp_touches.size()==1 && m_previous_touches.size()==1) {
			tmp_offset.x = tmp_point.x - m_previous_centra_point.x;
			tmp_offset.y = tmp_point.y - m_previous_centra_point.y;
			m_previous_centra_point = tmp_point;
			if (fabsf(tmp_offset.x) > RASTER_VIEW_CONTROLLER_MIN_MOVE_VALUE
				|| fabsf(tmp_offset.y) > RASTER_VIEW_CONTROLLER_MIN_MOVE_VALUE) {
				m_image_curr_offset = tmp_offset;
				is_moved = true;
			}
		}
		m_previous_touches = tmp_touches;

//MGLogD("RasterViewController::touchesMoved==%d",tmp_touches.size());
//MGLogD("m_image_curr_scaling==%f",m_image_curr_scaling);
		if (enable_move && is_moved) {
			m_image_real_offset.x += m_image_curr_offset.x;
			m_image_real_offset.y += m_image_curr_offset.y;
			is_need_redraw = true;
		}
		if(enable_scale && is_zoomed) {
			m_image_real_scaling *= m_image_curr_scaling;
			is_need_redraw = true;
		}

		if (is_need_redraw) {
			transView(m_image_curr_offset, m_image_curr_scaling);
			ret = MIEventHandleStateDone;
		}
		return ret;
	}
	MIEventHandleState RasterViewController::touchesEnded(const std::set<MITouch*>& touches,
		const MIEvent* const event)
	{
		MIEventHandleState ret = MIViewController::touchesEnded(touches, event);
		std::set<MITouch*> tmp_touches;
		std::set<MITouch*>::const_iterator touch_it;
		int touches_num = 0;

		//从m_previous_touches中删除ended的touch
		for (touch_it=touches.begin(); touch_it!=touches.end(); ++touch_it) {
			assert((*touch_it) != NULL);
			std::set<MITouch*>::const_iterator tmp_touch_it;
			for (tmp_touch_it=m_previous_touches.begin(); tmp_touch_it!=m_previous_touches.end(); ++tmp_touch_it) {
				assert((*tmp_touch_it) != NULL);
				if((*tmp_touch_it)->tag == (*touch_it)->tag) {
					m_previous_touches.erase(*tmp_touch_it);
					MGLogD("m_previous_touches.erase");
					break;
				}
			}
		}
		DataCalculator::getCentrePointFromTouches(m_previous_touches, m_previous_centra_point);
		DataCalculator::getRadiusFromTouches(m_previous_touches, m_previous_radius);
		m_previous_radius *= m_raster_view->getRectScale().axis_x;
		updateView();
		return ret;
	}
	MIEventHandleState RasterViewController::touchesCancelled(const std::set<MITouch*>& touches,
		const MIEvent* const event)
	{
		MIEventHandleState ret = MIViewController::touchesCancelled(touches, event);
		m_previous_touches.clear();
		return ret;
	}
	//事件响应   
	MIEventHandleState RasterViewController::handleEvent(const MIEvent* const event)
	{
		MIEventHandleState ret = MIViewController::handleEvent(event);
		return ret;
	}

	void RasterViewController::transView(const MGPoint &offset, const float &scale)
	{
		if (m_raster_view != NULL)
			m_raster_view->transView(offset, scale);
		if (m_raster_overlay != NULL)
			m_raster_overlay->transView(offset, scale);
	}

	void RasterViewController::updateView()
	{
		//if (m_raster_view != NULL)
		//	m_raster_view->scaleView();
	}

	void RasterViewController::startLineCollecting()
	{
		if (m_raster_overlay != NULL)
			m_raster_overlay->startLineCollecting();
	}
	void RasterViewController::stopLineCollecting()
	{
		if (m_raster_overlay != NULL)
			m_raster_overlay->stopLineCollecting();
	}
	void RasterViewController::startRegionCollecting()
	{
		if (m_raster_overlay != NULL)
			m_raster_overlay->startRegionCollecting();
	}
	void RasterViewController::stopRegionCollecting()
	{
		if (m_raster_overlay != NULL)
			m_raster_overlay->stopRegionCollecting();
	}
	double RasterViewController::getMeasuredDistance()
	{
		double ret = 0.0;
		if (m_raster_overlay != NULL)
			ret = m_raster_overlay->getMeasuredDistance();
		return ret;
	}
	double RasterViewController::getMeasuredAcreage()
	{
		double ret = 0.0;
		if (m_raster_overlay != NULL)
			ret = m_raster_overlay->getMeasuredAcreage();
		return ret;
	}

	vector<MGGeoPoint> RasterViewController::getCollectedPoints()
	{
		vector<MGGeoPoint> ret;
		if (m_raster_overlay != NULL)
			ret = m_raster_overlay->getCollectedPoints();
		return ret;
	}

	void RasterViewController::clearLines()
	{
		if (m_raster_overlay != NULL)
			m_raster_overlay->clearLines();
	}
	void RasterViewController::clearPoints()
	{
		if (m_raster_overlay != NULL)
			m_raster_overlay->clearPoints();
	}
	void RasterViewController::drawPolyLines(const vector<MGGeoPoint> &poly_points, RGBAColorRef color_ref)
	{
		if (m_raster_overlay != NULL)
			m_raster_overlay->drawPolyLines(poly_points);
	}
	void RasterViewController::drawFilledPolyLoopLines(const vector<MGGeoPoint> &poly_points, RGBAColorRef color_ref)
	{
		if (m_raster_overlay != NULL)
			m_raster_overlay->drawFilledPolyLoopLines(poly_points, color_ref);
	}
	void RasterViewController::drawPoint(const std::vector<MGGeoPoint> &poly_points, std::vector<std::string> &image_path)
	{
		if (m_raster_overlay != NULL)
			m_raster_overlay->drawPoint(poly_points, image_path);
	}

	void RasterViewController::drawPolyLines(const vector<MGPoint> &poly_points, RGBAColorRef color_ref)
	{
		vector<MGGeoPoint> geo_points;
		DataCalculator::transPointsToGeo(geo_points, poly_points);
		drawPolyLines(geo_points);
	}
	void RasterViewController::drawFilledPolyLoopLines(const vector<MGPoint> &poly_points, RGBAColorRef color_ref)
	{
		vector<MGGeoPoint> geo_points;
		DataCalculator::transPointsToGeo(geo_points, poly_points);
		drawFilledPolyLoopLines(geo_points, color_ref);
	}
	void RasterViewController::drawPoint(const std::vector<MGPoint> &poly_points, std::vector<std::string> &image_path)
	{
		vector<MGGeoPoint> geo_points;
		DataCalculator::transPointsToGeo(geo_points, poly_points);
		drawPoint(geo_points, image_path);
	}

	void RasterViewController::zoomIn()
	{
		if (m_raster_view != NULL)
			m_raster_view->zoomIn();
		if (m_raster_overlay != NULL)
			m_raster_overlay->zoomIn();
	}
	void RasterViewController::zoomOut()
	{
		if (m_raster_view != NULL)
			m_raster_view->zoomOut();
		if (m_raster_overlay != NULL)
			m_raster_overlay->zoomOut();
	}
	void RasterViewController::animateTo(const MGGeoPoint &point)
	{
		if (m_raster_view != NULL)
			m_raster_view->animateTo(point);
		if (m_raster_overlay != NULL)
		{
			m_raster_overlay->animateTo(point);
		}
	}
	void RasterViewController::setZoom(int_32 zoom_level)
	{
		if (m_raster_view != NULL)
			m_raster_view->setZoom(zoom_level);
		if (m_raster_overlay != NULL)
			m_raster_overlay->setZoom(zoom_level);
	}

	void RasterViewController::pixelToGeo(MGGeoPoint &dest, const MGPoint &src)
	{
		if (m_raster_view != NULL)
			m_raster_view->pixelToGeo(dest, src);
	}

	void RasterViewController::geoToPixel(MGPoint &dest, const MGGeoPoint &src)
	{
		if (m_raster_view != NULL)
			m_raster_view->geoToPixel(dest, src);
	}

	void RasterViewController::updatingLocation(const MGLocation& to_location, const MGLocation& from_location)
	{

	}
	void RasterViewController::updatingHeading(const MGHeading& new_heading)
	{

	}

	RasterView *RasterViewController::getRasterView()
	{
		return m_raster_view;
	}
	RasterOverlay *RasterViewController::getOverlayView()
	{
		return m_raster_overlay;
	}
	
}//z11