#include "../../ExternalLib/wykobi/wykobi.hpp"
#include "../../ExternalLib/wykobi/wykobi_algorithm.hpp"
#include "CGRegionClipper.h"
#include "../../System/Shared/MGLog.h"
using namespace std;
namespace z11
{
	typedef MGPoint PointEx; 

#define  eps 1e-6
	inline bool float_equal(float a,float b)
	{
		return abs(a-b)<eps;
	}
	inline bool point_equal(const MGPoint &a,const MGPoint &b )
	{
		return (float_equal(a.x,b.x)&&float_equal(a.y,b.y));
	}

	inline bool float_equal_rude(float a,float b)
	{
		return abs(a-b)<0.1;
	}
	inline bool point_equal_rude(const MGPoint &a,const MGPoint &b )
	{
		return (float_equal(a.x,b.x)&&float_equal(a.y,b.y));
	}
	size_t CGRegionClipper::CUT_NUM=30; 
	size_t CUT_MAX_NUM=90; 
	size_t CUT_MAX_NUM_2=300; 
	
	void CGRegionClipper::clipRect( std::vector<MGPoint> &points,const MGRect &rect )
	{
		if (points.size()>CUT_NUM)
		{      
			std::vector<wykobi::point2d<float> > need_to_clip_points; 
			//if (points.size()>CUT_MAX_NUM_2)
			//{
			//	size_t start=0;
			//	need_to_clip_points.push_back(wykobi::make_point(points[0].x,points[0].y));
			//	for (size_t i=1;i<points.size();i++)
			//	{
			//		if (!point_equal_rude(points[i],points[start]))
			//		{
			//			need_to_clip_points.push_back(wykobi::make_point(points[i].x,points[i].y));
			//			start=i;
			//		}
			//	}
			//}
			//else
			{
				for (size_t i=0;i<points.size();i++)
				{
					wykobi::point2d<float> pt;
					pt.x=points[i].x;
					pt.y=points[i].y;
					need_to_clip_points.push_back(pt);
				}
			}
			wykobi::polygon<float,2> polygon=wykobi::make_polygon<float>(need_to_clip_points);

			wykobi::rectangle<float> clip_boundry;
			clip_boundry[0].x=rect.x;
			clip_boundry[0].y=rect.y;
			clip_boundry[1].x=rect.x+rect.width;
			clip_boundry[1].y=rect.y+rect.height;
			wykobi::polygon<float,2> clipped_polygon;

			wykobi::algorithm::sutherland_hodgman_polygon_clipper< wykobi::point2d<float> >(clip_boundry,polygon,clipped_polygon);
			vector<MGPoint> need_to_render_points;
			for (size_t i=0;i<clipped_polygon.size();i++)
			{
				need_to_render_points.push_back(MGPointMake(clipped_polygon[i].x+0.0001,clipped_polygon[i].y+0.0001));
			}

			//int begin_point=points.size();
			//int middle_point=need_to_render_points.size();
			if (need_to_render_points.size()>CUT_MAX_NUM)
			{
                {
                    vector<MGPoint> tmp;
                    points.swap(tmp);
                }
				size_t start=0;
				size_t end=start+1;
				points.push_back(need_to_render_points[0]);
				for (;end<need_to_render_points.size();end++)
				{
					if (!point_equal_rude(need_to_render_points[end],points.back()))
					{
						points.push_back(need_to_render_points[end]);
					}
				}
			}
			else
				points.swap(need_to_render_points);
			//int end_point=points.size();
			//MGLogD("CGRegionClipper::clipRect result %d --> %d --> %d",begin_point,middle_point,end_point);
		}
	}

	std::vector<std::vector<MGPoint> > CGRegionClipper::clipPolyline(const std::vector<MGPoint> &points,const MGRect &rect )
	{
		std::vector<std::vector<MGPoint> > ret;
		if (points.size()<2)
		{
			ret.push_back(points);
			return ret;
		}
		std::vector<wykobi::segment<float,2> > segment_list;

		MGPoint last_point=points[0];
		for(std::size_t i = 1; i < points.size(); ++i)
		{
			if (!point_equal_rude(last_point,points[i]))
			{
				segment_list.push_back(
					wykobi::make_segment(
					wykobi::make_point(last_point.x,last_point.y),
					wykobi::make_point(points[i].x,points[i].y)));
				last_point=points[i];
			}
		}

		wykobi::rectangle<float> rectangle=wykobi::make_rectangle(rect.x-20,rect.y-20,rect.width+rect.x+20,rect.height+rect.y+20);
		

		wykobi::segment<float,2> clipped_segment;
		//bool outer=true;
		for(std::size_t i = 0; i < segment_list.size(); ++i)
		{
			if (wykobi::clip(segment_list[i],rectangle,clipped_segment))
			{
				if (ret.empty()||!(point_equal(ret.back().back(),MGPointMake(clipped_segment[0].x,clipped_segment[0].y))))
				{
					ret.push_back(std::vector<MGPoint>());
					ret.back().push_back(MGPointMake(clipped_segment[0].x,clipped_segment[0].y));
				}
				ret.back().push_back(MGPointMake(clipped_segment[1].x,clipped_segment[1].y));
			}
		}
		return ret;
	}
}