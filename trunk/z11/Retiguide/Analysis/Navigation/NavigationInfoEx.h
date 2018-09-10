#ifndef _GIS_NAVIGATIONINFOEX_H_
#define _GIS_NAVIGATIONINFOEX_H_

#include "../AnaEnumCommon.h"
namespace GIS_Embedded
{

class PathCross;
class PathSegment;


// ������Ϣ��ֻ�����¼��Ҫ����Ϣ����δ�������NaviHandler����������
class NavigationInfoEx
{
public:
	NavigationInfoEx()
		:m_dir_type(DirectionType(0)),m_next_dir_type(DIRT_UNKNOWN), m_dis(0),m_next_dis(0), m_total_dis(0),m_off_set(0),m_speed(0)/*, last_st(SONT_UNKNOWN)*/
		,m_cur_segment(NULL), m_next_segment(NULL) // zgr, 2008-02-21
	{}
	~NavigationInfoEx()
	{}

	// ��������, m_next_dir_type: ���¸�·�ڵ���Ϣ
	DirectionType m_dir_type, m_next_dir_type;
	// ������һ·�ڵľ���
	double m_dis, m_next_dis; // remain distance to the next cross
	// �����յ��ʣ�����
	double m_total_dis; // remain distance to the target
	// ��һ·��
	PathCross * m_cross; // used for index of the PathResult->PathCross
	// zgr, 2008-02-21�� ǰ������·�Σ�NavigationInfo ����delete����ָ��
	PathSegment * m_cur_segment; // ��ǰ����·��
	PathSegment * m_next_segment; // ��һ·�Σ� ����Ϊ��
	// zgr, 2008-01-13, ��·ƫ����,���̫��Ͳ�����  
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
