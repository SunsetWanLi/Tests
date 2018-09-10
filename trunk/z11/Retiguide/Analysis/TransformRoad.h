#ifndef IS_GIS_TRANSFORM_RAOD_H_
#define IS_GIS_TRANSFORM_RAOD_H_

#include "../SpatialMath/SpatialMath.h"
#include "../Common/CRS/basic_gm_type.h"
#include "../Common/Share/Scope.h"
#include "../Common/Util/PureMath.h"

namespace GIS_Embedded
{

class TransformRoad
{
public:
	static int calculatePathGeoBorder(vector<Pos2DEx>& control_pos2ds,const Scope& view_scope, double road_width, double filter_width ,vector<Pos2DEx>& border_pos2d,vector<Pos2DEx>& center_pos2d);

public:
	static int filtrationPointDouglas(const vector<Pos2DEx>& points, const double road_width, vector<Pos2DEx> &dest_pos);
	static int getExternalPos(const vector<Pos2DEx> &src_pos, const double road_width, vector<Pos2DEx> &dest_pos);
public:
	static int douglasLineSimplification(vector<pair<Pos2DEx, bool> >& pos_bool_vtr, const int begin, const int end, const double road_width);
	//static double getAreaPointToLine(Pos2DEx& pos, Pos2DEx& start, Pos2DEx& end);
	//static double getDistancePointToLine(const Pos2DEx& pos, const Pos2DEx& start, const Pos2DEx& end);
	static double getDistancePointToLine(const Pos2DEx& pos, const Pos2DEx& start, const Pos2DEx& end);
	static int initPosBoolVtr(vector<pair<Pos2DEx, bool> >& pos_bool_vtr, const vector<Pos2DEx>& points);
	static int getResultFromPosBoolVtr(const int size, const vector<pair<Pos2DEx, bool> >& pos_bool_vtr, vector<Pos2DEx>& dest_pos);
	//static int filtrationPointRing(vector<pair<Pos2DEx, bool> >& pos_bool_vtr, const int begin, const int end, const double road_width, int& save_point_index);
	//static int filtrationPointCurve(vector<pair<Pos2DEx, bool> >& pos_bool_vtr, const int begin, const int end, const double road_width, int& save_point_index);
	static int getTheFarthestPointOnLine(vector<pair<Pos2DEx, bool> >& pos_bool_vtr, const int begin, const int end, const double road_width, int& save_point_index);
public:
	static int getExternalPosTwoPointLine(const vector<Pos2DEx> &src_pos, const double road_width, vector<Pos2DEx> &dest_pos);
	static int getExternalPosMoreTwoPointLine(const vector<Pos2DEx> &src_pos, const double road_width, vector<Pos2DEx> &dest_pos);
	static int offsetAllSegment(const vector<Pos2DEx>& src_pos, vector<Pos2DEx>& side0, vector<Pos2DEx>& side1, const double road_width);
	static int intersectAllSegment(const vector<Pos2DEx>& src_pos, const vector<Pos2DEx> &side0, const vector<Pos2DEx> &side1,
		vector<Pos2DEx> &ctl_pos_0, vector<Pos2DEx> &ctl_pos_1);
	static int intersectAllSegment(const vector<Pos2DEx>& src_pos, const vector<Pos2DEx> &side0, const vector<Pos2DEx> &side1,
		vector<pair<Pos2DEx, int> > &ctl_pos_0, vector<pair<Pos2DEx, int> > &ctl_pos_1);
	static int getDestLineFromSegment(const vector<Pos2DEx>& src_pos, vector<Pos2DEx>& dest_pos, const double road_width,
		const vector<Pos2DEx>& side0, const vector<Pos2DEx>& side1, const vector<Pos2DEx>& ctl_pos_0, const vector<Pos2DEx>& ctl_pos_1);
	static int getDestLineFromSegment(const vector<Pos2DEx>& src_pos, vector<Pos2DEx>& dest_pos, const double road_width,
		const vector<Pos2DEx>& side0, const vector<Pos2DEx>& side1, const vector<pair<Pos2DEx, int> >& ctl_pos_0, const vector<pair<Pos2DEx, int> >& ctl_pos_1);
	static int getDestLineFromSegment(const vector<Pos2DEx>& src_pos, vector<Pos2DEx>& dest_pos, const double road_width,
		const vector<Pos2DEx>& side0, const vector<Pos2DEx>& side1);
	static int getDestLineFromSegment(const vector<Pos2DEx>& src_pos, vector<Pos2DEx>& dest_pos, const double road_width,
		const vector<pair<Pos2DEx, int> >& side0, const vector<pair<Pos2DEx, int> >& side1);
	static int computeCurveExtend(Pos2DEx& pos, const Pos2DEx& start, const Pos2DEx& end, double road_width);
	static bool isLess90Degree(const Pos2DEx& one, const Pos2DEx& cross_point, const Pos2DEx& other);

	//基于三角形的纯几何渐进方法滤点  lw
	static void filtrationPointGeoTriangle(const vector<Pos2DEx>& host_points, const double threshold_area, vector<Pos2DEx> &dest_points);

};
}
#endif //IS_GIS_TRANSFORM_RAOD_H_