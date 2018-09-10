#ifndef _GIS_NAVI_BIND_H_
#define _GIS_NAVI_BIND_H_

#include "../../Common/Types/basic_types.h"
#include "../../Common/Types/Fraction.h"
#include "../../../MapKit/GPSWrapper/GPS_Info.h"
#include "../../../MapKit/Embedded/Application/PathResult.h"
#include "../AnaEnumCommon.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288 
#endif
using namespace IS_GIS;
using namespace Embedded;

namespace GIS_Embedded
{
class CurvePoint;
class NaviPathContext;
struct BindWeight;
class GlobalCrt;

struct Navi_GPS_Info : public GPS_Info
{
public:
	explicit Navi_GPS_Info(const GPS_Info & gps_info);
private:
	static GlobalCrt * CRT;
};


struct BindWeight
{
	//BindWeight():m_dis(DBL_MAX), m_dir(M_PI / 2) GRF,DBL_MAX no definition
	BindWeight():m_dis(INT_MAX), m_dir(M_PI / 2)
	{}
	double m_dis;
	double m_dir;
	static bool lessor(const BindWeight & l_value,const BindWeight & r_value)
	{
		return l_value.m_dis < r_value.m_dis;
	}
	bool operator!=(const BindWeight & l_value)const
	{
		return m_dis != l_value.m_dis || m_dir != l_value.m_dir;
	}
	static BindWeight NULL_BIND_WEIGHT;
};

class NaviBind
{
public:
	NaviBind(){}
	~NaviBind(){}
public:
	static double DIR_TOL_BACK; // 最大向后弧度偏差
	static double DIR_TOL_FORTH; //最大向前弧度偏差
	static double MAX_OFF_SET; // the max 0ff set pos distance from the road
	static unsigned int MAX_DIR_BACK_COUNT;

	// zgr,2008-01-15,道路绑定距离
	static double BOUND_MAX_OFF_SET;
	static double BOUND_MAX_OFF_SET_FAR; // 这里,far和near只是相对而言
	static double BOUND_MAX_OFF_SET_NEAR;

	static double PRIORITY_FOR_BIND_ROAD; // when bind, the path_result is given the higher priority, 路径绑定优先级
public:
	// 綁定：根据当前GPS以及当前路径进行绑定，
	// 输出： bound_cp绑定点，bind_state绑定状态，PathContext--如果绑定到路径上时生成路径上下文，导航提示用
	// Navi_GPS_Info不同于原始的GPS_Info： 坐标为投影，角度为为弧度
	void naviBind(const Navi_GPS_Info & gps_info, PathResult * path_result, Pos2DEx * bound_pos,AnaPosVerdict * bind_state, NaviPathContext * path_navi_context);
	static void resetLastPath(); // 路径重算时进行重置
private:
	//
	struct PathBindParam
	{
		uint_32 m_ps_index;// the subscript of cur_bound_path_segment, 当前绑定的路段位于路径的哪段
		Fraction m_pos_param;// the bound param of cur_bound_path_segment, 当前绑定位置位于绑定路段的具体位置
		double m_off_set;// the off_set of pos and bound_ps

		double m_bound_angle;// the current angle of cur_bound_pos(according to the bound_path_segment)
		Pos2DEx m_bound_pos;
	};

	void bindToPath(const Pos2DEx & pos, const PathResult & path_result, PathBindParam * pbp);
private:
	double getAngle(const vector<Pos2DEx> &ctrl_points, bool from_start);

	// calculator of pointBindPLine
	static bool bindCalculator(const BindWeight & l_value,const BindWeight & r_value);
private:
	static unsigned int dir_back_count;

private:
	static const PathResult * last_path; // shallow copy, the addres  of the path result, to compare if it is the same with last time
	static uint_32 last_ps_index;
	// last_bind_param: 记录上次绑定的位置param，用于getNextCross时计算用
	static Fraction last_bind_param;
	// 记录上次偏移量，用于计算 重新计算路径的偏移量条件,
	static double last_offset;

};

}

#endif
