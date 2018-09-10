#include "MapView.h"
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
#include "System/Device/MGDevice.h"

#include "GUI/GestureRecognizer/MIPanGestureRecognizer.h"
#include "GUI/GestureRecognizer/MIPinchGestureRecognizer.h"
using namespace Embedded;
using namespace GIS_Embedded;
using namespace z11;

static bool bScaled=false;
static uint_32 adaptSize(uint_32 size)
{
	return size;
	uint_32 ret=1;
	size*=1.3;
	//size<<=1;
	size--;
	while (size!=0)
	{
		ret<<=1;
		size>>=1;
	}
	ret = MAXVALUE(ret,1024);
	return ret;
}
MapView::MapView()
{
	double width = adaptSize(MGDevice::getScreenWidth())*1.0f;
	double height = adaptSize(MGDevice::getScreenHeight())*1.0f;

	m_gps_host.d0 = 0;
	m_gps_host.d1 = 0;
	m_is_moved = false;
	m_is_from_gps = true;

	DataFetcher *data_fetcher = DataFetcher::getInstance();//init some setting of DataFetcher,read m_map_config,m_setting,m_languages,m_current_language
	Settings* setConf;
	data_fetcher->getSettings(&setConf);				//return  DataFetcher->m_setting
	TerrCtrl *terr_ctrl;
	data_fetcher->getTerrsConf(&terr_ctrl);				//reading GIS_Embedded/conf/TerrsConf.tc
	MapConfigType *map_config;
	data_fetcher->getMapConfig(&map_config);			//return DataFetcher->m_map_config 
	m_is_real_navi = false;
	m_is_simu_navi = false;
	m_scale_rank = 100;
	//m_is_rastermap = map_config->raster_map;
	m_is_vectormap = map_config->vector_map;
	m_raster_name = map_config->raster_file;
	std::string file_name = MGBundle::mainBundle()->pathWithFileName("GIS_Embedded/image/user/HOST_0.png");
	m_host_image = z11::MGImage::loadImageFromFile(file_name);
	GlobalCrt crt;
	//long geoX_start,geoX_end,geoY_start,geoY_end;

	Scope view_scope;
	double new_width = map_config->initial_scale*width/(double)map_config->scale_pixel_width;
	double new_height = new_width*height/width;

	double view_h_center= setConf->map.host_pos_y;
	double view_w_center= setConf->map.host_pos_x;
	terr_ctrl->initialScopeCenter(setConf->map.host_pos_x,setConf->map.host_pos_y,&view_w_center,&view_h_center);

	view_scope.setBottom(view_h_center - new_height/2);
	view_scope.setTop(view_h_center + new_height/2);
	view_scope.setLeft(view_w_center - new_width/2);
	view_scope.setRight(view_w_center + new_width/2);

	setScreenPix(width,height);
	setViewScope(view_scope);
	setScaleRank(map_config->initial_scale);

	m_vector_map = new VectorMapView(false);//ÁÙÊ±£¬ÐèÒª¼ÓÈëÆäËû³ÉÔ±±äÁ¿À´¿ØÖÆÊÇ·ñ»æÖÆÎÀÐÇµØÍ¼
	MapTerrCtrl *map_terr_ctrl = new MapTerrCtrl(setConf,*terr_ctrl);
	MapFrame *map_frame = getMapFrame();
	map_terr_ctrl->change(view_scope,map_frame);
	MapOpration::init(map_config->scale_pixel_width,map_config->scale_ranks,*map_terr_ctrl);
	RenderFlagAdjuster::adjust(*map_frame,map_config->initial_scale);
	m_vector_map->setViewScope(view_scope);
	m_vector_map->getLayerDataInScope();

	m_mapview_delegate = NULL;

	m_pre_radius=0;
    
    MapOpration::SCALE_CURRENT=map_config->initial_scale;
}

MapView::~MapView()
{
	m_map_frame.clear();
	_instance=NULL;

	if (m_mapview_delegate!=NULL)
	{
//		m_mapview_delegate->release();
	}

}
void MapView::setScreenPix(int width,int height)
{
	m_screen_pix_width = width;
	m_screen_pix_height = height;
	m_screen_pix_map.setScreenSize(width,height,10,10);	
}


void MapView::drawself()
{
	MIView::drawself();
	MGGraphicEngine *engine=MGGraphicEngine::sharedGraphicEngine();
	engine->drawFilledRect(MGRectMake(0,0,m_screen_pix_width,m_screen_pix_height),RGBAColorRef::darkGray());
	//PolygonStyleRender::fillRect(0,0,m_screen_pix_width,m_screen_pix_height,RGBAColorRef::darkGray());//m_map_view->getMapFrame()->m_bg_color
}
void MapView::initWithFrame( z11::MGRect frame )
{
	_instance=this;
	MIView::initWithFrame(frame);

	uint_32 screenWidth=m_screen_pix_width;
	uint_32 screenHeight=m_screen_pix_height;
	float scale_width = screenWidth / 480.0;
	float scale_height = screenHeight / 800.0;

	//m_hint = new MILabel();
	//m_hint->initWithFrame(MGRectMake(screenWidth-140*scale_width,0,140,60*scale_height));
	//m_hint->setBackgroundColor(z11::MGColor::fromRGBA(0,0,0,0));
	//addSubview(m_hint);

 
//50.0f*scale_height
	//m_vector_map = new VectorMapView(false);//ÁÙÊ±£¬ÐèÒª¼ÓÈëÆäËû³ÉÔ±±äÁ¿À´¿ØÖÆÊÇ·ñ»æÖÆÎÀÐÇµØÍ¼
	m_vector_map->setScreenPix(m_screen_pix_width,m_screen_pix_height);
	m_vector_map->setViewScope(m_view_scope);
	m_vector_map->setScaleRank(m_scale_rank);
	m_vector_map->setScale(m_scale_width,m_scale_height);
	m_vector_map->initWithFrame(MGRectMake(0.0f, 0.0f, screenWidth*1.0f, screenHeight*1.0f));//-50*scale_height
	m_vector_map->setBackgroundColor(z11::MGColor::fromRGBA(0,0,0,0));
	addSubview(m_vector_map);
	centerSubView(m_vector_map);

 	EventHandler handler;
 	handler.view_controller = delegate;
	
	m_pan_recognizer=new MIPanGestureRecognizer();
	m_pinch_recognizer=new MIPinchGestureRecognizer();
	SELHandler SEL_handler;
	SEL_handler.sender=m_pan_recognizer;
	SEL_handler.sel_func=(SELFunc)&MapView::panMoveCall;
	m_pan_recognizer->initWithTarget(this,SEL_handler);
	addGestureRecognizer(m_pan_recognizer);

	SEL_handler.sender=m_pinch_recognizer;
	SEL_handler.sel_func=(SELFunc)&MapView::scaleCall;
	m_pinch_recognizer->initWithTarget(this,SEL_handler);
	addGestureRecognizer(m_pinch_recognizer);

	m_pinch_recognizer->requireGestureRecognizerToFail(m_pan_recognizer);
}
//´¥ÃþÊÂ¼þµÄÏìÓ¦ 
MIEventHandleState MapView::touchesBegan(const std::set<MITouch*>& touches,
											   const MIEvent* const event)
{
	MIControl::touchesBegan(touches, event);
	MIEventHandleState ret = MIEventHandleStatePass;

	m_is_moved = false;

	MGPoint tmp_point;
	this->endPoint(touches,tmp_point);
// 	g_navi->leftMouseDown(tmp_point.x, tmp_point.y);


	//»ñµÃÆðÊ¼ÖÐÐÄµã
	this->centraPoint(touches, m_pre_centra_point);

	//»ñµÃÆðÊ¼µÈÐ§°ë¾¶
	float tmp_dis = 0;
	std::set<MITouch*>::const_iterator touch_it;
	for (touch_it = touches.begin(); touch_it != touches.end(); ++touch_it) {
		tmp_dis += sqrtf((float)pow((m_pre_centra_point.x - (*touch_it)->point.x) , 2) + (float)pow((m_pre_centra_point.y - (*touch_it)->point.y), 2));
	}
	m_pre_radius = tmp_dis / touches.size();

	return ret;

}
MIEventHandleState MapView::touchesMoved(const std::set<MITouch*>& touches,
											   const MIEvent* const event)
{
	MIControl::touchesMoved(touches, event);
	MIEventHandleState ret = MIEventHandleStatePass;

	if (m_is_real_navi || m_is_simu_navi)
		return ret;

	m_is_moved = true;


	{
		MGPoint cur_centra_point;
		this->centraPoint(touches, cur_centra_point);
		//m_transformer.setTansformPara(m_view_scope,0,m_screen_pix_width,m_screen_pix_height);

		//MGRectTransition trans=m_vector_map->getRectTransition();
		//trans.axis_x+=(cur_centra_point.x-m_pre_centra_point.x);
		//trans.axis_y+=(cur_centra_point.y-m_pre_centra_point.y);
		//float dist_width = ( m_bounds.width - m_vector_map->getBounds().width ) ; //may be positive ??
		//float dist_height = ( m_bounds.height - m_vector_map->getBounds().height ) ;
		//assert(dist_width<0.000005);
		//assert(dist_height<0.000005);
		//if (trans.axis_x<0 && trans.axis_x>dist_width && trans.axis_y<0 &&trans.axis_y>dist_height )
		//{
		//	m_vector_map->setRectTransition(trans);
		//	//MGLogD("cur_centra_point = %f,%f",cur_centra_point.x,cur_centra_point.y);
		//}
		//else
		//{
		//	int_32 map_x,map_y;
		//	MGPoint center_point = MGPointMake(m_vector_map->getBounds().width/2,m_vector_map->getBounds().height/2);
		//	m_transformer.translateFromScrDisToMap(center_point.x+trans.axis_x-dist_width/2,center_point.x,center_point.y + trans.axis_y-dist_height/2, center_point.y,&map_x,&map_y);
		//	MapOpration::moveXY(this,map_x,map_y);
		//	MGLogD("MapOpration::moveXY");

		//}
		m_pre_centra_point = cur_centra_point;
		if (m_mapview_delegate!=NULL)
		{
			m_mapview_delegate->afterMapMoved(this,m_pre_centra_point,cur_centra_point);

		}
	}

	return ret;
}
MIEventHandleState MapView::touchesEnded(const std::set<MITouch*>& touches,
											   const MIEvent* const event)
{
	MIControl::touchesEnded(touches, event);
	MIEventHandleState ret = MIEventHandleStatePass;

	//set dest
	MGPoint dest_point,prjPos;
	this->endPoint(touches,dest_point);
	
	dest_point = transVectorMapPoint(dest_point);//trans point,cause VectorMapView.bounds>MapView.bounds ,by GRF

	m_transformer.setTansformPara(m_view_scope,0,m_screen_pix_width,m_screen_pix_height);


	int_32 map_x,map_y;
	m_transformer.translateFromScrToMap(dest_point.x, dest_point.y,&map_x,&map_y);
	m_last_pos.d0 = map_x;
	m_last_pos.d1 = map_y;

	//don't every time to set ,should be set by GPS,GRF
	if(!m_is_moved)
		m_vector_map->setCurPos(m_last_pos);


	{
		MGRectTransition trans=m_vector_map->getRectTransition();
		float dist_width = ( m_bounds.width - m_vector_map->getBounds().width ) ; //may be positive ??
		float dist_height = ( m_bounds.height - m_vector_map->getBounds().height ) ;
		assert(dist_width<0.000005);
		assert(dist_height<0.000005);

			if (trans.axis_x<0.005 && trans.axis_x>dist_width && trans.axis_y<0.005 &&trans.axis_y>dist_height )
			{
			}
			else
			{
				int_32 map_x,map_y;
                MGRectScale rect_scale=m_vector_map->getRectScale();
				MGPoint center_point = MGPointMake(m_vector_map->getBounds().width/2,m_vector_map->getBounds().height/2);
				m_transformer.translateFromScrDisToMap(center_point.x+trans.axis_x/rect_scale.axis_x-dist_width/2,center_point.x,center_point.y + trans.axis_y/rect_scale.axis_y-dist_height/2, center_point.y,&map_x,&map_y);
				MapOpration::moveXY(this,map_x,map_y);
                m_vector_map->setRectTransition(MGRectTransitionMake(0,0,0));
				MGLogD("MapOpration::moveXY");

			}

	}
	do
	{
		MIPinchGestureRecognizer *recognizer=m_pinch_recognizer;
		float scale_value = recognizer->getScale();
        static bool flag=true;
		if (scale_value<1)
		{
//            if (flag) {
//                flag=false;
//            }
//            else {
//                break;
//            }
			
            scale_value *= mapZoomOut();
			if(scale_value < 1e-4)
				break;
			//recognizer->setScale(scale_value);
			this->setRectTransition(MGRectTransitionMake(m_transition.axis_x / scale_value, m_transition.axis_y / scale_value, 0));
			this->setScale(MGRectScaleMake(scale_value,scale_value,1));
			MGLogD("mapZoomOut scale =%f",scale_value);
			//mapZoomOut();
		}
		else if (scale_value<2.5)
		{
			//this->setScale(MGRectScaleMake(scale_value,scale_value,1));
		}
		else
		{
			scale_value *= mapZoomIn();
			if(scale_value < 1e-4)
				break;
			//recognizer->setScale(scale_value);
			//recognizer->setScale2(1);
			this->setRectTransition(MGRectTransitionMake(m_transition.axis_x / scale_value, m_transition.axis_y / scale_value, 0));
			this->setScale(MGRectScaleMake(scale_value, scale_value, 1));
			MGLogD("mapZoomIn scale =%f",scale_value);
		}
	}while(0);
    MapOpration::fetchData(this);

//	std::set<MITouch*>::const_iterator touch_it;		
//	MGPoint tmp_point;
//	this->centraPoint(touches, tmp_point);
//	float tmp_dis = 0;
//	float tmp_radius = 0;
//	for (touch_it = touches.begin(); touch_it != touches.end(); ++touch_it) {
//		tmp_dis += sqrtf((float)pow((tmp_point.x - (*touch_it)->point.x) , 2) + (float)pow((tmp_point.y - (*touch_it)->point.y), 2));
//	}
//	tmp_radius = tmp_dis / touches.size();
//
//	if (m_pre_radius < 10e-5)
//	{
//		//Ö»ÓÐÒ»¸ötouch
//		m_map_curr_scaling = 1;
//	}
//	else
//	{
//		m_map_curr_scaling = tmp_radius / m_pre_radius;
//	}
//	if (m_pre_radius > tmp_radius)
//	{
//		//zoom in
//		EventsMapIterator it = events_dispatch_map.find(MIMapEventZoomIn);
//		if (it == events_dispatch_map.end()) {
//			ret = MIEventHandleStateNormal;
//		}else{
//// 			MapOpration::ZoomIn(this);
////  			mapZoomIn();
//			assert(it->second.view_controller != NULL);
//			assert(it->second.event_func != NULL);
//			EventFunc func = it->second.event_func;
//			((it->second.view_controller)->*func)(this);
//		}
//	}
//
//	if (m_pre_radius < tmp_radius)
//	{
//		//zoom out
//		EventsMapIterator it = events_dispatch_map.find(MIMapEventZoomOut);
//		if (it == events_dispatch_map.end()) {
//			ret = MIEventHandleStateNormal;
//		}else{
//// 			MapOpration::ZoomOut(this);
////  			mapZoomOut();
//			assert(it->second.view_controller != NULL);
//			assert(it->second.event_func != NULL);
//			EventFunc func = it->second.event_func;
//			((it->second.view_controller)->*func)(this);
//		}
//	}

	if (m_mapview_delegate != NULL && !m_is_moved)
		m_mapview_delegate->mapClick(this,m_last_pos);

	return ret;
}
MIEventHandleState MapView::touchesCancelled(const std::set<MITouch*>& touches,
												   const MIEvent* const event)
{
	MIEventHandleState ret = MIControl::touchesCancelled(touches, event);
	m_curr_touches.clear();
	return ret;
}
//ÊÂ¼þÏìÓ¦   
MIEventHandleState MapView::handleEvent(const MIEvent* const event)
{
	MIEventHandleState ret = MIControl::handleEvent(event);
	return ret;
}

//»ñµÃtouchesµÄ¼¸ºÎÖÐÐÄ
void MapView::centraPoint(const std::set<MITouch*>& touches, MGPoint &point)
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
void MapView::endPoint(const std::set<MITouch*>& touches, MGPoint &point)
{
	std::set<MITouch*>::const_iterator touch_it;
	point.x = point.y = 0;
	for (touch_it = touches.begin(); touch_it != touches.end(); ++touch_it) {
		point.x = (*touch_it)->point.x;
		point.y = (*touch_it)->point.y;
	}
}
//»ñµÃtouchesµÄµÈÐ§°ë¾¶ 
void MapView::getRadiusFromTouches(const std::set<MITouch*>& touches, float &radius)
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
float MapView::mapZoomOut()
{
	return MapOpration::ZoomOut(this);
}
float MapView::mapZoomIn()
{
	return MapOpration::ZoomIn(this);
}
void MapView::locater()
{
	MapOpration::Locater(this,m_host.d0,m_host.d1);
}

MIMapViewResult MapView::roadSearch()
{
	if(m_host.isZero()&&m_src.isZero())
		return MIMapViewHostPositionInvalidError;
	if(m_dest.isZero())
		return MIMapViewDestPositionInvalidError;
    m_vector_map->roadSearch();
	//m_vector_map->roadSearch(m_host,m_dest);
	return MIMapViewNormalResult;
}

MIMapViewResult MapView::roadGPSSearch()
{
	if(m_dest.isZero())
		return MIMapViewDestPositionInvalidError;
	m_vector_map->roadSearch(m_gps_host,m_dest);
	return MIMapViewNormalResult;
}

void MapView::roadReturn()
{
}

void MapView::startNavi(MGLocation &gps)
{
	roadGPSSearch();
	m_is_real_navi = true;
	GlobalCrt crt;
	crt.geoToPrj(gps.longitude,gps.latitude,&m_host.d0,&m_host.d1);
	MapOpration::Locater(this,m_host.d0,m_host.d1);

	m_vector_map->startNavi(gps);
}

void MapView::startSimuNavi()
{
	m_is_simu_navi = true;
	m_simu_host = m_vector_map->getHost();
 	m_host = m_simu_host;
	MapOpration::Locater(this,m_host.d0,m_host.d1);
	m_vector_map->startSimuNavi();
}

void MapView::stopNavi()
{
	if (m_is_simu_navi)
	{
		stopSimuNavi();
	}
	else
	{
		m_is_real_navi = false;
		m_is_simu_navi = false;
		m_vector_map->stopNavi();
		MapOpration::Locater(this,m_host.d0,m_host.d1);
	}
}
void MapView::setHost()
{
 	m_host.d0 = m_last_pos.d0;
 	m_host.d1 = m_last_pos.d1;
// 	m_raster_image->setHost(m_host.,m_host);
	m_vector_map->setHost(m_host);
}
void MapView::setHost(float dest_x, float dest_y)
{
	m_host.d0 = dest_x;
	m_host.d1 = dest_y;
	m_vector_map->setHost(m_dest);
	// 	m_buttons[ROAD_MAP_SERCH]->show();

}
void MapView::setRasterHost(int_32 host_x,int_32 host_y)
{
	int scr_x= 0, scr_y = 0;
	double geo_x=0,geo_y=0;
	GlobalCrt crt;
	crt.prjToGeo(host_x,host_y,&geo_x,&geo_y);

// 	if(m_raster->prjPosToScrPos(geo_x, geo_y, scr_x, scr_y) < 0)
// 		return;

// 	m_raster->setHostPosition(scr_x, scr_y);
// 	long left,top,right,bottom;
// 	m_raster->getCurrentPrjScope(&left,&top,&right,&bottom);
// 	m_view_scope.setScope(left,top,right,bottom);
// 	m_trans_scope.setScope(m_view_scope);
}

bool MapView::updateHost(MGLocation &location,bool flag)
{
	bool is_navi = false;
	GIS_Embedded::Pos2DEx tmp_pos;
	GlobalCrt crt;
	crt.geoToPrj(location.longitude,location.latitude,&tmp_pos.d0,&tmp_pos.d1);
//flag is used to mark simu or real navi
//if host is setted by user,there are only simulate navigation and host is auto changed;
//else only when host from the gps and the real navi ,m_host  is setted by the gps   //lw
// 	if (m_is_from_gps)
	m_gps_host = tmp_pos;
	m_vector_map->setGPSHost(m_gps_host);

// 	if (m_map_config->raster_map)
// 		setRasterHost(m_host.d0,m_host.d1);
// 
	Navigation* navi = Navigation::getInstance();
	if (m_is_real_navi)
	{
		m_host = tmp_pos;
		if (!navi->naviOnPositionChange(location,m_host,m_dest))
		{
			m_is_real_navi = false;
			if (m_mapview_delegate != NULL)
				m_mapview_delegate->endRealNavi();

			m_vector_map->setDrawPath(false);//end navi don't draw path
		}

		is_navi = m_is_real_navi;
		m_host = navi->getNaviHost();

		m_vector_map->setHost(m_host);
		MapOpration::Locater(this,m_host.d0,m_host.d1 );
		//MapOpration::Locater(this,m_host.d0,m_host.d1 - m_view_scope.getHeight()/4);

	}
	else if (m_is_simu_navi)
	{
		m_host = tmp_pos;
		if (!navi->naviOnPositionChange(location,m_host,m_dest))
		{
			m_host = m_simu_host;
			if (m_mapview_delegate != NULL)
				m_mapview_delegate->endSimuNavi();

			m_is_simu_navi = false;
			m_vector_map->setDrawPath(false);//end navi don't draw path
		}
		else
			m_host = navi->getNaviHost();
		is_navi = m_is_simu_navi;

		m_vector_map->setHost(m_host);
		MapOpration::Locater(this,m_host.d0,m_host.d1 );
		//MapOpration::Locater(this,m_host.d0,m_host.d1 - m_view_scope.getHeight()/4);

	}
	else
		m_host = m_gps_host;


	return is_navi;
}
void MapView::setDest(float dest_x, float dest_y)
{
	m_dest.d0 = dest_x;
	m_dest.d1 = dest_y;
	m_vector_map->setDest(m_dest);
// 	m_buttons[ROAD_MAP_SERCH]->show();

}
void MapView::setDest()
{
	//m_dest.d0 = 13375024;
	//m_dest.d1 = 3520888;
	m_dest = m_last_pos;
	m_vector_map->setDest(m_dest);
	m_vector_map->setCurPos(Pos2DEx());//by GRF
// 	m_buttons[ROAD_MAP_SERCH]->show();

}

void MapView::moveXY(int offset_x,int offset_y)
{
	MapOpration::moveXY(this,offset_x,offset_y);
}

void GIS_Embedded::MapView::getLayerDataInRect( Scope &scope,vector<Pos2DEx>& points,vector<std::string>& names,bool flag )
{
	m_vector_map->getLayerDataInRect(scope,points,names,flag);
}

void GIS_Embedded::MapView::getLayerDataInRect( Scope& scope,vector<int>& fcls_id,vector<Pos2DEx>& points,vector<std::string>& names,bool flag /*= true*/ )
{
	m_vector_map->getLayerDataInRect(scope,fcls_id,points,names,flag);
}

void GIS_Embedded::MapView::locaterPos( GIS_Embedded::Pos2DEx& pos )
{
	MapOpration::Locater(this,pos.d0,pos.d1);
}

void GIS_Embedded::MapView::setMapViewDelegate( MapViewDelegate* delegate )
{
	m_mapview_delegate = delegate;
}


void GIS_Embedded::MapView::setPos( vector<Pos2DEx>& poses,vector<string>& images )
{
	m_vector_map->setPos(poses,images);
}

void GIS_Embedded::MapView::setSrc()
{
	m_src = m_last_pos;
	m_vector_map->setSrc(m_src);
	m_vector_map->setCurPos(Pos2DEx());//by GRF
}

void GIS_Embedded::MapView::stopSimuNavi()
{
	m_host=m_src;
	m_is_real_navi = false;
	m_is_simu_navi = false;
	m_vector_map->stopNavi();
	MapOpration::Locater(this,m_host.d0,m_host.d1);
}

void GIS_Embedded::MapView::centerSubView( MIView *sub_view )
{
	float half_width=(m_bounds.width-sub_view->getBounds().width)/2;
	float half_height=(m_bounds.height-sub_view->getBounds().height)/2;
	sub_view->setRectTransition(MGRectTransitionMake(half_width,half_height,0));
}

z11::MGPoint GIS_Embedded::MapView::transVectorMapPoint( const MGPoint &pt )
{
	MGPoint ret = pt;
	ret.x += (-m_vector_map->getRectTransition().axis_x);
	ret.y += (-m_vector_map->getRectTransition().axis_y);
	//float half_width=(m_bounds.width-m_vector_map->getBounds().width)/2;
	//float half_height=(m_bounds.height-m_vector_map->getBounds().height)/2;
	//ret.x += (m_vector_map->getRectTransition().axis_x-half_width);
	//ret.y += (m_vector_map->getRectTransition().axis_y-half_height);
	return ret;
}
void GIS_Embedded::MapView::scaleCall( z11::MGObject* _sender )
{
	MGLogD("Map scale call");

	MIPinchGestureRecognizer *recognizer=(MIPinchGestureRecognizer *)_sender;
	float scale_value = recognizer->getScale();
    if ((MapOpration::SCALE_CURRENT/scale_value<=MapOpration::SCALE[MapOpration::SCALE_SIZE-1]) &&(MapOpration::SCALE_CURRENT/scale_value>=MapOpration::SCALE[0])){
        this->setScale(MGRectScaleMake(m_scale.axis_x * scale_value, m_scale.axis_y * scale_value,1));
    }
    else {
        //recognizer->setScale(this->m_vector_map->getRectScale().axis_x);
    }
    bScaled=true;
}
void GIS_Embedded::MapView::setScale(const MGRectScale &scale)
{
    m_vector_map->setRectScale(scale);
}
void GIS_Embedded::MapView::panMoveCall( z11::MGObject* _sender )
{
	MGLogD("Map pan move call");
	MIPanGestureRecognizer *recognizer=(MIPanGestureRecognizer *)_sender;

	MGRectTransition trans=m_vector_map->getRectTransition();
    MGPoint _trans = recognizer->translationInView(this);
	trans.axis_x+=_trans.x/getRectScale().axis_x;
	trans.axis_y+=_trans.y/getRectScale().axis_y;

	m_vector_map->setRectTransition(trans);
	//recognizer->setTranslationInView(MGPointMake(0,0),this);
}

int GIS_Embedded::MapView::getScreenPixWidth() const
{
	return m_screen_pix_width;
}

int GIS_Embedded::MapView::getScreenPixHeight() const
{
	return m_screen_pix_height;
}

void GIS_Embedded::MapView::setGPSHostImage( string gps_image )
{
	m_vector_map->setGPSHostImage(gps_image);
}

void GIS_Embedded::MapView::setDestImage( string dest_image )
{
	m_vector_map->setDestImage(dest_image);
}

void GIS_Embedded::MapView::setHostImage( string host_image )
{
	m_vector_map->setHostImage(host_image);
}

void GIS_Embedded::MapView::setCurPosImage( string curpos_image )
{
	m_vector_map->setCurPosImage(curpos_image);
}

void GIS_Embedded::MapView::setCurPos( GIS_Embedded::Pos2DEx& point )
{
	m_vector_map->setCurPos(point);
}
MapView *MapView::_instance=NULL;
