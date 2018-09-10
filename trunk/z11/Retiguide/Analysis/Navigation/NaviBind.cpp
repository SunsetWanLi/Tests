#include "NaviBind.h"
#include "NavigationInfo.h" // ****
#include "../../SpatialMath/SpatialMath.h"
#include "../../Common/Util/PureMath.h"
//#include "../../TopoData/GraphFactory.h"
//#include "../../TopoData/GeoNWTypes/Geo_NW_Graph.h"
//#include "../Path/PathResult.h"
#include "NaviPathContext.h"
//#include "../../Common/Util/ErrorInfo.h"
//#include "../PointBindPLine.h"
//#include "../CurvePoint.h"
#include "../../Common/CRS/GlobalCrt.h"

using namespace IS_GIS;

namespace GIS_Embedded
{
GlobalCrt * Navi_GPS_Info::CRT = new GlobalCrt();

double NaviBind::MAX_OFF_SET  = 10 /*DOUBLE_ZERO*/;	//my 08.08.27
double NaviBind::DIR_TOL_BACK = 160.0/180.0 * M_PI; // ��󵼺��Ƕ�ƫ��
//double NaviBind::DIR_TOL_FORTH = 20.0/180.0 * M_PI; //������Ƕ�ƫ�� 
double NaviBind::DIR_TOL_FORTH = 60.0/180.0 * M_PI; //������Ƕ�ƫ�� 

double NaviBind::BOUND_MAX_OFF_SET = BOUND_MAX_OFF_SET_NEAR;
double NaviBind::BOUND_MAX_OFF_SET_FAR = /*60*/150; // ����,far��nearֻ����Զ���
double NaviBind::BOUND_MAX_OFF_SET_NEAR = /*40*/80;
double NaviBind::PRIORITY_FOR_BIND_ROAD = 0.5;

unsigned int NaviBind::dir_back_count  = 0;
unsigned int NaviBind::MAX_DIR_BACK_COUNT = 4; // 4 times for continued error dir;

const PathResult * NaviBind::last_path = NULL;
uint_32 NaviBind:: last_ps_index = -1;
Fraction NaviBind::last_bind_param(-1, 1);

Navi_GPS_Info::Navi_GPS_Info(const GPS_Info & gps_info):GPS_Info(gps_info)
{
	//Pos2DEx prj_pos;
	int_32 x, y;
	CRT->geoToPrj(longitude, latitude, &x, &y);
	
	longitude = x;
	latitude = y;
	
	heading = float(heading * M_PI / 180.0f);
}

void NaviBind::naviBind(const Navi_GPS_Info & gps_info, PathResult * path_result, Pos2DEx* bound_pos, AnaPosVerdict * ana_pos_ver, NaviPathContext * navi_path_context)
{
	
	//CurvePoint near_cp;
	double tol_dis = BOUND_MAX_OFF_SET_FAR;//150


	// bind to the road
	BindWeight bind_weight;
	//GridIndex * grid_index = GraphFactory::getGridIndex();
	//BindPos bind_pos(gps_info.longitude,gps_info.latitude,gps_info.heading);
	/*if(!PointBindPLine::pbLine(bind_pos, grid_index, &near_cp,&bind_weight,tol_dis,tol_dis,&bindCalculator))
	{
		*ana_pos_ver = APV_NOT_BOUND;
		dir_back_count = 0;		
		return ;
	}*/

	Pos2DEx near_pos(gps_info.longitude, gps_info.latitude);//bind_pos.m_pos;//near_cp.m_near_pos;

	// bind to the path
	if(path_result != NULL) // zgr, 2008-01-20, ���·��Ϊ��,����� DETURE
	{ 
		PathBindParam pbp;
		bindToPath(near_pos,*path_result,&pbp);

		*bound_pos = near_pos;

		*navi_path_context = NaviPathContext(pbp.m_ps_index, pbp.m_pos_param, path_result); // navi_path_contextֻ���ڰ󶨵�·��ʱ����
		
		if(pbp.m_off_set < tol_dis)//
		{
			double pri_road = PRIORITY_FOR_BIND_ROAD;

			BindWeight cur_weight;
			cur_weight.m_dis = pbp.m_off_set/* * pri_road*/;

			double tmp_radian = pbp.m_bound_angle;
			double cur_dir_off_set = fabs(gps_info.heading - tmp_radian);

			if(cur_dir_off_set > M_PI)
				cur_dir_off_set = M_PI * 2 - cur_dir_off_set;
			if(cur_dir_off_set > M_PI / 2)
				cur_dir_off_set = M_PI - cur_dir_off_set; 
			cur_weight.m_dir = cur_dir_off_set; 

			//my 08.08.27 �󶨾��벻Զ���ҽǶ�����Ҳ����Ϊ�ɹ�������������APV_DETOUR
			if(/*navi_info->m_off_set * pri_road < SpatialMath::getDistance(pos, near_pos) */
				/*bindCalculator(cur_weight,bind_weight) || */cur_weight.m_dis < BOUND_MAX_OFF_SET_NEAR)
			{
				// zgr, 2008-01-16, ����󶨵�·��������pos��һ��,������·����Ҳ��һ��,����CurveSegment
				//bound_cp->m_near_pos = pbp.m_bound_pos;
				//bound_cp->m_snw_re = path_result->m_segments[pbp.m_ps_index]->m_nw_edge;

				//last_offset_to_road = navi_info->m_off_set;
				//BOUND_MAX_OFF_SET = BOUND_MAX_OFF_SET_FAR; //zgr, 2008-01-17

				double angle_off_set = fabs(gps_info.heading - tmp_radian);
				if(angle_off_set > M_PI)
					angle_off_set = M_PI * 2 - angle_off_set;

				if(angle_off_set < DIR_TOL_FORTH)
				{
					//bound_cp->m_near_pos = pbp.m_bound_pos;
					//bound_cp->m_snw_re = path_result->m_segments[pbp.m_ps_index]->m_nw_edge;
					*ana_pos_ver = APV_DIRECTION_FORWARD;
					dir_back_count = 0;
				}
				else if(angle_off_set > DIR_TOL_BACK)
				{
					++ dir_back_count;
					if(dir_back_count >= MAX_DIR_BACK_COUNT)
					{
						*ana_pos_ver = APV_DIRECTION_BACKWARD;
						dir_back_count = 0;
					}
					else
					{
						*ana_pos_ver = APV_DIRECTION_UNSURE;
					}
				}
				else
				{
					*ana_pos_ver = APV_DIRECTION_UNSURE;
					dir_back_count = 0;
				}

				return;
			}
		}
	}

	*ana_pos_ver = APV_DETOUR;
	
	dir_back_count = 0;	
}

void NaviBind::bindToPath(const Pos2DEx & pos, const PathResult & path_result, PathBindParam * pbp)
{
	//SpatialMath::ne
	// cal the every segment  util the dis < max_off_set
	double d_offset = DBL_MAX;

	//const vector<Line> &segments = path_result.getLines();
	
	uint_32 index = 0, ps_size = path_result.m_segments.size();

	
	Pos2DEx bind_pos;
	Fraction param;
	double angle_to_next = 0;

	uint_32 start_i = 0, count = 0;
	if(last_path != NULL && last_path == &path_result) // use the last info
	{
		start_i = last_ps_index - 1;
		if((int)start_i < 0 || start_i >= ps_size) // ȷ��start_i < ps_size
			start_i = 0;
	}	

	
	for(uint_32 i = start_i; count < ps_size ; i = (i+1) % ps_size , ++ count)
	{
		Pos2DEx cur_bind_pos;
		Fraction cur_param;
		const vector<Pos2DEx> &ctrl_points = path_result.m_segments[i].m_ctrl_points;
		double cur_off_set = SpatialMath::NearestPointOnSegmentString(& cur_bind_pos, & cur_param, pos, ctrl_points);

		// update 
		// 
		// �ڶ����жϣ�cur_off_set == d_offset && ~
		//             ������ֵ���ʱ(���統·���л�ͷ·��ʱ��ͬһ��λ���ڲ�ͬ·�������������ֵ)�� Ҳ��Ҫ���и���
		//             ��Ϊǰ���·���Ѿ����й��󶨣�������ζ���Ѿ�����ǰ���·��
		if(cur_off_set < d_offset || (cur_off_set == d_offset && cur_off_set < MAX_OFF_SET))
		{
			index = i;
			bind_pos = cur_bind_pos;
			param = cur_param;
			d_offset = cur_off_set;
		}

		// ��ǰ���� ����·���ϵ�����㣺������������ֵ���������Ǻ��ϴ�ͬһ·������Ҫ���� λ�õ�ǰ���ԡ���cur_param ��С���ϴε�
		// ���򣬲�����ǰ�˳�����Ҫһֱ������ֱ������һ�����λ��
		// ����ǲ����Ƿ���ĵ�������������ʱ�����򲻻���� �л�ͷ·�ε����·��
		
		// ********
		// һ�ּ�������£����޷� ��ʾ�ﵽ�յ㣺
		// ��һ�λ�ͷ·�Σ�  BΪ�յ㣬AΪ�ϸ�·�Σ�����һ�ΰ�λ��ΪA�㣬����ǰλ��ΪB�㲢���Ѿ��ﵽ�յ�ֹͣ��
		// ��ʱ�����������Ϊ��ǰλ��ΪA��������ֹͣ��A��
		//            B----<-|
		//    -->-----A---->-|
		// ԭ��һ��Ŀǰ·��ֻ��һ���ߣ���ʵ�ʵ���Ҳ��A��B�ǲ�ͬ������λ�ã������ڱ�ϵͳ�����У�Ϊ�������ֵ
		//       
		if(cur_off_set < MAX_OFF_SET && 
			(index != last_ps_index || (index == last_ps_index && param >= last_bind_param)) )// find
			break;
	}

	
	pbp->m_ps_index = index;
	pbp->m_off_set = d_offset;
	const vector<Pos2DEx> &ctrl_points = path_result.m_segments[index].m_ctrl_points;
	double dis_to_cross = SpatialMath::getDistance(ctrl_points, param ,Fraction(ctrl_points.size() - 1, 1));
	
	uint_32 next_pos = (uint_32)param.getIntValue();
	if(dis_to_cross < NavigationInfo::NAV_TOLERANCE_CROSS // С��·�����̷�Χ������·��
		|| next_pos >= ctrl_points.size() - 1)// for it reach to the end of the path_segment
	{
		if(index == ps_size - 1)
		{
			angle_to_next = -1;  // it is the final now
		}
		else
			//angle_to_next = getAngle(*(path_result.m_segments[index + 1]),true);
			angle_to_next = getAngle(ctrl_points, true);
	}
	else
	{
		//next_pos = (param - next_pos) >= 0 ? next_pos + 1 : next_pos;
		angle_to_next = SpatialMath::calCurvePointNAngle(ctrl_points,param);
	}

	pbp->m_bound_angle = angle_to_next;
	pbp->m_bound_pos = bind_pos;
	pbp->m_pos_param = param;
		
	// update the last infomation
	last_path = &path_result;
	last_ps_index = index;
	last_bind_param = param;
}

double NaviBind::getAngle(const vector<Pos2DEx> &ctrl_points, bool from_start)
{
	assert(ctrl_points.size() >= 2);

	uint_32 s_index = from_start ? 0 : ctrl_points.size() - 2;
	const Pos2DEx &s_pos = ctrl_points[s_index];
	const Pos2DEx &e_pos = ctrl_points[s_index + 1];

	return SpatialMath::calLineNAngle(s_pos, e_pos);
}

bool NaviBind::bindCalculator(const BindWeight & l_value, const BindWeight & r_value)
{
	//	zgr,2008-01-26�����������������ֵ���Ѿ������̾�����ʱ������Ŀǰ����Ⱦ�����ɿ����ص㣬
	//  �Ƚ��������Ƕ�ƫ��ıȽ�
	double angle_off_set = fabs(l_value.m_dir - r_value.m_dir); 
	if(angle_off_set > M_PI / 4)
		return l_value.m_dir < r_value.m_dir;

	double l_weight = l_value.m_dis + (BOUND_MAX_OFF_SET) * (l_value.m_dir) / M_PI;
	double r_weight = r_value.m_dis + (BOUND_MAX_OFF_SET) * (r_value.m_dir) / M_PI;

	return l_weight < r_weight;
}

void NaviBind::resetLastPath()
{
	last_path = NULL;
	last_ps_index = 0;
}

}

