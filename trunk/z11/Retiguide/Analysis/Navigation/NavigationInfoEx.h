#ifndef _GIS_NAVIGATIONINFOEX_H_
#define _GIS_NAVIGATIONINFOEX_H_

#include "../AnaEnumCommon.h"
namespace GIS_Embedded
{

class PathCross;
class PathSegment;


// 导航信息，只负责记录需要的信息，如何处理，交给NaviHandler及其子类来
class NavigationInfoEx
{
public:
	NavigationInfoEx()
		:m_dir_type(DirectionType(0)),m_next_dir_type(DIRT_UNKNOWN), m_dis(0),m_next_dis(0), m_total_dis(0),m_off_set(0),m_speed(0)/*, last_st(SONT_UNKNOWN)*/
		,m_cur_segment(NULL), m_next_segment(NULL) // zgr, 2008-02-21
	{}
	~NavigationInfoEx()
	{}

	// 方向类型, m_next_dir_type: 下下个路口的信息
	DirectionType m_dir_type, m_next_dir_type;
	// 到达下一路口的距离
	double m_dis, m_next_dis; // remain distance to the next cross
	// 到达终点的剩余距离
	double m_total_dis; // remain distance to the target
	// 下一路口
	PathCross * m_cross; // used for index of the PathResult->PathCross
	// zgr, 2008-02-21， 前后两个路段，NavigationInfo 不能delete这两指针
	PathSegment * m_cur_segment; // 当前所在路段
	PathSegment * m_next_segment; // 下一路段， 可能为空
	// zgr, 2008-01-13, 道路偏移量,如果太大就不发声  
	double m_off_set;// TODEL, no use
	//zgr, 2008-01-14
	double m_speed;// m/s
	union m_special
	{
		int reach_final_state;
	};
};

}

#endif
