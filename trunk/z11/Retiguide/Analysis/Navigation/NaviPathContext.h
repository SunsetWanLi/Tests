#ifndef _GIS_NAVI_PATH_CONTEXT_H_
#define _GIS_NAVI_PATH_CONTEXT_H_ 

#include "../AnaEnumCommon.h"
//#include "../../TopoData/NaviNWTypes/CNW_Intersection.h"
#include "../../Common/Types/Fraction.h"
#include "../../../MapKit/Embedded/Application/PathResult.h"

using namespace Embedded;

namespace GIS_Embedded
{

	enum NWTypeEnum
	{
		SNW_JUNCTION_TYPE = 0x01010000,
		SNW_JUNCTION_BASE_TYPE = 0x01010100,
		SNW_JUNCTION_HIGH_TYPE = 0x01010200,
		SNW_ROAD_ELEMENT_TYPE = 0x01020000,
		SNW_ROAD_ELEMENT_BASE_TYPE = 0x01020100,
		SNW_ROAD_ELEMENT_HIGH_TYPE = 0x01020200,
		CNW_ROAD_TYPE = 0x02010000,
		CNW_INTERSECTION_TYPE = 0x02020000,
		CNW_ROUNDABOUT_TYPE = 0x02030000,
		CNW_INTERCHANGE_TYPE = 0x02040000,
		CNW_FINAL_TYPE = 0x02050000
	};

// ����·��������
// ���ڵ�ǰ��·��״̬���з�װ��
// ���ǵ�path_result���ݽ϶࣬������path_result ָ�룬���追��
// �߼��ϣ�����path_result�����������£�NaviPathContext��������
// ȷ���ⲿ�����path_result��NaviPathContext������֮ǰ��Ҫ���ͷŵ�
class NaviPathContext
{
public:
	// �������PathResultָ�룬��Ҫ��NaviPathContext�ڲ��ͷŸ�PathResult
	NaviPathContext():m_cur_index(0),m_cur_ps_param(Fraction()),m_path_result(NULL)
	{}
	NaviPathContext(uint_32 cur_index,const Fraction &cur_ps_param, const PathResult * path_result):m_cur_index(cur_index),m_cur_ps_param(cur_ps_param),m_path_result(path_result)
	{} 
	~NaviPathContext()
	{}
	//// �õ���ǰ���ڵĵ���Ԫ��
	//PER_REF(CNW_Object) getCurCNWObject()const;
	NWTypeEnum getCurCNWObjectType() const;

	//// �����ǰ���ڵ���Ԫ��Ϊ ������·Ԫ�صĻ��� ����ͨ��getNextNW_InterSection��ȡ��һ����ĵ����ڵ�Ԫ��
	//PER_REF(CNW_Object) getNextCNWJunction()const;
	NWTypeEnum getNextCNWJunctionType() const;
	//
	// ͨ���±�����õ���Ҫ��·�Σ����ڽ��е�������
	uint_32 getCurIndex()const;
	uint_32 getPathSegmentSize()const;
	const PathSegment * getPathSegment(uint_32 index)const;
	// NaviPathSegment: ���ڵ�����·�Σ�������2���е�·Ԫ�صıߣ�����˫��·�Ľ�����в����ڽ���ڵ�·��
	// ���ps_index֮��ĵ�һ���ɵ���·�Σ����ps_index����·��Ϊ��·Ԫ�أ��򷵻ظ�·�Σ�
	// ������������ֱ���ҵ���һ���ɵ���·��
	const PathSegment * getFirstNaviPathSegemnt(uint_32 ps_index)const;
	//
	const PathCross * getPathCross(uint_32 index)const;
	double getDisToInterSection()const; // ���ڵ���룬�����ǰ�ڽڵ��ڲ�������0
	double getDisToFinal()const;
	//
	DirectionType  calDirectionType(const PathSegment & l_ps, const PathSegment & r_ps)const;
private:
	double getAngle(const PathSegment & path_segment, bool from_start)const;
	DirectionType calDirType(double from, double to)const;
private:
	uint_32 m_cur_index;
	Fraction m_cur_ps_param;
	const PathResult * m_path_result; // ���ı�PathResult
};

}

#endif