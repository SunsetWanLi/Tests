#ifndef __EMBEDDED_APPLICATION_PATHRESULT_H
#define __EMBEDDED_APPLICATION_PATHRESULT_H
#include "../BasicType/Line.h"
#include <vector>
using namespace std;

namespace Embedded
{
	class PathSegment
	{
	public:
		PathSegment(): m_line(0){ 
			m_ctrl_points.clear(); 
			m_length = -1; 
		}
		PathSegment(Line* line, bool fromBegin):m_line(*line) { 			
			line->getPoints(&m_ctrl_points); 
			if(!fromBegin)
			{
				int i = 0, j = (int)m_ctrl_points.size() - 1;
				while(i < j)
				{
					ScreenPos2D tmpPos = m_ctrl_points[i];
					m_ctrl_points[i] = m_ctrl_points[j];
					m_ctrl_points[j] = tmpPos;
					++i; --j;
				}
			}
			m_length = -1;}
		double getLength() const;
		PathSegment(const PathSegment &seg): m_line(seg.m_line) { 
			m_ctrl_points = seg.m_ctrl_points; 
			m_length = seg.m_length;
		}
		PathSegment& operator=(const PathSegment &seg) { 
			m_line = seg.m_line;
			m_ctrl_points = seg.m_ctrl_points; 
			m_length = seg.m_length; 
			return *this;}
		vector<ScreenPos2D> m_ctrl_points;
		Line m_line;
	private:
		mutable double m_length;

	};

	class PathCross
	{
	public:
		bool operator !=(const PathCross & r_val)const;
	public:
		Pos2DEx m_pos;
	};

	class PathResult
	{
	public: 
		PathResult() {};
		void setLines(const std::vector<Line*> &route, const std::vector<bool> &fromBegins);
		//const vector<Line> & getLines() const;
		void getControlPoints(std::vector<ScreenPos2D> &ctrl_points);
		double getLength(int start_seg,double param)const;//lw

	public:
		vector<PathSegment> m_segments;
		vector<PathCross> m_crosses;
		vector<ScreenPos2D> m_ctrl_points;
		//vector<Line> m_lines;
	};

	

}

#endif