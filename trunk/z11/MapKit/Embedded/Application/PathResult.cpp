#include "PathResult.h"
#include "../../../Retiguide/SpatialMath/SpatialMath.h"
#include "../../../System/Shared/MGLog.h"

namespace Embedded
{
	double PathSegment::getLength() const
	{
		if(m_length < 0)
		{
			m_length = GIS_Embedded::SpatialMath::getDistance(m_ctrl_points);
		}

		return m_length;

	}

	bool PathCross::operator !=(const Embedded::PathCross &r_val) const
	{
		if(GIS_Embedded::SpatialMath::getDistance(m_pos, r_val.m_pos) > 1) //ด๓ำฺ1รื
			return true;
		else
			return false;
	}
	void PathResult::setLines(const std::vector<Line*> &route, const std::vector<bool> &fromBegins)
	{
		//char buf[1000];
		assert(route.size() == fromBegins.size());
		m_segments.clear();
		//m_segments.reserve(route.size());
		//uint_32 size = route.size();
		//MGLogD("%d", size);
		m_segments.resize(route.size());
		m_crosses.resize(route.size());
		//uint_32 s1 =m_segments.size();
		//MGLogD("%d", m_segments.size());
		//uint_32 s2 = sizeof(PathSegment);
		//MGLogD("%d", s2);

		//vector<ScreenPos2D> poses;
		MapPos2D pos;
		for(uint_32 i = 0; i < route.size(); ++i)
		{
			//poses.clear();
			//route[i]->getPoints(&poses);
			PathSegment segment(route[i], fromBegins[i]);
			m_segments[i] = segment ;
			m_crosses[i].m_pos = segment.m_ctrl_points[segment.m_ctrl_points.size()-1];			
		}
	}

	/*const vector<Line> &PathResult::getLines() const
	{
		return m_lines;
	}*/

	void PathResult::getControlPoints(std::vector<ScreenPos2D> &ctrl_points)
	{
		uint_32 cp_size = 0;
		for(uint_32 i = 0; i < m_segments.size(); ++i)
			cp_size += m_segments[i].m_ctrl_points.size();

		if(ctrl_points.size() > cp_size)
			ctrl_points.clear();

		ctrl_points.reserve(cp_size);

		ctrl_points.insert(ctrl_points.end(), m_segments[0].m_ctrl_points.begin(), m_segments[0].m_ctrl_points.end());

		for(uint_32 i = 1; i < m_segments.size(); ++i)
			ctrl_points.insert(ctrl_points.end(), m_segments[0].m_ctrl_points.begin()+1, m_segments[0].m_ctrl_points.end());
		
	}

	double PathResult::getLength(int start_seg,double param) const
	{
		double dis = 0;
		uint_32 ps_size = m_segments.size();
		if(ps_size < start_seg+1)
			dis = 0;
		else if(ps_size == start_seg+1)
			dis = m_segments[start_seg].getLength() * param;
		else
		{
			dis = m_segments[start_seg].getLength() * param;
			for(uint_32 i =start_seg+1; i < ps_size; ++i)
				dis += m_segments[i].getLength();
		}
		if(ps_size >= 2)
			dis += (*m_segments.rbegin()).getLength();

		return dis;

	}

}