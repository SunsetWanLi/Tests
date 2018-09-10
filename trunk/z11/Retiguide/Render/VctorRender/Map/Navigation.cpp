#include "Navigation.h"
#include "Retiguide/Common/Util/PureMath.h"
#include "Retiguide/Common/String_Tools.h"
#include "Retiguide/DataFetcher/DataFetcher.h"
#include "Retiguide/SpatialMath/SpatialMath.h"

#include "Retiguide/Analysis/Navigation/NaviBind.h"
#include "Retiguide/Analysis/Navigation/NaviPathContext.h"
#include "Retiguide/Analysis/Navigation/NaviHandlerParam.h"
#include "Retiguide/Analysis/Navigation/NaviProcessor.h"
#include "Retiguide/Analysis/AnaEnumCommon.h"

#include "MapKit/Embedded/Application/Pathfinding.h"
#include "MapKit/Embedded/Application/LayerMgr.h"
#include "MapKit/Embedded/BasicType/Line.h"
#include "System/Shared/Utility.h"
#include "System/Shared/MGLog.h"
// in degrees
#define MIN_POS_CHANGE 0.0001
using namespace Embedded;

namespace GIS_Embedded
{
	Navigation* Navigation::m_instance = NULL;
	Syncable* Navigation::m_syn = new Syncable();

	Navigation* Navigation::getInstance()
	{
		Syncable::AutoLock inner_syn(m_syn);
		if(m_instance == NULL)
		{
			m_instance = new Navigation();
		}
		return m_instance;
	}

	Navigation::Navigation()
	{
// 		_current_scale = m_map_config->initial_scale;

		m_bUserCancel = 0;
		_is_real_navi = false;
		_is_simu_navi = false;
		_path_res = NULL;
		_is_cross_map_shown = false;
		m_path_finding = 0;

		_gps_simulant = new GPS_FromLog();
		GPS_Filter::GPS_Filter_Para gf_para;
		m_gps_filter = new GPS_Filter(GPS_Filter::GPSFRM_SPEED_AVERAGE,gf_para);
		_gps_from_route = new GPS_FromRoute();

	}

	Navigation::~Navigation()
	{
		if(_path_res!=NULL)
			delete _path_res;
		_path_res = NULL;
	}

	int Navigation::searchShortestPath(Pos2DEx host_pos,Pos2DEx dest_pos)
	{
		int ret = -1;
		//host_pos.d0 = 13380664 ;
		//host_pos.d1 = 3512787 ;
		//dest_pos.d0 = 13380704 ;
		//dest_pos.d1 = 3513806 ;

		double heading = 0;//m_gps_filter->getLastValidInfo().heading;
		double direction = heading * M_PI / 180;

		std::string file_name = MGBundle::mainBundle()->pathWithFileName("GIS_Embedded/data/AdjacentList");
		z11::MGWString path = Utility::ustr2wstr(file_name);
		Embedded::Pathfinding* pf =  Pathfinding::getInstance();
		if(!pf->isInited())
			pf->init(path);
			
		double dis;
		double line_dis=0;
		bool isFirst = true;
		Embedded::MapPos2D start, end;

 		start.d0 = host_pos.d0;//13380664;
 		start.d1 = host_pos.d1;//;3512787
 		end.d0 = dest_pos.d0;//;13380704
 		end.d1 = dest_pos.d1;//;3513806

		//start.d0 =13380664;//;
		//start.d1 = 3512787;//;3512787
		//end.d0 = 13380704;//;
		//end.d1 = 3513806;//;3513806


		//lw 求距离
		//  		long x1,y1,x2,y2;
		//  		m_transformer.transfromMapToScr(start.d0,start.d1,&x1,&y1);
		//  		m_transformer.transfromMapToScr(end.d0,end.d1,&x2,&y2);
		//  
		//  		long xSpace = abs(x1 - x2);
		//  		long ySpace = abs(y1 - y2);
		//  		long tmp_dis = (long)sqrt(xSpace*xSpace+ySpace*ySpace);
		//  		double straight_dis = m_transformer.transformPixDisToDistance(tmp_dis);

		std::vector<Embedded::Line*> lines;
		std::vector<bool> fromBegins; 
		dis = pf->computeShortestRoute(start, end, &lines, &fromBegins);
		//if (dis)
		//{
		//	return ret;
		//}
		_path_points.clear();
		for (uint_32 j=0; j<lines.size(); j++)
		{
			Embedded::Line* line = lines[j];
			std::vector<Embedded::MapPos2D > points;
			if(line)
			{
				points.clear();
				line->getPoints(&points);
				line_dis += line->getLength();
			}
			vector<Pos2DEx> tmp_points;
			tmp_points.clear();
			for (uint_32 k=0; k<points.size(); k++)
			{
				Pos2DEx tmp_point ;
				tmp_point.d0 = points[k].d0;
				tmp_point.d1 = points[k].d1;
				tmp_points.push_back(tmp_point);
			}
			_path_points.push_back(tmp_points);
		}

		int loc = _path_points[_path_points.size()-1].size() - 1;
		Pos2DEx pos = _path_points[_path_points.size()-1].at(loc);

		if (_path_res != NULL)
		{
			delete _path_res;
			_path_res = NULL;
		}
		uint_32 s2 = sizeof(PathSegment);
		_path_res = new PathResult;
		_path_res->setLines(lines, fromBegins);
		
		ret = 0;
		return ret;
	}

 	int Navigation::startNavigation(MGLocation &gps,Pos2DEx &dest_pos)
 	{
		_is_real_navi = true;
 
  		double direction = gps.course; 
		Pos2DEx host_pos;
		GlobalCrt crt;
		crt.geoToPrj(gps.longitude,gps.latitude,&host_pos.d0,&host_pos.d1);
//  		double distance = calRemainDistance(host_pos, dest_pos); 
 		return 0;
 	}
 
 	void Navigation::startNaviSimulate(Pos2DEx &dest_pos)
 	{
		_is_simu_navi = true;
		Pos2DEx host_pos;
  		if(!_gps_simulant_state.is_device_connected)
  		{
  			_gps_simulant_state.is_device_connected = (0 == _gps_simulant->open("gps_data.txt",ios_base::in|ios_base::out));
  		}
  
  		SimParam sim_param;
  		sim_param.m_speed = 200;
  		sim_param.m_stop_times = 1;
  		sim_param.max_stop_per_line = 3;
		if(_path_res !=NULL)
  			((GPS_FromRoute *)_gps_from_route)->open(*_path_res, sim_param);
  
  		if(!_path_points.empty())
  			host_pos = _path_points[0].at(0);

		

// 		double distance = calRemainDistance(host_pos, dest_pos);   
 	}
 
//  	void Navigation::onTimer()
//  	{
//  		GPS_Info refined_gps_info;
//  		static bool last_from_simu = false;	
// 		_gps_from_route->read(&mGpsInfo);
// 
// 		last_from_simu = true;
//  		//GRF
//  		// zj 
//  		Pos2D posCurrent;
//  		GlobalCrt crt;
//  		crt.geoToPrj(mGpsInfo.longitude,mGpsInfo.latitude,&posCurrent.d0,&posCurrent.d1);
// 
//  	}

 	bool Navigation::naviOnPositionChange(MGLocation &location,Pos2DEx& host_pos,Pos2DEx& dest_pos)
 	{
		bool flag = true;

		static bool bUpate = true;
 		if (_is_simu_navi)
 		{
			if(bUpate)
			_gps_from_route->read(&mGpsInfo);
 		}
 		else
 		{
 			mGpsInfo.longitude = location.longitude;
 			mGpsInfo.latitude = location.latitude;
 			mGpsInfo.heading = location.course;
 			mGpsInfo.speed = location.speed;
 			mGpsInfo.altitude_84 = location.altitude;
 		}
 
  		AnaPosVerdict verdict;
  		NaviAnalyzeParam ana_param;
  		Pos2DEx prj_pos;
  		double to_cross_dis = 0;
  		double direction = 0;
  
  		NavigationInfo  navi_info;
  
  		Pos2DEx bindPos; 
  
  		GlobalCrt crt;
  		crt.geoToPrj(mGpsInfo.longitude, mGpsInfo.latitude,&(prj_pos.d0), &(prj_pos.d1)); 
  		direction = mGpsInfo.heading * M_PI / 180;//.heading * PI / 180;
  
  		Navi_GPS_Info navi_gps_info(mGpsInfo);
  		NaviBind navi_bind;
  		navi_bind.naviBind(navi_gps_info, _path_res, &bindPos, &verdict, & ana_param.m_navi_path_context); //瀵筧na_param.m_navi_path_context杩涜璧嬪�?
   
  		Pos2DEx track_pos = prj_pos;
  		if(verdict != APV_NOT_BOUND)
  			track_pos =/* curve_pos.m_near_pos*/bindPos;
  
  		host_pos = track_pos; // 
		m_navi_host = host_pos;
  
//  		int tmp_dis = calRemainDistance(track_pos, dest_pos);
//   		if(_is_navi_run && _is_to_start_navi )  //start to navi sounder
//   		{
  			//SoundPlayer::startNaviPlay(tmp_dis); 
//   			_is_to_start_navi = false;
//   		}
  
		int path_num = _path_points.size();
		int last_path_points_num = _path_points[path_num-1].size();
		GIS_Embedded::Pos2DEx path_last_point = _path_points[path_num-1][last_path_points_num-1];
   		if(isReach(track_pos,path_last_point/* , _path_points*/))
		{
			flag = false;
   			return flag;
		}
  
  		int cmd = (int)NHCT_NO_HANDLE;
  		switch(verdict)
  		{
  		case APV_NOT_BOUND:
  			break;
  		case APV_DIRECTION_UNSURE:
  		case APV_MAY_BOUND:
  			// TODO: re-route
  			cmd = (int)NHCT_SHOW_CROSS;
  			break;
  		case APV_DETOUR:
  		case APV_DIRECTION_BACKWARD:
  			// re-route
  			cmd = (int)(NHCT_SHOW_CROSS | NHCT_REROUTE);
  			break; // remove 'break' when you want to play speech here
  		case APV_DIRECTION_FORWARD:
  			// speech
  			{
  				cmd = (int)(NHCT_SHOW_CROSS | NHCT_SPEAK);
  				break;
  			}
  		default:
  			break;
  		}
  
  		if(cmd != NHCT_NO_HANDLE)
  		{
  			NaviProcessor * processor = NaviProcessor::getInstance();
  			ana_param.m_gps_info = mGpsInfo;
  			ana_param.m_target_pos = dest_pos;
  			int ret = processor->analyze(ana_param, &navi_info);
  
  			if(ret >= 0)
  			{
  				if(cmd & (int)NHCT_SHOW_CROSS)
  					toShowCross(navi_info,host_pos,direction);
  
  				if(cmd & (int)NHCT_SPEAK)
  				{
   				     NaviActParam act_param(ana_param.m_navi_path_context, navi_info/*, _navi_map,*/ /*_point_host*/);
  					 ret = processor->navigate(cmd, act_param);
  				}
  			}
  		}
  		updateTrack(track_pos,direction,0);//tmp_dis

		return flag;
 	}

	void Navigation::toShowCross(const NavigationInfo &navi_info,Pos2DEx& host_pos,double direction)
	{
		ShowCrossParam sc_param;

		if(navi_info.toShowCross(&sc_param))
		{
			if(sc_param.m_state == SCST_OPEN)
			{
				m_cross_info.cross_center = sc_param.m_cur_cross.m_pos;
				m_cross_info.host_pos = host_pos;
				m_cross_info.path_name = Utility::wstr2ustr(sc_param.m_cur_segment->m_line.getName());
				m_cross_info.turn_dir_type = sc_param.m_dir_type;
				m_cross_info.path_length_all = sc_param.m_cur_segment->getLength();
				m_cross_info.path_length_left = sc_param.m_remain_length;
				m_cross_info.host_dir_clock = direction;

				_is_cross_map_shown = true;
			}
			else if(sc_param.m_state == SCST_UPDATE && _is_cross_map_shown)
			{
				m_cross_info.path_length_left = sc_param.m_remain_length;
				m_cross_info.host_dir_clock = direction;
				m_cross_info.host_pos = host_pos;
			}
			else if(sc_param.m_state == SCST_CLOSE && _is_cross_map_shown)
			{
				_is_cross_map_shown = false;
			}
		}
	}

	bool Navigation::getCrossData(CrossInfo& cross_info)
	{
		bool flag = false;
		if (_is_cross_map_shown)			
		{
			cross_info.cross_center = m_cross_info.cross_center;
			cross_info.host_dir_clock = m_cross_info.host_dir_clock;
			cross_info.host_pos = m_cross_info.host_pos;
			cross_info.path_length_all = m_cross_info.path_length_all;
			cross_info.path_length_left = m_cross_info.path_length_left;
			cross_info.path_name = "";//m_cross_info.path_name;
			cross_info.turn_dir_type = m_cross_info.turn_dir_type;
			
			//cross_info = m_cross_info;
			flag = true;
		}
		return flag;
	}
 
   	void Navigation::updateTrack(const Pos2DEx & pos,double dir,int remain_dis)
   	{
//    		iss << dir;
//    		iss << remain_dis << '#';
//    		point_para.point.d0 = pos.d0;
//    		point_para.point.d1 = pos.d1;   
   	}
 
 	void Navigation::endNavigation()
 	{
 		_is_cross_map_shown = false;
  		m_near_final.reset();
 
//  		if(_is_real_navi)  
//  		{
 			if(_path_res!=NULL)
 				delete _path_res;
 			_path_res = NULL;
 			_path_points.clear(); 
			_is_real_navi = false;
//  		}
//  		else
			_is_simu_navi = false;
 	}
	double Navigation::calRemainDistance(const Pos2DEx & cur_pos, const Pos2DEx & dest_pos)
	{
		double tmp_dis = 0;
		if(_path_res != NULL)
		{
			Pos2DEx tmp_pos;
			Fraction tmp_param(0,1);
			double dis_pos_to_path = SpatialMath::NearestPointOnSegmentString(&tmp_pos, &tmp_param, cur_pos, _path_points);
// 			double dis_path_to_dest = SpatialMath::getDistance(*(_path_points.rbegin()), dest_pos);

			uint_32 path_segment_size = _path_res->m_segments.size();
			int path_points_size = _path_points.size();		
			int num=0,seg_pos=0;
			double tmp_path_pos = 0,no_double = 0;
			tmp_path_pos = tmp_param.getDoubleValue();
			int no_int = (int)tmp_path_pos;

			num += _path_res->m_segments[0].m_ctrl_points.size();
			if (num >= no_int)
			{
				no_double = tmp_path_pos/_path_res->m_segments[0].m_ctrl_points.size();
				seg_pos = 0;
			}
			else
			{
				for (int i=1 ;i<path_segment_size; i++)
				{
					int seg_size = _path_res->m_segments[i].m_ctrl_points.size() - 1;
					num = num + seg_size;
					if (num >= no_int)
					{
						no_double = (tmp_path_pos +seg_size - num)/seg_size;
						seg_pos = i;
						break;
					}
				}
			}
			tmp_dis = _path_res->getLength(seg_pos,1-no_double);

			tmp_dis += dis_pos_to_path;
		}
		else
		{
			tmp_dis = SpatialMath::getDistance(cur_pos, dest_pos); 
		}
		return tmp_dis;
	}

	void Navigation::getPathPoints( vector<vector<Pos2DEx> >& path )
	{
		path.clear();
		vector<Pos2DEx> tmp_points;
		for (int i=0 ; i< _path_points.size(); i++)
		{		
			tmp_points.clear();
			tmp_points.insert(tmp_points.end(),_path_points[i].begin(),_path_points[i].end());
			path.push_back(tmp_points);
		}
	}

	bool Navigation::isReach( const Pos2DEx & cur_pos, const Pos2DEx & dest_pos/*,const vector<Pos2DEx> & _path_points*/ )
	{
		double dis_to_target = SpatialMath::getDistance(cur_pos,dest_pos);

		if(dis_to_target <= NavigationInfo::NAV_TOLERANCE_TARGET)
		{
			reachTarget();
			return true;
		}
		// zgr, 2008-01-18, 添加一个判断，如果距离超出最大目标容忍值，就不需要判断是否到达路线终点
		else if(dis_to_target <= NavigationInfo::NAV_TOLERANCE_FINAL_MAX/* && ! path_points.empty()*/)
		{
			if(isReachedFinal(cur_pos))
			{
				reachFinal();
				return true;
			}
		}
		return false;
	}

	void Navigation::reachFinal() /* reach the path end */
	{
		//SoundPlayer::reachFinalPlay(NULL);
		endNavigation();
	}

	void Navigation::reachTarget() /* reach the dest */
	{
		//SoundPlayer::reachFinalPlay(NULL);
		endNavigation();
	}

	bool Navigation::isReachedFinal( const Pos2DEx & pos )
	{
		int path_num = _path_points.size();
		int last_path_points_num = _path_points[path_num-1].size();
		GIS_Embedded::Pos2DEx final_pos = _path_points[path_num-1][last_path_points_num-1];

		double dis_to_final = SpatialMath::getDistance(pos, final_pos);

		if(dis_to_final <= NavigationInfo::NAV_TOLERANCE_TARGET)
		{
			return true; 
		}

		if( dis_to_final <= 2 * NavigationInfo::NAV_TOLERANCE_TARGET) // zgr, 进入终点附近
		{
			m_near_final.setNearFinal(pos);
		}
		else if(m_near_final.m_is_near_final) // zgr, 进入并且将要离开终点附近
		{
			// zgr, 判断是否曾经经过终点附近
			Pos2DEx near_final_pos = SpatialMath::NearestPointOnSegment(final_pos, m_near_final.m_last_near_pos, pos);
			dis_to_final = SpatialMath::getDistance(pos, near_final_pos);

			if(dis_to_final <= NavigationInfo::NAV_TOLERANCE_TARGET)
			{
				return true;
			}
		}

		return false;
	}
}