//#include "Ana_Navigation.h"
//#include "NavigationInfo.h"
//#include "../../SpatialMath/SpatialMath.h"
//#include "../../Common/Util/PureMath.h"
//#include "../../TopoData/GraphFactory.h"
//#include "../../TopoData/GeoNWTypes/Geo_NW_Graph.h"
//#include "../Path/PathResult.h"
//#include "../../Common/Util/ErrorInfo.h"
//#include "../../Common/common_config_if.h"
//#include "../../Analysis/PointBindPLine.h"
//#include "../../Analysis/CurvePoint.h"
//#include "../../GeoData/MapToolBox.h"
//
//#include <assert.h>
//using namespace std;
//
//namespace GIS_Embedded
//{
//double Ana_Navigation::MAX_OFF_SET  = DOUBLE_ZERO;
//unsigned int Ana_Navigation::MAX_DIR_ERROR_COUNT = 2; // 3 times for continued error dir;
//double Ana_Navigation::DEFAULT_TOLERANCE = -1; // to set when get the Pos firstly
//												// if the pos.d0 < 180 && pos.d1 < 180-> DEFAULT_TOLERANCE = 0.0002
//												// else ->DEFAULT_TOLERANCE = 20.0 meter
//double Ana_Navigation::DIR_TOL_BACK = 160.0/180.0 * PI; // 最大导航角度偏差
//double Ana_Navigation::DIR_TOL_FORTH = 20.0/180.0 * PI; //最大发声角度偏差 
//
//const PathResult * Ana_Navigation::last_path = NULL;
//uint_32 Ana_Navigation:: last_cross_index = -1;
//Fraction Ana_Navigation::last_bind_param(-1, 1);
//double Ana_Navigation::last_offset = DBL_MAX;
//bool Ana_Navigation::b_recalculated = false;
//double Ana_Navigation::m_tolerance = DEFAULT_TOLERANCE;
//unsigned int Ana_Navigation::dir_error_count = 0;
//
//double Ana_Navigation::BOUND_MAX_OFF_SET = BOUND_MAX_OFF_SET_NEAR;
//const double Ana_Navigation::BOUND_MAX_OFF_SET_FAR = 60; // 这里,far和near只是相对而言
//const double Ana_Navigation::BOUND_MAX_OFF_SET_NEAR = 40;
//
//unsigned int Ana_Navigation::dir_back_count  = 0;
//const unsigned int Ana_Navigation::MAX_DIR_BACK_COUNT = 2; // 2 times for continued error dir;
//
//const double Ana_Navigation::PRIORITY_HIGH_FOR_BIND_ROAD = 0.5;
//const double Ana_Navigation::PRIORITY_LOW_FOR_BIND_ROAD = 0.8;
//
////double Ana_Navigation::last_offset_to_road = DBL_MAX;
////int Ana_Navigation::off_set_enlarge_count = 0;
//
//PathPosIndex::~PathPosIndex()
//{
//}
//
//double Ana_Navigation::setTolerance(double tolerance)
//{
//	double tmp = m_tolerance;
//	m_tolerance = tolerance;
//	return tmp;
//}
//
//int Ana_Navigation::navigate( NavigationInfo ** nav_info,  bool * to_recalculate,  PathPosIndex ** path_pos_index, 
//							 const Pos2DEx & pos,double radian,double speed, const PathResult & path_result)
//{
//	int ret = 0;
//	//double dir = radian / 180.0f * PI;
//#ifdef _LOG_INFO
//	LOG_INFO("*** navigate at time: ");
//	LOG_INFO_CUR_TIME();
//	LOG_INFO("dir: " );
//	LOG_INFO(radian);
//	LOG_INFO("pos: " );
//	LOG_INFO(pos.d0);
//	LOG_INFO(",");
//	LOG_INFO_ENDL(pos.d1);
//#endif
//
//	double dir = radian;
//	double offset = 0;
//	uint_32 path_cross_index;
//	double dis_to_cross, dis_to_target;
//	double angle_to_next; 
//
//	// calculate the infomation from cur_pos to the next cross
//
//	getNextCross(&path_cross_index,& dis_to_cross, &offset,& angle_to_next,path_pos_index, pos, path_result);
//
//	last_path = & path_result;
//	last_cross_index = path_cross_index;
//
//#ifdef _LOG_INFO
//	LOG_INFO_ENDL(" getnext cross complete");
//	// zgr, 2008-01-13, 修改log顺序
//	LOG_INFO("path_cross_index : ");
//	LOG_INFO_ENDL(path_cross_index);
//	LOG_INFO("path result:");
//	LOG_INFO(path_result.m_segments.size());
//	LOG_INFO_ENDL(path_result.m_crosses.size());
//
//	LOG_INFO(" dir_error_count:");
//	LOG_INFO(dir_error_count);
//	LOG_INFO(" angle_to_next : ");
//	LOG_INFO_ENDL(angle_to_next);
//	LOG_INFO(" cur_offset: ");
//	LOG_INFO_ENDL(offset);
//	LOG_INFO(" last_offset:");
//	LOG_INFO_ENDL(last_offset);
//#endif
//	// calculate the remain distance from cur_pos to the target
//	dis_to_target = dis_to_cross;
//	for(uint_32 ps_i = path_cross_index + 1, ps_size = path_result.m_segments.size(); 
//		ps_i < ps_size; ++ps_i)
//		dis_to_target += path_result.m_segments[ps_i]->getLength();
//
//	//zgr, 2008-01-14
//	double angle_off_set = fabs(dir - angle_to_next);
//	if(angle_off_set > PI)
//		angle_off_set = PI * 2 - angle_off_set;
//
//	if(angle_to_next < 0) // reach the final 
//	{
//		//
//#ifdef _LOG_INFO
//		LOG_INFO_ENDL(" reach final");
//#endif
//		PathCross * pc = path_result.m_crosses[path_cross_index];
//		*nav_info = new NavigationInfo(DIRT_STOP, 0,0, *pc,0,speed);
//		clear();
//		b_recalculated = false;
//		return 0;
//	}
//	// 这里，由于GPS传入的方向有噪声值或者人行走时方向的不确定性，
//	// 采用当 当前方向和预测方向 相反时，记录错误数
//	else if(angle_off_set > DIR_TOL_BACK )
//	{
//		++ dir_error_count;
//		if(dir_error_count > MAX_DIR_ERROR_COUNT)
//		{
//			//if(recalculate() < 0)
//			//	return -1;
//			clear();
//			*to_recalculate = true;
//			return 0;
//		}
//	}
//	else // 
//		dir_error_count = 0;
//
//	// 当偏移量超过容忍值，并且偏移量是在增大时，需要进行重计算
//	if(offset > m_tolerance && offset > last_offset)
//	{
//		//if(recalculate() < 0)
//		//	return -1;
//		clear();
//		*to_recalculate = true;
//		return 0;
//	}
//
//
//	DirectionType dir_t = DIRT_UNKNOWN;
//	calDirType(&dir_t, path_cross_index, path_result);
//#ifdef _LOG_INFO
//	LOG_INFO("cal dir type complete, dir_type:");
//	LOG_INFO_ENDL(dir_t);
//#endif
//	*nav_info = new NavigationInfo(dir_t, dis_to_cross, dis_to_target,*(path_result.m_crosses[path_cross_index]),offset,speed);
//	
//	//zgr, 2008-01-14, 如果方向偏差较大，不确定，不发声
//	if( angle_off_set > DIR_TOL_FORTH)
//		(*nav_info)->to_sound_pre = false;
//
//	b_recalculated = false;
//	last_offset = offset;// zgr, 2008-01-12
//
//#ifdef _LOG_INFO
//	LOG_INFO_ENDL(" ana_navigate end");
//#endif
//	return ret;
//}
//
//int Ana_Navigation::calNaviInfo(const Pos2DEx & pos, double radian, const PathResult & path_result, 
//								double * angle_to_next,NavigationInfo * nav_info, PathPosIndex * path_pos_index)
//{
//	double dir = radian;
//	double offset = 0;
//	uint_32 path_cross_index;
//	double dis_to_cross = DBL_MAX, dis_to_target = DBL_MAX;
//	double angle_tn; 
//
//	// calculate the infomation from cur_pos to the next cross
//	PathPosIndex * cur_ppi = NULL;
//	getNextCross(&path_cross_index,& dis_to_cross, &offset,& angle_tn, &cur_ppi, pos, path_result);
//
//	if(cur_ppi!= NULL)
//		*path_pos_index = *cur_ppi;
//	delete cur_ppi, cur_ppi = NULL;
//
//	last_path = & path_result;
//	last_cross_index = path_cross_index;
//
//	// calculate the remain distance from cur_pos to the target
//	dis_to_target = dis_to_cross;
//	uint_32 ps_i = path_cross_index + 1, ps_size = path_result.m_segments.size(); 
//	for(;ps_i < ps_size; ++ps_i)
//		dis_to_target += path_result.m_segments[ps_i]->getLength();
//
//	DirectionType dir_t = DIRT_UNKNOWN;
//	calDirType(&dir_t, path_cross_index, path_result);
//	// 
//	*nav_info = NavigationInfo(dir_t, dis_to_cross, dis_to_target,*(path_result.m_crosses[path_cross_index]),offset,0);
//
//	//zgr, 2008-02-21, 取前后两个路段信息
//	nav_info->m_cur_segment = path_result.m_segments[path_cross_index];
//	if(path_cross_index + 1 < ps_size)
//		nav_info->m_next_segment = path_result.m_segments[path_cross_index + 1];
//
//	// 预取下个路口方向信息作为提示
//	if(path_cross_index + 1 < ps_size)
//	{
//		nav_info->m_next_dis = path_result.m_segments[path_cross_index + 1]->getLength();
//		calDirType(&(nav_info->m_next_dir_type), path_cross_index + 1, path_result);
//	}
//
//	*angle_to_next = angle_tn;
//	return 0;
//}
//
//void Ana_Navigation::initTolerance()
//{
//	if(DEFAULT_TOLERANCE < 0)
//	{
//		DEFAULT_TOLERANCE = 100.0; //投影坐标下的容忍距离：100mi
//
//		MAX_OFF_SET = DEFAULT_TOLERANCE / 100; 
//	}
//	if(m_tolerance < 0)
//	{
//		m_tolerance = DEFAULT_TOLERANCE;
//	}
//}
//int Ana_Navigation::navigateForVehicle(NavigationInfo ** nav_info, bool * to_recalculate,  PathPosIndex ** path_pos_index, 
//									   const Pos2DEx & pos, double dir, const PathResult & path_result)
//{
//	int ret = 0;
//	assert(0);
//	return ret;
//}
//void Ana_Navigation::calPathPosIndex(PathPosIndex ** path_pos_index, uint_32 cross_index, const Fraction &pos_param,const PathResult& path_result)
//{
//	Fraction param;
//	for(uint_32 i =0; i < cross_index; ++i)
//	{
//		if(!path_result.m_segments[i]->m_ctrl_points.empty())
//			param += Fraction(path_result.m_segments[i]->m_ctrl_points.size() - 1, 1);
//	}
//	param += pos_param;
//	*path_pos_index = new PathPosIndex(param);
//}
//
//void Ana_Navigation::getNextCross(uint_32 * cross_index,double * dis_to_cross,double * dis_off_set,
//								  double * angle_to_next,PathPosIndex ** path_pos_index,
//								const Pos2DEx & pos, const PathResult & path_result)
//{
//	//SpatialMath::ne
//	// cal the every segment  util the dis < max_off_set
//	double d_offset = DBL_MAX;
//	
//	uint_32 index = 0, ps_size = path_result.m_segments.size();
//
//	
//	Pos2DEx bind_pos;
//	Fraction param;
//
//	uint_32 start_i = 0, count = 0;
//	if(last_path != NULL && last_path == &path_result) // use the last info
//	{
//		start_i = last_cross_index - 1;
//		if((int)start_i < 0|| start_i >= ps_size) // 确保start_i < ps_size
//			start_i = 0;
//	}	
//
//	for(uint_32 i = start_i; count < ps_size ; i = (i+1) % ps_size , ++ count)
//	{
//		Pos2DEx cur_bind_pos;
//		Fraction cur_param;
//		P_PS  cur_ps = path_result.m_segments[i];
//		int cur_off_set = SpatialMath::NearestPointOnSegmentString(& cur_bind_pos, & cur_param, pos, cur_ps->m_ctrl_points);
//		
////		if(cur_off_set < MAX_OFF_SET) // find
////		{
////			* cross_index = i;
////			* dis_off_set = cur_off_set;
////
////			uint_32 next_pos_i = (uint_32)cur_param;
////			if(next_pos_i >= cur_ps->m_ctrl_points.size() - 1)// for it reach to the end of the path_segment
////			{
////				if(i == ps_size - 1) 
////				{
////#ifdef _LOG_INFO
////			LOG_INFO("ctrl_points size:");
////			LOG_INFO_ENDL(cur_ps->m_ctrl_points.size());
////			LOG_INFO(" param:");
////			LOG_INFO_ENDL(cur_param);
////#endif
////					* angle_to_next = -1; // it is the final now
////				}
////				else
////					* angle_to_next = getAngle(*(path_result.m_segments[i+1]), true);
////			}
////			else
////			{
////				next_pos_i = (cur_param - next_pos_i) >= 0 ? next_pos_i + 1 : next_pos_i;
////				* angle_to_next = SpatialMath::calLineNAngle(cur_bind_pos, *(cur_ps->m_ctrl_points[next_pos_i]));
////			}
////
////			* dis_to_cross = SpatialMath::getDistance(cur_ps->m_ctrl_points, cur_param ,cur_ps->m_ctrl_points.size() - 1);
////			
////			calPathPosIndex(path_pos_index, *cross_index, cur_param, path_result);
////
////			return ;
////		}
//
//		// update 
//		// 
//		// 第二段判断：cur_off_set == d_offset && ~
//		//             当容忍值相等时(比如当路线有回头路段时，同一个位置在不同路段上有相等容忍值)， 也需要进行更新
//		//             因为前面的路段已经进行过绑定，这里意味着已经经过前面的路段
//		if(cur_off_set < d_offset || cur_off_set == d_offset && cur_off_set < MAX_OFF_SET)
//		{
//			index = i;
//			bind_pos = cur_bind_pos;
//			param = cur_param;
//			d_offset = cur_off_set;
//		}
//
//		// 提前满足 点在路径上的最近点：首先满足容忍值，其次如果是和上次同一路段则需要满足 位置的前进性——cur_param 不小于上次的
//		// 否则，不能提前退出，需要一直往下找直到最后的一个最近位置
//		// 如果是不考虑方向的导航（如人行走时），则不会出现 有回头路段的最短路径
//		
//		// ********
//		// 一种极端情况下，会无法 提示达到终点：
//		// 如一段回头路段，  B为终点，A为上个路段，当上一次绑定位置为A点，而当前位置为B点并且已经达到终点停止，
//		// 这时，程序可以认为当前位置为A，并且是停止在A点
//		//            B----<-|
//		//    -->-----A---->-|
//		// 原因：一：目前路段只是一条线，在实际当中也许A和B是不同的坐标位置，但是在本系统数据中，为相等坐标值
//		//       
//		if(cur_off_set < MAX_OFF_SET && 
//			(index != last_cross_index || index == last_cross_index && param >= last_bind_param) )// find
//			break;
//	}
//
//	P_PS ps = path_result.m_segments[index];
//	* cross_index = index;
//	*dis_off_set = d_offset;
//	* dis_to_cross = SpatialMath::getDistance(ps->m_ctrl_points, param ,Fraction(ps->m_ctrl_points.size() - 1, 1));
//	
//	uint_32 next_pos = (uint_32)param.getIntValue();
//	if(*dis_to_cross < NavigationInfo::NAV_TOLERANCE_CROSS // 小于路口容忍范围，到达路口
//		|| next_pos >= ps->m_ctrl_points.size() - 1)// for it reach to the end of the path_segment
//	{
//		if(index == ps_size - 1)
//		{
//#ifdef _LOG_INFO
//			LOG_INFO("ctrl_points size:");
//			LOG_INFO_ENDL(ps->m_ctrl_points.size());
//			LOG_INFO(" param:");
//			LOG_INFO_ENDL(param);
//#endif
//			*angle_to_next = -1;  // it is the final now
//		}
//		else
//			*angle_to_next = getAngle(*(path_result.m_segments[index + 1]),true);
//	}
//	else
//	{
//		next_pos = (param.getDecimalFraction().numerator >= 0) ? next_pos + 1 : next_pos;
//		* angle_to_next = SpatialMath::calLineNAngle(bind_pos, ps->m_ctrl_points[next_pos]);
//	}
//
//	calPathPosIndex(path_pos_index, *cross_index, param,path_result);
//	(*path_pos_index)->m_cur_pos = bind_pos;
//	(*path_pos_index)->m_snw_re = ps->m_nw_edge;
//	//CMPL_MESSAGE("将这些静态数据集中处理");
//
//	// 记录本次的绑定信息
//	last_bind_param = param;
//	//last_offset = *dis_off_set;// zgr, 2008-01-12
//}
////
//int Ana_Navigation::calDirType(DirectionType * dir_type, uint_32 segment_index,const PathResult & path_result)
//{
//	int ret = 0;
//	if(segment_index < 0 && segment_index > path_result.m_segments.size() - 1)
//	{
//		return -1;
//	}
//
//	if(segment_index == path_result.m_segments.size() - 1)
//	{
//		*dir_type = DIRT_STOP;
//		return ret;
//	}
//
//	P_PS ps = path_result.m_segments[segment_index];
//	P_PS next_ps = path_result.m_segments[segment_index+1];
//
//	double angle_1 = getAngle(*ps, false);
//	double angle_2 = getAngle(*next_ps, true);
//
//	//angle_1 = angle_1 + PI;
//	//if(angle_1 > 2 * PI)
//	//	angle_1 -= 2 * PI;
//	int tmp_dir = (int)calDirType(angle_1, angle_2);
//	tmp_dir |= int(calDirType_2(path_result.m_crosses[segment_index]->m_dirs,angle_1, angle_2));
//
//	*dir_type = (DirectionType)tmp_dir;
//
//	return ret;
//}
//
//double Ana_Navigation::getAngle(const PathSegment & path_segment, bool from_start)
//{
//	assert(path_segment.m_ctrl_points.size() >= 2);
//
//	uint_32 s_index = from_start ? 0 : path_segment.m_ctrl_points.size() - 2;
//	Pos2DEx s_pos = path_segment.m_ctrl_points[s_index];
//	Pos2DEx e_pos = path_segment.m_ctrl_points[s_index + 1];
//
//	return SpatialMath::calLineNAngle(s_pos, e_pos);
//}
//DirectionType Ana_Navigation::calDirType(double from, double to)
//{
//	int ret = (int)DIRT_UNKNOWN;// 0x0
//	double minus = to - from;
//	if(minus < 0)
//		minus += 2 * PI;
//	
//	int off_set = (int)(minus / (PI / 8)) % 16;
//	if(PureMath::between(off_set, 1, 7))// [2,6) 0.25pi->0.75pi
//		ret |= (int)DIRT_RIGHT;
//	if(PureMath::between(off_set, 5,11)) // [6, 10) 0.75pi->1.25pi
//		ret |= (int)DIRT_BACK;
//	if(PureMath::between(off_set, 9, 15)) //[10, 14), 1.25pi->1.75pi
//		ret |= (int)DIRT_LEFT;
//	if(PureMath::between(off_set, 13, 16) || PureMath::between(off_set, 0, 3)) // 1.75pi->2pi 0->0.25pi
//		ret |= (int)DIRT_FORTH;
//	
//	//if(PureMath::between(off_set, 1,3))
//	//	ret = DIRT_FORTH_RIGHT;
//	//else if(PureMath::between(off_set, 3,5))
//	//	ret = DIRT_RIGHT;
//	//else if(PureMath::between(off_set, 5,7))
//	//	ret = DIRT_RIGHT_BACK;
//	//else if(PureMath::between(off_set, 7,9))
//	//	ret = DIRT_BACK;
//	//else if(PureMath::between(off_set, 9,11))
//	//	ret = DIRT_BACK_LEFT;
//	//else if(PureMath::between(off_set, 11,13))
//	//	ret = DIRT_LEFT;
//	//else if(PureMath::between(off_set, 13,15))
//	//	ret = DIRT_LEFT_FORTH;
//	//else // [15, 16), [0, 1)
//	//	ret = DIRT_FORTH;
//	
//	return (DirectionType)ret;
//}
//
//DirectionType Ana_Navigation::calDirType_2(const vector<double> &v_dirs,double from, double to)
//{
//	DirectionType ret = DIRT_UNKNOWN;// 0x0
//	if(v_dirs.empty())
//		return ret;
//
//	double pi_8 = PI / 4;
//	uint_32 i = 1, size = v_dirs.size();
//	for(; i < size; ++i)
//	{
//		if(fabs(v_dirs[i-1] - v_dirs[i]) <= pi_8)
//			break;
//	}
//	if(i == size)
//	{
//		if(fabs(v_dirs[size-1]- v_dirs[0]) > pi_8)
//			return ret;
//	}
//
//	// need to calculate
//	double minus = to - from;
//	if(minus > 0)
//		ret = DIRT_RIGHT;
//	else
//		ret = DIRT_LEFT;
//	return (DirectionType) ret;
//}
//void Ana_Navigation::clear()
//{
//	last_path = NULL;// not to delete
//	last_cross_index = -1;
//	last_bind_param = Fraction(-1, 1); // 取最小值，并且是非法值
//	last_offset = DBL_MAX; // 取最大值
//	//b_recalculated = false;
//	//m_tolerance = DEFAULT_TOLERANCE;
//	dir_error_count = 0;
//}
//int Ana_Navigation::recalculate()
//{
//	if(b_recalculated
//		/*&& last_path == &path_result*/) // recalculated right now
//	{
//		clear();
//		b_recalculated = false;
//		return -1; // can not navigate
//	}
//	b_recalculated = true;
//	clear();
//	return 0;
//}
//
//void Ana_Navigation::findPosState(const Pos2DEx & pos,double radian,PathResult * path_result,
//	AnaPosVerdict * ana_pos_ver,CurvePoint * bound_pos,NavigationInfo * navi_info)
//{
//	//vector<FCLocator*> v_fc_loc;
//	//vector<wstring> v_fc_str;
//	//Geo_NW_Graph * graph = GraphFactory::crtGeoNWGraph();
//	
//	//FCLocator near_fc;
//	CurvePoint near_cp;
//	double tol_dis = BOUND_MAX_OFF_SET;
//
//	//graph->getAllFCLocators(&v_fc_str);
//	//for(uint_32 i = 0, size = v_fc_str.size(); i < size; ++i)
//	//{
//	//	v_fc_loc.push_back(new FCLocator(v_fc_str[i]));
//	//}	
//	// bind to the road
//	BindWeight bind_weight;
//	GridIndex * grid_index = GraphFactory::getGridIndex();
//
//	if(!PointBindPLine::pbLine(BindPos(pos.d0, pos.d1,radian),grid_index, &near_cp,&bind_weight,tol_dis, tol_dis, &bindCalculator))
//	{
//#ifndef _SIM_PATH_
//		*ana_pos_ver = APV_NOT_BOUND;
//		dir_back_count = 0;
//		//off_set_enlarge_count = 0;
//		//last_offset_to_road = DBL_MAX;  //zgr, 2008-01-16
//		BOUND_MAX_OFF_SET = BOUND_MAX_OFF_SET_NEAR; //zgr, 2008-01-17
//		//for_each(v_fc_loc.begin(), v_fc_loc.end(), DeletePointerFunctor<FCLocator>());
//		return ;
//#endif
//	}
//
//	double dir = radian;
//	Pos2DEx near_pos = pos;
//
//
//	//uint_32 cross_index;
//	//double dis_to_cross;
//	//double dis_off_set;
//	double angle_to_next;
//	PathPosIndex path_pos_index;
//
//	// bind to the path
////	getNextCross(&cross_index, &dis_to_cross, &dis_off_set, &angle_to_next, &path_pos_index, near_pos, path_result);
//	if(path_result == NULL) // zgr, 2008-01-20, 如果路径为空,则进入 DETURE
//	{
//		navi_info->m_off_set = DBL_MAX;
//	}
//	else
//	{
//		calNaviInfo(near_pos,dir,*path_result, &angle_to_next, navi_info, &path_pos_index);
//	}
//
//	*bound_pos = near_cp;
//	//
//
//	if((navi_info)->m_off_set < tol_dis)//
//	{
//		// zgr, 2008-01-16, 用于绑定时的两个优先级,使得状态具有延续本身状态的特点,但是又不至于太难改变,
//		//    根据last_offset_to_road与当前的比较设置不同的优先级
//		double pri_road = PRIORITY_HIGH_FOR_BIND_ROAD;
//		//if(navi_info->m_off_set < last_offset_to_road) // 改为设置两个绑定距离
//		//{
//		//	++ off_set_enlarge_count;
//		//	if(off_set_enlarge_count == 2) //两次连续的增大来过滤可能的噪声点
//		//	{
//		//		pri_road = PRIORITY_LOW_FOR_BIND_ROAD;
//		//		off_set_enlarge_count = 0;
//		//	}
//		//}
//		//else
//		//{
//		//	off_set_enlarge_count = 0;
//		//}
//		BindWeight cur_weight;
//		cur_weight.m_dis = navi_info->m_off_set * pri_road;
//
//		vector<Pos2DEx> ctrl_points;
//		path_result->getControlPoints(&ctrl_points);
//
//		double tmp_radian = SpatialMath::calCurvePointNAngle(ctrl_points,path_pos_index.m_ctrl_pos_param);
//		double cur_dir_off_set = fabs(radian - tmp_radian);
//			if(cur_dir_off_set > PI)
//				cur_dir_off_set = PI * 2 - cur_dir_off_set;
//
//		if(cur_dir_off_set > PI / 2)
//			cur_dir_off_set = PI - cur_dir_off_set; 
//		cur_weight.m_dir = cur_dir_off_set; 
//
//		if(/*navi_info->m_off_set * pri_road < SpatialMath::getDistance(pos, near_pos) */
//			bindCalculator(cur_weight,bind_weight))
//		{
//			// zgr, 2008-01-16, 这里绑定的路不仅仅是pos不一样,将来的路名等也不一样,改用CurveSegment
//			bound_pos->m_near_pos = path_pos_index.m_cur_pos;
//			bound_pos->m_snw_re = path_pos_index.m_snw_re;
//			//last_offset_to_road = navi_info->m_off_set;
//			BOUND_MAX_OFF_SET = BOUND_MAX_OFF_SET_FAR; //zgr, 2008-01-17
//
//			double angle_off_set = fabs(dir - angle_to_next);
//			if(angle_off_set > PI)
//				angle_off_set = PI * 2 - angle_off_set;
//
//			if(angle_off_set < DIR_TOL_FORTH)
//			{
//				*ana_pos_ver = APV_DIRECTION_FORWARD;
//				dir_back_count = 0;
//			}
//			else if(angle_off_set > DIR_TOL_BACK)
//			{
//				++ dir_back_count;
//				if(dir_back_count >= MAX_DIR_BACK_COUNT)
//				{
//					*ana_pos_ver = APV_DIRECTION_BACKWARD;
//					dir_back_count = 0;
//				}
//				else
//				{
//					*ana_pos_ver = APV_DIRECTION_UNSURE;
//				}
//			}
//			else
//			{
//				*ana_pos_ver = APV_DIRECTION_UNSURE;
//				dir_back_count = 0;
//			}
//
//			return;
//		}
//
//	}
//
//	*ana_pos_ver = APV_DETOUR;
//	//last_offset_to_road = DBL_MAX; // zgr, 2008-01-16
//	dir_back_count = 0;
//	//off_set_enlarge_count = 0;
//	BOUND_MAX_OFF_SET = BOUND_MAX_OFF_SET_NEAR; //zgr, 2008-01-17
//	//for_each(v_fc_loc.begin(), v_fc_loc.end(), DeletePointerFunctor<FCLocator>());
//}
//
//bool Ana_Navigation::bindCalculator(const BindWeight & l_value, const BindWeight & r_value)
//{
////	double l_weight = l_value.m_dis * max(10/PI, l_value.m_dir / (PI / 2)); //  距离 * 角度/120度
////	double r_weight = r_value.m_dis * max(10/PI, r_value.m_dir / (PI / 2));
//
//	//return l_value.m_dis < r_value.m_dis;
//	//	zgr,2008-01-26，这里基于左右两个值都已经在容忍距离内时，利用目前方向比距离更可靠的特点，
//	//  先进行两个角度偏差的比较
//	double angle_off_set = fabs(l_value.m_dir - r_value.m_dir); 
//	if(angle_off_set > PI / 4)
//		return l_value.m_dir < r_value.m_dir;
//
//	double l_weight = l_value.m_dis + (BOUND_MAX_OFF_SET) * (l_value.m_dir) / PI;
//	double r_weight = r_value.m_dis + (BOUND_MAX_OFF_SET) * (r_value.m_dir) / PI;
//
//	return l_weight < r_weight;
//}
//
//void Ana_Navigation::resetLastPath()
//{
//	last_path = NULL;
//	last_cross_index = 0;
//}
//
//}