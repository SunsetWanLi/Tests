#ifndef ANA_NAVIGATION_H_
#define ANA_NAVIGATION_H_

#include "../../Common/Types/basic_types.h"
#include "../../Common/Types/Fraction.h"
#include "NavigationInfo.h"
#include "../AnaEnumCommon.h"
#include "../../DataFetcher/p_Ref.h"

namespace GIS_Embedded
{

class PathResult;
class PathSegment;
class PathCross;
class CurvePoint;
class SNW_RoadElement;

struct BindWeight;
//
//enum AnaPosVerdict
//{
//	APV_NOT_BOUND,
//	APV_MAY_BOUND,
//	//APV_BOUND,
//	APV_DETOUR,
//	//APV_ON_PATH,
//	APV_DIRECTION_FORWARD,
//	APV_DIRECTION_BACKWARD,
//	APV_DIRECTION_UNSURE
//};

struct PathPosIndex
{
	PathPosIndex():m_ctrl_pos_param(Fraction(-1, 1))
	{}
	PathPosIndex(const Fraction &ctrl_pos_param)
		:m_ctrl_pos_param(ctrl_pos_param)
	{}
	~PathPosIndex();
	PER_REF(SNW_RoadElement) m_snw_re;
	Fraction m_ctrl_pos_param;
	Pos2DEx m_cur_pos;
};
// TODO: zgr, 2008-01-15, 使用角度（以北为轴)

class Ana_Navigation
{
public:
	static double DIR_TOL_BACK; // 最大导航弧度偏差, 度
	static double DIR_TOL_FORTH; //最大发声弧度偏差 
	static double MAX_OFF_SET; // the max 0ff set pos distance from the road
	static double DEFAULT_TOLERANCE;
	static unsigned int MAX_DIR_ERROR_COUNT;

	// zgr,2008-01-15,道路绑定距离
	static double BOUND_MAX_OFF_SET;
private:
	static const double BOUND_MAX_OFF_SET_FAR; // 这里,far和near只是相对而言
	static const double BOUND_MAX_OFF_SET_NEAR;
public:
	Ana_Navigation()
		//: last_path(NULL),
		//last_cross_index(-1),
		//b_recalculated(false),m_tolerance(DEFAULT_TOLERANCE),
		//dir_error_count(0)
	{
		initTolerance();
	}
	~Ana_Navigation()
	{
		//clear();
	}
private:
	Ana_Navigation(const Ana_Navigation & r_val);
	Ana_Navigation & operator = (const Ana_Navigation & r_val);
public:
	double setTolerance(double tolerance);
public:	
		//zgr, 2008-01-15,
	void findPosState(const Pos2DEx & pos,double radian,PathResult * path_result, AnaPosVerdict * ana_pos_ver,
		CurvePoint * bound_pos, NavigationInfo * navi_info);

	static void resetLastPath();
	// condition to recalculate:
	// 1. the off_set cur_pos depart from the path exceed the tolerance 
	// 2. the cur_dir continued differ from the next_dir according to the path
	//zgr, 2008-01-14, speed:unit m/s
	int navigate( NavigationInfo ** nav_info, bool * to_recalculate, PathPosIndex ** path_pos_index, const Pos2DEx & pos,
		double radian,double speed, const PathResult & path_result);
private:
	int calNaviInfo(const Pos2DEx & pos,double radian, const PathResult & path_result,
		double * angel_to_next,NavigationInfo * nav_info,PathPosIndex * path_pos_index);
	// need it ??
	int navigateForVehicle(NavigationInfo ** nav_info, bool * to_recalculate, PathPosIndex ** path_pos_index, const Pos2DEx & pos, double dir, const PathResult & path_result);
private:
	// calPathPosIndex:
	// calculate the index of cur_pos in the whole path ctrl_points
	// cross_index: the path_segment where the cur_pos is at
	// pos_param: the param of the cur_pos at path_segment
	void calPathPosIndex(PathPosIndex ** path_pos_index, uint_32 cross_index, const Fraction &pos_param,const PathResult& path_result);
	// not to delete the target_cross, just use it for analysis
	void getNextCross(uint_32 * cross_index,double * dis_to_cross,double * dis_off_set,double * angle_to_next,
		PathPosIndex ** path_pos_index,
		const Pos2DEx & pos, const PathResult & path_result);
	//
	int calDirType(DirectionType * dir_type, uint_32 segment_index,const PathResult & path_result);
	// getTolerance, if not set the tolerance, use the default tolerance according to pos
	void initTolerance();
	// get the angle of the froam_start_to_next or from_front_to_end
	double getAngle(const PathSegment & path_segment, bool from_start);
	//    \ | /
	//    -   -
	//    / | \
	//
	DirectionType calDirType(double from, double to);
	void clear();
	// 只计算左右两个方向,且当路口的其中两个方向非常靠近时精确计算
	DirectionType calDirType_2(const vector<double> &v_dirs, double from, double to);
	int recalculate();

private:
	static const PathResult * last_path; // shallow copy, the addres  of the path result, to compare if it is the same with last time
	static uint_32 last_cross_index;
	// last_bind_param: 记录上次绑定的位置param，用于getNextCross时计算用
	static Fraction last_bind_param;
	// 记录上次偏移量，用于计算 重新计算路径的偏移量条件,
	static double last_offset;
	// if(b_recalculate && last_path_start_pos == cur_path_start_pos)
	//     it is too far from the gis_net_work, can not to navigation
	static bool b_recalculated;
	//
	static double m_tolerance;
	// 
	static unsigned int dir_error_count;
private:
	// zgr, 2008-01-15
	static unsigned int dir_back_count;
	static const unsigned int MAX_DIR_BACK_COUNT;
	static const double PRIORITY_HIGH_FOR_BIND_ROAD;
	static const double PRIORITY_LOW_FOR_BIND_ROAD;
	static const double LENGTH_FOR_GET_NEXT; // zgr, 2008-01-17,取下个路口的方向提示信息的路段距离值

	//static double last_offset_to_road; // zgr,2008-01-17,改为设置两个绑定距离
	//static int off_set_enlarge_count;// zgr,偏移量增大次数,如果连着增大两次,则将道路优先级降低

	static bool bindCalculator(const BindWeight & l_value,const BindWeight & r_value);
};

}
#endif