#ifndef NAVIGATION_INFO_H_
#define NAVIGATION_INFO_H_
#include "../../Common/Types/basic_types.h"
#include "../../../MapKit/Embedded/Application/PathResult.h"
#include "../AnaEnumCommon.h"
#include "../../../CommonBase/MGWString.h"
#include <string>
using namespace std;
using namespace Embedded;
using namespace z11;
namespace GIS_Embedded
{

typedef enum
{
	SONT_UNKNOWN = 0x0,// no sound 
	SONT_RIGHT_NOW = 0x1,// cross
	SONT_HM_BELOW_TWO = 0x2, // 小于两百, 
	SONT_HM_TWO_SIX = 0x4, // 200-600之间
	SONT_KILO_METER = 0x8, //> 700 
	SONT_REACHED = 0x10 // reached
} SoundType ;

typedef enum
{
	SCST_CLOSE,
	SCST_UPDATE,
	SCST_OPEN
} ShowCrossStateType;

struct ShowCrossParam
{
	ShowCrossParam():m_remain_length(0),m_state(SCST_CLOSE),
		m_cur_segment(NULL), m_next_segment(NULL),
		m_dir_type(DIRT_UNKNOWN)
	{}

	double m_remain_length;
	ShowCrossStateType m_state;

	// 不要在外部删除
	const PathSegment * m_cur_segment; 
	const PathSegment * m_next_segment;
	Embedded::PathCross m_cur_cross;

	DirectionType m_dir_type;
};

class NavigationInfo
{
private:
	static z11::MGWString NAV_INFO_PREFIX;
	static z11::MGWString NAV_INFO_DIR[18]; // eight dir;
	static int NAV_INFO_SOUND_UPPER[15];
	static SoundType NAV_INFO_SOUND_TYPE[15];
	
	static int NAV_DISTANCE_SOUNT_UNIT;
	static int NAV_DISTANCE_SOUNT_UPPER;
public:
	static int NAV_TOLERANCE_CROSS; // 到达路口容忍距离
	static int NAV_TOLERANCE_TARGET; // 到达目的地的容忍距离
	static int NAV_TOLERANCE_FINAL_MAX;// 到达路线终点但是需要距离目的地的容忍距离

	static const double NAVI_DISTANCE_HINT_NEXT; // 提示下个路口信息的距离上限
	static const double NAVI_DISTANCE_SHOW_CROSS; // 提示路口放大操作的距离
	
	//重置声音信息
	static void reset()
	{
		last_st = SONT_UNKNOWN;
		last_dir = DIRT_UNKNOWN;

		//20090615 add duanjf
		special_last_dir = DIRT_UNKNOWN;

		to_sound_pre = true;

		//first_show_cross = true;
	}

	//重置声音信息 非重算路径时使用
	static void reset1()
	{
		last_st = SONT_UNKNOWN;
		last_dir = DIRT_UNKNOWN;
		to_sound_pre = true;
	}
public:
	NavigationInfo()
		:m_dir_type(DirectionType(0)),m_next_dir_type(DIRT_UNKNOWN), m_dis(0),m_next_dis(0), m_total_dis(0),m_off_set(0),m_speed(0)/*, last_st(SONT_UNKNOWN)*/
		,m_cur_segment(NULL), m_next_segment(NULL)// zgr, 2008-02-21
	{}
	NavigationInfo(DirectionType dir_type, double dis, double total_dis,const PathCross & path_cross,double off_set,double speed/*, SoundType sound_type = SONT_UNKNOWN*/);


// 	z11::MGWString toWString()const;
	bool isReachFinal()const;
	bool toSound(int * sound_distance, DirectionType * dir_type, DirectionType * next_dir_type)const;
	int toSound_int(int * sound_distance, DirectionType * dir_type, DirectionType * next_dir_type)const;//lw 2010/10/07

	// zgr, 2008-02-20, 显示路口,传出两个PathSegment指针，不在外面delete
	bool toShowCross(ShowCrossParam * sc_param)const;

	// 方向类型, m_next_dir_type: 下下个路口的信息
	DirectionType m_dir_type, m_next_dir_type;
	// 到达下一路口的距离
	double m_dis, m_next_dis; // remain distance to the next cross
	// 到达终点的剩余距离
	double m_total_dis; // remain distance to the target
	//直行路口计数
	int m_straight_crs_num;
	//直行距离计数
	double m_straight_length;
	// 下一路口
	Embedded::PathCross m_cross; // used for index of the PathResult->PathCross

	// zgr, 2008-02-21， 前后两个路段，不要释放这两指针
	const PathSegment * m_cur_segment; // 当前所在路段
	const PathSegment * m_next_segment; // 下一路段， 可能为空
	static bool first_show_cross;
	// zgr, 2008-01-13, 道路偏移量,如果太大就不发声  
	double m_off_set;
	//zgr, 2008-01-14
	double m_speed;// m/s

	// zgr, 2008-01-13, GPS_MAX_OFF_SET: 表示GPS最大可能的误差偏移量
	static double GPS_MAX_OFF_SET;
	// zgr, 2008-01-14, 用于与处理发声
	static bool to_sound_pre;
private:
	// zgr,更新当前发声，如果和 上次相同就返回false
	// my, 加入bool cross_changed如果路口改变，则需发声
	bool toUpdateSound(DirectionType * dir_type, SoundType cur_type, bool cross_changed)const;
	int toUpdateSound_int(DirectionType * dir_type, SoundType cur_type, bool cross_changed)const;//lw  2010/10/07

	static SoundType last_st;
	// my
	static DirectionType last_dir;
	//200090615 add duanjf
	static DirectionType special_last_dir;
	static ShowCrossStateType last_scst; // 上次路口显示状态值
	static PathCross last_cross;
};

}
#endif