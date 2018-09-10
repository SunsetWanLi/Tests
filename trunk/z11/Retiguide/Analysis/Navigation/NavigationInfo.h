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
	SONT_HM_BELOW_TWO = 0x2, // С������, 
	SONT_HM_TWO_SIX = 0x4, // 200-600֮��
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

	// ��Ҫ���ⲿɾ��
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
	static int NAV_TOLERANCE_CROSS; // ����·�����̾���
	static int NAV_TOLERANCE_TARGET; // ����Ŀ�ĵص����̾���
	static int NAV_TOLERANCE_FINAL_MAX;// ����·���յ㵫����Ҫ����Ŀ�ĵص����̾���

	static const double NAVI_DISTANCE_HINT_NEXT; // ��ʾ�¸�·����Ϣ�ľ�������
	static const double NAVI_DISTANCE_SHOW_CROSS; // ��ʾ·�ڷŴ�����ľ���
	
	//����������Ϣ
	static void reset()
	{
		last_st = SONT_UNKNOWN;
		last_dir = DIRT_UNKNOWN;

		//20090615 add duanjf
		special_last_dir = DIRT_UNKNOWN;

		to_sound_pre = true;

		//first_show_cross = true;
	}

	//����������Ϣ ������·��ʱʹ��
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

	// zgr, 2008-02-20, ��ʾ·��,��������PathSegmentָ�룬��������delete
	bool toShowCross(ShowCrossParam * sc_param)const;

	// ��������, m_next_dir_type: ���¸�·�ڵ���Ϣ
	DirectionType m_dir_type, m_next_dir_type;
	// ������һ·�ڵľ���
	double m_dis, m_next_dis; // remain distance to the next cross
	// �����յ��ʣ�����
	double m_total_dis; // remain distance to the target
	//ֱ��·�ڼ���
	int m_straight_crs_num;
	//ֱ�о������
	double m_straight_length;
	// ��һ·��
	Embedded::PathCross m_cross; // used for index of the PathResult->PathCross

	// zgr, 2008-02-21�� ǰ������·�Σ���Ҫ�ͷ�����ָ��
	const PathSegment * m_cur_segment; // ��ǰ����·��
	const PathSegment * m_next_segment; // ��һ·�Σ� ����Ϊ��
	static bool first_show_cross;
	// zgr, 2008-01-13, ��·ƫ����,���̫��Ͳ�����  
	double m_off_set;
	//zgr, 2008-01-14
	double m_speed;// m/s

	// zgr, 2008-01-13, GPS_MAX_OFF_SET: ��ʾGPS�����ܵ����ƫ����
	static double GPS_MAX_OFF_SET;
	// zgr, 2008-01-14, �����봦����
	static bool to_sound_pre;
private:
	// zgr,���µ�ǰ����������� �ϴ���ͬ�ͷ���false
	// my, ����bool cross_changed���·�ڸı䣬���跢��
	bool toUpdateSound(DirectionType * dir_type, SoundType cur_type, bool cross_changed)const;
	int toUpdateSound_int(DirectionType * dir_type, SoundType cur_type, bool cross_changed)const;//lw  2010/10/07

	static SoundType last_st;
	// my
	static DirectionType last_dir;
	//200090615 add duanjf
	static DirectionType special_last_dir;
	static ShowCrossStateType last_scst; // �ϴ�·����ʾ״ֵ̬
	static PathCross last_cross;
};

}
#endif