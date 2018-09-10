#include "NaviHandler.h"
//#include "NaviHandlerParam.h"
//#include "../../Shell/Navigation/NaviMap.h"
//#include "../../SpatialMath/SpatialMath.h"
//
//#include <cassert>
//#include <stdlib.h>
//#include <time.h>

using namespace std;


namespace GIS_Embedded
{

int CNW_JunctionHandler::crtNaviInfo( const NaviAnalyzeParam & navi_ana_param,  NavigationInfo * navi_info)
{
	const GPS_Info & gps_info = navi_ana_param.m_gps_info;
	const NaviPathContext & navi_path_context = navi_ana_param.m_navi_path_context;
	const Pos2DEx & target_pos = navi_ana_param.m_target_pos;

	uint_32 cur_index = navi_path_context.getCurIndex();
	uint_32 ps_size = navi_path_context.getPathSegmentSize();

	navi_info->m_cur_segment = navi_path_context.getPathSegment(cur_index); 
	navi_info->m_cross = *(navi_path_context.getPathCross(cur_index));
	navi_info->m_dis = navi_path_context.getDisToInterSection();
	navi_info->m_total_dis = navi_path_context.getDisToFinal();
	navi_info->m_speed = gps_info.speed;// * 1.85 / 3.6;

	if(cur_index + 1 < ps_size)
	{
		navi_info->m_next_segment = navi_path_context.getPathSegment(cur_index + 1);
		navi_info->m_next_dis = navi_info->m_next_segment->getLength();
		navi_info->m_dir_type = navi_path_context.calDirectionType(*(navi_info->m_cur_segment),*(navi_info->m_next_segment));

		//20090615添加开始。。。duanjf
		uint_32 tmp_ps_index = cur_index + 1;	
		DirectionType tmp_dir_type = navi_info->m_dir_type;
		const PathSegment * tmp_cur_segment;
		const PathSegment * tmp_next_segment;
		double tmp_straight_length = navi_info->m_dis;	//初始化为到第一个路口的距离。。。

		int straight_crs_num = 0;	//连续直行路口计数。。。
		double straight_length = 0;
		
		while( DIRT_FORTH == tmp_dir_type && tmp_ps_index + 1 < ps_size )
		{
			straight_crs_num += 1;
			straight_length += tmp_straight_length;
			tmp_cur_segment = navi_path_context.getPathSegment( tmp_ps_index );
			tmp_next_segment = navi_path_context.getPathSegment( tmp_ps_index + 1 );
			tmp_dir_type = navi_path_context.calDirectionType( *tmp_cur_segment, *tmp_next_segment );
			tmp_straight_length = tmp_cur_segment->getLength();
			tmp_ps_index++;
		}

		navi_info->m_straight_crs_num = straight_crs_num;
		navi_info->m_straight_length = straight_length;
		//20090615添加结束。。。duanjf

		if(cur_index + 2 < ps_size)
		{
			navi_info->m_next_dir_type = navi_path_context.calDirectionType(*(navi_info->m_next_segment), *(navi_path_context.getPathSegment(cur_index + 2)));
		}
		else if(cur_index + 2 == ps_size)
		{
			navi_info->m_next_dir_type = DIRT_STOP;
		}
	}
	else if(cur_index == ps_size - 1)
	{
		navi_info->m_dir_type = DIRT_STOP;
	}

	return 0;
}

int CNW_JunctionHandler::actWithNaviInfo(int cmd, NaviActParam & navi_act_param)
{
	int ret = -1;
	/*if(NULL == navi_act_param.m_navi_map)
		return ret;

	if(cmd & (int)NHCT_SPEAK)
		ret = toSpeak(navi_act_param);*/
	//assert(0);
	return ret;
}

int CNW_JunctionHandler::toSpeak(NaviActParam & navi_act_param)
{
	int ret=0;
	NavigationInfo & navi_info = navi_act_param.m_navi_info;

	int to_cross_dis;
	DirectionType dir_type;
	DirectionType next_dir_type;
	int straight_crs_num = navi_info.m_straight_crs_num;
	int straight_length = navi_info.m_straight_length;

	bool flag = navi_info.toSound(&to_cross_dis,&dir_type,&next_dir_type);

	if(flag)//lw
	{
		 //ret = SoundPlayer::NaviInfoPlay(dir_type,next_dir_type,to_cross_dis,straight_crs_num,straight_length);
		assert(0);
		return ret;
	}
	else
		return -1;
}

void CNW_JunctionHandler::toShowCross(NaviActParam & navi_act_param)
{
	/*NavigationInfo & navi_info = navi_act_param.m_navi_info;
	NaviMap * navi_map = navi_act_param.m_navi_map;
	Pos2DEx & cur_pos = navi_act_param.m_cur_pos;

	ShowCrossParam sc_param;

	if(navi_info.toShowCross(&sc_param))
	{
		ostringstream iss;
		string action_info;
		AppAction* app_action = new AppAction();
		if(sc_param.m_state == SCST_OPEN)
		{
			iss << "show_cross#";
			iss << sc_param.m_dir_type;
			iss << '#';
			iss << (int)sc_param.m_cur_cross.m_pos.d0;
			iss << '#';
			iss << (int)sc_param.m_cur_cross.m_pos.d1;
			iss << '#';
			iss << (int)cur_pos.d0;
			iss << '#';
			iss << (int)cur_pos.d1;
			iss << '#';
			iss << sc_param.m_remain_length;
			iss << '#';
			iss << sc_param.m_cur_segment->getLength();
			iss << '#';
			string pathName = String_Tools::wstrToStr(sc_param.m_cur_segment->m_line.getName());
			if(pathName.empty())
				iss <<" ";
			else
				iss << pathName;
			iss << '#';

			vector<Pos2DEx> cross_line;
			cross_line.insert(cross_line.end(),sc_param.m_cur_segment->m_ctrl_points.begin(),sc_param.m_cur_segment->m_ctrl_points.end());
			cross_line.insert(cross_line.end(),sc_param.m_next_segment->m_ctrl_points.begin(),sc_param.m_next_segment->m_ctrl_points.end());
			app_action->ACTIONPARA = new LineActionPara(cross_line);

			callBack->crossMapOpen();
			_is_cross_map_shown = true;
		}
		else if(sc_param.m_state == SCST_UPDATE && _is_cross_map_shown)
		{
			iss << "update_cross#";
			iss << sc_param.m_remain_length;
			iss << '#';

			app_action->ACTIONPARA = new PointActionPara(_point_host.d0,_point_host.d1);

		}
		else if(sc_param.m_state == SCST_CLOSE && _is_cross_map_shown)
		{

			iss << "close_cross#";
			callBack->crossMapClose();

		}

		action_info = iss.str();
		if(action_info.empty())
			action_info="NULL#";

		app_action->TYPE = Action::APP_ACTION;
		app_action->ACTION_ID = AppAction::VALUE_PATH_SET;
		app_action->ACTIONINFO = action_info;


		_navi_map->actionHandle(*app_action);

		delete app_action->ACTIONPARA;
		delete app_action;


	}*/

}

}