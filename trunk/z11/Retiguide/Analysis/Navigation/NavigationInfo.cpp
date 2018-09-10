#include "NavigationInfo.h"
#include "../../SpatialMath/SpatialMath.h"
#include "../../Common/Util/PureMath.h"
#include "../../../System/Shared/Utility.h"
#include <assert.h>
#include <cstdlib>

#ifdef _LOG_INFO
#include "../../Common/Util/ErrorInfo.h"
#endif
using namespace std;
using namespace z11;

namespace GIS_Embedded
{
z11::MGWString NavigationInfo::NAV_INFO_PREFIX = WS(L" meters from the intersection:");// L"In the front, ";//L"前方";
//z11::MGWString NavigationInfo::NAV_INFO_CROSS = L"Cross : "; //L"路口";
//z11::MGWString NavigationInfo::NAV_INFO_DISTANCE = L" with a distance of : ";//L"距离";
//z11::MGWString NavigationInfo::NAV_INFO_DISTANCE_UNIT = L" meters. ";//L"米";
//z11::MGWString NavigationInfo::NAV_INFO_POS = L" coordinate :";//L"坐标";

int NavigationInfo::NAV_DISTANCE_SOUNT_UNIT = 50;
int NavigationInfo::NAV_DISTANCE_SOUNT_UPPER = 700;

int NavigationInfo::NAV_TOLERANCE_CROSS = 49;
//int NavigationInfo::NAV_TOLERANCE_TARGET = 70;
int NavigationInfo::NAV_TOLERANCE_TARGET = 100;
int NavigationInfo::NAV_TOLERANCE_FINAL_MAX = 700;

SoundType NavigationInfo::last_st = SONT_UNKNOWN;
DirectionType NavigationInfo::last_dir = DIRT_UNKNOWN;

//20090615 add duanjf
DirectionType NavigationInfo::special_last_dir = DIRT_UNKNOWN;
ShowCrossStateType NavigationInfo::last_scst = SCST_CLOSE;
PathCross NavigationInfo::last_cross = PathCross();
// zgr, 2008-01-13
double NavigationInfo::GPS_MAX_OFF_SET  = 32;// meters
bool NavigationInfo::to_sound_pre = true;
bool NavigationInfo::first_show_cross = true;

const double NavigationInfo::NAVI_DISTANCE_HINT_NEXT = 200;
const double NavigationInfo::NAVI_DISTANCE_SHOW_CROSS = 300;

z11::MGWString NavigationInfo::NAV_INFO_DIR[] = 
{
	WS(L"Unknown condition"),//L"未知",//0x0
	WS(L"go straight"),//L"向前直行", //0x1
	WS(L"turn right"),//L"向右转", //0x2
	WS(L"make a slight right turn"),//L"向右前方转", //0x3
	WS(L"make a U turn"),//L"向后转",//0x4
	WS(L""),//L"",// 0x5
	WS(L"make a sharp turn to the right"),//L"向右后方转",//0x6
	WS(L""),//L"",// 0x7
	WS(L"turn left"),//L"向左转",// 0x8
	WS(L"make a slight left turn"),//L"向左前方转",//0x9
	WS(L""),//L"", //0xa
	WS(L""),//L"",//0xb
	WS(L"make a sharp turn to the left"),//L"向左后方转",//0xc
	WS(L""),//L"",//0xd
	WS(L""),//L"",//0xe
	WS(L"You have arrived at the destination.")//L"到达终点",//0xf
};
int NavigationInfo::NAV_INFO_SOUND_UPPER[] = 
{
	NAV_TOLERANCE_CROSS,// 
	75,// 50-75			//
	150,// 100-150
	199,// 150-199 。。not 149
	250,// 200-250
	249,// 250-250
	350, //300-350
	349, //350-450
	450,//400-450
	449,// 450- 450
	550,// 500-550
	549,//550
	650,//600-650
	649,//650		// 
	NAV_DISTANCE_SOUNT_UPPER,//>= 700
};
SoundType NavigationInfo::NAV_INFO_SOUND_TYPE[] = 
{
	SONT_RIGHT_NOW,// 
	SONT_RIGHT_NOW/*SONT_FIFTY*/,// 50-75 // zgr, 2008-01-10, 50-100只讲一次 TODO: 重新定义和分配这些枚举
	SONT_HM_TWO_SIX,// 100-150				//duanjf SONT_HM_BELOW_TWO--->SONT_HM_TWO_SIX
	SONT_HM_TWO_SIX,// 150-199				//duanjf SONT_HM_BELOW_TWO--->SONT_HM_TWO_SIX
	SONT_HM_TWO_SIX,// 200-249
	SONT_HM_TWO_SIX,// 250-299
	SONT_HM_TWO_SIX, //300-349
	SONT_HM_TWO_SIX, //350-399
	SONT_HM_TWO_SIX,//400-449
	SONT_HM_TWO_SIX,// 450-499
	SONT_HM_TWO_SIX,// 500
	SONT_HM_TWO_SIX,//550
	SONT_HM_TWO_SIX,//600
	SONT_HM_TWO_SIX,//650
	SONT_KILO_METER,// >=700
};
NavigationInfo::NavigationInfo(DirectionType dir_type, double dis,double total_dis, const PathCross & path_cross,double off_set,double speed/*, SoundType sound_type*/)
	:m_dir_type(dir_type),m_dis(dis),m_total_dis(total_dis),/*add begin*/m_straight_crs_num(0),m_straight_length(0),/*add end*/m_cross(path_cross),m_off_set(off_set),m_speed(speed)/*, last_st(sound_type)*/
	, m_cur_segment(NULL), m_next_segment(NULL) // zgr, 2008-02-21
{}
// z11::MGWString NavigationInfo::toWString()const
// {
// 	z11::MGWString ret;
// 	if(m_dir_type == DIRT_STOP && SpatialMath::isEqualToDoubleZero(m_dis))
// 	{
// 		ret = WS(L"You have arrived at the destination.");
// 		return ret;
// 	}
// 	ret += z11::Utility::BuiltInTypeToWString(m_dis); // 
// 
// 	ret += NAV_INFO_PREFIX; 
// 
// 	assert(m_dir_type != DIRT_UNKNOWN);
// 	ret += NAV_INFO_DIR[(int)m_dir_type];
// 
// 	return ret;
// }

bool NavigationInfo::toSound(int * sound_distance, DirectionType * dir_type, DirectionType * next_dir_type)const
{
	//// zgr, 2008-01-14, 本次不要发声
	//if(!to_sound_pre)
	//{
	//	to_sound_pre = true;
	//	return false;
	//}
	//// 如果和上个路口不同,重置
	//bool cross_changed = false;
	//if(last_cross != m_cross)
	//{
	//	last_cross = m_cross;
	//	cross_changed = true;
	//	reset1();
	//}

	//// zgr, 如果是无效节点，则不要发声，无效节点 包括路径终点
	////if(! m_cross.isValidJunc())
	////	return false;

	//// zgr, 2008-01-13, 超过偏移容忍值,不要提示
	//if(m_off_set > GPS_MAX_OFF_SET)
	//	return false;

	////zgr, 到达终点，zgr，到达终点在外面（Navigation中）判断，不在这里做
	////if(m_dir_type == DIRT_STOP && SpatialMath::isEqualToDoubleZero(m_dis))
	////{
	////	*sound_distance = 0;
	////	return toUpdateSound(dir_type, SONT_REACHED);
	////}

	//int s_dis = (int)(m_dis - m_speed * 1);// zgr, 预测1s后距离，1s包含GPS的本身延迟
	//if(s_dis < 0)
	//	s_dis = 0;

	//	//s_dis = 170;

	//*sound_distance = s_dis;
	//*next_dir_type = DIRT_UNKNOWN;
	//// zgr，操作700米,报一次,且忽略下个路口
	//if(s_dis >= NAV_DISTANCE_SOUNT_UPPER)
	//{
	//	return toUpdateSound(dir_type,SONT_KILO_METER, cross_changed);
	//}

	//int cur_st = s_dis / NAV_DISTANCE_SOUNT_UNIT;
	////zgr, 到达路口, 不要提示下个路口
	//if(/*cur_st == 0 */NAV_INFO_SOUND_TYPE[cur_st] == SONT_RIGHT_NOW && s_dis < NAV_INFO_SOUND_UPPER[cur_st])
	//{
	//	*sound_distance = 0;
	//	return toUpdateSound(dir_type,NAV_INFO_SOUND_TYPE[cur_st], cross_changed);
	//}

	//// zgr, 距下个路口有一定距离,提示距离信息,并且判断是否需要提示下个路口信息
	//if(PureMath::between(s_dis, NAV_DISTANCE_SOUNT_UNIT * cur_st, NAV_INFO_SOUND_UPPER[cur_st]))
	//{
	//	/*if(NAV_INFO_SOUND_TYPE[cur_st] == last_st)
	//		return false;*/
	//	// 如果下个路段的长度小于NAVI_DISTANCE_HINT_NEXT，则需要做提示
	//	//int test = 0;
	//	if(m_next_dis <= NAVI_DISTANCE_HINT_NEXT)
	//	{
	//		*next_dir_type = m_next_dir_type;
	//	}

	//	return toUpdateSound(dir_type, NAV_INFO_SOUND_TYPE[cur_st], cross_changed);
	//}

	//*sound_distance = s_dis;
	//*dir_type = m_dir_type;
	//*next_dir_type = m_next_dir_type;
	return false;
}
int NavigationInfo::toSound_int(int * sound_distance, DirectionType * dir_type, DirectionType * next_dir_type)const
{
// 	// zgr, 2008-01-14, 本次不要发声
// 	if(!to_sound_pre)
// 	{
// 		to_sound_pre = true;
// 		return -1;
// 	}
// 	// 如果和上个路口不同,重置
// 	bool cross_changed = false;
// 	if(last_cross != m_cross)
// 	{
// 		last_cross = m_cross;
// 		cross_changed = true;
// 		reset1();
// 	}
// 
// 	// zgr, 如果是无效节点，则不要发声，无效节点 包括路径终点
// 	//if(! m_cross.isValidJunc())
// 	//	return -1;
// 
// 	// zgr, 2008-01-13, 超过偏移容忍值,不要提示
// 	if(m_off_set > GPS_MAX_OFF_SET)
// 		return -1;
// 
// 	//zgr, 到达终点，zgr，到达终点在外面（Navigation中）判断，不在这里做
// 	//if(m_dir_type == DIRT_STOP && SpatialMath::isEqualToDoubleZero(m_dis))
// 	//{
// 	//	*sound_distance = 0;
// 	//	return toUpdateSound(dir_type, SONT_REACHED);
// 	//}
// 
// 	int s_dis = (int)(m_dis - m_speed * 1);// zgr, 预测1s后距离，1s包含GPS的本身延迟
// 	if(s_dis < 0)
// 		s_dis = 0;
// 
// 	//s_dis = 170;
// 
// 	*sound_distance = s_dis;
// 	*next_dir_type = DIRT_UNKNOWN;
// 	// zgr，操作700米,报一次,且忽略下个路口  理论上是700米报一次，这个需要测试 lw
// 	if(s_dis >= NAV_DISTANCE_SOUNT_UPPER)
// 	{
// 		return toUpdateSound_int(dir_type,SONT_KILO_METER, cross_changed);
// 	}
// 
// 	int cur_st = s_dis / NAV_DISTANCE_SOUNT_UNIT;
// 	//zgr, 到达路口, 不要提示下个路口
// 	if(/*cur_st == 0 */NAV_INFO_SOUND_TYPE[cur_st] == SONT_RIGHT_NOW && s_dis < NAV_INFO_SOUND_UPPER[cur_st])
// 	{
// 		*sound_distance = 0;
// 		return toUpdateSound_int(dir_type,NAV_INFO_SOUND_TYPE[cur_st], cross_changed);
// 	}
// 
// 	// zgr, 距下个路口有一定距离,提示距离信息,并且判断是否需要提示下个路口信息
// 	if(PureMath::between(s_dis, NAV_DISTANCE_SOUNT_UNIT * cur_st, NAV_INFO_SOUND_UPPER[cur_st]))
// 	{
// 		/*if(NAV_INFO_SOUND_TYPE[cur_st] == last_st)
// 		return false;*/
// 		// 如果下个路段的长度小于NAVI_DISTANCE_HINT_NEXT，则需要做提示
// 		//int test = 0;
// 		if(m_next_dis <= NAVI_DISTANCE_HINT_NEXT)
// 		{
// 			*next_dir_type = m_next_dir_type;
// 		}
// 
// 		return toUpdateSound_int(dir_type, NAV_INFO_SOUND_TYPE[cur_st], cross_changed);
// 	}
// 
// 	*sound_distance = s_dis;
// 	*dir_type = m_dir_type;
// 	*next_dir_type = m_next_dir_type;
	return -1;
}
/*
bool NavigationInfo::toUpdateSound(DirectionType * dir_type, SoundType cur_type, bool cross_changed)const
{
	*dir_type = m_dir_type;
	if(last_st == cur_type && last_dir == m_dir_type && !cross_changed)
		return false;
	last_st = cur_type;
	last_dir = m_dir_type;
	return true;
}

*/
bool NavigationInfo::toUpdateSound(DirectionType * dir_type, SoundType cur_type, bool cross_changed)const
{
	*dir_type = m_dir_type;
	if(last_st == cur_type && last_dir == m_dir_type && !cross_changed)
		return false;

	//20090615 add begin duanjf
	bool ret = true;
	if(special_last_dir == DIRT_FORTH && m_dir_type == DIRT_FORTH )	//如果上一个路口是直行，这个路个还是直行，就不再报了  //同一个路段，已经报过直行了，就不再报了
	{
		ret = false;
	}
	
	//if(special_last_dir == DIRT_FORTH && m_dir_type == DIRT_FORTH )	//同一个路段，已经报过直行了，就不再报了
	//{
	//	ret = false;
	//}
	//20090615 add end duanjf

	last_st = cur_type;
	last_dir = m_dir_type;
	special_last_dir = m_dir_type;
	return ret;
}
int NavigationInfo::toUpdateSound_int(DirectionType * dir_type, SoundType cur_type, bool cross_changed)const
{
	*dir_type = m_dir_type;
	if(last_st == cur_type && last_dir == m_dir_type && !cross_changed)
		return -1;

	//20090615 add begin duanjf
	int ret = 1;
	//lw  101007
	if(special_last_dir == DIRT_FORTH && m_dir_type == DIRT_FORTH )	//如果上一个路口是直行，这个路个还是直行，就不再报了  //同一个路段，已经报过直行了，就不再报了
	{
		ret = 0;
	}

	//if(special_last_dir == DIRT_FORTH && m_dir_type == DIRT_FORTH )	//同一个路段，已经报过直行了，就不再报了
	//{
	//	ret = false;
	//}
	//20090615 add end duanjf

	last_st = cur_type;
	last_dir = m_dir_type;
	special_last_dir = m_dir_type;
	return ret;
}

bool NavigationInfo::isReachFinal()const
{
	return m_dir_type == DIRT_STOP && SpatialMath::isEqualToDoubleZero(m_dis);
}

bool NavigationInfo::toShowCross(ShowCrossParam * sc_param)const
{
	bool ret = false;
	// zgr, 2008-01-13, 超过偏移容忍值,不要提示
	// zgr, 路口提示信息不完整，不提示
	if(m_off_set > GPS_MAX_OFF_SET || m_cur_segment == NULL || m_next_segment == NULL || m_dir_type == DIRT_UNKNOWN)
	{
		if(last_scst == SCST_OPEN || last_scst == SCST_UPDATE) // 已经打开的，将之关闭
		{
			last_scst = SCST_CLOSE;
			first_show_cross = true;
			return true;
		}
		return false;
	}

	ShowCrossStateType sc_state = SCST_CLOSE;
	// 如果和上个路口不同,重置,关闭路口放大
	if(last_cross != m_cross)
	{
		//del by my: 模拟导航式两个
		//last_cross = m_cross;
		ret = last_scst != SCST_CLOSE; // 上次路口已经关闭，则不需要再操作
		last_scst = SCST_CLOSE;
		first_show_cross = true;
		//reset();
	}

	int s_dis = (int)(m_dis);// zgr, 预测1s后距离，1s包含GPS的本身延迟 
	if(s_dis < 0)
		s_dis = 0;
	
	/*int cur_st = s_dis / NAV_DISTANCE_SOUNT_UNIT;*/
	//zgr, 到达路口, 和声音的判断一致,当前路口若无效，则不要放大，无效节点包括路径终点
	if(s_dis < NAVI_DISTANCE_SHOW_CROSS /*&& m_cross.isValidJunc()*//*to_show_cross && NAV_INFO_SOUND_TYPE[cur_st] == SONT_RIGHT_NOW && s_dis < NAV_INFO_SOUND_UPPER[cur_st]*/)
	{
		if(first_show_cross)
		{
			first_show_cross = false;
			sc_state = SCST_OPEN;
		}
		else
		{
			sc_state = SCST_UPDATE;
		}
		ret = true;
	}
	
	if(ret)
	{
		sc_param->m_cur_segment = m_cur_segment;
		sc_param->m_cur_cross = m_cross;
		sc_param->m_next_segment = m_next_segment;
		sc_param->m_dir_type = m_dir_type;
		sc_param->m_remain_length = s_dis;
		sc_param->m_state = sc_state;
	}

	last_scst = sc_state;
	return ret;
}

}