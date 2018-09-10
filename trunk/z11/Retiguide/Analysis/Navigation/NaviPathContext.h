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

// 导航路径上下文
// 对于当前的路径状态进行封装；
// 考虑到path_result内容较多，仅保存path_result 指针，不需拷贝
// 逻辑上，仅当path_result有意义的情况下，NaviPathContext才有意义
// 确保外部传入的path_result在NaviPathContext仍引用之前不要被释放掉
class NaviPathContext
{
public:
	// 传入的是PathResult指针，不要在NaviPathContext内部释放该PathResult
	NaviPathContext():m_cur_index(0),m_cur_ps_param(Fraction()),m_path_result(NULL)
	{}
	NaviPathContext(uint_32 cur_index,const Fraction &cur_ps_param, const PathResult * path_result):m_cur_index(cur_index),m_cur_ps_param(cur_ps_param),m_path_result(path_result)
	{} 
	~NaviPathContext()
	{}
	//// 得到当前所在的导航元素
	//PER_REF(CNW_Object) getCurCNWObject()const;
	NWTypeEnum getCurCNWObjectType() const;

	//// 如果当前所在导航元素为 导航道路元素的话， 可以通过getNextNW_InterSection获取下一最近的导航节点元素
	//PER_REF(CNW_Object) getNextCNWJunction()const;
	NWTypeEnum getNextCNWJunctionType() const;
	//
	// 通过下标可以拿到想要的路段，用于进行导航分析
	uint_32 getCurIndex()const;
	uint_32 getPathSegmentSize()const;
	const PathSegment * getPathSegment(uint_32 index)const;
	// NaviPathSegment: 用于导航的路段，即属于2层中道路元素的边，比如双线路的交叉口中不属于交叉口的路段
	// 获得ps_index之后的第一个可导航路段，如果ps_index所在路段为道路元素，则返回该路段，
	// 否则，往后搜索直到找到第一个可导航路段
	const PathSegment * getFirstNaviPathSegemnt(uint_32 ps_index)const;
	//
	const PathCross * getPathCross(uint_32 index)const;
	double getDisToInterSection()const; // 到节点距离，如果当前在节点内部，返回0
	double getDisToFinal()const;
	//
	DirectionType  calDirectionType(const PathSegment & l_ps, const PathSegment & r_ps)const;
private:
	double getAngle(const PathSegment & path_segment, bool from_start)const;
	DirectionType calDirType(double from, double to)const;
private:
	uint_32 m_cur_index;
	Fraction m_cur_ps_param;
	const PathResult * m_path_result; // 不改变PathResult
};

}

#endif