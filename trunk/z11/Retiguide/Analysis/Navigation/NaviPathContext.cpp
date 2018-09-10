
#include "NaviPathContext.h"
//#include "../Path/PathResult.h"
#include "../../SpatialMath/SpatialMath.h"
#include "../../Common/Util/PureMath.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288 
#endif
namespace GIS_Embedded
{

	// 得到当前所在的导航元素
	NWTypeEnum NaviPathContext::getCurCNWObjectType()const
	{
		//PathSegment * cur_ps = getPathSegment(m_cur_index);
		//PER_REF(SNW_RoadElement) cur_edge = cur_ps->m_nw_edge;
		//PER_REF(CNW_Object) cur_cnw_obj = cur_edge->getParentObject();

		//return cur_cnw_obj;
		return CNW_ROAD_TYPE;
	}
	// 如果当前所在导航元素为 导航道路元素的话， 可以通过getNextNW_InterSection获取下一最近的导航节点元素
	NWTypeEnum NaviPathContext::getNextCNWJunctionType()const
	{
		NWTypeEnum cur_cnw_obj = getCurCNWObjectType();
		NWTypeEnum ret;
		switch(cur_cnw_obj)
		{
		case CNW_ROAD_TYPE:
			if(m_cur_index == getPathSegmentSize() - 1) // next is final
			{
				//CNW_Final final;
				//ret = final.getPrefCNWFinal();
				ret = CNW_FINAL_TYPE;
			}
			else
			{
				/*PathSegment * cur_ps = getPathSegment(m_cur_index);
				PER_REF(SNW_RoadElement) cur_edge = cur_ps->m_nw_edge;
				PER_REF(SNW_Junction) next_node = cur_edge->end();
				PER_REF(CNW_Object) next_cnw_obj = next_node->getParentObject();
				ret = next_cnw_obj;*/
				ret = CNW_INTERSECTION_TYPE;
			}

			break;
		case CNW_INTERSECTION_TYPE:
		case CNW_INTERCHANGE_TYPE:
		case CNW_ROUNDABOUT_TYPE:
			ret = cur_cnw_obj;
		default:
			break;
		}
		return ret;
	}
// 通过下标可以拿到想要的路段，用于进行导航分析
uint_32 NaviPathContext::getCurIndex()const
{
	return m_cur_index;
}

uint_32 NaviPathContext::getPathSegmentSize()const
{
	//uint_32 pc_size = m_path_result->m_crosses.size();
	return m_path_result->m_segments.size();
}

const PathSegment * NaviPathContext::getPathSegment(uint_32 index)const
{
	if(index < 0 || index > getPathSegmentSize() - 1)
	{
		assert(0);
		return NULL;
	}
	return &m_path_result->m_segments[index];
}
// NaviPathSegment: 用于导航的路段，即属于2层中道路元素的边，比如双线路的交叉口中不属于交叉口的路段
const PathSegment * NaviPathContext::getFirstNaviPathSegemnt(uint_32 ps_index)const
{
	uint_32 i = ps_index;
	/*uint_32 size = getPathSegmentSize();
	PathSegment * ret = NULL;

	for(;i < size; ++ i)
	{
		PathSegment * cur_ps = getPathSegment(i);
		PER_REF(SNW_RoadElement) nw_edge = cur_ps->m_nw_edge;
		PER_REF(CNW_Object) cnw_obj = nw_edge->getParentObject();
		assert(cnw_obj.isValid());

		if((cnw_obj->NWType() | CNW_ROAD_TYPE) == CNW_ROAD_TYPE)
		{
			ret = cur_ps;
			break;
		}
	}
	return ret;*/
	assert(0);
	return 0;
}

const PathCross * NaviPathContext::getPathCross(uint_32 index)const
{
	if(index < 0 || index > getPathSegmentSize() - 1)
	{	
		assert(0);
		return NULL;
	}
	return &m_path_result->m_crosses[index];
}

double NaviPathContext::getDisToInterSection()const
{
	if(getCurCNWObjectType() == CNW_INTERSECTION_TYPE) // 如果当前元素是节点，表示当前位于节点内部，返回0
		return 0;

	const PathSegment * cur_ps = getPathSegment(m_cur_index);
	double ret = SpatialMath::getDistance(cur_ps->m_ctrl_points, m_cur_ps_param ,Fraction(cur_ps->m_ctrl_points.size() - 1, 1));

	uint_32 i = m_cur_index + 1;
	uint_32 size = getPathSegmentSize();

	//if(i < size)
	//	ret += getPathSegment(i)->getLength();

	//for(;i < size; ++ i)
	//{
	//	cur_ps = getPathSegment(i);
	//	PER_REF(SNW_RoadElement) nw_edge = cur_ps->m_nw_edge;
	//	cnw_obj = nw_edge->getParentObject();
	//	assert(cnw_obj.isValid());
	//	unsigned long test_nw_type = cnw_obj->NWType();
	//	if(cnw_obj->NWType() | CNW_INTERSECTION_TYPE) // 下一节点元素
	//	{
	//		break;
	//	}

	//	ret += cur_ps->getLength();
	//}
	
	return ret;
}

double NaviPathContext::getDisToFinal()const
{
	const PathSegment * cur_ps = getPathSegment(m_cur_index);
	double ret = SpatialMath::getDistance(cur_ps->m_ctrl_points, m_cur_ps_param ,Fraction(cur_ps->m_ctrl_points.size() - 1, 1));

	uint_32 i = m_cur_index + 1;
	uint_32 size = getPathSegmentSize();

	for(;i < size; ++ i)
	{
		cur_ps = getPathSegment(i);
		ret += cur_ps->getLength();
	}
	
	return ret;
}
//
DirectionType NaviPathContext::calDirectionType(const PathSegment & from_ps, const PathSegment & to_ps)const
{
	DirectionType ret = DIRT_UNKNOWN;

	double angle_1 = getAngle(from_ps, false);
	double angle_2 = getAngle(to_ps, true);

	int tmp_dir = (int)calDirType(angle_1, angle_2);
//	tmp_dir |= int(calDirType_2(path_result.m_crosses[segment_index]->m_dirs,angle_1, angle_2));

	ret = (DirectionType)tmp_dir;

	return ret;
}

double NaviPathContext::getAngle(const PathSegment & path_segment, bool from_start)const
{
	assert(path_segment.m_ctrl_points.size() >= 2);

	uint_32 s_index = from_start ? 0 : path_segment.m_ctrl_points.size() - 2;
	Pos2DEx s_pos = path_segment.m_ctrl_points[s_index];
	Pos2DEx e_pos = path_segment.m_ctrl_points[s_index + 1];

	return SpatialMath::calLineNAngle(s_pos, e_pos);
}

DirectionType NaviPathContext::calDirType(double from, double to)const
{
	int ret = (int)DIRT_UNKNOWN;// 0x0
	double minus = to - from;
	if(minus < 0)
		minus += 2 * M_PI;
	
	int off_set = (int)(minus / (M_PI / 8)) % 16;
	if(PureMath::between(off_set, 1, 7))// [2,6) 0.25pi->0.75pi
		ret |= (int)DIRT_RIGHT;
	if(PureMath::between(off_set, 5,11)) // [6, 10) 0.75pi->1.25pi
		ret |= (int)DIRT_BACK;
	if(PureMath::between(off_set, 9, 15)) //[10, 14), 1.25pi->1.75pi
		ret |= (int)DIRT_LEFT;
	if(PureMath::between(off_set, 13, 16) || PureMath::between(off_set, 0, 3)) // 1.75pi->2pi 0->0.25pi
		ret |= (int)DIRT_FORTH;
		
	return (DirectionType)ret;
}

}