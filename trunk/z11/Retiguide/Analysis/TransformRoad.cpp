#include "TransformRoad.h"
namespace GIS_Embedded
{

int TransformRoad::calculatePathGeoBorder(vector<Pos2DEx>& control_pos2ds,const Scope& view_scope, double road_width, double filter_width ,
										  vector<Pos2DEx>& border_pos2d,vector<Pos2DEx>& center_pos2d)
{
	border_pos2d.clear();

	//GeometryLine *geometry_line = (GeometryLine*)geo._geometry;
	//vector<Pos2DEx> &control_pos2d_src = geometry_line->_control_pos2d;
	static vector<Pos2DEx> /*control_pos2d_clipped(256), */control_pos2d_filtered(256);
	control_pos2d_filtered.clear();

	if (filter_width <= 0)//防止下面调用函数出错  lw
		filter_width = 1;

	filtrationPointDouglas(/*control_pos2d_src*/control_pos2ds, filter_width, control_pos2d_filtered);

	//切割
	center_pos2d.resize(control_pos2d_filtered.size() * 2 + 6);
	long ret_size = SpatialMath::polygon_clip(control_pos2d_filtered.size(), &control_pos2d_filtered[0], &center_pos2d[0], view_scope.m_left - 10, view_scope.m_bottom - 10, view_scope.m_right + 10, view_scope.m_top + 10);
	if(ret_size <=1 )
		return -1;

	center_pos2d.resize(ret_size);
	TransformRoad::getExternalPos(center_pos2d, road_width, border_pos2d);

	return 0;
}

int TransformRoad::filtrationPointDouglas(const vector<Pos2DEx>& points, 
										  const double road_width, 
										  vector<Pos2DEx> &dest_pos)
{
	static vector<pair<Pos2DEx, bool> > pos_bool_vtr(100);
	initPosBoolVtr(pos_bool_vtr, points);
	douglasLineSimplification(pos_bool_vtr, 0, points.size() - 1, 1 * road_width);		//简化数组，但是保证最后一个点不变   lw
	getResultFromPosBoolVtr(points.size(), pos_bool_vtr, dest_pos);

	return 0;
}

int TransformRoad::getExternalPos(const vector<Pos2DEx> &src_pos, 
								  const double road_width, 
								  vector<Pos2DEx> &dest_pos)
{
	assert(src_pos.size() > 1);
	dest_pos.reserve(src_pos.size() * 4 + 4);
	if(src_pos.size() == 2)
		getExternalPosTwoPointLine(src_pos, road_width, dest_pos);
	else
		getExternalPosMoreTwoPointLine(src_pos, road_width, dest_pos);

	return 0;
}

int TransformRoad::douglasLineSimplification(vector<pair<Pos2DEx, bool> >& pos_bool_vtr, 
											 const int begin, 
											 const int end, 
											 const double road_width)	// 控制滤点精度
{
	if( begin > end )
		return -1;

	if( begin == end )
	{
		pos_bool_vtr[begin].second = true;
		return 0;
	}

	if( end - begin < 2 )
	{
		pos_bool_vtr[begin].second = true;
		pos_bool_vtr[end].second = true;
		return 0;
	}

	int save_point_index = 0;
	if(getTheFarthestPointOnLine(pos_bool_vtr, begin, end, road_width, save_point_index) == 0)
		return 0;

	douglasLineSimplification(pos_bool_vtr, begin, save_point_index, road_width);
	douglasLineSimplification(pos_bool_vtr, save_point_index, end, road_width);
	return 0;
}

//double TransformRoad::getAreaPointToLine(Pos2DEx& pos, Pos2DEx& start, Pos2DEx& end)
//{
//	return fabs((start.d0 - pos.d0) * (end.d1 - pos.d1) - (start.d1 - pos.d1) * (end.d0 - pos.d0));
//}
//
//double TransformRoad::getDistancePointToLine(const Pos2DEx& pos, const Pos2DEx& start, const Pos2DEx& end)
//{
//	// 海伦公式
//	//double a, b, c;
//	//a = SpatialMath::getDistance(pos, start);
//	//b = SpatialMath::getDistance(start, end);
//	//c = SpatialMath::getDistance(pos, end);
//	//double s = ( a + b + c ) / 2.0;
//	//double tmp = s * ( s - a ) * ( s - b ) * ( s - c );
//	//return 2 * sqrt(tmp) / b;
//
//	// 叉乘公式
//	return fabs((start.d0 - pos.d0) * (end.d1 - pos.d1) - (start.d1 - pos.d1) * (end.d0 - pos.d0)) /
//		SpatialMath::getDistance(start, end);
//}

double TransformRoad::getDistancePointToLine(const Pos2DEx& pos, const Pos2DEx& start, const Pos2DEx& end)
{
	if(!isLess90Degree(pos, start, end))
	{
		return SpatialMath::getDistance(pos, start);
	}
	else if(!isLess90Degree(pos, end, start))
	{
		return SpatialMath::getDistance(pos, end);
	}
	else
		return fabs(0.0+(start.d0 - pos.d0) * (end.d1 - pos.d1) - (start.d1 - pos.d1) * (end.d0 - pos.d0)) /
		SpatialMath::getDistance(start, end);
}

int TransformRoad::initPosBoolVtr(vector<pair<Pos2DEx, bool> >& pos_bool_vtr,
								  const vector<Pos2DEx>& points)
{
	pos_bool_vtr.clear();
	vector<Pos2DEx>::const_iterator itr_pos = points.begin();
	for( ; itr_pos != points.end(); ++itr_pos )
	{
		pos_bool_vtr.push_back(make_pair(*itr_pos, false));
	}

	return 0;
}

int TransformRoad::getResultFromPosBoolVtr(const int size, 
										   const vector<pair<Pos2DEx, bool> >& pos_bool_vtr, 
										   vector<Pos2DEx>& dest_pos)
{
	dest_pos.reserve(size);
	vector<pair<Pos2DEx, bool> >::const_iterator itr_pair = pos_bool_vtr.begin();
	for( ; itr_pair != pos_bool_vtr.end(); ++itr_pair)
	{
		if( (*itr_pair).second == true )
			dest_pos.push_back((*itr_pair).first);
	}

	return 0;
}

//int TransformRoad::filtrationPointRing(vector<pair<Pos2DEx, bool> >& pos_bool_vtr, 
//									   const int begin, 
//									   const int end, 
//									   const double road_width,
//									   int& save_point_index)
//{
//	double max_length = 0.0;
//	for(int i = begin + 1; i < end; ++i)
//	{
//		double length = getDistancePointToLine(pos_bool_vtr[i].first, pos_bool_vtr[begin].first, pos_bool_vtr[end].first);
//		if(length > max_length)
//		{
//			max_length = length;
//			save_point_index = i;
//		}
//	}
//
//	if(max_length < road_width)
//	{
//		pos_bool_vtr[begin].second = true;
//		pos_bool_vtr[end].second = true;
//		return 0;
//	}
//
//	return 1;
//}
//
//int TransformRoad::filtrationPointCurve(vector<pair<Pos2DEx, bool> >& pos_bool_vtr, 
//										const int begin, 
//										const int end, 
//										const double road_width,
//										int& save_point_index)
//{
//	double max_area = 0.0;
//	for(int i = begin + 1; i < end; ++i)
//	{
//		double area = getAreaPointToLine(pos_bool_vtr[i].first, pos_bool_vtr[begin].first, pos_bool_vtr[end].first);
//		if(area > max_area)
//		{
//			max_area = area;
//			save_point_index = i;
//		}
//	}
//	if(max_area < road_width * SpatialMath::getDistance(pos_bool_vtr[begin].first, pos_bool_vtr[end].first))
//	{
//		pos_bool_vtr[begin].second = true;
//		pos_bool_vtr[end].second = true;
//		return 0;
//	}
//
//	return 1;
//}

int TransformRoad::getTheFarthestPointOnLine(vector<pair<Pos2DEx, bool> >& pos_bool_vtr, 
											 const int begin, 
											 const int end, 
											 const double road_width,
											 int& save_point_index)
{
	double max_distance = 0.0;
	int end2=end;
	if (pos_bool_vtr.size()<end)
	{
		end2=pos_bool_vtr.size();
	}
	for(int i = begin + 1; i < end2; ++i)
	{
		double distance = getDistancePointToLine(pos_bool_vtr[i].first, pos_bool_vtr[begin].first, pos_bool_vtr[end].first);
		if(distance > max_distance)
		{
			max_distance = distance;
			save_point_index = i;
		}
	}
	if(max_distance < road_width)
	{
		pos_bool_vtr[begin].second = true;
		pos_bool_vtr[end].second = true;
		return 0;
	}

	return 1;
}

int TransformRoad::getExternalPosTwoPointLine(const vector<Pos2DEx> &src_pos, 
											  const double road_width, 
											  vector<Pos2DEx> &dest_pos)
{
	Pos2DEx pos1, pos2;
	SpatialMath::offsetSegment(src_pos[0], src_pos[1], -road_width/2, &pos1, &pos2);
	Pos2DEx pos3(Pos2DEx(src_pos[1].d0 + src_pos[1].d0 - pos2.d0, src_pos[1].d1 + src_pos[1].d1 - pos2.d1)),
		pos4(Pos2DEx(src_pos[0].d0 + src_pos[0].d0 - pos1.d0, src_pos[0].d1 + src_pos[0].d1 - pos1.d1));
	static vector<Pos2DEx> ctl_pos_0, ctl_pos_1;
	ctl_pos_0.clear();
	ctl_pos_1.clear();

	ctl_pos_0.push_back(pos1);
	ctl_pos_0.push_back(pos2);
	ctl_pos_1.push_back(pos4);
	ctl_pos_1.push_back(pos3);
	getDestLineFromSegment(src_pos, dest_pos, road_width, ctl_pos_0, ctl_pos_1);

	return 0;
}

int TransformRoad::getExternalPosMoreTwoPointLine(const vector<Pos2DEx> &src_pos, 
												  const double road_width, 
												  vector<Pos2DEx>& dest_pos)
{
	static vector<Pos2DEx> side0, side1;
	offsetAllSegment(src_pos, side0, side1, road_width/2);
	static vector<Pos2DEx> ctl_pos_0, ctl_pos_1;
	ctl_pos_0.clear();
	ctl_pos_1.clear();
	ctl_pos_0.reserve(src_pos.size() * 2);
	ctl_pos_1.reserve(src_pos.size() * 2);
	intersectAllSegment(src_pos, side0, side1, ctl_pos_0, ctl_pos_1);
	getDestLineFromSegment(src_pos, dest_pos, road_width, side0, side1, ctl_pos_0, ctl_pos_1);

	return 0;
}

int TransformRoad::offsetAllSegment(const vector<Pos2DEx>& src_pos, 
									vector<Pos2DEx>& side0, 
									vector<Pos2DEx>& side1, 
									const double road_width)
{
	if(src_pos.size()<2)//lw no matter,之前已然判断过
		return -1;
	side0.clear();
	side1.clear();
	side0.reserve(src_pos.size() * 2 - 2);
	side1.reserve(src_pos.size() * 2 - 2);
	Pos2DEx start_pos = src_pos[0], end_pos = src_pos[1];
	for( int i = 1; i < src_pos.size(); ++i )
	{
		Pos2DEx tmp_0, tmp_1;
		SpatialMath::offsetSegment(start_pos, end_pos, -road_width, &tmp_0, &tmp_1);
		Pos2DEx tmp_2(Pos2DEx(start_pos.d0 + start_pos.d0 - tmp_0.d0, start_pos.d1 + start_pos.d1 - tmp_0.d1)),
			tmp_3(Pos2DEx(end_pos.d0 + end_pos.d0 - tmp_1.d0, end_pos.d1 + end_pos.d1 - tmp_1.d1));
		side0.push_back(tmp_0);
		side0.push_back(tmp_1);
		side1.push_back(tmp_2);
		side1.push_back(tmp_3);
		start_pos = end_pos;
		if( (i+1) != src_pos.size() )
			end_pos = src_pos[i+1];
	}

	return 0;
}

int TransformRoad::intersectAllSegment(const vector<Pos2DEx>& src_pos, 
									   const vector<Pos2DEx> &side0, 
									   const vector<Pos2DEx> &side1,
									   vector<Pos2DEx> &ctl_pos_0, 
									   vector<Pos2DEx> &ctl_pos_1)
{
	int first_start = 0, first_end = 1, second_start = 2, second_end = 3;
	for( int i = 1; i < src_pos.size() - 1; ++i )
	{
		Pos2DEx cross_pos;
		if( !isLess90Degree(src_pos[i-1], src_pos[i], src_pos[i+1]) )
		{
			if(SpatialMath::isParallel(side0[first_start], side0[first_end], side0[second_start], side0[second_end]))
			{
				ctl_pos_0.push_back(side0[first_end]);
				ctl_pos_1.push_back(side1[first_end]);
			}
			else
			{
				SpatialMath::isLineCross(side0[first_start], side0[first_end], side0[second_start], side0[second_end], &cross_pos);
				ctl_pos_0.push_back(cross_pos);
				SpatialMath::isLineCross(side1[first_start], side1[first_end], side1[second_start], side1[second_end], &cross_pos);
				ctl_pos_1.push_back(cross_pos);
			}

			first_start += 2;
			first_end += 2;
			second_start += 2;
			second_end += 2;

			continue;
		}
		if( SpatialMath::lineCrossPoint(side0[first_start], side0[first_end], side0[second_start], side0[second_end], &cross_pos) )
			ctl_pos_0.push_back(cross_pos);
		else
		{
			ctl_pos_0.push_back(side0[first_end]);
			ctl_pos_0.push_back(side0[second_start]);
		}

		if( SpatialMath::lineCrossPoint(side1[first_start], side1[first_end], side1[second_start], side1[second_end], &cross_pos) )
			ctl_pos_1.push_back(cross_pos);
		else
		{
			ctl_pos_1.push_back(side1[first_end]);
			ctl_pos_1.push_back(side1[second_start]);
		}

		first_start += 2;
		first_end += 2;
		second_start += 2;
		second_end += 2;
	}

	return 0;
}

int TransformRoad::intersectAllSegment(const vector<Pos2DEx>& src_pos, 
									   const vector<Pos2DEx> &side0, 
									   const vector<Pos2DEx> &side1,
									   vector<pair<Pos2DEx, int> > &ctl_pos_0, 
									   vector<pair<Pos2DEx, int> > &ctl_pos_1)
{
	int first_start = 0, first_end = 1, second_start = 2, second_end = 3;
	ctl_pos_0.push_back(make_pair(side0.front(), 0));
	ctl_pos_1.push_back(make_pair(side1.front(), 0));
	for( int i = 1; i < src_pos.size() - 1; ++i )
	{
		Pos2DEx cross_pos;
		if( !isLess90Degree(src_pos[i-1], src_pos[i], src_pos[i+1]) )
		{
			if(SpatialMath::isParallel(side0[first_start], side0[first_end], side0[second_start], side0[second_end]))
			{
				ctl_pos_0.push_back(make_pair(side0[first_end], i));
				ctl_pos_1.push_back(make_pair(side1[first_end], i));
			}
			else
			{
				SpatialMath::isLineCross(side0[first_start], side0[first_end], side0[second_start], side0[second_end], &cross_pos);
				ctl_pos_0.push_back(make_pair(cross_pos, i));
				SpatialMath::isLineCross(side1[first_start], side1[first_end], side1[second_start], side1[second_end], &cross_pos);
				ctl_pos_1.push_back(make_pair(cross_pos, i));
			}

			first_start += 2;
			first_end += 2;
			second_start += 2;
			second_end += 2;

			continue;
		}
		if( SpatialMath::lineCrossPoint(side0[first_start], side0[first_end], side0[second_start], side0[second_end], &cross_pos) )
			ctl_pos_0.push_back(make_pair(cross_pos, i));
		else
		{
			ctl_pos_0.push_back(make_pair(side0[first_end], i));
			ctl_pos_0.push_back(make_pair(side0[second_start], i));
		}

		if( SpatialMath::lineCrossPoint(side1[first_start], side1[first_end], side1[second_start], side1[second_end], &cross_pos) )
			ctl_pos_1.push_back(make_pair(cross_pos, i));
		else
		{
			ctl_pos_1.push_back(make_pair(side1[first_end], i));
			ctl_pos_1.push_back(make_pair(side1[second_start], i));
		}

		first_start += 2;
		first_end += 2;
		second_start += 2;
		second_end += 2;
	}
	ctl_pos_0.push_back(make_pair(side0.back(), src_pos.size() - 1));
	ctl_pos_1.push_back(make_pair(side1.back(), src_pos.size() - 1));

	return 0;
}

int TransformRoad::getDestLineFromSegment(const vector<Pos2DEx>& src_pos,
										  vector<Pos2DEx>& dest_pos, 
										  const double road_width,
										  const vector<Pos2DEx>& side0, 
										  const vector<Pos2DEx>& side1,
										  const vector<Pos2DEx>& ctl_pos_0, 
										  const vector<Pos2DEx>& ctl_pos_1)
{
	dest_pos.push_back(side0[0]);
	Pos2DEx extend_point;
	computeCurveExtend(extend_point, src_pos[1], src_pos[0], road_width);
	dest_pos.push_back(Pos2DEx((extend_point.d0+side0[0].d0)/2, (extend_point.d1+side0[0].d1)/2));
	dest_pos.push_back(Pos2DEx((extend_point.d0+side1[0].d0)/2, (extend_point.d1+side1[0].d1)/2));
	dest_pos.push_back(side1[0]);
	vector<Pos2DEx>::const_iterator pos = ctl_pos_1.begin();
	for( ; pos != ctl_pos_1.end(); ++pos )
		dest_pos.push_back(*pos);

	dest_pos.push_back(side1[side1.size()-1]);
	computeCurveExtend(extend_point, src_pos[src_pos.size()-2], src_pos[src_pos.size()-1], road_width);
	dest_pos.push_back(Pos2DEx((extend_point.d0+side1[side1.size()-1].d0)/2, (extend_point.d1+side1[side1.size()-1].d1)/2));
	dest_pos.push_back(Pos2DEx((extend_point.d0+side0[side0.size()-1].d0)/2, (extend_point.d1+side0[side0.size()-1].d1)/2));		
	dest_pos.push_back(side0[side0.size()-1]);
	vector<Pos2DEx>::const_reverse_iterator rpos = ctl_pos_0.rbegin();
	for( ; rpos != ctl_pos_0.rend(); ++rpos )
		dest_pos.push_back(*rpos);
	dest_pos.push_back(side0[0]);

	return 0;
}

int TransformRoad::getDestLineFromSegment(const vector<Pos2DEx>& src_pos,
										  vector<Pos2DEx>& dest_pos, 
										  const double road_width,
										  const vector<Pos2DEx>& side0, 
										  const vector<Pos2DEx>& side1,
										  const vector<pair<Pos2DEx, int> >& ctl_pos_0, 
										  const vector<pair<Pos2DEx, int> >& ctl_pos_1)
{
	dest_pos.push_back(side0[0]);
	Pos2DEx extend_point;
	computeCurveExtend(extend_point, src_pos[1], src_pos[0], road_width);
	dest_pos.push_back(Pos2DEx((extend_point.d0+side0[0].d0)/2, (extend_point.d1+side0[0].d1)/2));
	dest_pos.push_back(Pos2DEx((extend_point.d0+side1[0].d0)/2, (extend_point.d1+side1[0].d1)/2));
	dest_pos.push_back(side1[0]);
	vector<pair<Pos2DEx, int> >::const_iterator pos = ctl_pos_1.begin();
	for( ; pos != ctl_pos_1.end(); ++pos )
		dest_pos.push_back((*pos).first);

	dest_pos.push_back(side1[side1.size()-1]);
	computeCurveExtend(extend_point, src_pos[src_pos.size()-2], src_pos[src_pos.size()-1], road_width);
	dest_pos.push_back(Pos2DEx((extend_point.d0+side1[side1.size()-1].d0)/2, (extend_point.d1+side1[side1.size()-1].d1)/2));
	dest_pos.push_back(Pos2DEx((extend_point.d0+side0[side0.size()-1].d0)/2, (extend_point.d1+side0[side0.size()-1].d1)/2));		
	dest_pos.push_back(side0[side0.size()-1]);
	vector<pair<Pos2DEx, int> >::const_reverse_iterator rpos = ctl_pos_0.rbegin();
	for( ; rpos != ctl_pos_0.rend(); ++rpos )
		dest_pos.push_back((*rpos).first);
	dest_pos.push_back(side0[0]);

	return 0;
}

int TransformRoad::getDestLineFromSegment(const vector<Pos2DEx>& src_pos, 
										  vector<Pos2DEx>& dest_pos, 
										  const double road_width,
										  const vector<Pos2DEx>& side0, 
										  const vector<Pos2DEx>& side1)
{
	dest_pos.push_back(side0.at(0));
	Pos2DEx extend_point;
	computeCurveExtend(extend_point, src_pos[1], src_pos[0], road_width);
	dest_pos.push_back(Pos2DEx((extend_point.d0+side0.at(0).d0)/2, (extend_point.d1+side0.at(0).d1)/2));
	dest_pos.push_back(Pos2DEx((extend_point.d0+side1.at(0).d0)/2, (extend_point.d1+side1.at(0).d1)/2));
	dest_pos.push_back(side1.at(0));
	dest_pos.push_back(side1.at(1));
	computeCurveExtend(extend_point, src_pos[0], src_pos[1], road_width);
	dest_pos.push_back(Pos2DEx((extend_point.d0+side1.at(1).d0)/2, (extend_point.d1+side1.at(1).d1)/2));
	dest_pos.push_back(Pos2DEx((extend_point.d0+side0.at(1).d0)/2, (extend_point.d1+side0.at(1).d1)/2));
	dest_pos.push_back(side0.at(1));
	dest_pos.push_back(side0.at(0));

	return 0;
}

int TransformRoad::getDestLineFromSegment(const vector<Pos2DEx>& src_pos, 
										  vector<Pos2DEx>& dest_pos, 
										  const double road_width,
										  const vector<pair<Pos2DEx, int> >& side0, 
										  const vector<pair<Pos2DEx, int> >& side1)
{
	dest_pos.push_back(side0.at(0).first);
	Pos2DEx extend_point;
	computeCurveExtend(extend_point, src_pos[1], src_pos[0], road_width);
	dest_pos.push_back(Pos2DEx((extend_point.d0+side0.at(0).first.d0)/2, (extend_point.d1+side0.at(0).first.d1)/2));
	dest_pos.push_back(Pos2DEx((extend_point.d0+side1.at(0).first.d0)/2, (extend_point.d1+side1.at(0).first.d1)/2));
	dest_pos.push_back(side1.at(0).first);
	dest_pos.push_back(side1.at(1).first);
	computeCurveExtend(extend_point, src_pos[0], src_pos[1], road_width);
	dest_pos.push_back(Pos2DEx((extend_point.d0+side1.at(1).first.d0)/2, (extend_point.d1+side1.at(1).first.d1)/2));
	dest_pos.push_back(Pos2DEx((extend_point.d0+side0.at(1).first.d0)/2, (extend_point.d1+side0.at(1).first.d1)/2));
	dest_pos.push_back(side0.at(1).first);
	dest_pos.push_back(side0.at(0).first);

	return 0;
}

int TransformRoad::computeCurveExtend(Pos2DEx &pos, 
									  const Pos2DEx &start,
									  const Pos2DEx &end, 
									  double road_width)
{
	double multiple = SpatialMath::getDistance(start, end) / (road_width/2);
	pos.d0 = end.d0 + ( end.d0 - start.d0 ) / multiple;
	pos.d1 = end.d1 + ( end.d1 - start.d1 ) / multiple;
	return 0;
}

bool TransformRoad::isLess90Degree(const Pos2DEx& one, 
								   const Pos2DEx& cross_point, 
								   const Pos2DEx& other)
{
	if((one.d0 - cross_point.d0) * (one.d0 - cross_point.d0) + (one.d1 - cross_point.d1) * (one.d1 - cross_point.d1)
		+ (other.d0 - cross_point.d0) * (other.d0 - cross_point.d0) + (other.d1 - cross_point.d1) * (other.d1 - cross_point.d1)
		- (one.d0 - other.d0) * (one.d0 - other.d0) - (one.d1 - other.d1) * (one.d1 - other.d1) > 0)
		return true;
	else
		return false;

}


void TransformRoad::filtrationPointGeoTriangle(const vector<Pos2DEx>& host_points, const double threshold_area, vector<Pos2DEx> &dest_points)
{
	int points_num = host_points.size();
	if (points_num < 3)
		return;
	vector<double> trangle_area;
	trangle_area.clear();
	vector<Pos2DEx> tmp_dest_points;
	tmp_dest_points.clear();
	int i;
	double min_area = PureMath::AreaOfTriangle(host_points[0].d0,host_points[0].d1,host_points[1].d0,host_points[1].d1,host_points[2].d0,host_points[2].d1);
	trangle_area.push_back(min_area);
	for (i=1; i<points_num-1; i++)
	{
		double tmp_area = PureMath::AreaOfTriangle(host_points[i-1].d0,host_points[i-1].d1,host_points[i].d0,host_points[i].d1,host_points[i+1].d0,host_points[i+1].d1);
		trangle_area.push_back(tmp_area);
		if (tmp_area <= min_area && tmp_area!=0)
			min_area = tmp_area;
	}
	if (min_area > threshold_area)
	{
		dest_points.clear();
		dest_points.insert(dest_points.begin(),host_points.begin(),host_points.end());
		return;
	}
	else
	{
		for (i=0; i< points_num-1; i++)
		{
			if (trangle_area[i] > min_area)
				tmp_dest_points.push_back(host_points[i]);
		}
		filtrationPointGeoTriangle(tmp_dest_points,threshold_area,dest_points);
	}
}

}