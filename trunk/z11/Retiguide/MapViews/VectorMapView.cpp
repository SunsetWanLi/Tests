#include "VectorMapView.h"
#include "Retiguide/Common/String_Tools.h"
#include "Retiguide/Render/VctorRender/Map/MapOpration.h"
#include "Retiguide/Render/VctorRender/Map/Navigation.h"
#include "Retiguide/SpatialMath/SpatialMath.h"

#include "MapKit/Embedded/Application/Pathfinding.h"
#include "MapKit/Embedded/Application/LayerMgr.h"
#include "MapKit/Embedded/BasicType/Line.h"
#include "MapKit/Embedded/Application/AttributeMgr.h"
#include "MapKit/Embedded/BasicType/Rect.h"
#include "MapKit/Embedded/Application/QueryMgr.h"
#include "MapKit/Embedded/BasicType/Point.h"
#include "MapKit/Embedded/BasicType/Line.h"
#include "MapKit/Embedded/BasicType/Polygon.h"

#include "System/IO/MGBundle.h"
#include "CommonBase/MGWString.h"
#include "System/Shared/MGLog.h"
#include "System/Shared/MGTime.h"

#include <iostream>

using namespace Embedded;
using namespace GIS_Embedded;
using namespace z11;

#include "GraphicCore/MGCanvas.h"
#include "System/Device/MGDevice.h"
extern MGCanvas *global_test_canvas;
VectorMapView::VectorMapView(bool is_raster_map)//:m_feature_layer_render(setConf),m_annotation_layer_render(setConf)
{
	initData();
	m_is_render_host = true;
	m_is_render_dest = true;
	m_is_render_src = true;

	m_need_redraw = true;
    std::string filename=MGBundle::mainBundle()->pathWithFileName("GIS_Embedded/image/host.png");
    MGLogD(filename.c_str());
	m_host_image = z11::MGImage::loadImageFromFile(filename);

	m_host.d0 = 0;
	m_host.d1 = 0;

	m_dest.d0 = m_host.d0;
	m_dest.d1 = m_host.d1;
	m_src_image = z11::MGImage::loadImageFromFile(MGBundle::mainBundle()->pathWithFileName("GIS_Embedded/image/src.png"));
	m_dest_image = z11::MGImage::loadImageFromFile(MGBundle::mainBundle()->pathWithFileName("GIS_Embedded/image/dest.png"));
	m_cur_image = z11::MGImage::loadImageFromFile(MGBundle::mainBundle()->pathWithFileName("GIS_Embedded/image/curpos.png"));
	
	m_gps_host.d0 = m_host.d0;
	m_gps_host.d1 = m_host.d1;	
	m_gps_host_image = z11::MGImage::loadImageFromFile(MGBundle::mainBundle()->pathWithFileName("GIS_Embedded/image/host.png"));
	
	m_path_points.clear();
	m_draw_path = false;
	m_fechting_data = false;
	m_is_navi = false;
	m_scale_rank = 100;
	m_visual_angle_state = LOOK_DOWN;
	m_is_rastermap = is_raster_map;

	m_path_line_render.setLineColor(RGBAColorRef::skyBlue().convertToARGB());
	m_path_line_render.m_line_style = SOLID;
	m_path_line_render.m_pixel_width = 7;
	m_track_line_render.setLineColor(0x00ff00);
	m_track_line_render.m_line_style = SOLID;
	m_track_line_render.m_pixel_width = 5;
	m_dash_line_render.setLineColor(0xff0000);
	m_dash_line_render.m_line_style = DASH;
	m_dash_line_render.m_pixel_width = 1;
}

VectorMapView::~VectorMapView()
{
	m_map_frame.clear();
	// 	delete m_raster;
	_instance=NULL;
}
void VectorMapView::setScreenPix(int width,int height)
{
	m_screen_pix_width = width;
	m_screen_pix_height = height;
	m_screen_pix_map.setScreenSize(width,height,10,10);
}

void VectorMapView::initData()
{

	m_feature_layer_render.setTransformer(&m_transformer);
	m_annotation_layer_render.setTransformer(&m_transformer);
	m_feature_layer_render.setViewScopeAdjust(&m_view_scope);

	m_feature_layer_render.setIsRasterMap(m_is_rastermap);
	//vector map data
	// 	if (m_is_vectormap)
	// 	{
	m_datasrc_nodes = m_map_frame.getFclsGlobalIndex()->getAllFclsNames();
	int src_size = m_datasrc_nodes.size();

	m_layerMgr = Embedded::LayerMgr::getInstance();
	for (int i=0; i<src_size; i++)
	{
		int fcls_size = m_datasrc_nodes[i].second.size();

		for (int j=0 ;j <fcls_size; j++)
		{
			std::string fcls_name = m_datasrc_nodes[i].second[j];
			m_fcls_names.push_back(fcls_name);
			std::string file_name = MGBundle::mainBundle()->pathWithFileName("GIS_Embedded/data/"+fcls_name+".dat");
			z11::MGWString wstrLayerName = Utility::ustr2wstr(file_name);
			m_layerMgr->loadLayer(wstrLayerName);
		}
	}
	AttributeMgr::getInstance()->loadAttributes(Utility::ustr2wstr(MGBundle::mainBundle()->pathWithFileName("GIS_Embedded/data")));
	// 	}
	//host/dest

}
void VectorMapView::getLayerDataInScope()
{
	m_fechting_data = true;
	m_map_frame.getMapLayersNeedRender(m_scale_rank,m_view_scope,&m_layer_global_ids,&m_layer_mapframe_indexs);
	Embedded::MapPos2D leftlow(m_view_scope.m_left,m_view_scope.m_bottom), righttop(m_view_scope.m_right,m_view_scope.m_top);
	Embedded::Rect r(leftlow,righttop);

	std::vector<Embedded::IShape*> shapes;

	for(int i = 0; i < m_shapes.size(); ++i)
		for (int j=0; j<m_shapes[i].size(); ++j)
			delete m_shapes[i][j];

	m_shapes.clear();
	int fcls_size = m_layer_global_ids.size();
	for (int i=0; i<fcls_size; i++)
	{
		std::string fcls_name = m_fcls_names[m_layer_global_ids[i]];
		z11::MGWString wFcls_name = Utility::ustr2wstr(fcls_name);
		m_layerMgr->queryShapeByRect(r, shapes, wFcls_name);
		m_shapes.push_back(shapes);
		shapes.clear();
	}
	m_fechting_data = false;
}
void VectorMapView::getLayerDataInRect(Scope &scope,vector<GIS_Embedded::Pos2DEx>& points,vector<std::string>& names,bool flag)//框选
{
	static vector<int> layer_global_ids;
	vector<int> layer_mapframe_indexs;

	m_map_frame.getMapLayersNeedRender(m_scale_rank,m_view_scope,&layer_global_ids,&layer_mapframe_indexs);

	Embedded::MapPos2D leftlow(scope.m_left,scope.m_bottom), righttop(scope.m_right,scope.m_top);
	Embedded::Rect r(leftlow,righttop);

	std::vector<Embedded::IShape*> shapes;

	for(int i = 0; i < m_rect_shapes.size(); ++i)
		for (int j=0; j<m_rect_shapes[i].size(); ++j)
			delete m_rect_shapes[i][j];

	m_rect_shapes.clear();
	int fcls_size = layer_global_ids.size();

	for (int i=0; i<fcls_size; i++)
	{
		std::string fcls_name = m_fcls_names[layer_global_ids[i]];
		z11::MGWString wFcls_name = Utility::ustr2wstr(fcls_name);
		m_layerMgr->queryShapeByRect(r, shapes, wFcls_name);
		m_rect_shapes.push_back(shapes);
		shapes.clear();
	}

	int sum = layer_global_ids.size();
	for (int i=0;i<sum;i++)
	{
		FeatureLayer* f_layer=m_map_frame.m_feature_layers[layer_mapframe_indexs[i]];
		vector<Embedded::IShape*> layer_data = m_rect_shapes[i];//last_scope_data[i];
		for (int j=0 ;j<layer_data.size(); j++)
		{
			IShape *geo=layer_data[j];
			switch(f_layer->m_layer_type)
			{
			case FeatureLayer::t_POINT:
				{
					IShape *geo = layer_data[i];
					Embedded::Point* gm_point = dynamic_cast< Embedded::Point*>(geo);
					Embedded::ScreenPos2D pos;
					gm_point->getPoint(&pos);
					GIS_Embedded::Pos2DEx posEx(pos.d0,pos.d1);
					string point_name = "";
					point_name = Utility::wstr2ustr(geo->getName());
					points.push_back(posEx);
					names.push_back(point_name);
				}
				break;
			default:
				break;
			}
		}
	}
}

void VectorMapView::getLayerDataInRect(Scope& scope,vector<int>& fcls_id,vector<GIS_Embedded::Pos2DEx>& points,vector<std::string>& names,bool flag)
{
	static vector<int> layer_global_ids;
	vector<int> layer_mapframe_indexs;

	if (flag)
		m_map_frame.getMapLayers(m_scale_rank,scope,fcls_id,&layer_global_ids,&layer_mapframe_indexs);
	else
		m_map_frame.getMapLayersNeedRender(m_scale_rank,scope,fcls_id,&layer_global_ids,&layer_mapframe_indexs);

	Embedded::MapPos2D leftlow(scope.m_left,scope.m_bottom), righttop(scope.m_right,scope.m_top);
	Embedded::Rect r(leftlow,righttop);

	std::vector<Embedded::IShape*> shapes;

	for(int i = 0; i < m_rect_shapes.size(); ++i)
		for (int j=0; j<m_rect_shapes[i].size(); ++j)
			delete m_rect_shapes[i][j];

	m_rect_shapes.clear();
	int fcls_size = layer_global_ids.size();

	for (int i=0; i<fcls_size; i++)
	{
		std::string fcls_name = m_fcls_names[layer_global_ids[i]];
		z11::MGWString wFcls_name = Utility::ustr2wstr(fcls_name);
		m_layerMgr->queryShapeByRect(r, shapes, wFcls_name);
		m_rect_shapes.push_back(shapes);
		shapes.clear();
	}

	int sum = layer_global_ids.size();
	for (int i=0;i<sum;i++)
	{
		FeatureLayer* f_layer=m_map_frame.m_feature_layers[layer_mapframe_indexs[i]];
		vector<Embedded::IShape*> layer_data = m_rect_shapes[i];//last_scope_data[i];
		for (int j=0 ;j<layer_data.size(); j++)
		{
			IShape *geo=layer_data[j];
			switch(f_layer->m_layer_type)
			{
			case FeatureLayer::t_POINT:
				{
					IShape *geo = layer_data[i];
					Embedded::Point* gm_point = dynamic_cast< Embedded::Point*>(geo);
					Embedded::ScreenPos2D pos;
					gm_point->getPoint(&pos);
					GIS_Embedded::Pos2DEx posEx(pos.d0,pos.d1);
					string point_name = "";
					point_name = Utility::wstr2ustr(geo->getName());
					points.push_back(posEx);
					names.push_back(point_name);
				}
				break;
			default:
				break;
			}
		}
	}
}
void VectorMapView::renderFeatureLayers()
{
	if (m_fechting_data)
		return;
	m_style_render.setCanvas(global_test_canvas);
	m_trans_scope.setScope(m_view_scope);
	m_transformer.setTansformPara(m_view_scope,0,m_screen_pix_width,m_screen_pix_height);
	if (m_is_rastermap)
		m_transformer.setTansformPara(m_scale_width,m_scale_height);

	m_screen_pix_map.clear();

	int i,sum = m_layer_global_ids.size();
	for (i=0;i<sum;i++)
	{

		FeatureLayer* f_layer=m_map_frame.m_feature_layers[m_layer_mapframe_indexs[i]];
		AnnotationLayer* a_layer= m_map_frame.m_annotation_layers[m_layer_mapframe_indexs[i]];

		if(a_layer->m_layer_type == FeatureLayer::t_POINT) //draw point in renderPointFeatureLayers
		{
			break;
		}

		static int layer = -1;
		if(i == layer)
			continue;
		a_layer->clearGeoLabelPos();
		m_feature_layer_render.renderMapLayer(m_style_render,*f_layer,m_shapes[i],*a_layer,m_screen_pix_map);
	}
}
void VectorMapView::renderAnnotationLayers()
{
	if (m_fechting_data)
		return;
	m_style_render.setCanvas(global_test_canvas);

	int sum = m_layer_mapframe_indexs.size();
	int index = sum - 1;
	for(; index >= 0; index --)	//render all the point layers
	{
		AnnotationLayer* a_layer= m_map_frame.m_annotation_layers[m_layer_mapframe_indexs[index]];
		if(a_layer->m_layer_type != FeatureLayer::t_POINT)
			break;

		if(!a_layer->m_need_render)
			continue;
		m_annotation_layer_render.renderAnnotationLayer(m_style_render,*a_layer);
	}

	for(int i =0; i <= index; i++) //render all the line layers, from high-grade to low-grade
	{
		AnnotationLayer* a_layer= m_map_frame.m_annotation_layers[m_layer_mapframe_indexs[i]];

		if(!a_layer->m_need_render)
			continue;

		m_annotation_layer_render.renderAnnotationLayer(m_style_render,*a_layer);
	}
}
void VectorMapView::renderHost()
{
	if (m_is_render_host)
	{
		if(m_host.d0 !=0 && m_host.d1 != 0)
		{
			m_transformer.setTansformPara(m_view_scope,0,m_screen_pix_width,m_screen_pix_height);
			if (m_is_rastermap)
				m_transformer.setTansformPara(m_scale_width,m_scale_height);

			long x,y;
			if(m_transformer.transfromMapToScr(m_host.d0,m_host.d1,&x,&y)>=0)
				if (m_host_image!=NULL)
				{
					//now host_image is a pointer
					//ImageStyleRender::drawImageCenterXY(m_host_image,x,y);
					m_host_button->setCenter(MGPointMake(x,y));
				}
		}
	}
}

void VectorMapView::renderDest()
{
	if (m_is_render_dest)
	{
		if(m_dest.d0 !=0 && m_dest.d1 != 0)
		{
			m_transformer.setTansformPara(m_view_scope,0,m_screen_pix_width,m_screen_pix_height);
			if (m_is_rastermap)
				m_transformer.setTansformPara(m_scale_width,m_scale_height);

			long x,y;
			if(m_transformer.transfromMapToScr(m_dest.d0,m_dest.d1,&x,&y)>=0)
				if (m_dest_image!=NULL)
				{
					//now dest image is a pointer
					ImageStyleRender::drawImageCenterXY(m_dest_image,x,y-m_dest_image->getHeight()/2);
				}
		}
	}
}

void VectorMapView::renderCurPos()
{
	if(m_cur_pos.d0 !=0 && m_cur_pos.d1 != 0)
	{
		m_transformer.setTansformPara(m_view_scope,0,m_screen_pix_width,m_screen_pix_height);
		if (m_is_rastermap)
			m_transformer.setTansformPara(m_scale_width,m_scale_height);

		long x,y;
		if(m_transformer.transfromMapToScr(m_cur_pos.d0,m_cur_pos.d1,&x,&y)>=0)
			ImageStyleRender::drawImageCenterXY(m_cur_image,x,y);
	}
}
void VectorMapView::renderPath()
{
	if (m_draw_path && m_path_points.size() > 0)
	{
		m_trans_scope.setScope(m_view_scope);
		m_transformer.setTansformPara(m_view_scope,0,m_screen_pix_width,m_screen_pix_height);
		if (m_is_rastermap)
			m_transformer.setTansformPara(m_scale_width,m_scale_height);


		int path_seg = m_path_points.size();
		for (int i = 0 ;i < path_seg; i++)
		{
			int sum = m_path_points[i].size();
			MGPoint* m_gm_buff = new MGPoint[sum];
			m_transformer.transformPos2DsToArray(m_path_points[i],m_gm_buff);

			vector<MGPoint> mg_points;
			MGPoint tmp_point;
			for(int j=0; j < sum; j++)
			{
				tmp_point.x = m_gm_buff[j].x;
				tmp_point.y = m_gm_buff[j].y;
				mg_points.push_back(tmp_point);
			}
			MGGraphicEngine *engine=MGGraphicEngine::sharedGraphicEngine();
			engine->drawPolyLines(mg_points,m_path_line_render.getLineColor(),m_path_line_render.m_pixel_width);//by GRF
			//m_path_line_render.drawPolyline(global_test_canvas,mg_points);
			delete[] m_gm_buff;
		}
	}
}

void VectorMapView::drawself()
{
//	LogStart();
	MGGraphicEngine *engine=MGGraphicEngine::sharedGraphicEngine();
	//engine->enableClip(parent()->getBounds());
	MIView::drawself();
	if (m_need_redraw)
	{
		MGLogD("global_test_canvas redraw");
		PolygonStyleRender::fillRect(global_test_canvas,0,0,m_bounds.width,m_bounds.height,RGBAColorRef::darkGray());
		renderFeatureLayers();
	}
//	LogEnd("fill in memory");
//	LogStart();
	if (global_test_canvas)
	{
		global_test_canvas->draw(m_need_redraw);
	}
//	LogEnd("draw in screen");
	m_need_redraw =false;
//	LogStart();
	renderPath();
	renderPointFeatureLayers();

	renderAnnotationLayers();

	renderHost();
	renderDest();
	renderSrc();
	renderPos();
	renderCurPos();
	renderGPSHost();
//	LogEnd("draw the rest part");
	//engine->disableClip();
}
void VectorMapView::initWithFrame( z11::MGRect frame )
{
	_instance=this;
	MIView::initWithFrame(frame);
	//global_test_canvas = new MGCanvas(frame.width,frame.height);
	//global_test_canvas->clipRect(0,0,frame.width/2,frame.height/2);

	m_host_button=new MIButton();
	m_host_button->initWithFrame(MGRectMake(-100,-100,32,32));
	m_host_button->setBackgroundColor(MGColor::fromImage(m_host_image));
	addSubview(m_host_button);
}
//触摸事件的响应 
MIEventHandleState VectorMapView::touchesBegan(const std::set<MITouch*>& touches,
										 const MIEvent* const event)
{
	MIControl::touchesBegan(touches, event);
	MIEventHandleState ret = MIEventHandleStatePass;

	MGPoint tmp_point;
	this->endPoint(touches,tmp_point);
	// 	g_navi->leftMouseDown(tmp_point.x, tmp_point.y);


	//获得起始中心点
	this->centraPoint(touches, m_pre_centra_point);

	//获得起始等效半径
	float tmp_dis = 0;
	std::set<MITouch*>::const_iterator touch_it;
	for (touch_it = touches.begin(); touch_it != touches.end(); ++touch_it) {
		tmp_dis += sqrtf((float)pow((m_pre_centra_point.x - (*touch_it)->point.x) , 2) + (float)pow((m_pre_centra_point.y - (*touch_it)->point.y), 2));
	}
	m_pre_radius = tmp_dis / touches.size();

	return ret;

}
MIEventHandleState VectorMapView::touchesMoved(const std::set<MITouch*>& touches,
										 const MIEvent* const event)
{
	MIControl::touchesMoved(touches, event);
	MIEventHandleState ret = MIEventHandleStatePass;
	MGPoint cur_centra_point;
	this->centraPoint(touches, cur_centra_point);
// 	MapOpration::moveXY(this,m_pre_centra_point.x - cur_centra_point.x,cur_centra_point.y - m_pre_centra_point.y);
	m_pre_centra_point = cur_centra_point;
	return ret;
}
MIEventHandleState VectorMapView::touchesEnded(const std::set<MITouch*>& touches,
										 const MIEvent* const event)
{
	MIControl::touchesEnded(touches, event);
	MIEventHandleState ret = MIEventHandleStatePass;

	//set dest
	MGPoint dest_point;
	this->endPoint(touches,dest_point);

	std::set<MITouch*>::const_iterator touch_it;		
	MGPoint tmp_point;
	this->centraPoint(touches, tmp_point);
	float tmp_dis = 0;
	float tmp_radius = 0;
	for (touch_it = touches.begin(); touch_it != touches.end(); ++touch_it) {
		tmp_dis += sqrtf((float)pow((tmp_point.x - (*touch_it)->point.x) , 2) + (float)pow((tmp_point.y - (*touch_it)->point.y), 2));
	}
	tmp_radius = tmp_dis / touches.size();

	if (m_pre_radius < 10e-5)
	{
		//只有一个touch
		m_map_curr_scaling = 1;
	}
	else
	{
		m_map_curr_scaling = tmp_radius / m_pre_radius;
	}
// 	if (m_pre_radius > tmp_radius)
// 	{
// 		//zoom in
// 		EventsMapIterator it = events_dispatch_map.find(MIMapEventZoomIn);
// 		if (it == events_dispatch_map.end()) {
// 			ret = MIEventHandleStateNormal;
// 		}else{
// // 			MapOpration::ZoomIn(this);
// 			//  			mapZoomIn();
// 			assert(it->second.view_controller != NULL);
// 			assert(it->second.event_func != NULL);
// 			EventFunc func = it->second.event_func;
// 			((it->second.view_controller)->*func)(this);
// 		}
// 	}
// 
// 	if (m_pre_radius < tmp_radius)
// 	{
// 		//zoom out
// 		EventsMapIterator it = events_dispatch_map.find(MIMapEventZoomOut);
// 		if (it == events_dispatch_map.end()) {
// 			ret = MIEventHandleStateNormal;
// 		}else{
// // 			MapOpration::ZoomOut(this);
// 			//  			mapZoomOut();
// 			assert(it->second.view_controller != NULL);
// 			assert(it->second.event_func != NULL);
// 			EventFunc func = it->second.event_func;
// 			((it->second.view_controller)->*func)(this);
// 		}
// 	}
	return ret;
}
MIEventHandleState VectorMapView::touchesCancelled(const std::set<MITouch*>& touches,
											 const MIEvent* const event)
{
	MIEventHandleState ret = MIControl::touchesCancelled(touches, event);
	m_curr_touches.clear();
	return ret;
}
//事件响应   
MIEventHandleState VectorMapView::handleEvent(const MIEvent* const event)
{
	MIEventHandleState ret = MIControl::handleEvent(event);
	return ret;
}

//获得touches的几何中心
void VectorMapView::centraPoint(const std::set<MITouch*>& touches, MGPoint &point)
{
	std::set<MITouch*>::const_iterator touch_it;
	point.x = point.y = 0;
	for (touch_it = touches.begin(); touch_it != touches.end(); ++touch_it) {
		point.x += (*touch_it)->point.x;
		point.y += (*touch_it)->point.y;
	}
	point.x = point.x / touches.size();
	point.y = point.y / touches.size();
}
//get the last point
void VectorMapView::endPoint(const std::set<MITouch*>& touches, MGPoint &point)
{
	std::set<MITouch*>::const_iterator touch_it;
	point.x = point.y = 0;
	for (touch_it = touches.begin(); touch_it != touches.end(); ++touch_it) {
		point.x = (*touch_it)->point.x;
		point.y = (*touch_it)->point.y;
	}
}
//获得touches的等效半径 
void VectorMapView::getRadiusFromTouches(const std::set<MITouch*>& touches, float &radius)
{
	radius = 0.0f;
	if (touches.size() == 0)
		return;
	std::set<MITouch*>::const_iterator touch_it;
	for (touch_it = touches.begin(); touch_it != touches.end(); ++touch_it) {
		assert((*touch_it) != NULL);
		radius += sqrtf((float)pow((m_pre_centra_point.x - (*touch_it)->point.x) , 2) + (float)pow((m_pre_centra_point.y - (*touch_it)->point.y), 2));
	}
	radius = radius / touches.size();
}

void VectorMapView::roadSearch(GIS_Embedded::Pos2DEx host,GIS_Embedded::Pos2DEx dest)
{
	Navigation* navi = Navigation::getInstance();
	navi->searchShortestPath(host,dest);
	navi->getPathPoints(m_path_points);
	m_draw_path = true;
}
void VectorMapView::roadSearch()
{
    Navigation* navi = Navigation::getInstance();
	if (m_host.isZero())
	{
		navi->searchShortestPath(m_src,m_dest);
	}
	else
	{
		navi->searchShortestPath(m_host,m_dest);

	}
	navi->getPathPoints(m_path_points);
	m_draw_path = true;
}

void VectorMapView::roadReturn()
{
}

void VectorMapView::startNavi(MGLocation &gps)
{
	m_is_navi = true;
	GlobalCrt crt;
	crt.geoToPrj(gps.longitude,gps.latitude,&m_gps_host.d0,&m_gps_host.d1);

	m_host = m_gps_host;

// 	MapOpration::Locater(this,m_host.d0,m_host.d1);
	Navigation* navi = Navigation::getInstance();
	navi->startNavigation(gps,m_dest);
}

void VectorMapView::startSimuNavi()
{
	m_is_navi = true;
// 	m_host = m_path_points[0].at(0);
// 	MapOpration::Locater(this,m_host.d0,m_host.d1);
	Navigation* navi = Navigation::getInstance();
	navi->startNaviSimulate(m_dest);
}

void VectorMapView::stopNavi()
{
	m_is_navi = false;
	m_draw_path = false;
	Navigation* navi = Navigation::getInstance();
	navi->endNavigation();
	//MapOpration::Locater(this,m_host.d0,m_host.d1);
}


void VectorMapView::setHost(GIS_Embedded::Pos2DEx hostPos)
{
	m_host = hostPos;

	m_host_button->setRectRotate(MGRectRotateMake(0.0f, 0.0f, 0.0f, 0.0f));
	m_host_button->setRectScale(MGRectScaleMake(0.9f, 0.9f, 0.9f));
	MIView::beginAnimation("host button");
	MIView::setAnimationDuration(1);
	MIView::setAnimationRepeatCount(-1);
	MIView::setAnimationCurve(MIViewAnimationOptionCurveInertia);
	m_host_button->setRectScale(MGRectScaleMake(1.1f, 1.1f, 1.1f));
	//m_host_button->setRectRotate(MGRectRotateMake(0.0f, 0.0f, 360.0f));
	MIView::commitAnimation();
}
void VectorMapView::setDest(float dest_x, float dest_y)
{
	m_dest.d0 = dest_x;
	m_dest.d1 = dest_y;
}
void VectorMapView::setDest(GIS_Embedded::Pos2DEx destPos)
{
	m_dest = destPos;
}



VectorMapView *VectorMapView::_instance=NULL;




void GIS_Embedded::VectorMapView::renderPos()
{
	for (int i=0; i<m_poses.size(); i++)
	{
		Pos2DEx tmp_pos = m_poses[i];
		if(tmp_pos.d0 !=0 && tmp_pos.d1 != 0)
		{
			m_transformer.setTansformPara(m_view_scope,0,m_screen_pix_width,m_screen_pix_height);
			if (m_is_rastermap)
				m_transformer.setTansformPara(m_scale_width,m_scale_height);

			long x,y;
			if(m_transformer.transfromMapToScr(tmp_pos.d0,tmp_pos.d1,&x,&y)>=0)
				ImageStyleRender::drawImageCenterXY(m_poses_images[i],x,y);
		}
	}
}

void GIS_Embedded::VectorMapView::setPos( vector<Pos2DEx>& poses,vector<string>& images )
{
	m_poses.clear();
	m_poses.insert(m_poses.end(),poses.begin(),poses.end());
	for (int i=0; i<images.size();i++)
	{
		z11::MGImage* tmp_image = z11::MGImage::loadImageFromFile(images[i]);
		m_poses_images.push_back(tmp_image);
	}
}

void GIS_Embedded::VectorMapView::setHostImage( string host_image )
{
	m_host_image = z11::MGImage::loadImageFromFile(MGBundle::mainBundle()->pathWithFileName(host_image));
}

void GIS_Embedded::VectorMapView::setDestImage( string dest_image )
{
	m_dest_image = z11::MGImage::loadImageFromFile(MGBundle::mainBundle()->pathWithFileName(dest_image));
}

void GIS_Embedded::VectorMapView::setGPSHostImage( string gps_image )
{
	m_gps_host_image = z11::MGImage::loadImageFromFile(MGBundle::mainBundle()->pathWithFileName(gps_image));
}

void GIS_Embedded::VectorMapView::setGPSHost( GIS_Embedded::Pos2DEx gpsPos )
{
	m_gps_host = gpsPos;
}

void GIS_Embedded::VectorMapView::renderGPSHost()
{
	if(m_gps_host.d0 !=0 && m_gps_host.d1 != 0)
	{
		m_transformer.setTansformPara(m_view_scope,0,m_screen_pix_width,m_screen_pix_height);
		if (m_is_rastermap)
			m_transformer.setTansformPara(m_scale_width,m_scale_height);

		long x,y;
		if(m_transformer.transfromMapToScr(m_gps_host.d0,m_gps_host.d1,&x,&y)>=0)
			ImageStyleRender::drawImageCenterXY(m_gps_host_image,x,y);
	}
}

void GIS_Embedded::VectorMapView::setCurPosImage( string curpos_image )
{
	m_cur_image = z11::MGImage::loadImageFromFile(MGBundle::mainBundle()->pathWithFileName(curpos_image));
}

void GIS_Embedded::VectorMapView::renderPointFeatureLayers()
{
	if (m_fechting_data)
		return;
	m_style_render.setCanvas(global_test_canvas);
	m_trans_scope.setScope(m_view_scope);
	m_transformer.setTansformPara(m_view_scope,0,m_screen_pix_width,m_screen_pix_height);
	if (m_is_rastermap)
		m_transformer.setTansformPara(m_scale_width,m_scale_height);

	m_screen_pix_map.clear();

	int i,sum = m_layer_global_ids.size();
	for (i=sum-1;i>=0;i--) //draw point layer
	{
		FeatureLayer* f_layer=m_map_frame.m_feature_layers[m_layer_mapframe_indexs[i]];
		AnnotationLayer* a_layer= m_map_frame.m_annotation_layers[m_layer_mapframe_indexs[i]];
		
		if(a_layer->m_layer_type != FeatureLayer::t_POINT)
		{
			break;
		}
		a_layer->clearGeoLabelPos();
		m_feature_layer_render.renderMapLayer(m_style_render,*f_layer,m_shapes[i],*a_layer,m_screen_pix_map); //GRF 12-3-29
	}
}

void GIS_Embedded::VectorMapView::setSrc( GIS_Embedded::Pos2DEx srcPos )
{
	m_src = srcPos;
}

void GIS_Embedded::VectorMapView::renderSrc()
{
	if (m_is_render_src)
	{
		if(!m_src.isZero())
		{
			m_transformer.setTansformPara(m_view_scope,0,m_screen_pix_width,m_screen_pix_height);
			if (m_is_rastermap)
				m_transformer.setTansformPara(m_scale_width,m_scale_height);

			long x,y;
			if(m_transformer.transfromMapToScr(m_src.d0,m_src.d1,&x,&y)>=0)
				if (m_src_image!=NULL)
				{
					//now dest image is a pointer
					ImageStyleRender::drawImageCenterXY(m_src_image,x,y-m_src_image->getHeight()/2);
				}
		}
	}
}