#include "RasterOverlay.h"
#include "DataCalculator.h"
#include "RasterViewController.h"
#include "RasterView.h"
#include "../../System/Shared/MGLog.h"

namespace z11 
{
	RasterOverlay::RasterOverlay()
	{
		//manualClean();
		_view_flags.clip_subviews = 1;
		m_collect_phase = MIPointCollectorCancelled;
		m_draw_type = MINoneDrawing;
		m_controller = NULL;
	}
	RasterOverlay::~RasterOverlay()
	{
		clearLines();
		clearPoints();
	}
	void RasterOverlay::drawself()
	{
		MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();
		if (graphic_engine == NULL) 
			return;
		if (_view_flags.clip_subviews != 0)
			graphic_engine->enableClip(m_bounds);

		RGBAColorRef color_ref = RGBAColorRef(255, 255, 255, 0);
		graphic_engine->drawFilledRect(m_bounds, color_ref);

		std::vector<MGGeoPoint>::const_iterator const_it;
		std::vector<MGPoint> point_vector;
		m_collect_point_locker.lock();
		point_vector.clear();
		for (const_it=m_point_collector.begin(); const_it!=m_point_collector.end(); const_it++)
		{
			MGPoint tmp_point;
			if (m_controller != NULL)
				m_controller->geoToPixel(tmp_point, (*const_it));
			tmp_point = MGPointMake(m_bounds.x+tmp_point.x, m_bounds.y+tmp_point.y);
			point_vector.push_back(tmp_point);
		}
		m_collect_point_locker.unlock();
		if (point_vector.size() > 0)
		{
			graphic_engine->drawPoint(point_vector[0], RGBAColorMake(255, 0, 0), 4);
			if (m_collect_element == MILineCollecting)
				graphic_engine->drawPolyLines(point_vector, RGBAColorMake(255, 0, 0), 2);
			else if (m_collect_element == MIRegionCollecting)
			{
				graphic_engine->drawPolyLoopLines(point_vector, RGBAColorMake(255, 0, 0), 2);
				graphic_engine->drawFilledPolyLoopLines(point_vector, RGBAColorMake(255, 0, 0, 128));
			}
		}

		RegionColorVector::iterator v_it;
		m_compare_region_locker.lock();
		for (v_it=m_compare_regions.begin(); v_it!=m_compare_regions.end(); v_it++)
		{
			if (m_draw_type == MILineDrawing)
				graphic_engine->drawPolyLines(transPointsToSrc((*v_it)->region), (*v_it)->color, 2);
			else if (m_draw_type == MIRegionDrawing)
				graphic_engine->drawFilledPolyLoopLines(transPointsToSrc((*v_it)->region), (*v_it)->color, 2);
		}
		m_compare_region_locker.unlock();

		int i = 0;
		MGColor *_color = NULL;
		color_ref = RGBAColorRef(255, 255, 255);
		m_draw_point_locaker.lock();
		for (const_it=m_draw_points.begin(); const_it!=m_draw_points.end(); const_it++)
		{
			_color = m_draw_point_colors[i++];
			if (_color!=NULL && _color->m_image!=NULL)
			{
				MGPoint tmp_point;
				if (m_controller != NULL)
					m_controller->geoToPixel(tmp_point, *const_it);
				PixelInt width = _color->m_image->getWidth();
				PixelInt height = _color->m_image->getHeight();
				MGRect location_rect = MGRectMake(m_bounds.x+tmp_point.x-width/2, m_bounds.y+tmp_point.y-height, width, height);
				_color->m_image->sendDataToContext();
				graphic_engine->drawImage(_color->m_image, location_rect, color_ref, draw_image_mode);
			}
		}
		m_draw_point_locaker.unlock();

		if (_view_flags.clip_subviews != 0)
			graphic_engine->disableClip();
	}

	//触摸事件的响应 
	MIEventHandleState RasterOverlay::touchesBegan(const std::set<MITouch*>& touches,
		const MIEvent* const event)
	{
		MIEventHandleState ret = MIControl::touchesBegan(touches, event);
		m_curr_collect_cancel = false;
		return ret;
	}
	MIEventHandleState RasterOverlay::touchesMoved(const std::set<MITouch*>& touches,
		const MIEvent* const event)
	{
		MIEventHandleState ret = MIControl::touchesMoved(touches, event);
		return ret;
	}
	MIEventHandleState RasterOverlay::touchesEnded(const std::set<MITouch*>& touches,
		const MIEvent* const event)
	{
		MIEventHandleState ret = MIControl::touchesEnded(touches, event);
		std::set<MITouch*>::const_iterator touch_it;
		//collect
		if (m_collect_phase == MIPointCollectorBegan && !m_curr_collect_cancel)
		{
			m_collect_point_locker.lock();
			for (touch_it=touches.begin(); touch_it!=touches.end(); ++touch_it) {
				if ((*touch_it) == NULL)
					continue;
				MGGeoPoint point;
				MGPoint relation_point = MGPointMake((*touch_it)->point.x-m_bounds.x, (*touch_it)->point.y-m_bounds.y);
				if (m_controller != NULL)
					m_controller->pixelToGeo(point, relation_point);
				m_point_collector.push_back(point);
			}
			m_collect_point_locker.unlock();
		}
		return ret;
	}
	MIEventHandleState RasterOverlay::touchesCancelled(const std::set<MITouch*>& touches,
		const MIEvent* const event)
	{
		MIEventHandleState ret = MIControl::touchesCancelled(touches, event);
		return ret;
	}
	//事件响应   
	MIEventHandleState RasterOverlay::handleEvent(const MIEvent* const event)
	{
		MIEventHandleState ret = MIControl::handleEvent(event);
		return ret;
	}

	void RasterOverlay::transView(const MGPoint &offset, const float &scale)
	{
		m_curr_collect_cancel = true;
	}

	void RasterOverlay::zoomIn()
	{
		
	}
	void RasterOverlay::zoomOut()
	{
		
	}
	void RasterOverlay::animateTo(const MGGeoPoint &point)
	{
		
	}
	void RasterOverlay::setZoom(int_32 zoom_level)
	{

	}

	double RasterOverlay::getMeasuredDistance()
	{
		double ret = 0.0f;
		if (m_collect_phase == MIPointCollectorEnded)
			ret = m_measured_length;
		return ret;
	}

	double RasterOverlay::getMeasuredAcreage()
	{
		double ret = 0.0f;
		if (m_collect_phase == MIPointCollectorEnded)
			ret = m_measured_acreage;
		return ret;
	}

	vector<MGGeoPoint> RasterOverlay::getCollectedPoints()
	{
		return m_point_collector;
	}

	void RasterOverlay::startLineCollecting()
	{
		m_collect_point_locker.lock();
		m_collect_element = MILineCollecting;
		m_collect_phase = MIPointCollectorBegan;
		m_point_collector.clear();
		m_collect_point_locker.unlock();
	}

	void RasterOverlay::stopLineCollecting()
	{
		m_measured_length = DataCalculator::getDistance(m_point_collector);

		m_collect_point_locker.lock();
		m_collect_phase = MIPointCollectorEnded;
		m_point_collector.clear();
		m_collect_point_locker.unlock();
	}

	void RasterOverlay::startRegionCollecting()
	{
		m_collect_point_locker.lock();
		m_collect_element = MIRegionCollecting;
		m_collect_phase = MIPointCollectorBegan;
		m_point_collector.clear();
		m_collect_point_locker.unlock();
	}

	void RasterOverlay::stopRegionCollecting()
	{
		m_measured_length = DataCalculator::getPerimeter(m_point_collector);
		m_measured_acreage = DataCalculator::getAcreage(m_point_collector);

		m_collect_point_locker.lock();
		m_collect_phase = MIPointCollectorEnded;
		m_point_collector.clear();
		m_collect_point_locker.unlock();
	}

	void RasterOverlay::drawPolyLines(const vector<MGGeoPoint> &poly_points, RGBAColorRef color_ref)
	{
		m_compare_region_locker.lock();
		m_draw_type = MILineDrawing;
		RegionColor *_reg_color = new RegionColor;
		_reg_color->color = color_ref;
		_reg_color->region = poly_points;
		m_compare_regions.push_back(_reg_color);
		m_compare_region_locker.unlock();
	}
	void RasterOverlay::drawFilledPolyLoopLines(const vector<MGGeoPoint> &poly_points, RGBAColorRef color_ref)
	{
		m_compare_region_locker.lock();
		m_draw_type = MIRegionDrawing;
		RegionColor *_reg_color = new RegionColor;
		_reg_color->color = color_ref;
		_reg_color->region = poly_points;
		m_compare_regions.push_back(_reg_color);
		m_compare_region_locker.unlock();
	}
	void RasterOverlay::drawPoint(const std::vector<MGGeoPoint> &poly_points, std::vector<std::string> &image_path)
	{
		if (poly_points.size() != image_path.size())
			return;
		std::vector<MGGeoPoint>::const_iterator const_points_it;
		std::vector<MGColor *>::iterator const_colors_it;
		MGColor *_color = NULL;
		int i = 0;
		m_draw_point_locaker.lock();
		for (const_colors_it=m_draw_point_colors.begin(); const_colors_it!=m_draw_point_colors.end();)
		{
			_color = *const_colors_it;
			const_colors_it = m_draw_point_colors.erase(const_colors_it);
			delete_and_set_null(_color);
		}

		m_draw_point_colors.clear();
		m_draw_points.clear();
		for (const_points_it=poly_points.begin(); const_points_it!=poly_points.end(); const_points_it++)
		{
			_color = MGColor::fromImage(image_path[i++]);
			m_draw_point_colors.push_back(_color);
			m_draw_points.push_back(*const_points_it);
		}
		m_draw_point_locaker.unlock();
	}

	void RasterOverlay::clearLines()
	{
		m_compare_region_locker.lock();
		RegionColorVector::iterator _region_it;
		RegionColor *_region_color = NULL;
		for (_region_it=m_compare_regions.begin(); _region_it!=m_compare_regions.end();)
		{
			_region_color = *_region_it;
			_region_it = m_compare_regions.erase(_region_it);
			delete_and_set_null(_region_color);
		}
		m_compare_regions.clear();
		m_compare_region_locker.unlock();
	}
	void RasterOverlay::clearPoints()
	{
		m_draw_point_locaker.lock();
		std::vector<MGColor *>::iterator _colors_it;
		MGColor *_color = NULL;
		for (_colors_it=m_draw_point_colors.begin(); _colors_it!=m_draw_point_colors.end();)
		{
			_color = *_colors_it;
			_colors_it = m_draw_point_colors.erase(_colors_it);
			delete_and_set_null(_color);
		}
		m_draw_points.clear();
		m_draw_point_locaker.unlock();
	}

	std::vector<MGPoint> RasterOverlay::transPointsToSrc(const std::vector<MGGeoPoint> &v_points)
	{
		std::vector<MGGeoPoint>::const_iterator const_it;
		std::vector<MGPoint> point_vector;
		point_vector.clear();
		for (const_it=v_points.begin(); const_it!=v_points.end(); const_it++)
		{
			MGPoint tmp_point;
			if (m_controller != NULL)
				m_controller->geoToPixel(tmp_point, (*const_it));
			tmp_point = MGPointMake(m_bounds.x+tmp_point.x, m_bounds.y+tmp_point.y);
			point_vector.push_back(tmp_point);
		}
		return point_vector;
	}
} //namespace z11
