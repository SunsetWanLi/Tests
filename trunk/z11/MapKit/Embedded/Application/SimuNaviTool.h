#include <vector>
#include "../BasicType/BasicGMType.h"
using namespace std;

namespace IS_GIS
{
	struct GPS_Info;
}


namespace Embedded
{
	struct SimParam
	{
		SimParam():m_speed(30), m_stop_times(2)
		{}
		float m_speed; // 和GPS_Info的速度单位一致，均为海里
		int m_stop_times; // 到终点时停顿多少次
		int max_stop_per_line; //每条路中最多生成的GPS数量（防止路过长，长时间在某条路上）
	};
	class Line;
	class PathResult;
	using IS_GIS::GPS_Info;	

	class SimuNaviTool
	{
	public:
		// 根据当前路径和模拟参数，生成模拟的GPS序列，目前仅是控制生成序列，以及在路口处停顿
		// 可考虑使用速度来模拟
		static int transPath2GPSData(const PathResult &route,const SimParam & sim_param, vector<GPS_Info> * v_gps);
		static int getInterCtrlPoints(const vector<ScreenPos2D> & ctrl_points, double inter_dis, vector<ScreenPos2D> * inter_ctrl_points, vector<double> *dirs);
		// 从两点之间获得插值点序列
		static int getInterpolations(const ScreenPos2D & start_pos, const ScreenPos2D & end_pos, double inter_dis, vector<ScreenPos2D> * inter_pols);

	};

	
}