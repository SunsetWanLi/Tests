#include "SpatialMath.h"
#include "../Common/Util/PureMath.h"

#include <assert.h>
#include <float.h>
#include <algorithm>
#include "../../System/Shared/MGLog.h"
#include <limits.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

namespace GIS_Embedded
{

bool SpatialMath::isPos2DInRect(const Pos2D& pos,const Rect2D& rect)
{
	return pos.d0>=rect.left_low.d0 && pos.d0<=rect.right_top.d0 && pos.d1>=rect.left_low.d1 && pos.d1<=rect.right_top.d1;
}

long SpatialMath::polygon_clip( long src_sum, const Pos2D * src_pts,
									  Pos2D * dest_pts, int x1, int y1, int x2, int y2 )
{
	// 区域裁减算法，依次将四条边外面的删掉。
	// 首先是左边
	static Pos2D * temp_pts = NULL;
	static uint_32 current_size = 0;
	if(current_size < src_sum * 2)
	{
		current_size = src_sum * 2;
		delete []temp_pts;
		temp_pts = new Pos2D[current_size];
	}

	long i, k;
	long size = src_sum;
	bool inflag;
	// 始点如果在里面，首先加入
	if( size > 0 && src_pts[0].d0 >= x1 )
	{
		temp_pts[0] = src_pts[0];
		k = 1;
		inflag = true;
	}
	else
	{
		k = 0;
		inflag = false;
	}
	for( i = 1; i < size; i++ )
	{
		if( true == inflag )
		{
			// 都在里面，加入终点，
			if( src_pts[i].d0 >= x1 )
				temp_pts[k++] = src_pts[i];
			// 由里到外，加入交界点。
			else
			{
				double y;
				y = src_pts[i - 1].d1 + ( src_pts[i].d1 - src_pts[i - 1].d1 )
					/ ( src_pts[i].d0 - src_pts[i - 1].d0 )
					* (x1 - src_pts[i - 1].d0);
				temp_pts[k].d0 = x1;
				temp_pts[k].d1 = y;
				k++;
				inflag = false;
			}
		}
		else
		{
			// 由外到内，加入交界点和终点。
			if( src_pts[i].d0 >= x1 )
			{
				double y;
				y = src_pts[i - 1].d1 + ( src_pts[i].d1 - src_pts[i - 1].d1 )
					/ ( src_pts[i].d0 - src_pts[i - 1].d0 )
					* (x1 - src_pts[i - 1].d0);
				temp_pts[k].d0 = x1;
				temp_pts[k].d1 = y;
				k++;
				temp_pts[k++] = src_pts[i];
				inflag = true;
			}
			// 都在外边，不动
			//	else if( src_pts[i].x < x1 && src_pts[i + 1].x < x1 )
			//		;
		}
	}
	// 0601；如果原来首尾点重合，新的首尾点不重合，则强制重合，保证面切割的正确性。
	if( k >= 2 && src_pts[0].d0 == src_pts[size - 1].d0
		&& src_pts[0].d1 == src_pts[size - 1].d1
		&& (temp_pts[0].d0 != temp_pts[k - 1].d0
		|| temp_pts[0].d1 != temp_pts[k - 1].d1)  )
	{
		// 增加一个点。
		temp_pts[k++] = temp_pts[0];
	}

	// 然后是右边，
	size = k;
	if( size > 0 && temp_pts[0].d0 <= x2 )
	{
		dest_pts[0] = temp_pts[0];
		k = 1;
		inflag = true;
	}
	else
	{
		k = 0;
		inflag = false;
	}
	for( i = 1; i < size; i++ )
	{
		if( true == inflag )
		{
			// 都在里面，加入终点，
			if( temp_pts[i].d0 <= x2 )
				dest_pts[k++] = temp_pts[i];
			// 由里到外，加入交界点。
			else
			{
				double y;
				y = temp_pts[i - 1].d1 + ( temp_pts[i].d1 - temp_pts[i - 1].d1 )
					/ ( temp_pts[i].d0 - temp_pts[i - 1].d0 )
					* (x2 - temp_pts[i - 1].d0);
				dest_pts[k].d0 = x2;
				dest_pts[k].d1 = y;
				k++;
				inflag = false;
			}
		}
		else
		{
			// 由外到内，加入交界点和终点。
			if( temp_pts[i].d0 <= x2 )
			{
				double y;
				y = temp_pts[i - 1].d1 + ( temp_pts[i].d1 - temp_pts[i - 1].d1 )
					/ ( temp_pts[i].d0 - temp_pts[i - 1].d0 )
					* (x2 - temp_pts[i - 1].d0);
				dest_pts[k].d0 = x2;
				dest_pts[k].d1 = y;
				k++;
				dest_pts[k++] = temp_pts[i];
				inflag = true;
			}
			// 都在外边，不动
		}
	}
	if( k >= 2 && temp_pts[0].d0 == temp_pts[size - 1].d0
		&& temp_pts[0].d1 == temp_pts[size - 1].d1
		&& (dest_pts[0].d0 != dest_pts[k - 1].d0
		|| dest_pts[0].d1 != dest_pts[k - 1].d1) )
	{
		// 增加一个点。
		dest_pts[k++] = dest_pts[0];
	}

	// 首先是下边
	size = k;
	if( k > 0 && dest_pts[0].d1 >= y1 )
	{
		temp_pts[0] = dest_pts[0];
		k = 1;
		inflag = true;
	}
	else
	{
		k = 0;
		inflag = false;
	}
	for( i = 1; i < size; i++ )
	{
		if( true == inflag )
		{
			// 都在里面，加入终点，
			if( dest_pts[i].d1 >= y1 )
				temp_pts[k++] = dest_pts[i];
			// 由里到外，加入交界点。
			else
			{
				double x;
				x = dest_pts[i - 1].d0 + ( dest_pts[i].d0 - dest_pts[i - 1].d0 )
					/ ( dest_pts[i].d1 - dest_pts[i - 1].d1 )
					* (y1 - dest_pts[i - 1].d1);
				temp_pts[k].d0 = x;
				temp_pts[k].d1 = y1;
				k++;
				inflag = false;
			}
		}
		else
		{
			// 由外到内，加入交界点和终点。
			if( dest_pts[i].d1 >= y1 )
			{
				double x;
				x = dest_pts[i - 1].d0 + ( dest_pts[i].d0 - dest_pts[i - 1].d0 )
					/ ( dest_pts[i].d1 - dest_pts[i - 1].d1 )
					* (y1 - dest_pts[i - 1].d1);
				temp_pts[k].d0 = x;
				temp_pts[k].d1 = y1;
				k++;
				temp_pts[k++] = dest_pts[i];
				inflag = true;
			}
			// 都在外边，不动
		}
	}
	if( k >= 2 && dest_pts[0].d0 == dest_pts[size - 1].d0
		&& dest_pts[0].d1 == dest_pts[size - 1].d1
		&& (temp_pts[0].d0 != temp_pts[k - 1].d0
		|| temp_pts[0].d1 != temp_pts[k - 1].d1) )
	{
		// 增加一个点。
		temp_pts[k++] = temp_pts[0];
	}

	// 最后是上边。
	size = k;
	if( k > 0 && temp_pts[0].d1 <= y2 )
	{
		dest_pts[0] = temp_pts[0];
		k = 1;
		inflag = true;
	}
	else
	{
		k = 0;
		inflag = false;
	}
	for( i = 1; i < size; i++ )
	{
		if( true == inflag )
		{
			// 都在里面，加入终点，
			if( temp_pts[i].d1 <= y2)
				dest_pts[k++] = temp_pts[i];
			// 由里到外，加入交界点。
			else
			{
				double x;
				x = temp_pts[i - 1].d0 + ( temp_pts[i].d0 - temp_pts[i - 1].d0 )
					/ ( temp_pts[i].d1 - temp_pts[i - 1].d1 )
					* (y2 - temp_pts[i - 1].d1);
				dest_pts[k].d0 = x;
				dest_pts[k].d1 = y2;
				k++;
				inflag = false;
			}
		}
		else
		{
			// 由外到内，加入交界点和终点。
			if( temp_pts[i].d1 <= y2 )
			{
				double x;
				x = temp_pts[i - 1].d0 + ( temp_pts[i].d0 - temp_pts[i - 1].d0 )
					/ ( temp_pts[i].d1 - temp_pts[i - 1].d1 )
					* (y2 - temp_pts[i - 1].d1);
				dest_pts[k].d0 = x;
				dest_pts[k].d1 = y2;
				k++;
				dest_pts[k++] = temp_pts[i];
				inflag = true;
			}
			// 都在外边，不动
		}
	}
	if( k >= 2 && temp_pts[0].d0 == temp_pts[size - 1].d0
		&& temp_pts[0].d1 == temp_pts[size - 1].d1
		&& (dest_pts[0].d0 != dest_pts[k - 1].d0
		|| dest_pts[0].d1 != dest_pts[k - 1].d1) )
	{
		// 增加一个点。
		dest_pts[k++] = dest_pts[0];
	}

	//delete[] temp_pts;
	return k;
}

inline bool equalToZero(double x){return fabs(x) < DOUBLE_ZERO;}

inline bool equalPos2D(const Pos2D& p1, const Pos2D& p2, double tolerance = DOUBLE_ZERO)
{
	return fabs((p1.d0-p2.d0)) < tolerance
		&& fabs((p1.d1-p2.d1)) < tolerance;
}

bool SpatialMath::isAnticlockwise(const Pos2D & p0, const Pos2D & p1, const Pos2D & p2)
{
/*
 连续线段p0p1p2,走向为p0p1->p1p2,
 _p0p1p2逆时针方向=〉p0p1在p1p2顺时针方向=〉p0p1 * p1p2 >0。
 _p0p1p2顺时针方向=〉p0p1在p1p2逆时针方向=〉p0p1 * p1p2 <0。
 其中p0p1 * p1p2 = |p1.x-p0.x p1.y-p0.y|
				   |p2.x-p1.x p2.y-p1.y|
*/
	return (p1.d0-p0.d0)*(p2.d1-p1.d1)-(p1.d1-p0.d1)*(p2.d0-p1.d0) > 0;
}
double SpatialMath::getDistance(const vector<Pos2D> & pos_array,double param_start, double param_end)
{
	double dis = 0;
	if(pos_array.empty())
		return dis;
	uint_32 max_index = pos_array.size() - 1;
	//zgr, 2008-01-15, 确保param_start < param_end
	if(param_start > param_end)
		swap(param_start,param_end);

	param_start = (0 > param_start) ? 0 : param_start;
	param_end = (param_end < max_index) ? param_end : max_index;
	if(isEqualToDoubleZero(param_end))
		param_end = max_index;

	uint_32 s = uint_32(param_start);
	uint_32 e = uint_32(param_end);

	if(s != max_index && e > s)
	{
		dis += getDistance(pos_array[s], pos_array[s+1]) * (s + 1 - param_start);
	}
	for(uint_32 i = s + 1; i < e; ++ i)
	{

		dis += getDistance(pos_array[i], pos_array[i+1]);

	}
	if(e != max_index)
	{
		dis += getDistance(pos_array[e], pos_array[e+1]) * (param_end - e);
	}

	return dis;
}

Pos2D SpatialMath::NearestPointOnSegment(const Pos2D & aa, const Pos2D & pp, const Pos2D & qq )
{
	Pos2D result;
	// 先判断目标直线是否垂直。
	if( pp.d0 == qq.d0 )
	{
		result.d0 = pp.d0;

		double max, min;
		if( pp.d1 > qq.d1 )
			max = pp.d1, min = qq.d1;
		else
			min = pp.d1, max = qq.d1;

		if( aa.d1 > max )
			result.d1 = max;
		else if( aa.d1 < min )
			result.d1 = min;
		else
			result.d1 = aa.d1;
	}
	else
	{
		double a, b;
		a = qq.d0 - pp.d0;
		b = qq.d1 - pp.d1;
		result.d0 = ( a * a * aa.d0 + b * b * pp.d0 + a * b * ( aa.d1 - pp.d1 ) )
					/ ( a * a + b * b );
		if(isEqualToDoubleZero(a))
			result.d1 = aa.d1;
		else
			result.d1 = PureMath::FindYOnLine( pp.d0, pp.d1, qq.d0, qq.d1, result.d0 );

		// 判断是否在线内。
		if( (qq.d0 > pp.d0 && (result.d0 > qq.d0 || result.d0 < pp.d0) )
				|| (qq.d0 < pp.d0 && (result.d0 < qq.d0 || result.d0 > pp.d0) )   )
		{
			double len1, len2;
			len1 = getDistanceSquare( aa, pp );
			len2 = getDistanceSquare( aa, qq );
			if( len1 < len2 )
				result = pp;
			else
				result = qq;
		}
	}

	return result;
}
double SpatialMath::NearestPointOnSegmentString(Pos2D * pos, double * param,const Pos2D & aa, const vector<Pos2D> & pos_array,double tolerance )
{
	double ret = -1;
	double min_dis = DBL_MAX;
	Pos2D nearest_pos;
	double tol_squ = pow(tolerance, 2);

	uint_32 size = pos_array.size();
	if(size == 1)
	{
		return DBL_MAX;
	}

	for(uint_32 i = 1; i < size; ++i)
	{

		Pos2D cur_pos = NearestPointOnSegment(aa, pos_array[i-1],pos_array[i]);

		double cur_dis = getDistanceSquare(aa, cur_pos);
		if(min_dis > cur_dis)
		{
			nearest_pos = cur_pos;
			*param = i-1;
			min_dis = cur_dis;
			if(min_dis < tol_squ)
				break;
		}
	}

	*pos = nearest_pos;

	*param += PointPositionOnSegment(nearest_pos, pos_array[(int)(*param)], pos_array[(int)(*param) + 1]);

	return sqrt(min_dis);
}
Pos2D SpatialMath::getPosFromSegmentParam(const Pos2D & pp, const Pos2D & qq,
								 double param)
{
	Pos2D ret;

	if(param <= 0)
	{
		ret.d0 = pp.d0, ret.d1 = pp.d1;
		return ret;
	}

	if(param >= 1)
	{
		ret.d0 = qq.d0, ret.d1 = qq.d1;
		return ret;
	}

	ret.d0 = pp.d0 * (1 - param) + qq.d0 * param;
	ret.d1 = pp.d1 * (1 - param) + qq.d1 * param;

	return ret;
}

double SpatialMath::PointPositionOnSegment(const Pos2D & aa, const Pos2D & pp, const Pos2D & qq )
{
	if(pp == qq) // 两个点相等
		return 0;
	return getDistance(aa, pp)/getDistance(qq, pp);
}
double SpatialMath::calLineXAngle(const Pos2D &pos_start, const Pos2D &pos_end)
{
	double angle;
	double tan_value;

	if(pos_end.d0 == pos_start.d0)
		angle = M_PI/2;
	else
	{
		tan_value = (pos_end.d1 - pos_start.d1) / (pos_end.d0 - pos_start.d0);
		angle = atan(tan_value);
	}

	return angle;
}

double SpatialMath::calLineYAngle(const Pos2D &pos_start, const Pos2D &pos_end)
{
	double angle;
	double tan_value;

	if(pos_end.d1 == pos_start.d1)
		angle = M_PI/2;
	else
	{
		tan_value = (pos_end.d0 - pos_start.d0) / (pos_end.d1 - pos_start.d1);
		angle = atan(tan_value);
	}

	return angle;
}
double SpatialMath::calLineNAngle(const Pos2D & pos_start, const Pos2D & pos_end)
{
	double angle = calLineYAngle(pos_start, pos_end);
	if(M_PI/2 == fabs(angle))
	{
		if(pos_start.d0 > pos_end.d0)// <-
			angle = 3*M_PI/2;// 270
	}
	else if(isEqualToDoubleZero(angle))
	{
		if(pos_start.d1 > pos_end.d1) // down
			angle = M_PI; //180
	}
	else if(angle < 0)
	{
		if(pos_start.d1 > pos_end.d1)
			angle = M_PI + angle;
		else
			angle = 2*M_PI + angle;
	}
	else
	{
		if(pos_start.d1 > pos_end.d1)
			angle = M_PI + angle;
	}
	return angle;
}

void SpatialMath::getPosesBetweenParam(vector<Pos2D> * part,const vector<Pos2D> & v_ctrl_pos, double _start_param, double _end_param)
{
	assert(v_ctrl_pos.size() >= 2);
	part->clear();

	double start_param = min( _start_param, _end_param);
	double end_param = max( _start_param, _end_param);
	if(end_param > v_ctrl_pos.size()-1)
		end_param = v_ctrl_pos.size() - 1;

	uint_32 start = (uint_32)start_param;
	uint_32 end = (uint_32) end_param;

	if(start >= v_ctrl_pos.size()-1)
	{

		part->push_back(*v_ctrl_pos.rbegin());
		part->push_back(*v_ctrl_pos.rbegin());

		return;
	}

	Pos2D s_pos = SpatialMath::getPosFromSegmentParam(v_ctrl_pos[start],v_ctrl_pos[start+1], start_param - start);
	part->push_back(s_pos);
	for(uint_32 i = start+1; i <= end; ++i)
	{
		part->push_back(v_ctrl_pos[i]);
	}

	double seg_param = end_param - end;
	//if(SpatialMath::isEqualToDoubleZero(seg_param))
	//{
	//	part->push_back(new Pos2D(**v_ctrl_pos.rbegin()/*[end]*/));
	//}
	//else
	if(!SpatialMath::isEqualToDoubleZero(seg_param))
	{
		Pos2D end_pos = SpatialMath::getPosFromSegmentParam(v_ctrl_pos[end],v_ctrl_pos[end + 1], seg_param);
		part->push_back(end_pos);
	}

	if( _start_param > _end_param)// not start_param / end_param
	{
		reverse(part->begin(), part->end());
	}
}

double SpatialMath::calIncludedAngle(const Pos2D & pa, const Pos2D & pb,
									 const Pos2D & pA, const Pos2D & pB)
{
	Pos2D new_pB;
	new_pB.d0 = pB.d0 + pa.d0 - pA.d0; // 取pa作为中间点,将pa与pA重合
	new_pB.d1 = pB.d1 + pa.d1 - pA.d1;

	return calIncludedAngle(pa, pb, new_pB);
}

// 余弦定理求夹角, 0~M_PI
double SpatialMath::calIncludedAngle(const Pos2D & pa, const Pos2D & pm, const Pos2D & pb)
{
	double cos_value;
	double edge_len[3];

	edge_len[0] = getDistance(pa, pm);
	edge_len[1] = getDistance(pm, pb);
	edge_len[2] = getDistance(pb, pb);

	cos_value = (edge_len[0]*edge_len[0] + edge_len[1]*edge_len[1] - edge_len[2]*edge_len[2])
				/ (2*edge_len[0]*edge_len[1]);

	return acos(cos_value);
}

double SpatialMath::calCurvePointNAngle(const vector<Pos2D> & v_ctrl_pos, double param)
{
	if(v_ctrl_pos.size() < 2)
	{
		assert(0);
		return 0;
	}
	Pos2D s_pos, e_pos;
	int size = v_ctrl_pos.size();
	int param_index = (int)param;

	double angle_1  = 0,angle_2 = -1;
	if(param_index <= 0)
	{
		s_pos = v_ctrl_pos[0];
		e_pos = v_ctrl_pos[1];
	}
	else if(param_index >= size - 1)
	{
		s_pos = v_ctrl_pos[size - 1];
		e_pos = v_ctrl_pos[size - 2];
	}
	else
	{
		s_pos = v_ctrl_pos[param_index];
		e_pos = v_ctrl_pos[param_index + 1];

		if(isEqualToDoubleZero(param - param_index)) // 刚好位于中间控制点上
		{
			angle_2 = calLineNAngle(v_ctrl_pos[param_index - 1],v_ctrl_pos[param_index]);
		}
	}

	angle_1 = calLineNAngle(s_pos, e_pos);

	return ( angle_1 + (angle_2 < 0 ? angle_1 : angle_2 )) / 2; // 中间控制点的角度，取其前后两段角度平均
}

//计算圆心角，x轴正向向量同start->end向量的夹角，逆时针走向，范围：[0,2*M_PI)。若2点同返回0
double SpatialMath::getCentralAngleAgainstXAxis(const Pos2D& start, const Pos2D& end, double tolerance/* = frac*10.0*/)
{
	if(equalPos2D(start, end, tolerance))	//如果2点相同，直接返回0
		return 0;
	//Pos2D right_most(start.d0+2.0*abs(start.d0)+2.0*abs(end.d0), start.d1);
	Pos2D right_most(start.d0+2.0*fabs((start.d0))+2.0*fabs(end.d0)+10.0, start.d1);//changed by wdc 2008-3-31
	return getCentralAngle(start, right_most, end, tolerance);
}

//求由圆心+半径+圆心角所确定的点
int_32 SpatialMath::calculateCirclePoint(const Pos2D& center, double radius, double angle, Pos2D* result)
{
	result->d0 = center.d0 + radius * cos(angle);
	result->d1 = center.d1 + radius * sin(angle);
	return 0;
}

//计算圆心角，P1-Origin-P2的夹角，逆时针走向，范围：[0,2*M_PI)。若2点同返回0
double SpatialMath::getCentralAngle(const Pos2D& origin, const Pos2D& p1, const Pos2D& p2, double tolerance/* = frac*10.0*/)
{
	if(equalPos2D(origin, p1, tolerance) || equalPos2D(origin, p2, tolerance))
		return 0;
	//(x1,y1).(x2,y2) = x1*x2+y1*y2 = |(x1,y1)|*|(x2,y2)|*cos sinta
	double x1 = p1.d0 - origin.d0, y1 = p1.d1 - origin.d1,
		x2 = p2.d0 - origin.d0, y2 = p2.d1 - origin.d1;
	double len1 = sqrt(x1*x1+y1*y1), len2 = sqrt(x2*x2+y2*y2);
	if(equalToZero(len1) || equalToZero(len2))	//距离太小
		return 0;
	double cos_value = (x1*x2+y1*y2)/(len1*len2);
	//assert(-1 - DOUBLE_ZERO < cos_value
	//	&& cos_value < 1+ DOUBLE_ZERO );
	//防止因为精度表示而导致该值无效，如-1.0000000000002
	if(fabs(cos_value-1) < DOUBLE_ZERO)	//changed 2008-3-31
		cos_value = 1;
	else if(fabs(cos_value+1) < DOUBLE_ZERO)
		cos_value = -1;
	double sinta = acos(cos_value);
	Pos2D new_p1(x1,y1), new_p2(x2,y2);
	bool anticlockwise =  SpatialMath::isAnticlockwise(Pos2D(0,0), new_p1,new_p2);
	return anticlockwise ? sinta : 2.0 * M_PI - sinta;
}

double SpatialMath::TwoPointDistanceSquare(const Pos2D & pa, const Pos2D & pb)
{
	return (pb.d0 - pa.d0) * (pb.d0 - pa.d0) + (pb.d1 - pa.d1) * (pb.d1 - pa.d1 );
}

double SpatialMath::TwoPointDistance( const Pos2D & pa, const Pos2D & pb )
{
	return sqrt( TwoPointDistanceSquare( pa, pb ) );
}

bool SpatialMath::isLineCross(const Pos2D &s1, const Pos2D &e1, const Pos2D &s2, const Pos2D &e2, Pos2D* CrossPos)
{//added by wdc
	if(fabs((s1.d0 - e1.d0)) <= DOUBLE_ZERO && fabs((s2.d0 - e2.d0)) <= DOUBLE_ZERO)
	{
		return false;
	}
	else if(fabs((s1.d0 - e1.d0)) <= DOUBLE_ZERO)
	{
		double x = s1.d0;
		double y;
		if(s2.d0 == x)
		{
			y = s2.d1;
		}
		else if(e2.d0 == x)
		{
			y = e2.d1;
		}
		else
		{
			y = (s2.d1 - e2.d1)*(x - s2.d0)/(s2.d0 - e2.d0) + s2.d1;
		}
		CrossPos->d0 = x;
		CrossPos->d1 = y;
		return true;
	}
	else if(fabs((s2.d0 - e2.d0)) <= DOUBLE_ZERO)
	{
		double x = s2.d0;
		double y;
		if(s1.d0 == x)
		{
			y = s1.d1;
		}
		else if(e1.d0 == x)
		{
			y = e1.d1;
		}
		else
		{
			y = (s1.d1 - e1.d1)*(x - s1.d0)/(s1.d0 - e1.d0) + s1.d1;
		}
		CrossPos->d0 = x;
		CrossPos->d1 = y;
		return true;
	}
	else
	{
		double slope1 = (e1.d1 - s1.d1)/(e1.d0 - s1.d0);
		double slope2 = (e2.d1 - s2.d1)/(e2.d0 - s2.d0);
		if(fabs((slope1 - slope2) )<= DOUBLE_ZERO)
		{
			return false;
		}
		else
		{
			double a = e1.d1 - s1.d1;
			double b = e1.d0 - s1.d0;
			double c = e2.d1 - s2.d1;
			double d = e2.d0 - s2.d0;
			double x = (a * d * s1.d0 - b * d * s1.d1 - b * c * s2.d0 + b * d * s2.d1)/(a * d - b * c);
			double y = (a * c * s2.d0 - a * d * s2.d1 - a * c * s1.d0 + b * c * s1.d1)/(b * c - a * d);
			CrossPos->d0 = x;
			CrossPos->d1 = y;
			return true;
		}
	}
}


bool SpatialMath::isClockwise(const Pos2D & p0, const Pos2D & p1, const Pos2D & p2)
{
/*
 连续线段p0p1p2,走向为p0p1->p1p2,
 _p0p1p2逆时针方向=〉p0p1在p1p2顺时针方向=〉p0p1 * p1p2 >0。
 _p0p1p2顺时针方向=〉p0p1在p1p2逆时针方向=〉p0p1 * p1p2 <0。
*/
	return (p1.d0-p0.d0)*(p2.d1-p1.d1)-(p1.d1-p0.d1)*(p2.d0-p1.d0) < 0;
}

int_32 SpatialMath::pointOnSection( const Pos2D &startPos, const Pos2D &endPos,
								   const Pos2D &thdPos, double tolerance /*= frac*/)
{//Added by qzy!
	assert( tolerance > 0.0 );
	const double llx = thdPos.d0 - tolerance / 2.0;
	const double lly = thdPos.d1 - tolerance / 2.0;
	const double rtx = thdPos.d0 + tolerance / 2.0;
	const double rty = thdPos.d1 + tolerance / 2.0;

	if( !PureMath::LineOverArea( startPos.d0, startPos.d1,
		endPos.d0, endPos.d1, llx, lly, rtx, rty ) ) return -1;//点不在线段上

	int_32 resultA = 0;
	int_32 resultB = 0;
	if( PureMath::PointInArea( startPos.d0, startPos.d1, llx, lly, rtx, rty ) )
	{//点与startPos重合
		resultA = 1;
	}
	if( PureMath::PointInArea( endPos.d0, endPos.d1, llx, lly, rtx, rty ) )
	{//点与endPos重合
		resultB = 2;
	}

	return resultA + resultB;
}

int_32 SpatialMath::getIntersectionOfLineCirclePlus(const Pos2D& start, const Pos2D& end, const Pos2D& origin, double radius, vector<Pos2D>* result)
{
	Pos2D root(0,0);
	getFootOfPerpendicular(start,end, origin, &root);
	double distance = getDistance(root, origin);
	if(distance > radius + DOUBLE_ZERO)	//no intersection
		return -1;
	if(fabs((distance - radius)) < DOUBLE_ZERO)
	{
		result->push_back(root);
		return 0;
	}
	//利用向量平移来做
	Pos2D temp;
	//double a,b,c;
	//calStdLineEquation(start, end, &a, &b, &c);
	double sinta = calLineXAngle(start,end);
	double standard_a = cos(sinta), standard_b = sin(sinta);
	double cur_dis = sqrt(radius*radius-distance*distance);

	temp.d0 = root.d0 - cur_dis * standard_a;
	temp.d1 = root.d1 - cur_dis * standard_b;
	result->push_back(temp);

	temp.d0 = root.d0 + cur_dis * standard_a;
	temp.d1 = root.d1 + cur_dis * standard_b;
	result->push_back(temp);

	return 1;
}

int_32 SpatialMath::getIntersectionOfCircleCircle(const Pos2D& o1, const Pos2D& o2, double r, vector<Pos2D>* result)
{
	double distance = getDistance(o1, o2);
	Pos2D mid((o1.d0+o2.d0)/2.0, (o1.d1+o2.d1)/2.0);
	if(distance < DOUBLE_ZERO)
	{
		return 2;
	}
	if(distance > r*2.0 + DOUBLE_ZERO)
		return -1;
	if(fabs((distance - r*2.0)) < DOUBLE_ZERO)
	{

		result->push_back(mid);
		return 0;
	}
	Pos2D temp;
	double sinta = calLineXAngle(o1, o2);
	double standard_a = cos(sinta), standard_b = sin(sinta);
	double cur_dis = sqrt(r*r-distance*distance/4.0);

	temp.d0 = mid.d0 - cur_dis * standard_b;
	temp.d1 = mid.d1 + cur_dis * standard_a;
	result->push_back(temp);
	temp.d0 = mid.d0 + cur_dis * standard_b;
	temp.d1 = mid.d1 - cur_dis * standard_a;
	result->push_back(temp);

	return 1;
}

int_32 SpatialMath::getIntersectionOfCircleCirclePlus(const Pos2D& o1, const Pos2D& o2, double r1, double r2, vector<Pos2D>* result)
{
	if(fabs((r1 - r2)) < DOUBLE_ZERO)
	{
		return SpatialMath::getIntersectionOfCircleCircle(o1, o2, r1, result);
	}
	double distance = SpatialMath::getDistance(o1, o2);
	if(distance - fabs((r1 - r2)) < DOUBLE_ZERO)
	{
		return -1;
	}
	if(distance > r1 + r2 + DOUBLE_ZERO)
	{
		return -1;
	}
	if(fabs((distance - r1 - r2)) < DOUBLE_ZERO)
	{
		return 0;
	}
	double a = (o2.d0 - o1.d0) * 2.0;
	double b = (o2.d1 - o1.d1) * 2.0;
	double c = r1*r1 - r2*r2 - o1.d0 * o1.d0 + o2.d0 * o2.d0 - o1.d1 * o1.d1 + o2.d1 * o2.d1;

	double d = 1 + (a*a)/(b*b);
	double e = (o1.d0 + a/b*(c/b - o1.d1))*(-2);
	double f = o1.d0 * o1.d0 + (c/b - o1.d1)*(c/b - o1.d1) - r1*r1;

	double g = e*e - d*f*4.0;
	if(g < 0)
	{
		return -1;
	}
	if(g == 0)
	{
		return 0;
	}
	Pos2D temp;
	temp.d0 = (e*(-1) + sqrt(g))/(d*2);
	temp.d1 = (c - a*temp.d0)/b;
	result->push_back(temp);
	temp.d0 = (e*(-1) - sqrt(g))/(d*2);
	temp.d1 = (c - a*temp.d0)/b;
	result->push_back(temp);

	return 1;
}

//求某点在线段上的垂足。假如垂足在线段外，垂足仍然被设置，但是return -1
int_32 SpatialMath::getFootOfPerpendicular(const Pos2D& p1, const Pos2D& p2, const Pos2D& in, Pos2D* result)
{
	//copy from NearestPointOnSegment, and it should call this...
	double a, b;
	a = p2.d0 - p1.d0;
	b = p2.d1 - p1.d1;
	if(a*a + b*b < DOUBLE_ZERO)	//p1,p2重合.导致下面的表达式分母为0
	{
		result->d0 = p1.d0;
		result->d1 = p1.d1;
		return -1;
	}
	result->d0 = ( a * a * in.d0 + b * b * p1.d0 + a * b * ( in.d1 - p1.d1 ) )
				/ ( a * a + b * b );
	//result->d1 = PureMath::FindYOnLine( p1.d0, p1.d1, p2.d0, p2.d1, result->d0 );//some problems here
	if(p1.d0 == p2.d0)//changed by wdc
	{
		result->d1 = in.d1;
	}
	else
	{
		result->d1 = PureMath::FindYOnLine( p1.d0, p1.d1, p2.d0, p2.d1, result->d0 );
	}
	if( fabs((getDistance(*result, p1)+getDistance(*result, p2)-getDistance(p1, p2))) >= frac)
		return -1;
	return 0;
}

// 垂直方向平移一个线段（等距平行线）,offset >0 右侧
int SpatialMath::offsetSegment(const Pos2D &pos0, const Pos2D &pos1, double offset, Pos2D *out_pos0, Pos2D *out_pos1)
{
	int ret = 0;
	ret = pointOffsetLine(pos0, pos1, pos0, offset, out_pos0);
	ret = pointOffsetLine(pos0, pos1, pos1, offset, out_pos1);
	return ret;
}

bool SpatialMath::isParallel(const Pos2D& start1, const Pos2D& end1, const Pos2D& start2, const Pos2D& end2)
{
	double x1 = start1.d0, y1 = start1.d1,
		x2 = end1.d0, y2 = end1.d1,
		x3 = start2.d0, y3 = start2.d1,
		x4 = end2.d0, y4 = end2.d1;
	return fabs((y4 - y3)*(x2 - x1) - (y2 - y1)*(x4 - x3)) < DOUBLE_ZERO;
}

//若不相交，则返回false;同时:若平行，pos值被设为最大值；否则交点仍被求出并设置ret.by hqs
bool SpatialMath::lineCrossPoint(const Pos2D& p1, const Pos2D& p2, const Pos2D& p3, const Pos2D& p4, Pos2D* ret)
{
	return PureMath::lineCrossPoint(p1.d0, p1.d1, p2.d0, p2.d1, p3.d0, p3.d1, p4.d0, p4.d1, &ret->d0, &ret->d1);
}

bool SpatialMath::isIntersect ( const Rect2D & left, const Rect2D & right )
{
	return !(left.left_low.d0 > right.right_top.d0+DOUBLE_ZERO || left.left_low.d1 > right.right_top.d1+DOUBLE_ZERO
			|| left.right_top.d0+DOUBLE_ZERO < right.left_low.d0 || left.right_top.d1+DOUBLE_ZERO < right.left_low.d1 ) ;
}

bool SpatialMath::polygonIntersectsPolygon(const vector<Pos2D>&lv_pos, const vector<Pos2D>& rv_pos)
{
	bool isCross = false;
	Pos2D startPos1,endPos1; //被“框”选的面外环的线段起点，终点坐标


	assert(lv_pos.size() >= 3 && rv_pos.size() >= 3);

	vector<Pos2D>::const_iterator it1 = lv_pos.begin();
	vector<Pos2D>::const_iterator it2 = lv_pos.end();

	for( ; it1 != it2; ++it1 ) // 被选面上的每一个点
	{
		startPos1 = (*it1);
		if((it1+1) != it2)
		{
			endPos1 = *(it1+1);
			isCross = isLineIntersectsPolygon(startPos1,endPos1,rv_pos);
			if(isCross)
				return isCross;
		}
	}
	return false;
}

bool SpatialMath::lineWithinPolygon(const vector<Pos2D>&lv_pos, const vector<Pos2D>&rv_pos)
{
	assert(lv_pos.size() >= 2);
	vector<Pos2D>::const_iterator itr = lv_pos.begin();
	for( ; itr != lv_pos.end(); ++itr )
	{
		if( !isPointInArea(*itr, rv_pos) )
			return false;
	}

	return true;
}

bool SpatialMath::is2LineStringsIntersect(const vector<Pos2D>& l_pos, const vector<Pos2D>&r_pos)
{
	assert(l_pos.size() >= 2 && r_pos.size() >= 2);

	vector<Pos2D>::const_iterator iter1 = l_pos.begin();
	vector<Pos2D>::const_iterator iter2 = l_pos.end();
	vector<Pos2D>::const_iterator iter3 = r_pos.begin();
	vector<Pos2D>::const_iterator iter4 = r_pos.end();

	Pos2D start_pos1, end_pos1; // for l_pos
	Pos2D start_pos2, end_pos2; // for r_pos


	for(; iter1 != iter2; ++iter1)
	{
		start_pos1 = *iter1; // 线段的起点
		if((iter1+1) != iter2)
		{
			end_pos1 = *(iter1 + 1); //线段的终点
			for(iter3 = r_pos.begin(); iter3 != iter4; ++iter3)
			{
				start_pos2 = *iter3;
				if((iter3+1) != iter4)
				{
					end_pos2 = *(iter3+1);
					if(SpatialMath::isSegmentIntersect(start_pos1,end_pos1,start_pos2,end_pos2))
						return true;
				}
			}
		}
	}

	return false;
}

double SpatialMath::getDistancePointToLine( const Pos2D & pos, const Pos2D & start, const Pos2D & end , Pos2D * rt_point )
{
	findNearestPointOnLine( pos, start, end, rt_point ) ;

	return getDistance( pos, *rt_point ) ;
}

void SpatialMath::calStdLineEquation( const Pos2D & startPos, const Pos2D & endPos,
									 double * A, double * B, double * C )
{//已知两个点，计算直线的标准方程：Ax+By+Cz=0
	//两点式方程为：(y-ya)/(yb-ya) = (x-xa)/(xb-xa) =>
	//系数为：A = yb - ya, B = xa - xb,
	//c = ya( xb - xa ) - xa ( yb- ya ) = -ya( xa - xb ) - xa( yb - ya ) = ya*(-B) - xa*A
	*A = endPos.d1 - startPos.d1;
	*B = startPos.d0 - endPos.d0;
	*C = startPos.d1 * ( -(*B) ) - startPos.d0 * (*A);
}

bool SpatialMath::getLineCrossPos( double A1, double B1, double C1,
								  double A2, double B2, double C2, Pos2D * CrossPos )
{//Added by qzy!
	const double denominator = A1*B2 - A2*B1;
	if( denominator > -DOUBLE_ZERO && denominator < DOUBLE_ZERO )
	{//两直线平行
		return false;
	}

	CrossPos->d0 = ( B1*C2 - B2*C1 )/denominator;
	CrossPos->d1 = ( A2*C1 - A1*C2 )/denominator;

	return true;
}

//pos1-pos2上的点ex,当offset>0时求在前进方向右侧的对称点;否则求在左侧的对称点
int SpatialMath::pointOffsetLine(const Pos2D &pos1, const Pos2D &pos2, const Pos2D& ex, double offset, Pos2D *result)
{
	int state = relativePointsLine(pos1, pos2, ex);
	if( !(-1 <= state && state <=1) )	//not 3 points on line
		return -1;
    Pos2D start(pos1), end(pos2);
	double dis = offset;
	if(start.d0 > end.d0)
	{
		swap(start, end);
		dis *= -1.0;
	}

	double u = end.d0 - start.d0;
	double v = end.d1 - start.d1;
	double w = sqrt( u*u + v*v);
	// 利用相似三角形
	double dx = fabs(v * dis / w),
		dy = fabs(u * dis / w);
	assert( u > -1.0*DOUBLE_ZERO);	//u >= 0
	dy *= -1.0 * dis/fabs(dis);
	if(! equalToZero(v))	//分母不为0
		dx *=  (dis/fabs(dis)) * (v/fabs(v));
	result->d0 = ex.d0+dx, result->d1 = ex.d1+dy;
	return 0;
}


void SpatialMath::isLineIntersectsPolygon(const Pos2DEx& pos1, const Pos2DEx& pos2, const vector<Pos2DEx>&pos,vector<Pos2DEx> &ret_pos)
{
	ret_pos.clear();
	Pos2DEx startPos,endPos;

	vector<Pos2DEx>::const_iterator it1 = pos.begin();
	vector<Pos2DEx>::const_iterator it2 = pos.end();
	Pos2DEx tmp_pos;

	for(; it1 != it2; ++it1)
	{
		startPos = (*it1);
		if((it1+1) != it2)
		{
			endPos = (*(it1+1));

			if (SpatialMath::lineCrossPoint(pos1,pos2,startPos,endPos,&tmp_pos))
				ret_pos.push_back(tmp_pos);
		}
	}
}

bool SpatialMath::isLineIntersectsPolygon(const Pos2D& pos1, const Pos2D& pos2, const vector<Pos2D>&pos)
{
	bool isCross = false;
	Pos2D startPos,endPos;

	vector<Pos2D>::const_iterator it1 = pos.begin();
	vector<Pos2D>::const_iterator it2 = pos.end();

	for(; it1 != it2; ++it1)
	{
		startPos = (*it1);
		if((it1+1) != it2)
		{
			endPos = (*(it1+1));

			isCross = SpatialMath::isSegmentIntersect(pos1,pos2,startPos,endPos);
			if(isCross)
				return isCross;
		}
	}
	return false;
}

bool SpatialMath::isPointInArea(const Pos2D& p_pos,const vector<Pos2D>&pos)
{
	int crossNum=0; //记录交点个数
	Pos2D p_end_pos; //射线的终点   ------>
	Pos2D startPos, endPos; //线段的起点和终点

	int_32 status; ////判断点是否在线段上，“-1”——不在，“0”——在中部，“1”——与startPos重合，
	//“2”——与endPos重合，“3”——三点重合

	bool isLeft = false;
	bool isChange = false;
	int times=0;

	vector<Pos2D>::const_iterator it1 = pos.begin();
	vector<Pos2D>::const_iterator it2 = pos.end();
	for( ; it1 != it2; ++it1 )
	{
       	startPos=(*it1);
		if((it1 + 1) != it2)
		{
			endPos=*(it1+1);
			status=SpatialMath::pointOnSection(startPos,endPos,p_pos,GIS_Embedded::frac);
			switch(status)
			{
			case 0:
			case 1:
			case 2:
			case 3:
				return true;
			case -1:

				if(isLeftSide(startPos.d0,startPos.d1,endPos.d0,endPos.d1,p_pos.d0,p_pos.d1))
				{
					crossNum++;
				}
				break;
			}
		}
	}

	return crossNum % 2 ? true : false;


}

bool SpatialMath::isSegmentIntersect(const Pos2D &pos00, const Pos2D &pos01, const Pos2D &pos10, const Pos2D &pos11)
{
	return PureMath::LineCross(pos00.d0, pos00.d1, pos01.d0, pos01.d1, pos10.d0, pos10.d1, pos11.d0, pos11.d1);
}

int_32 SpatialMath::findNearestPointOnLine( const Pos2D & p0, const Pos2D & p1, const Pos2D & p2 , Pos2D * result )
{

	double x0 = p0.d0, y0 = p0.d1;
	double x1 = p1.d0, y1 = p1.d1;
	double x2 = p2.d0, y2 = p2.d1;

	double difx12;
	double dify01, dify12;
	double sdx12, sdy12;

	if(/*x1 == x2*/equalToZero(x1-x2))
	{
		result->d0 = x1;

		if(y1 > y2)
			swap(y1, y2);

		if(y0 < y1)
			result->d1 = y1;
		else if(y0 > y2)
			result->d1 = y2;
		else
			result->d1 = y0;
	}
	else
	{
		//	drangon; 2001-05-13. 寻找线段上距离某点最近的点，先找出垂线的点，如果在线内，
		//	则返回此点，否则比较两个端点谁近，返回该点即可。
		difx12 = x2 - x1;
		dify12 = y2 - y1;
		dify01 = y0 - y1;
		sdx12 = difx12 * difx12;
		sdy12 = dify12 * dify12;

		//result->d0 = (sdx12 * x0 + sdy12 * x1 + difx12 * dify12 * dify01)
		//	/ (sdx12 + sdy12);
		//采用分配律防止大数吃小数
		result->d0 = sdx12 * x0/ (sdx12 + sdy12) + sdy12 * x1/ (sdx12 + sdy12) + difx12 * dify12 * dify01/ (sdx12 + sdy12);
		result->d1 = PureMath::FindYOnLine(x1, y1, x2, y2, result->d0);

		if( (x2 > x1 && (result->d0 > x2 || result->d0 < x1) )  ||
			(x2 < x1 && (result->d0 < x2 || result->d0 > x1) )   )
		{
			double len1, len2;
			len1 = getDistance( p0, p1 );
			len2 = getDistance( p0, p2 );
			if( len1 < len2 )
				*result = p1;
			else
				*result = p2;

		}

	}

	return 0;
}

//
//              (x1,y1)          (x1,y1)
//                  /               \
//              .  /              .  \                <----------------true
//                /                   \
//				(x2,y2)          (x2,y2)
bool SpatialMath::isLeftSide(double x1, double y1, double x2, double y2, double x, double y)
{
	if( ((y > y1 && y < y2) || (y < y1 && y > y2)) && ((y - y1)*(x2 - x1) / (y2 - y1) + (x1 - x) > DOUBLE_ZERO))
		return true;
    return false;
}

//                            +2
//          -1  stdPos ------- 0 ------  endPos  +1
//                            +2
int SpatialMath::relativePointsLine( const Pos2D &stdPos , const Pos2D &endPos , const Pos2D &thdPos, double tolerance /*= frac*/)
{
	double dis0 , dis1 , dis2;

	dis0 = SpatialMath::getDistance( stdPos , endPos );
	dis1 = SpatialMath::getDistance( stdPos , thdPos );
	dis2 = SpatialMath::getDistance( endPos , thdPos );

	if( /*dis1 + dis2 - dis0 == 0*/fabs(dis1 + dis2 - dis0) < tolerance)	//by hqs 2007-5-6
	{
		return 0;
	}
	else if( /*dis1 - dis2 - dis0 == 0*/fabs(dis1 - dis2 - dis0) < tolerance)
	{
		return 1;
	}
	else if( /*dis2 - dis1 - dis0 == 0*/fabs(dis2 - dis1 - dis0) < tolerance)
	{
		return -1;
	}
	else
	{
		return 2;
	}
}

bool SpatialMath::isTheSamePosition(const Pos2D& l_pos, const Pos2D& r_pos)
{
	return fabs(l_pos.d0 - r_pos.d0) <= DOUBLE_ZERO
		&& fabs(l_pos.d1 - r_pos.d1) <= DOUBLE_ZERO;
}

// 垂直方向平移一个线段（等距平行线）,offset >0 右侧
//int SpatialMath::offsetSegment(const Pos2D &pos0, const Pos2D &pos1, double offset, Pos2D *out_pos0, Pos2D *out_pos1)
//{
//	int ret = 0;
//	ret = pointOffsetLine(pos0, pos1, pos0, offset, out_pos0);
//	ret = pointOffsetLine(pos0, pos1, pos1, offset, out_pos1);
//	return ret;
//}
//
//bool SpatialMath::isParallel(const Pos2D& start1, const Pos2D& end1, const Pos2D& start2, const Pos2D& end2)
//{
//	double x1 = start1.d0, y1 = start1.d1,
//		x2 = end1.d0, y2 = end1.d1,
//		x3 = start2.d0, y3 = start2.d1,
//		x4 = end2.d0, y4 = end2.d1;
//	return abs((y4 - y3)*(x2 - x1) - (y2 - y1)*(x4 - x3)) < DOUBLE_ZERO;
//}

//若不相交，则返回false;同时:若平行，pos值被设为最大值；否则交点仍被求出并设置ret.by hqs
//bool SpatialMath::lineCrossPoint(const Pos2D& p1, const Pos2D& p2, const Pos2D& p3, const Pos2D& p4, Pos2D* ret)
//{
//	return PureMath::lineCrossPoint(p1.d0, p1.d1, p2.d0, p2.d1, p3.d0, p3.d1, p4.d0, p4.d1, &ret->d0, &ret->d1);
//}

//bool SpatialMath::isIntersect ( const Rect2D & left, const Rect2D & right )
//{
//	return !(left.left_low.d0 > right.right_top.d0+DOUBLE_ZERO || left.left_low.d1 > right.right_top.d1+DOUBLE_ZERO
//			|| left.right_top.d0+DOUBLE_ZERO < right.left_low.d0 || left.right_top.d1+DOUBLE_ZERO < right.left_low.d1 ) ;
//}
//
//bool SpatialMath::polygonIntersectsPolygon(const vector<Pos2D>&lv_pos, const vector<Pos2D>& rv_pos)
//{
//	bool isCross = false;
//	Pos2D startPos1,endPos1; //被“框”选的面外环的线段起点，终点坐标
//
//
//	assert(lv_pos.size() >= 3 && rv_pos.size() >= 3);
//
//	vector<Pos2D>::const_iterator it1 = lv_pos.begin();
//	vector<Pos2D>::const_iterator it2 = lv_pos.end();
//
//	for( ; it1 != it2; ++it1 ) // 被选面上的每一个点
//	{
//		startPos1 = (*it1);
//		if((it1+1) != it2)
//		{
//			endPos1 = *(it1+1);
//			isCross = isLineIntersectsPolygon(startPos1,endPos1,rv_pos);
//			if(isCross)
//				return isCross;
//		}
//	}
//	return false;
//}
//
//bool SpatialMath::lineWithinPolygon(const vector<Pos2D>&lv_pos, const vector<Pos2D>&rv_pos)
//{
//	assert(lv_pos.size() >= 2);
//	vector<Pos2D>::const_iterator itr = lv_pos.begin();
//	for( ; itr != lv_pos.end(); ++itr )
//	{
//		if( !isPointInArea(*itr, rv_pos) )
//			return false;
//	}
//
//	return true;
//}
//
//bool SpatialMath::is2LineStringsIntersect(const vector<Pos2D>& l_pos, const vector<Pos2D>&r_pos)
//{
//	assert(l_pos.size() >= 2 && r_pos.size() >= 2);
//
//	vector<Pos2D>::const_iterator iter1 = l_pos.begin();
//	vector<Pos2D>::const_iterator iter2 = l_pos.end();
//	vector<Pos2D>::const_iterator iter3 = r_pos.begin();
//	vector<Pos2D>::const_iterator iter4 = r_pos.end();
//
//	Pos2D start_pos1, end_pos1; // for l_pos
//	Pos2D start_pos2, end_pos2; // for r_pos
//
//
//	for(; iter1 != iter2; ++iter1)
//	{
//		start_pos1 = *iter1; // 线段的起点
//		if((iter1+1) != iter2)
//		{
//			end_pos1 = *(iter1 + 1); //线段的终点
//			for(iter3 = r_pos.begin(); iter3 != iter4; ++iter3)
//			{
//				start_pos2 = *iter3;
//				if((iter3+1) != iter4)
//				{
//					end_pos2 = *(iter3+1);
//					if(SpatialMath::isSegmentIntersect(start_pos1,end_pos1,start_pos2,end_pos2))
//						return true;
//				}
//			}
//		}
//	}
//
//	return false;
//}

//double SpatialMath::getDistancePointToLine( const Pos2D & pos, const Pos2D & start, const Pos2D & end , Pos2D * rt_point )
//{
//	findNearestPointOnLine( pos, start, end, rt_point ) ;
//
//	return getDistance( pos, *rt_point ) ;
//}

//void SpatialMath::calStdLineEquation( const Pos2D & startPos, const Pos2D & endPos,
//									 double * A, double * B, double * C )
//{//已知两个点，计算直线的标准方程：Ax+By+Cz=0
//	//两点式方程为：(y-ya)/(yb-ya) = (x-xa)/(xb-xa) =>
//	//系数为：A = yb - ya, B = xa - xb,
//	//c = ya( xb - xa ) - xa ( yb- ya ) = -ya( xa - xb ) - xa( yb - ya ) = ya*(-B) - xa*A
//	*A = endPos.d1 - startPos.d1;
//	*B = startPos.d0 - endPos.d0;
//	*C = startPos.d1 * ( -(*B) ) - startPos.d0 * (*A);
//}
//
//bool SpatialMath::getLineCrossPos( double A1, double B1, double C1,
//								  double A2, double B2, double C2, Pos2D * CrossPos )
//{//Added by qzy!
//	const double denominator = A1*B2 - A2*B1;
//	if( denominator > -DOUBLE_ZERO && denominator < DOUBLE_ZERO )
//	{//两直线平行
//		return false;
//	}
//
//	CrossPos->d0 = ( B1*C2 - B2*C1 )/denominator;
//	CrossPos->d1 = ( A2*C1 - A1*C2 )/denominator;
//
//	return true;
//}


/////////////////////////////////////////////////////////////////////

bool SpatialMath::isPos2DInRect(const Pos2DEx& pos,const Rect2DEx& rect)
{
	return pos.d0>=rect.left_low.d0 && pos.d0<=rect.right_top.d0 && pos.d1>=rect.left_low.d1 && pos.d1<=rect.right_top.d1;
}

long SpatialMath::polygon_clip( long src_sum, const Pos2DEx * src_pts,
							   Pos2DEx * dest_pts, int x1, int y1, int x2, int y2 )
{
	// 区域裁减算法，依次将四条边外面的删掉。
	// 首先是左边
	static Pos2DEx * temp_pts = NULL;
	static uint_32 current_size = 0;
	if(current_size < src_sum * 2)
	{
		current_size = src_sum * 2;
		delete []temp_pts;
		temp_pts = new Pos2DEx[current_size];
	}

	long i, k;
	long size = src_sum;
	bool inflag;
	// 始点如果在里面，首先加入
	if( size > 0 && src_pts[0].d0 >= x1 )
	{
		temp_pts[0] = src_pts[0];
		k = 1;
		inflag = true;
	}
	else
	{
		k = 0;
		inflag = false;
	}
	for( i = 1; i < size; i++ )
	{
		if( true == inflag )
		{
			// 都在里面，加入终点，
			if( src_pts[i].d0 >= x1 )
				temp_pts[k++] = src_pts[i];
			// 由里到外，加入交界点。
			else
			{
				int y;
				y = src_pts[i - 1].d1 + ( src_pts[i].d1 - src_pts[i - 1].d1 )
					* (x1 - src_pts[i - 1].d0)
					/ ( src_pts[i].d0 - src_pts[i - 1].d0 );
				temp_pts[k].d0 = x1;
				temp_pts[k].d1 = y;
				k++;
				inflag = false;
			}
		}
		else
		{
			// 由外到内，加入交界点和终点。
			if( src_pts[i].d0 >= x1 )
			{
				int y;
				y = src_pts[i - 1].d1 + ( src_pts[i].d1 - src_pts[i - 1].d1 )
					* (x1 - src_pts[i - 1].d0)
					/ ( src_pts[i].d0 - src_pts[i - 1].d0 );
				temp_pts[k].d0 = x1;
				temp_pts[k].d1 = y;
				k++;
				temp_pts[k++] = src_pts[i];
				inflag = true;
			}
			// 都在外边，不动
			//	else if( src_pts[i].x < x1 && src_pts[i + 1].x < x1 )
			//		;
		}
	}
	// 0601；如果原来首尾点重合，新的首尾点不重合，则强制重合，保证面切割的正确性。
	if( k >= 2 && src_pts[0].d0 == src_pts[size - 1].d0
		&& src_pts[0].d1 == src_pts[size - 1].d1
		&& (temp_pts[0].d0 != temp_pts[k - 1].d0
		|| temp_pts[0].d1 != temp_pts[k - 1].d1)  )
	{
		// 增加一个点。
		temp_pts[k++] = temp_pts[0];
	}

	// 然后是右边，
	size = k;
	if( size > 0 && temp_pts[0].d0 <= x2 )
	{
		dest_pts[0] = temp_pts[0];
		k = 1;
		inflag = true;
	}
	else
	{
		k = 0;
		inflag = false;
	}
	for( i = 1; i < size; i++ )
	{
		if( true == inflag )
		{
			// 都在里面，加入终点，
			if( temp_pts[i].d0 <= x2 )
				dest_pts[k++] = temp_pts[i];
			// 由里到外，加入交界点。
			else
			{
				int y;
				y = temp_pts[i - 1].d1 + ( temp_pts[i].d1 - temp_pts[i - 1].d1 )
					* (x2 - temp_pts[i - 1].d0)
					/ ( temp_pts[i].d0 - temp_pts[i - 1].d0 ) ;
				dest_pts[k].d0 = x2;
				dest_pts[k].d1 = y;
				k++;
				inflag = false;
			}
		}
		else
		{
			// 由外到内，加入交界点和终点。
			if( temp_pts[i].d0 <= x2 )
			{
				int y;
				y = temp_pts[i - 1].d1 + ( temp_pts[i].d1 - temp_pts[i - 1].d1 )
					* (x2 - temp_pts[i - 1].d0)
					/ ( temp_pts[i].d0 - temp_pts[i - 1].d0 ) ;
				dest_pts[k].d0 = x2;
				dest_pts[k].d1 = y;
				k++;
				dest_pts[k++] = temp_pts[i];
				inflag = true;
			}
			// 都在外边，不动
		}
	}
	if( k >= 2 && temp_pts[0].d0 == temp_pts[size - 1].d0
		&& temp_pts[0].d1 == temp_pts[size - 1].d1
		&& (dest_pts[0].d0 != dest_pts[k - 1].d0
		|| dest_pts[0].d1 != dest_pts[k - 1].d1) )
	{
		// 增加一个点。
		dest_pts[k++] = dest_pts[0];
	}

	// 首先是下边
	size = k;
	if( k > 0 && dest_pts[0].d1 >= y1 )
	{
		temp_pts[0] = dest_pts[0];
		k = 1;
		inflag = true;
	}
	else
	{
		k = 0;
		inflag = false;
	}
	for( i = 1; i < size; i++ )
	{
		if( true == inflag )
		{
			// 都在里面，加入终点，
			if( dest_pts[i].d1 >= y1 )
				temp_pts[k++] = dest_pts[i];
			// 由里到外，加入交界点。
			else
			{
				int x;
				x = dest_pts[i - 1].d0 + ( dest_pts[i].d0 - dest_pts[i - 1].d0 )
					* (y1 - dest_pts[i - 1].d1)
					/ ( dest_pts[i].d1 - dest_pts[i - 1].d1 ) ;
				temp_pts[k].d0 = x;
				temp_pts[k].d1 = y1;
				k++;
				inflag = false;
			}
		}
		else
		{
			// 由外到内，加入交界点和终点。
			if( dest_pts[i].d1 >= y1 )
			{
				int x;
				x = dest_pts[i - 1].d0 + ( dest_pts[i].d0 - dest_pts[i - 1].d0 )
					* (y1 - dest_pts[i - 1].d1)
					/ ( dest_pts[i].d1 - dest_pts[i - 1].d1 ) ;
				temp_pts[k].d0 = x;
				temp_pts[k].d1 = y1;
				k++;
				temp_pts[k++] = dest_pts[i];
				inflag = true;
			}
			// 都在外边，不动
		}
	}
	if( k >= 2 && dest_pts[0].d0 == dest_pts[size - 1].d0
		&& dest_pts[0].d1 == dest_pts[size - 1].d1
		&& (temp_pts[0].d0 != temp_pts[k - 1].d0
		|| temp_pts[0].d1 != temp_pts[k - 1].d1) )
	{
		// 增加一个点。
		temp_pts[k++] = temp_pts[0];
	}

	// 最后是上边。
	size = k;
	if( k > 0 && temp_pts[0].d1 <= y2 )
	{
		dest_pts[0] = temp_pts[0];
		k = 1;
		inflag = true;
	}
	else
	{
		k = 0;
		inflag = false;
	}
	for( i = 1; i < size; i++ )
	{
		if( true == inflag )
		{
			// 都在里面，加入终点，
			if( temp_pts[i].d1 <= y2)
				dest_pts[k++] = temp_pts[i];
			// 由里到外，加入交界点。
			else
			{
				int x;
				x = temp_pts[i - 1].d0 + ( temp_pts[i].d0 - temp_pts[i - 1].d0 )
					* (y2 - temp_pts[i - 1].d1)
					/ ( temp_pts[i].d1 - temp_pts[i - 1].d1 ) ;
				dest_pts[k].d0 = x;
				dest_pts[k].d1 = y2;
				k++;
				inflag = false;
			}
		}
		else
		{
			// 由外到内，加入交界点和终点。
			if( temp_pts[i].d1 <= y2 )
			{
				int x;
				x = temp_pts[i - 1].d0 + ( temp_pts[i].d0 - temp_pts[i - 1].d0 )
					* (y2 - temp_pts[i - 1].d1)
					/ ( temp_pts[i].d1 - temp_pts[i - 1].d1 ) ;
				dest_pts[k].d0 = x;
				dest_pts[k].d1 = y2;
				k++;
				dest_pts[k++] = temp_pts[i];
				inflag = true;
			}
			// 都在外边，不动
		}
	}
	if( k >= 2 && temp_pts[0].d0 == temp_pts[size - 1].d0
		&& temp_pts[0].d1 == temp_pts[size - 1].d1
		&& (dest_pts[0].d0 != dest_pts[k - 1].d0
		|| dest_pts[0].d1 != dest_pts[k - 1].d1) )
	{
		// 增加一个点。
		dest_pts[k++] = dest_pts[0];
	}

	//delete[] temp_pts;


	//add by my 删除重复点
	int valid_index = 0;
	for(int index = 1; index < k; index ++)
	{
		if(dest_pts[index] != dest_pts[valid_index])
		{
			++ valid_index;
			if(valid_index != index)
				dest_pts[valid_index] = dest_pts[index];
		}
	}

	//by my
	return valid_index + 1;
	//return k;
}

//inline bool equalToZero(int x){return fabs(x) < DOUBLE_ZERO;}

inline bool equalPos2D(const Pos2DEx& p1, const Pos2DEx& p2)
{
	return p1 == p2;
}

//bool SpatialMath::isAnticlockwise(const Pos2DEx & p0, const Pos2DEx & p1, const Pos2DEx & p2)
//{
//	/*
//	连续线段p0p1p2,走向为p0p1->p1p2,
//	_p0p1p2逆时针方向=〉p0p1在p1p2顺时针方向=〉p0p1 * p1p2 >0。
//	_p0p1p2顺时针方向=〉p0p1在p1p2逆时针方向=〉p0p1 * p1p2 <0。
//	其中p0p1 * p1p2 = |p1.x-p0.x p1.y-p0.y|
//	|p2.x-p1.x p2.y-p1.y|
//	*/
//	return (p1.d0-p0.d0)*(p2.d1-p1.d1)-(p1.d1-p0.d1)*(p2.d0-p1.d0) > 0;
//}


int SpatialMath::getDistance(const vector<Pos2DEx> & pos_array)
{
	int dis = 0, sum = pos_array.size();
	for(int i = 0; i < sum - 1; i ++)
	{
		dis += getDistance(pos_array[i], pos_array[i + 1]);
	}
	return dis;
}

int SpatialMath::getDistance(const vector<Pos2DEx> & pos_array,const Fraction &_param_start, const Fraction &_param_end)
{
	int dis = 0;
	if(pos_array.empty())
		return dis;
	uint_32 max_index = pos_array.size() - 1;
	Fraction max_param(max_index, 1);
	Fraction param_start(_param_start), param_end(_param_end);
	//zgr, 2008-01-15, 确保param_start < param_end
	if(param_start > param_end)
		swap(param_start,param_end);

	param_start = max(Fraction(0, 1), param_start);
	param_end = min(param_end, max_param);

	if(param_end.numerator == 0)
		param_end = max_param;

	uint_32 s = uint_32(param_start.numerator / param_start.denominator);
	uint_32 e = uint_32(param_end.numerator / param_end.denominator);

	if(s != max_index && e > s)
	{
		Fraction tmp_param(Fraction(s + 1, 1) - param_start);
		dis += getDistance(pos_array[s], pos_array[s+1]) * tmp_param.numerator / tmp_param.denominator;
	}
	for(uint_32 i = s + 1; i < e; ++ i)
	{

		dis += getDistance(pos_array[i], pos_array[i+1]);

	}
	if(e != max_index)
	{
		Fraction tmp_param(param_end - Fraction(e , 1));
		dis += getDistance(pos_array[e], pos_array[e+1]) * tmp_param.numerator / tmp_param.denominator;
	}

	return dis;
}

Pos2DEx SpatialMath::NearestPointOnSegment(const Pos2DEx & aa, const Pos2DEx & pp, const Pos2DEx & qq )
{
	Pos2DEx result;
	// 先判断目标直线是否垂直。
	if( pp.d0 == qq.d0 )
	{
		result.d0 = pp.d0;

		int max, min;
		if( pp.d1 > qq.d1 )
			max = pp.d1, min = qq.d1;
		else
			min = pp.d1, max = qq.d1;

		if( aa.d1 > max )
			result.d1 = max;
		else if( aa.d1 < min )
			result.d1 = min;
		else
			result.d1 = aa.d1;
	}
	else
	{
		z11::int_64 a, b;
		a = qq.d0 - pp.d0;
		b = qq.d1 - pp.d1;
		result.d0 = ( a * a * aa.d0 + b * b * pp.d0 + a * b * ( aa.d1 - pp.d1 ) )
			/ ( a * a + b * b );
		if(a == 0)
			result.d1 = aa.d1;
		else
			result.d1 = PureMath::FindYOnLine( pp.d0, pp.d1, qq.d0, qq.d1, result.d0 );

		// 判断是否在线内。
		if( (qq.d0 > pp.d0 && (result.d0 > qq.d0 || result.d0 < pp.d0) )
			|| (qq.d0 < pp.d0 && (result.d0 < qq.d0 || result.d0 > pp.d0) )   )
		{
			z11::int_64 len1, len2;
			len1 = getDistanceSquare( aa, pp );
			len2 = getDistanceSquare( aa, qq );
			if( len1 < len2 )
				result = pp;
			else
				result = qq;
		}
	}

	return result;
}
int SpatialMath::NearestPointOnSegmentString(Pos2DEx * pos, Fraction * param,const Pos2DEx & aa, const vector<Pos2DEx> & pos_array,int tolerance )
{
	int ret = -1;
	int min_dis = INT_MAX;
	Pos2DEx nearest_pos;
	int tol_squ = tolerance * tolerance;

	uint_32 size = pos_array.size();
	if(size == 1)
	{
		return INT_MAX;
	}

	for(uint_32 i = 1; i < size; ++i)
	{

		Pos2DEx cur_pos = NearestPointOnSegment(aa, pos_array[i-1],pos_array[i]);

		z11::int_64 cur_dis = getDistanceSquare(aa, cur_pos);
		if(min_dis > cur_dis)
		{
			nearest_pos = cur_pos;
			*param = Fraction(i-1, 1);
			min_dis = cur_dis;
			if(min_dis < tol_squ)
				break;
		}
	}

	*pos = nearest_pos;


	int int_param = param->getIntValue();

	// debug
	if(int_param >= 1000)
		int debug_zz = 0;
	//

	*param += PointPositionOnSegment(nearest_pos, pos_array[int_param], pos_array[int_param + 1]);

	return (int)sqrt(0.0+min_dis);
}

int SpatialMath::NearestPointOnSegmentString(Pos2DEx * pos, Fraction * param,const Pos2DEx & aa, const vector<vector<Pos2DEx> > & pos_array,int tolerance )
{
	int ret = -1;
	int min_dis = INT_MAX;
	Pos2DEx nearest_pos;
	int tol_squ = tolerance * tolerance;

	uint_32 size = pos_array.size();
	uint_32 pos_add;
	for (uint_32 j=0; j<size ; j++)
	{
		uint_32 each_size = pos_array[j].size();
		if(each_size == 1)
		{
			return INT_MAX;
		}

		for(uint_32 i = 1; i < each_size; ++i)
		{

			Pos2DEx cur_pos = NearestPointOnSegment(aa, pos_array[j].at(i-1),pos_array[j].at(i));

			z11::int_64 cur_dis = getDistanceSquare(aa, cur_pos);
			if(min_dis > cur_dis)
			{
				pos_add = j;
				nearest_pos = cur_pos;
				*param = Fraction(i-1, 1);
				min_dis = cur_dis;
				if(min_dis < tol_squ)
					break;
			}
		}
	}
	*pos = nearest_pos;

	int int_param = param->getIntValue();

	*param += PointPositionOnSegment(nearest_pos, pos_array[pos_add].at(int_param), pos_array[pos_add].at(int_param + 1));

	return (int)sqrt(0.0+min_dis);
}

//zj 2010-1-11 返回小数部分分割的点，_param 属于[0, 1], 当为0时，返回pp, 当为1时返回 qq
Pos2DEx SpatialMath::getPosFromSegmentParam(const Pos2DEx & pp, const Pos2DEx & qq,
										  const Fraction &_param)
{
	Pos2DEx ret;
	Fraction param(_param);
	Fraction fraction_zero(0, 1);
	Fraction fraction_one(1, 1);

	if(param <= fraction_zero)
	{
		ret.d0 = pp.d0, ret.d1 = pp.d1;
		return ret;
	}

	if(param >= fraction_one)
	{
		ret.d0 = qq.d0, ret.d1 = qq.d1;
		return ret;
	}

	Fraction tmp = fraction_one - param;
	ret.d0 = ((z11::int_64)pp.d0) * tmp.numerator / tmp.denominator + ((z11::int_64)qq.d0) * param.numerator / param.denominator;
	ret.d1 = ((z11::int_64)pp.d1) * tmp.numerator / tmp.denominator + ((z11::int_64)qq.d1) * param.numerator / param.denominator;

	return ret;
}

Fraction SpatialMath::PointPositionOnSegment(const Pos2DEx & aa, const Pos2DEx & pp, const Pos2DEx & qq )
{
	if(pp == qq) // 两个点相等
		return Fraction();
	return Fraction(getDistance(aa, pp), getDistance(qq, pp));
}
//int SpatialMath::calLineXAngle(const Pos2DEx &pos_start, const Pos2DEx &pos_end)
//{
//	int angle;
//	int tan_value;
//
//	if(pos_end.d0 == pos_start.d0)
//		angle = M_PI/2;
//	else
//	{
//		tan_value = (pos_end.d1 - pos_start.d1) / (pos_end.d0 - pos_start.d0);
//		angle = atan(tan_value);
//	}
//
//	return angle;
//}
//
double SpatialMath::calLineYAngle(const Pos2DEx &pos_start, const Pos2DEx &pos_end)
{
	double angle;
	double tan_value;

	if(pos_end.d1 == pos_start.d1)
		angle = M_PI/2;
	else
	{
		tan_value = (double)(pos_end.d0 - pos_start.d0) / (pos_end.d1 - pos_start.d1);
		angle = atan(tan_value);
	}

	return angle;
}
double SpatialMath::calLineNAngle(const Pos2DEx & pos_start, const Pos2DEx & pos_end)
{
	double angle = calLineYAngle(pos_start, pos_end);
	if(M_PI/2 == fabs(angle))
	{
		if(pos_start.d0 > pos_end.d0)// <-
			angle = 3*M_PI/2;// 270
	}
	else if(isEqualToDoubleZero(angle))
	{
		if(pos_start.d1 > pos_end.d1) // down
			angle = M_PI; //180
	}
	else if(angle < 0)
	{
		if(pos_start.d1 > pos_end.d1)
			angle = M_PI + angle;
		else
			angle = 2*M_PI + angle;
	}
	else
	{
		if(pos_start.d1 > pos_end.d1)
			angle = M_PI + angle;
	}
	return angle;
}

// zj 2010-1-11 返回子曲线段，返回结果中除第一个和最后一个点可能不是v_ctrl_pos中的原存在的点，其它点都是
void SpatialMath::getPosesBetweenParam(vector<Pos2DEx> * part,const vector<Pos2DEx> & v_ctrl_pos, const Fraction &_start_param, const Fraction &_end_param)
{
	assert(v_ctrl_pos.size() >= 2);
	part->clear();

	Fraction start_param;
	Fraction end_param;
	start_param = min( _start_param, _end_param);
	end_param = max( _start_param, _end_param);

	Fraction last_param(v_ctrl_pos.size() - 1, 1);
	if(end_param > last_param)
		end_param = last_param;

	uint_32 start = (uint_32)start_param.numerator / start_param.denominator;
	uint_32 end = (uint_32) end_param.numerator / end_param.denominator;

	if(start >= v_ctrl_pos.size()-1)
	{

		part->push_back(*v_ctrl_pos.rbegin());
		part->push_back(*v_ctrl_pos.rbegin());

		return;
	}



	Pos2DEx s_pos = SpatialMath::getPosFromSegmentParam(v_ctrl_pos[start],v_ctrl_pos[start+1], start_param.getDecimalFraction());
	part->push_back(s_pos);
	for(uint_32 i = start+1; i <= end; ++i)
	{
		part->push_back(v_ctrl_pos[i]);
	}

	Fraction seg_param = end_param.getDecimalFraction();
	//if(SpatialMath::isEqualToDoubleZero(seg_param))
	//{
	//	part->push_back(new Pos2DEx(**v_ctrl_pos.rbegin()/*[end]*/));
	//}
	//else
	if(!seg_param.numerator == 0)
	{
		Pos2DEx end_pos = SpatialMath::getPosFromSegmentParam(v_ctrl_pos[end],v_ctrl_pos[end + 1], seg_param);
		part->push_back(end_pos);
	}

	if( _start_param > _end_param)// not start_param / end_param
	{
		reverse(part->begin(), part->end());
	}
}

//int SpatialMath::calIncludedAngle(const Pos2DEx & pa, const Pos2DEx & pb,
//									 const Pos2DEx & pA, const Pos2DEx & pB)
//{
//	Pos2DEx new_pB;
//	new_pB.d0 = pB.d0 + pa.d0 - pA.d0; // 取pa作为中间点,将pa与pA重合
//	new_pB.d1 = pB.d1 + pa.d1 - pA.d1;
//
//	return calIncludedAngle(pa, pb, new_pB);
//}
//
//// 余弦定理求夹角, 0~M_PI
//int SpatialMath::calIncludedAngle(const Pos2DEx & pa, const Pos2DEx & pm, const Pos2DEx & pb)
//{
//	int cos_value;
//	int edge_len[3];
//
//	edge_len[0] = getDistance(pa, pm);
//	edge_len[1] = getDistance(pm, pb);
//	edge_len[2] = getDistance(pb, pb);
//
//	cos_value = (edge_len[0]*edge_len[0] + edge_len[1]*edge_len[1] - edge_len[2]*edge_len[2])
//		/ (2*edge_len[0]*edge_len[1]);
//
//	return acos(cos_value);
//}
//
double SpatialMath::calCurvePointNAngle(const vector<Pos2DEx> & v_ctrl_pos, const Fraction &param)
{
	if(v_ctrl_pos.size() < 2)
	{
		assert(0);
		return 0;
	}
	Pos2DEx s_pos, e_pos;
	int size = v_ctrl_pos.size();
	int param_index = param.getIntValue();

	double angle_1 = 0,angle_2 = -1;
	if(param_index <= 0)
	{
		s_pos = v_ctrl_pos[0];
		e_pos = v_ctrl_pos[1];
	}
	else if(param_index >= size - 1)
	{
		s_pos = v_ctrl_pos[size - 1];
		e_pos = v_ctrl_pos[size - 2];
	}
	else
	{
		s_pos = v_ctrl_pos[param_index];
		e_pos = v_ctrl_pos[param_index + 1];

		if(param.getDecimalFraction().numerator == 0) // 刚好位于中间控制点上
		{
			angle_2 = calLineNAngle(v_ctrl_pos[param_index - 1],v_ctrl_pos[param_index]);
		}
	}

	angle_1 = calLineNAngle(s_pos, e_pos);

	return ( angle_1 + (angle_2 < 0 ? angle_1 : angle_2 )) / 2; // 中间控制点的角度，取其前后两段角度平均
}

//计算圆心角，x轴正向向量同start->end向量的夹角，逆时针走向，范围：[0,2*M_PI)。若2点同返回0
//int SpatialMath::getCentralAngleAgainstXAxis(const Pos2DEx& start, const Pos2DEx& end, int tolerance/* = frac*10.0*/)
//{
//	if(equalPos2D(start, end, tolerance))	//如果2点相同，直接返回0
//		return 0;
//	//Pos2DEx right_most(start.d0+2.0*abs(start.d0)+2.0*abs(end.d0), start.d1);
//	Pos2DEx right_most(start.d0+2.0*fabs((start.d0))+2.0*fabs(end.d0)+10.0, start.d1);//changed by wdc 2008-3-31
//	return getCentralAngle(start, right_most, end, tolerance);
//}

//求由圆心+半径+圆心角所确定的点
//int_32 SpatialMath::calculateCirclePoint(const Pos2DEx& center, int radius, int angle, Pos2DEx* result)
//{
//	result->d0 = center.d0 + radius * cos(angle);
//	result->d1 = center.d1 + radius * sin(angle);
//	return 0;
//}

//计算圆心角，P1-Origin-P2的夹角，逆时针走向，范围：[0,2*M_PI)。若2点同返回0
//int SpatialMath::getCentralAngle(const Pos2DEx& origin, const Pos2DEx& p1, const Pos2DEx& p2, int tolerance/* = frac*10.0*/)
//{
//	if(equalPos2D(origin, p1, tolerance) || equalPos2D(origin, p2, tolerance))
//		return 0;
//	//(x1,y1).(x2,y2) = x1*x2+y1*y2 = |(x1,y1)|*|(x2,y2)|*cos sinta
//	int x1 = p1.d0 - origin.d0, y1 = p1.d1 - origin.d1,
//		x2 = p2.d0 - origin.d0, y2 = p2.d1 - origin.d1;
//	int len1 = sqrt(x1*x1+y1*y1), len2 = sqrt(x2*x2+y2*y2);
//	if(equalToZero(len1) || equalToZero(len2))	//距离太小
//		return 0;
//	int cos_value = (x1*x2+y1*y2)/(len1*len2);
//	//assert(-1 - DOUBLE_ZERO < cos_value
//	//	&& cos_value < 1+ DOUBLE_ZERO );
//	//防止因为精度表示而导致该值无效，如-1.0000000000002
//	if(fabs(cos_value-1) < DOUBLE_ZERO)	//changed 2008-3-31
//		cos_value = 1;
//	else if(fabs(cos_value+1) < DOUBLE_ZERO)
//		cos_value = -1;
//	int sinta = acos(cos_value);
//	Pos2DEx new_p1(x1,y1), new_p2(x2,y2);
//	bool anticlockwise =  SpatialMath::isAnticlockwise(Pos2DEx(0,0), new_p1,new_p2);
//	return anticlockwise ? sinta : 2.0 * M_PI - sinta;
//}

int SpatialMath::TwoPointDistanceSquare(const Pos2DEx & pa, const Pos2DEx & pb)
{
	return (pb.d0 - pa.d0) * (pb.d0 - pa.d0) + (pb.d1 - pa.d1) * (pb.d1 - pa.d1 );
}

int SpatialMath::TwoPointDistance( const Pos2DEx & pa, const Pos2DEx & pb )
{
	return (int)sqrt( 0.0+TwoPointDistanceSquare( pa, pb ) );
}

bool SpatialMath::isLineCross(const Pos2DEx &s1, const Pos2DEx &e1, const Pos2DEx &s2, const Pos2DEx &e2, Pos2DEx* CrossPos)
{//added by wdc
	//Pos2D ss1(s1.d0, s1.d1), ee1(e1.d0, e1.d1), ss2(s2.d0, s2.d1), ee2(e2.d0, e2.d1), cp;
	//bool b = isLineCross(ss1, ee1, ss2, ee2, &cp);
	//CrossPos->d0 = cp.d0;
	//CrossPos->d1 = cp.d1;
	//return b;

	if(s1.d0 == e1.d0 && s2.d0 == e2.d0)
	{
		return false;
	}
	else if(s1.d0 == e1.d0)
	{
		int x = s1.d0;
		int y;
		if(s2.d0 == x)
		{
			y = s2.d1;
		}
		else if(e2.d0 == x)
		{
			y = e2.d1;
		}
		else
		{
			assert((s2.d0 - e2.d0)!=0);
			y = /*z11::int_64*/(s2.d1 - e2.d1)*(x - s2.d0)/(s2.d0 - e2.d0) + s2.d1;
		}
		CrossPos->d0 = x;
		CrossPos->d1 = y;
		return true;
	}
	else if(s2.d0 == e2.d0)
	{
		int x = s2.d0;
		int y;
		if(s1.d0 == x)
		{
			y = s1.d1;
		}
		else if(e1.d0 == x)
		{
			y = e1.d1;
		}
		else
		{
			assert((s1.d0 - e1.d0)!=0);
			y = /*z11::int_64*/(s1.d1 - e1.d1)*(x - s1.d0)/(s1.d0 - e1.d0) + s1.d1;
		}
		CrossPos->d0 = x;
		CrossPos->d1 = y;
		return true;
	}
	else
	{
		Fraction slope1(e1.d1 - s1.d1, e1.d0 - s1.d0);
		Fraction slope2(e2.d1 - s2.d1, e2.d0 - s2.d0);
		if(slope1 == slope2)
		{
			return false;
		}
		else
		{
			z11::int_64 a = e1.d1 - s1.d1;
			z11::int_64 b = e1.d0 - s1.d0;
			z11::int_64 c = e2.d1 - s2.d1;
			z11::int_64 d = e2.d0 - s2.d0;

			assert((a * d - b * c)!=0);

			int x = (a * d * (s1.d0) - b * d * (s1.d1) - b * c * (s2.d0) + b * d * (s2.d1))/(a * d - b * c);
			int y = (a * c * (s2.d0) - a * d * (s2.d1) - a * c * (s1.d0) + b * c * (s1.d1))/(b * c - a * d);
			CrossPos->d0 = x;
			CrossPos->d1 = y;
			return true;
		}
	}
}


//bool SpatialMath::isClockwise(const Pos2DEx & p0, const Pos2DEx & p1, const Pos2DEx & p2)
//{
//	/*
//	连续线段p0p1p2,走向为p0p1->p1p2,
//	_p0p1p2逆时针方向=〉p0p1在p1p2顺时针方向=〉p0p1 * p1p2 >0。
//	_p0p1p2顺时针方向=〉p0p1在p1p2逆时针方向=〉p0p1 * p1p2 <0。
//	*/
//	return (p1.d0-p0.d0)*(p2.d1-p1.d1)-(p1.d1-p0.d1)*(p2.d0-p1.d0) < 0;
//}

int_32 SpatialMath::pointOnSection( const Pos2DEx &startPos, const Pos2DEx &endPos,
								   const Pos2DEx &thdPos, double tolerance /*= frac*/)
{//Added by qzy!
	assert( tolerance > 0.0 );
	const int llx = thdPos.d0 - tolerance / 2.0;
	const int lly = thdPos.d1 - tolerance / 2.0;
	const int rtx = thdPos.d0 + tolerance / 2.0;
	const int rty = thdPos.d1 + tolerance / 2.0;

	if( !PureMath::LineOverArea( startPos.d0, startPos.d1,
		endPos.d0, endPos.d1, llx, lly, rtx, rty ) ) return -1;//点不在线段上

	int_32 resultA = 0;
	int_32 resultB = 0;
	if( PureMath::PointInArea( startPos.d0, startPos.d1, llx, lly, rtx, rty ) )
	{//点与startPos重合
		resultA = 1;
	}
	if( PureMath::PointInArea( endPos.d0, endPos.d1, llx, lly, rtx, rty ) )
	{//点与endPos重合
		resultB = 2;
	}

	return resultA + resultB;
}
//
//int_32 SpatialMath::getIntersectionOfLineCirclePlus(const Pos2DEx& start, const Pos2DEx& end, const Pos2DEx& origin, int radius, vector<Pos2DEx>* result)
//{
//	Pos2DEx root(0,0);
//	getFootOfPerpendicular(start,end, origin, &root);
//	int distance = getDistance(root, origin);
//	if(distance > radius + DOUBLE_ZERO)	//no intersection
//		return -1;
//	if(fabs((distance - radius)) < DOUBLE_ZERO)
//	{
//		result->push_back(root);
//		return 0;
//	}
//	//利用向量平移来做
//	Pos2DEx temp;
//	//int a,b,c;
//	//calStdLineEquation(start, end, &a, &b, &c);
//	int sinta = calLineXAngle(start,end);
//	int standard_a = cos(sinta), standard_b = sin(sinta);
//	int cur_dis = sqrt(radius*radius-distance*distance);
//
//	temp.d0 = root.d0 - cur_dis * standard_a;
//	temp.d1 = root.d1 - cur_dis * standard_b;
//	result->push_back(temp);
//
//	temp.d0 = root.d0 + cur_dis * standard_a;
//	temp.d1 = root.d1 + cur_dis * standard_b;
//	result->push_back(temp);
//
//	return 1;
//}
//
//int_32 SpatialMath::getIntersectionOfCircleCircle(const Pos2DEx& o1, const Pos2DEx& o2, int r, vector<Pos2DEx>* result)
//{
//	int distance = getDistance(o1, o2);
//	Pos2DEx mid((o1.d0+o2.d0)/2.0, (o1.d1+o2.d1)/2.0);
//	if(distance < DOUBLE_ZERO)
//	{
//		return 2;
//	}
//	if(distance > r*2.0 + DOUBLE_ZERO)
//		return -1;
//	if(fabs((distance - r*2.0)) < DOUBLE_ZERO)
//	{
//
//		result->push_back(mid);
//		return 0;
//	}
//	Pos2DEx temp;
//	int sinta = calLineXAngle(o1, o2);
//	int standard_a = cos(sinta), standard_b = sin(sinta);
//	int cur_dis = sqrt(r*r-distance*distance/4.0);
//
//	temp.d0 = mid.d0 - cur_dis * standard_b;
//	temp.d1 = mid.d1 + cur_dis * standard_a;
//	result->push_back(temp);
//	temp.d0 = mid.d0 + cur_dis * standard_b;
//	temp.d1 = mid.d1 - cur_dis * standard_a;
//	result->push_back(temp);
//
//	return 1;
//}
//
//int_32 SpatialMath::getIntersectionOfCircleCirclePlus(const Pos2DEx& o1, const Pos2DEx& o2, int r1, int r2, vector<Pos2DEx>* result)
//{
//	if(fabs((r1 - r2)) < DOUBLE_ZERO)
//	{
//		return SpatialMath::getIntersectionOfCircleCircle(o1, o2, r1, result);
//	}
//	int distance = SpatialMath::getDistance(o1, o2);
//	if(distance - fabs((r1 - r2)) < DOUBLE_ZERO)
//	{
//		return -1;
//	}
//	if(distance > r1 + r2 + DOUBLE_ZERO)
//	{
//		return -1;
//	}
//	if(fabs((distance - r1 - r2)) < DOUBLE_ZERO)
//	{
//		return 0;
//	}
//	int a = (o2.d0 - o1.d0) * 2.0;
//	int b = (o2.d1 - o1.d1) * 2.0;
//	int c = r1*r1 - r2*r2 - o1.d0 * o1.d0 + o2.d0 * o2.d0 - o1.d1 * o1.d1 + o2.d1 * o2.d1;
//
//	int d = 1 + (a*a)/(b*b);
//	int e = (o1.d0 + a/b*(c/b - o1.d1))*(-2);
//	int f = o1.d0 * o1.d0 + (c/b - o1.d1)*(c/b - o1.d1) - r1*r1;
//
//	int g = e*e - d*f*4.0;
//	if(g < 0)
//	{
//		return -1;
//	}
//	if(g == 0)
//	{
//		return 0;
//	}
//	Pos2DEx temp;
//	temp.d0 = (e*(-1) + sqrt(g))/(d*2);
//	temp.d1 = (c - a*temp.d0)/b;
//	result->push_back(temp);
//	temp.d0 = (e*(-1) - sqrt(g))/(d*2);
//	temp.d1 = (c - a*temp.d0)/b;
//	result->push_back(temp);
//
//	return 1;
//}
//
////求某点在线段上的垂足。假如垂足在线段外，垂足仍然被设置，但是return -1
//int_32 SpatialMath::getFootOfPerpendicular(const Pos2DEx& p1, const Pos2DEx& p2, const Pos2DEx& in, Pos2DEx* result)
//{
//	//copy from NearestPointOnSegment, and it should call this...
//	int a, b;
//	a = p2.d0 - p1.d0;
//	b = p2.d1 - p1.d1;
//	if(a*a + b*b < DOUBLE_ZERO)	//p1,p2重合.导致下面的表达式分母为0
//	{
//		result->d0 = p1.d0;
//		result->d1 = p1.d1;
//		return -1;
//	}
//	result->d0 = ( a * a * in.d0 + b * b * p1.d0 + a * b * ( in.d1 - p1.d1 ) )
//		/ ( a * a + b * b );
//	//result->d1 = PureMath::FindYOnLine( p1.d0, p1.d1, p2.d0, p2.d1, result->d0 );//some problems here
//	if(p1.d0 == p2.d0)//changed by wdc
//	{
//		result->d1 = in.d1;
//	}
//	else
//	{
//		result->d1 = PureMath::FindYOnLine( p1.d0, p1.d1, p2.d0, p2.d1, result->d0 );
//	}
//	if( fabs((getDistance(*result, p1)+getDistance(*result, p2)-getDistance(p1, p2))) >= frac)
//		return -1;
//	return 0;
//}
//
// 垂直方向平移一个线段（等距平行线）,offset >0 右侧
int SpatialMath::offsetSegment(const Pos2DEx &pos0, const Pos2DEx &pos1, int offset, Pos2DEx *out_pos0, Pos2DEx *out_pos1)
{
	int ret = 0;
	ret = pointOffsetLine(pos0, pos1, pos0, offset, out_pos0);
	ret = pointOffsetLine(pos0, pos1, pos1, offset, out_pos1);

	//Pos2D p0, p1, o0, o1;
	//p0.d0 = pos0.d0, p0.d1 = pos0.d1;
	//p1.d0 = pos1.d0, p1.d1 = pos1.d1;
	//ret = pointOffsetLine(p0, p1, p0, offset, &o0);
	//ret = pointOffsetLine(p0, p1, p1, offset, &o1);
	//if(o0.d0 - int(o0.d0) > 0.5)
	//	o0.d0 += 1;
	//if(o0.d1 - int(o0.d1) > 0.5)
	//	o0.d1 += 1;
	//if(o1.d0 - int(o1.d0) > 0.5)
	//	o1.d0 += 1;
	//if(o1.d1 - int(o1.d1) > 0.5)
	//	o1.d1 += 1;
	//out_pos0->d0 = o0.d0;
	//out_pos0->d1 = o0.d1;
	//out_pos1->d0 = o1.d0;
	//out_pos1->d1 = o1.d1;

	return ret;
}

bool SpatialMath::isParallel(const Pos2DEx& start1, const Pos2DEx& end1, const Pos2DEx& start2, const Pos2DEx& end2)
{
	int x1 = start1.d0, y1 = start1.d1,
		x2 = end1.d0, y2 = end1.d1,
		x3 = start2.d0, y3 = start2.d1,
		x4 = end2.d0, y4 = end2.d1;
	return /*z11::int_64*/(y4 - y3)*(x2 - x1) == /*z11::int_64*/(y2 - y1)*(x4 - x3);
}

//若不相交，则返回false;同时:若平行，pos值被设为最大值；否则交点仍被求出并设置ret.by hqs
bool SpatialMath::lineCrossPoint(const Pos2DEx& p1, const Pos2DEx& p2, const Pos2DEx& p3, const Pos2DEx& p4, Pos2DEx* ret)
{
	return PureMath::lineCrossPoint(p1.d0, p1.d1, p2.d0, p2.d1, p3.d0, p3.d1, p4.d0, p4.d1, &ret->d0, &ret->d1);
	//double tmp1, tmp2;
	//bool b =  PureMath::lineCrossPoint((double)p1.d0, (double)p1.d1, (double)p2.d0, (double)p2.d1, (double)p3.d0, (double)p3.d1, (double)p4.d0, (double)p4.d1, &tmp1, &tmp2);
	//ret->d0 = tmp1;
	//ret->d1 = tmp2;
	//return b;
}

//bool SpatialMath::isIntersect ( const Rect2DEx & left, const Rect2DEx & right )
//{
//	return !(left.left_low.d0 > right.right_top.d0+DOUBLE_ZERO || left.left_low.d1 > right.right_top.d1+DOUBLE_ZERO
//		|| left.right_top.d0+DOUBLE_ZERO < right.left_low.d0 || left.right_top.d1+DOUBLE_ZERO < right.left_low.d1 ) ;
//}

//bool SpatialMath::polygonIntersectsPolygon(const vector<Pos2DEx>&lv_pos, const vector<Pos2DEx>& rv_pos)
//{
//	bool isCross = false;
//	Pos2DEx startPos1,endPos1; //被“框”选的面外环的线段起点，终点坐标
//
//
//	assert(lv_pos.size() >= 3 && rv_pos.size() >= 3);
//
//	vector<Pos2DEx>::const_iterator it1 = lv_pos.begin();
//	vector<Pos2DEx>::const_iterator it2 = lv_pos.end();
//
//	for( ; it1 != it2; ++it1 ) // 被选面上的每一个点
//	{
//		startPos1 = (*it1);
//		if((it1+1) != it2)
//		{
//			endPos1 = *(it1+1);
//			isCross = isLineIntersectsPolygon(startPos1,endPos1,rv_pos);
//			if(isCross)
//				return isCross;
//		}
//	}
//	return false;
//}
//
//bool SpatialMath::lineWithinPolygon(const vector<Pos2DEx>&lv_pos, const vector<Pos2DEx>&rv_pos)
//{
//	assert(lv_pos.size() >= 2);
//	vector<Pos2DEx>::const_iterator itr = lv_pos.begin();
//	for( ; itr != lv_pos.end(); ++itr )
//	{
//		if( !isPointInArea(*itr, rv_pos) )
//			return false;
//	}
//
//	return true;
//}
//
//bool SpatialMath::is2LineStringsIntersect(const vector<Pos2DEx>& l_pos, const vector<Pos2DEx>&r_pos)
//{
//	assert(l_pos.size() >= 2 && r_pos.size() >= 2);
//
//	vector<Pos2DEx>::const_iterator iter1 = l_pos.begin();
//	vector<Pos2DEx>::const_iterator iter2 = l_pos.end();
//	vector<Pos2DEx>::const_iterator iter3 = r_pos.begin();
//	vector<Pos2DEx>::const_iterator iter4 = r_pos.end();
//
//	Pos2DEx start_pos1, end_pos1; // for l_pos
//	Pos2DEx start_pos2, end_pos2; // for r_pos
//
//
//	for(; iter1 != iter2; ++iter1)
//	{
//		start_pos1 = *iter1; // 线段的起点
//		if((iter1+1) != iter2)
//		{
//			end_pos1 = *(iter1 + 1); //线段的终点
//			for(iter3 = r_pos.begin(); iter3 != iter4; ++iter3)
//			{
//				start_pos2 = *iter3;
//				if((iter3+1) != iter4)
//				{
//					end_pos2 = *(iter3+1);
//					if(SpatialMath::isSegmentIntersect(start_pos1,end_pos1,start_pos2,end_pos2))
//						return true;
//				}
//			}
//		}
//	}
//
//	return false;
//}

int SpatialMath::getDistancePointToLine( const Pos2DEx & pos, const Pos2DEx & start, const Pos2DEx & end , Pos2DEx * rt_point )
{
	findNearestPointOnLine( pos, start, end, rt_point ) ;

	return getDistance( pos, *rt_point ) ;
}

void SpatialMath::calStdLineEquation( const Pos2DEx & startPos, const Pos2DEx & endPos,
									 int * A, int * B, int * C )
{//已知两个点，计算直线的标准方程：Ax+By+Cz=0
	//两点式方程为：(y-ya)/(yb-ya) = (x-xa)/(xb-xa) =>
	//系数为：A = yb - ya, B = xa - xb,
	//c = ya( xb - xa ) - xa ( yb- ya ) = -ya( xa - xb ) - xa( yb - ya ) = ya*(-B) - xa*A
	*A = endPos.d1 - startPos.d1;
	*B = startPos.d0 - endPos.d0;
	*C = startPos.d1 * ( -(*B) ) - startPos.d0 * (*A);
}

bool SpatialMath::getLineCrossPos( int A1, int B1, int C1,
								  int A2, int B2, int C2, Pos2DEx * CrossPos )
{//Added by qzy!
	const int denominator = A1*B2 - A2*B1;
	if( denominator == 0)
	{//两直线平行
		return false;
	}

	CrossPos->d0 = ( B1*C2 - B2*C1 )/denominator;
	CrossPos->d1 = ( A2*C1 - A1*C2 )/denominator;

	return true;
}

//pos1-pos2上的点ex,当offset>0时求在前进方向右侧的对称点;否则求在左侧的对称点
int SpatialMath::pointOffsetLine(const Pos2DEx &pos1, const Pos2DEx &pos2, const Pos2DEx& ex, int offset, Pos2DEx *result)
{
	//assert(pos1 != pos2);
	//assert(offset !=0);

 	if(pos1 == pos2)//lw
	{
		MGLogD("start = end !\n");
 		return -1;
	}

	if(offset ==0)
		offset = 1;

	int state = relativePointsLine(pos1, pos2, ex);
	if( !(-1 <= state && state <=1) )	//not 3 points on line
		return -1;
	Pos2DEx start(pos1), end(pos2);
	int dis = offset;
	if(start.d0 > end.d0)
	{
		swap(start, end);
		dis *= -1;
	}

	long u = end.d0 - start.d0;
	long v = end.d1 - start.d1;
	double tmp = u;
	tmp *=tmp;
	double tmp_1 = v;
	tmp_1*=tmp_1;
	double w = sqrt( tmp + tmp_1);

	// 利用相似三角形
	long dx = abs(v * dis / w),
		dy = abs(u * dis / w);
	//assert( u > -1.0*DOUBLE_ZERO);	//u >= 0
	dy *= -1 * dis/abs(dis);
	if(! (v == 0))	//分母不为0
		dx *=  (dis/abs(dis)) * (v/abs(v));
	result->d0 = ex.d0+dx, result->d1 = ex.d1+dy;
	return 0;
}
//
//bool SpatialMath::isLineIntersectsPolygon(const Pos2DEx& pos1, const Pos2DEx& pos2, const vector<Pos2DEx>&pos)
//{
//	bool isCross = false;
//	Pos2DEx startPos,endPos;
//
//	vector<Pos2DEx>::const_iterator it1 = pos.begin();
//	vector<Pos2DEx>::const_iterator it2 = pos.end();
//
//	for(; it1 != it2; ++it1)
//	{
//		startPos = (*it1);
//		if((it1+1) != it2)
//		{
//			endPos = (*(it1+1));
//
//			isCross = SpatialMath::isSegmentIntersect(pos1,pos2,startPos,endPos);
//			if(isCross)
//				return isCross;
//		}
//	}
//	return false;
//}
//
bool SpatialMath::isPointInArea(const Pos2DEx& p_pos,const vector<Pos2DEx>&pos)
{
	int crossNum=0; //记录交点个数
	Pos2DEx p_end_pos; //射线的终点   ------>
	Pos2DEx startPos, endPos; //线段的起点和终点

	int_32 status; ////判断点是否在线段上，“-1”——不在，“0”——在中部，“1”——与startPos重合，
	//“2”——与endPos重合，“3”——三点重合

	bool isLeft = false;
	bool isChange = false;
	int times=0;

	vector<Pos2DEx>::const_iterator it1 = pos.begin();
	vector<Pos2DEx>::const_iterator it2 = pos.end();
	for( ; it1 != it2; ++it1 )
	{
		startPos=(*it1);
		if((it1 + 1) != it2)
		{
			endPos=*(it1+1);
			status=SpatialMath::pointOnSection(startPos,endPos,p_pos,GIS_Embedded::frac);
			switch(status)
			{
			case 0:
			case 1:
			case 2:
			case 3:
				return true;
			case -1:

				if(isLeftSide(startPos.d0,startPos.d1,endPos.d0,endPos.d1,p_pos.d0,p_pos.d1))
				{
					crossNum++;
				}
				break;
			}
		}
	}

	return crossNum % 2 ? true : false;


}
//
//bool SpatialMath::isSegmentIntersect(const Pos2DEx &pos00, const Pos2DEx &pos01, const Pos2DEx &pos10, const Pos2DEx &pos11)
//{
//	return PureMath::LineCross(pos00.d0, pos00.d1, pos01.d0, pos01.d1, pos10.d0, pos10.d1, pos11.d0, pos11.d1);
//}
//
int_32 SpatialMath::findNearestPointOnLine( const Pos2DEx & p0, const Pos2DEx & p1, const Pos2DEx & p2 , Pos2DEx * result )
{

	int x0 = p0.d0, y0 = p0.d1;
	int x1 = p1.d0, y1 = p1.d1;
	int x2 = p2.d0, y2 = p2.d1;

	int difx12;
	int dify01, dify12;
	int sdx12, sdy12;

	if(x1==x2)
	{
		result->d0 = x1;

		if(y1 > y2)
			swap(y1, y2);

		if(y0 < y1)
			result->d1 = y1;
		else if(y0 > y2)
			result->d1 = y2;
		else
			result->d1 = y0;
	}
	else
	{
		//	drangon; 2001-05-13. 寻找线段上距离某点最近的点，先找出垂线的点，如果在线内，
		//	则返回此点，否则比较两个端点谁近，返回该点即可。
		difx12 = x2 - x1;
		dify12 = y2 - y1;
		dify01 = y0 - y1;
		sdx12 = difx12 * difx12;
		sdy12 = dify12 * dify12;

		result->d0 = (sdx12 * x0 + sdy12 * x1 + difx12 * dify12 * dify01)
			/ (sdx12 + sdy12);

		//采用分配律防止大数吃小数
		//result->d0 = sdx12 * x0/ (sdx12 + sdy12) + sdy12 * x1/ (sdx12 + sdy12) + difx12 * dify12 * dify01/ (sdx12 + sdy12);

		result->d1 = PureMath::FindYOnLine(x1, y1, x2, y2, result->d0);

		if( (x2 > x1 && (result->d0 > x2 || result->d0 < x1) )  ||
			(x2 < x1 && (result->d0 < x2 || result->d0 > x1) )   )
		{
			int len1, len2;
			len1 = getDistance( p0, p1 );
			len2 = getDistance( p0, p2 );
			if( len1 < len2 )
				*result = p1;
			else
				*result = p2;

		}

	}

	return 0;
}

//
//              (x1,y1)          (x1,y1)
//                  /               \
//              .  /              .  \                <----------------true
//                /                   \
//				(x2,y2)          (x2,y2)
bool SpatialMath::isLeftSide(int x1, int y1, int x2, int y2, int x, int y)
{
	if( ((y > y1 && y < y2) || (y < y1 && y > y2)) && ((y - y1)*(x2 - x1) / (y2 - y1) + (x1 - x) > DOUBLE_ZERO))
		return true;
	return false;
}

//                            +2
//          -1  stdPos ------- 0 ------  endPos  +1
//                            +2
int SpatialMath::relativePointsLine( const Pos2DEx &stdPos , const Pos2DEx &endPos , const Pos2DEx &thdPos, int tolerance /*= frac*/)
{
	int dis0 , dis1 , dis2;

	dis0 = SpatialMath::getDistance( stdPos , endPos );
	dis1 = SpatialMath::getDistance( stdPos , thdPos );
	dis2 = SpatialMath::getDistance( endPos , thdPos );

	//if( /*dis1 + dis2 - dis0 == 0*/abs(dis1 + dis2 - dis0) < tolerance)	//by hqs 2007-5-6
	if( /*dis1 + dis2 - dis0 == 0*/abs(dis1 + dis2 - dis0) <= tolerance)	//by my
	{
		return 0;
	}
	//else if( /*dis1 - dis2 - dis0 == 0*/abs(dis1 - dis2 - dis0) < tolerance)
	else if( /*dis1 - dis2 - dis0 == 0*/abs(dis1 - dis2 - dis0) <= tolerance)	//by my
	{
		return 1;
	}
	//else if( /*dis2 - dis1 - dis0 == 0*/abs(dis2 - dis1 - dis0) < tolerance)
	else if( /*dis2 - dis1 - dis0 == 0*/abs(dis2 - dis1 - dis0) <= tolerance)	//by my
	{
		return -1;
	}
	else
	{
		return 2;
	}
}

bool SpatialMath::isTheSamePosition(const Pos2DEx& l_pos, const Pos2DEx& r_pos)
{
	return l_pos == r_pos;
}

bool SpatialMath::calculateCentroidOfPolygon(const std::vector<Pos2DEx> &control_points, const int pos_num)
{
	return false;
}

bool SpatialMath::calculateCentroidOfPolygon(const vector<ScreenPos2D> &control_points,const int pos_num,ScreenPos2D &centroid_pos)
{
	if (pos_num <3)
		return false;

	vector<int> per_area;
	per_area.resize(0);
	per_area.reserve(pos_num);

	int area = 0;
	int sum_x = 0,sum_y = 0;
	int i;
	for (i=0; i<pos_num-1; i++)
	{
		int tmp_per_area = control_points[i].X * control_points[i+1].Y - control_points[i+1].X * control_points[i].Y;
		per_area.push_back(tmp_per_area);
		area += tmp_per_area;
	}
	if (area <0 )
		area = -area;
	else if (area == 0)
		return false;

	for (i=0; i<pos_num-1; i++)
	{
		int tmp_x = (control_points[i].X + control_points[i+1].X) * per_area[i];
		sum_x += tmp_x;
		int tmp_y = (control_points[i].Y + control_points[i+1].Y) * per_area[i];
		sum_y += tmp_y;
	}
	centroid_pos.X = sum_x /(3 * area);
	centroid_pos.Y = sum_y /(3 * area);

	return true;
}

void SpatialMath::rotationPolygon(vector<ScreenPos2D> &rotation_points,const vector<ScreenPos2D>& control_points,const int pos_num,double angle,const ScreenPos2D &centroid_pos)
{
	rotation_points.resize(0);
	rotation_points.reserve(pos_num);
	double sin_angle = sin(angle);
	double cos_angle = cos(angle);

	for (int i=0; i<pos_num; i++)
	{
		ScreenPos2D tmp_pos;
		tmp_pos.X = (control_points[i].X - centroid_pos.X) * cos_angle - (control_points[i].Y - centroid_pos.Y)*sin_angle + centroid_pos.X;
		tmp_pos.Y = (control_points[i].X - centroid_pos.X) * sin_angle + (control_points[i].Y - centroid_pos.Y)*cos_angle + centroid_pos.Y;
		rotation_points.push_back(tmp_pos);
	}
}

/////////////////////////////////////////////////////////////////////////////////

}// end GIS_Embedded
