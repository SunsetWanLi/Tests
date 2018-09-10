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
z11::MGWString NavigationInfo::NAV_INFO_PREFIX = WS(L" meters from the intersection:");// L"In the front, ";//L"ǰ��";
//z11::MGWString NavigationInfo::NAV_INFO_CROSS = L"Cross : "; //L"·��";
//z11::MGWString NavigationInfo::NAV_INFO_DISTANCE = L" with a distance of : ";//L"����";
//z11::MGWString NavigationInfo::NAV_INFO_DISTANCE_UNIT = L" meters. ";//L"��";
//z11::MGWString NavigationInfo::NAV_INFO_POS = L" coordinate :";//L"����";

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
	WS(L"Unknown condition"),//L"δ֪",//0x0
	WS(L"go straight"),//L"��ǰֱ��", //0x1
	WS(L"turn right"),//L"����ת", //0x2
	WS(L"make a slight right turn"),//L"����ǰ��ת", //0x3
	WS(L"make a U turn"),//L"���ת",//0x4
	WS(L""),//L"",// 0x5
	WS(L"make a sharp turn to the right"),//L"���Һ�ת",//0x6
	WS(L""),//L"",// 0x7
	WS(L"turn left"),//L"����ת",// 0x8
	WS(L"make a slight left turn"),//L"����ǰ��ת",//0x9
	WS(L""),//L"", //0xa
	WS(L""),//L"",//0xb
	WS(L"make a sharp turn to the left"),//L"�����ת",//0xc
	WS(L""),//L"",//0xd
	WS(L""),//L"",//0xe
	WS(L"You have arrived at the destination.")//L"�����յ�",//0xf
};
int NavigationInfo::NAV_INFO_SOUND_UPPER[] = 
{
	NAV_TOLERANCE_CROSS,// 
	75,// 50-75			//
	150,// 100-150
	199,// 150-199 ����not 149
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
	SONT_RIGHT_NOW/*SONT_FIFTY*/,// 50-75 // zgr, 2008-01-10, 50-100ֻ��һ�� TODO: ���¶���ͷ�����Щö��
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
	//// zgr, 2008-01-14, ���β�Ҫ����
	//if(!to_sound_pre)
	//{
	//	to_sound_pre = true;
	//	return false;
	//}
	//// ������ϸ�·�ڲ�ͬ,����
	//bool cross_changed = false;
	//if(last_cross != m_cross)
	//{
	//	last_cross = m_cross;
	//	cross_changed = true;
	//	reset1();
	//}

	//// zgr, �������Ч�ڵ㣬��Ҫ��������Ч�ڵ� ����·���յ�
	////if(! m_cross.isValidJunc())
	////	return false;

	//// zgr, 2008-01-13, ����ƫ������ֵ,��Ҫ��ʾ
	//if(m_off_set > GPS_MAX_OFF_SET)
	//	return false;

	////zgr, �����յ㣬zgr�������յ������棨Navigation�У��жϣ�����������
	////if(m_dir_type == DIRT_STOP && SpatialMath::isEqualToDoubleZero(m_dis))
	////{
	////	*sound_distance = 0;
	////	return toUpdateSound(dir_type, SONT_REACHED);
	////}

	//int s_dis = (int)(m_dis - m_speed * 1);// zgr, Ԥ��1s����룬1s����GPS�ı����ӳ�
	//if(s_dis < 0)
	//	s_dis = 0;

	//	//s_dis = 170;

	//*sound_distance = s_dis;
	//*next_dir_type = DIRT_UNKNOWN;
	//// zgr������700��,��һ��,�Һ����¸�·��
	//if(s_dis >= NAV_DISTANCE_SOUNT_UPPER)
	//{
	//	return toUpdateSound(dir_type,SONT_KILO_METER, cross_changed);
	//}

	//int cur_st = s_dis / NAV_DISTANCE_SOUNT_UNIT;
	////zgr, ����·��, ��Ҫ��ʾ�¸�·��
	//if(/*cur_st == 0 */NAV_INFO_SOUND_TYPE[cur_st] == SONT_RIGHT_NOW && s_dis < NAV_INFO_SOUND_UPPER[cur_st])
	//{
	//	*sound_distance = 0;
	//	return toUpdateSound(dir_type,NAV_INFO_SOUND_TYPE[cur_st], cross_changed);
	//}

	//// zgr, ���¸�·����һ������,��ʾ������Ϣ,�����ж��Ƿ���Ҫ��ʾ�¸�·����Ϣ
	//if(PureMath::between(s_dis, NAV_DISTANCE_SOUNT_UNIT * cur_st, NAV_INFO_SOUND_UPPER[cur_st]))
	//{
	//	/*if(NAV_INFO_SOUND_TYPE[cur_st] == last_st)
	//		return false;*/
	//	// ����¸�·�εĳ���С��NAVI_DISTANCE_HINT_NEXT������Ҫ����ʾ
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
// 	// zgr, 2008-01-14, ���β�Ҫ����
// 	if(!to_sound_pre)
// 	{
// 		to_sound_pre = true;
// 		return -1;
// 	}
// 	// ������ϸ�·�ڲ�ͬ,����
// 	bool cross_changed = false;
// 	if(last_cross != m_cross)
// 	{
// 		last_cross = m_cross;
// 		cross_changed = true;
// 		reset1();
// 	}
// 
// 	// zgr, �������Ч�ڵ㣬��Ҫ��������Ч�ڵ� ����·���յ�
// 	//if(! m_cross.isValidJunc())
// 	//	return -1;
// 
// 	// zgr, 2008-01-13, ����ƫ������ֵ,��Ҫ��ʾ
// 	if(m_off_set > GPS_MAX_OFF_SET)
// 		return -1;
// 
// 	//zgr, �����յ㣬zgr�������յ������棨Navigation�У��жϣ�����������
// 	//if(m_dir_type == DIRT_STOP && SpatialMath::isEqualToDoubleZero(m_dis))
// 	//{
// 	//	*sound_distance = 0;
// 	//	return toUpdateSound(dir_type, SONT_REACHED);
// 	//}
// 
// 	int s_dis = (int)(m_dis - m_speed * 1);// zgr, Ԥ��1s����룬1s����GPS�ı����ӳ�
// 	if(s_dis < 0)
// 		s_dis = 0;
// 
// 	//s_dis = 170;
// 
// 	*sound_distance = s_dis;
// 	*next_dir_type = DIRT_UNKNOWN;
// 	// zgr������700��,��һ��,�Һ����¸�·��  ��������700�ױ�һ�Σ������Ҫ���� lw
// 	if(s_dis >= NAV_DISTANCE_SOUNT_UPPER)
// 	{
// 		return toUpdateSound_int(dir_type,SONT_KILO_METER, cross_changed);
// 	}
// 
// 	int cur_st = s_dis / NAV_DISTANCE_SOUNT_UNIT;
// 	//zgr, ����·��, ��Ҫ��ʾ�¸�·��
// 	if(/*cur_st == 0 */NAV_INFO_SOUND_TYPE[cur_st] == SONT_RIGHT_NOW && s_dis < NAV_INFO_SOUND_UPPER[cur_st])
// 	{
// 		*sound_distance = 0;
// 		return toUpdateSound_int(dir_type,NAV_INFO_SOUND_TYPE[cur_st], cross_changed);
// 	}
// 
// 	// zgr, ���¸�·����һ������,��ʾ������Ϣ,�����ж��Ƿ���Ҫ��ʾ�¸�·����Ϣ
// 	if(PureMath::between(s_dis, NAV_DISTANCE_SOUNT_UNIT * cur_st, NAV_INFO_SOUND_UPPER[cur_st]))
// 	{
// 		/*if(NAV_INFO_SOUND_TYPE[cur_st] == last_st)
// 		return false;*/
// 		// ����¸�·�εĳ���С��NAVI_DISTANCE_HINT_NEXT������Ҫ����ʾ
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
	if(special_last_dir == DIRT_FORTH && m_dir_type == DIRT_FORTH )	//�����һ��·����ֱ�У����·������ֱ�У��Ͳ��ٱ���  //ͬһ��·�Σ��Ѿ�����ֱ���ˣ��Ͳ��ٱ���
	{
		ret = false;
	}
	
	//if(special_last_dir == DIRT_FORTH && m_dir_type == DIRT_FORTH )	//ͬһ��·�Σ��Ѿ�����ֱ���ˣ��Ͳ��ٱ���
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
	if(special_last_dir == DIRT_FORTH && m_dir_type == DIRT_FORTH )	//�����һ��·����ֱ�У����·������ֱ�У��Ͳ��ٱ���  //ͬһ��·�Σ��Ѿ�����ֱ���ˣ��Ͳ��ٱ���
	{
		ret = 0;
	}

	//if(special_last_dir == DIRT_FORTH && m_dir_type == DIRT_FORTH )	//ͬһ��·�Σ��Ѿ�����ֱ���ˣ��Ͳ��ٱ���
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
	// zgr, 2008-01-13, ����ƫ������ֵ,��Ҫ��ʾ
	// zgr, ·����ʾ��Ϣ������������ʾ
	if(m_off_set > GPS_MAX_OFF_SET || m_cur_segment == NULL || m_next_segment == NULL || m_dir_type == DIRT_UNKNOWN)
	{
		if(last_scst == SCST_OPEN || last_scst == SCST_UPDATE) // �Ѿ��򿪵ģ���֮�ر�
		{
			last_scst = SCST_CLOSE;
			first_show_cross = true;
			return true;
		}
		return false;
	}

	ShowCrossStateType sc_state = SCST_CLOSE;
	// ������ϸ�·�ڲ�ͬ,����,�ر�·�ڷŴ�
	if(last_cross != m_cross)
	{
		//del by my: ģ�⵼��ʽ����
		//last_cross = m_cross;
		ret = last_scst != SCST_CLOSE; // �ϴ�·���Ѿ��رգ�����Ҫ�ٲ���
		last_scst = SCST_CLOSE;
		first_show_cross = true;
		//reset();
	}

	int s_dis = (int)(m_dis);// zgr, Ԥ��1s����룬1s����GPS�ı����ӳ� 
	if(s_dis < 0)
		s_dis = 0;
	
	/*int cur_st = s_dis / NAV_DISTANCE_SOUNT_UNIT;*/
	//zgr, ����·��, ���������ж�һ��,��ǰ·������Ч����Ҫ�Ŵ���Ч�ڵ����·���յ�
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