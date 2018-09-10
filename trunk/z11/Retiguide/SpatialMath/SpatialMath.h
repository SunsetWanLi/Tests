#ifndef GIS_SPATIAL_MATH_H_
#define GIS_SPATIAL_MATH_H_
#include <cmath>
#include <vector>
#include "../Common/Types/basic_types.h"
#include "../Common/Share/ScreenPos2D.h"
#include "../Common/CRS/basic_gm_type.h"
#include "../Common/Types/Fraction.h"
#include "../../CommonBase/CommonDef.h"
using namespace std;
using namespace z11;

namespace GIS_Embedded
{

const double frac = 0.000001/*0.000001*/;	//数据精度

struct Pos2D;
class SpatialMath
{
public:
	static bool isEqualToDoubleZero(double val)
	{
		return fabs(val) < DOUBLE_ZERO;
	}

	//loujt
	static bool isPos2DInRect(const Pos2D& pos,const Rect2D& rect);
	//loujt

	//my
	static 	long polygon_clip( long src_sum, const Pos2D * src_pts, Pos2D * dest_pts, int x1, int y1, int x2, int y2 );

	static double getDistanceSquare(const Pos2D & start, const Pos2D & end);
	static double getDistance(const Pos2D & start, const Pos2D & end);
	static double getDistance(const vector<Pos2D> & pos_array, double param_start = 0, double param_end = 0);
	// 线段上距离点最近的点
	static Pos2D NearestPointOnSegment(const Pos2D & aa, const Pos2D & pp, const Pos2D & qq );
	// 折线段上距离点最近/满足容忍值的第一个 的点，返回最近距离
	// param: 最近点的比例值，即整数部分表示第几折线段，小数部分表示该折线段上位置比例
	static double NearestPointOnSegmentString(Pos2D * pos, double * param,const Pos2D & aa, const vector<Pos2D> & pos_array,
		double tolerance = DOUBLE_ZERO);
	//
	static Pos2D getPosFromSegmentParam(const Pos2D & start_pos, const Pos2D & end_pos, double param);

	// 点在线段上的比例位置，已假设点在线上了
	static double PointPositionOnSegment(const Pos2D & aa, const Pos2D & pp, const Pos2D & qq );
	//计算两个二维点与某一坐标轴的角度（X or Y）
    static double calLineXAngle(const Pos2D &pos_start, const Pos2D &pos_end);
	static double calLineYAngle(const Pos2D &pos_start, const Pos2D &pos_end);
	//计算两点与正北方向所成角度
	static double calLineNAngle(const Pos2D & pos_start, const Pos2D & pos_end);

	// zgr,2008-01-18,这里的part是new的,请在外面删除,todo: 将控制点信息使用vector<Pos2D> 代替vector<Pos2D*>
	static void getPosesBetweenParam(vector<Pos2D> * part,const vector<Pos2D> & v_ctrl_pos, double _start_param, double _end_param);

	// zgr, 2008-01-18, 两条边pa-〉pb， pA->pB, 的夹角， 不考虑方向，ret(0~90)
	static double calIncludedAngle(const Pos2D & pa, const Pos2D & pb, const Pos2D & pA, const Pos2D & pB);
	// zgr, 2008-01-18, 两条边pa-〉pm->pB, 的夹角， 不考虑方向，ret(0~90)
	static double calIncludedAngle(const Pos2D & pa, const Pos2D & pm, const Pos2D & pb);
	// zgr, 2008-01-17, 计算曲线段上某个点的角度,以正北为轴,
	// 对于刚好是控制点的，取两端正北角度的平均值，起末点取单个方向的
	static double calCurvePointNAngle(const vector<Pos2D> & v_ctrl_pos, double param);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//2008-03-20,王咚春，路口放大图所需接口
	//计算圆心角，x轴正向向量同start->end向量的夹角，逆时针走向，范围：[0,2*PI).若2点同返回0
	static bool isAnticlockwise(const Pos2D & p0, const Pos2D & p1, const Pos2D & p2);
	static bool isClockwise(const Pos2D & p0, const Pos2D & p1, const Pos2D & p2);

	static double getCentralAngleAgainstXAxis(const Pos2D& start, const Pos2D& end, double tolerance = frac*10.0);
	//求由圆心+半径+圆心角所确定的点
	static int_32 calculateCirclePoint(const Pos2D& center, double radius, double angle, Pos2D* result);
	//计算圆心角，P1-Origin-P2的夹角，逆时针走向，范围：[0,2*PI)若2点同返回0
	static double getCentralAngle(const Pos2D& origin, const Pos2D& p1, const Pos2D& p2, double tolerance = frac*10.0);

	static double TwoPointDistanceSquare(const Pos2D & pa, const Pos2D & pb);
	static double TwoPointDistance( const Pos2D & pa, const Pos2D & pb );

	static bool isLineCross(const Pos2D &s1, const Pos2D &e1, const Pos2D &s2, const Pos2D &e2, Pos2D* CrossPos);//added by wdc
	//判断点是否在线段上，“-1”——不在，“0”——在中部，“1”——与startPos重合，
	//“2”——与endPos重合，“3”——三点重合 //Added by qzy!
	static int_32 pointOnSection( const Pos2D &startPos, const Pos2D &endPos, const Pos2D &thdPos, double tolerance );
	//求线和圆的交点//by wdc //相交和相切返回不同值
	static int_32 getIntersectionOfLineCirclePlus(const Pos2D& start, const Pos2D& end, const Pos2D& origin, double radius, vector<Pos2D>* result);
	//求半径相等两圆的交点//by wdc
	static int_32 getIntersectionOfCircleCircle(const Pos2D& o1, const Pos2D& o2, double r, vector<Pos2D>* result);
	//求半径不等两圆的交点//by wdc
	static int_32 getIntersectionOfCircleCirclePlus(const Pos2D& o1, const Pos2D& o2, double r1, double r2, vector<Pos2D>* result);
	//求某点在线段上的垂足。假如垂足在线段外，垂足仍然被设置，但是return -1
	static int_32 getFootOfPerpendicular(const Pos2D& p1, const Pos2D& p2, const Pos2D& ex, Pos2D* result);

	// 垂直方向平移一个线段（等距平行线）,offset >0 右侧
	static int offsetSegment(const Pos2D &pos0, const Pos2D &pos1, double offset, Pos2D *out_pos0, Pos2D *out_pos1);
	//2直线是否平行
	static bool isParallel(const Pos2D& start1, const Pos2D& end1, const Pos2D& start2, const Pos2D& end2);
	//若不相交，则返回false;同时:若平行，pos值被设为最大值；否则交点仍被求出并设置ret.by hqs
	static bool lineCrossPoint(const Pos2D& p1, const Pos2D& p2, const Pos2D& p3, const Pos2D& p4, Pos2D* ret);
	static bool isIntersect ( const Rect2D & left, const Rect2D & right ) ;
	//面和面相交，但不是包含关系
	static bool polygonIntersectsPolygon(const vector<Pos2D>&lv_pos, const vector<Pos2D>&rv_pos);
	//线被面包含
	static bool lineWithinPolygon(const vector<Pos2D>&lv_pos, const vector<Pos2D>&rv_pos);
	//两条LineString是否相交
	static bool is2LineStringsIntersect(const vector<Pos2D>& l_pos, const vector<Pos2D>&r_pos);
	// 获取点到线的最短距离,输入，点，线的起点，终点，如果最近距离是垂直线段返回垂足
	// 如果最近距离是头节点或者尾节点，则返回头或者尾坐标
	static double getDistancePointToLine( const Pos2D & pos, const Pos2D & start, 
										const Pos2D & end , Pos2D * rt_point ) ;
	//计算过两个2d点的标准方程，如果两点重合会有除零错误！
	static void calStdLineEquation( const Pos2D & startPos, const Pos2D & endPos,
		double * A, double * B, double * C );//Added by qzy!
	//计算两条2D直线的交点，直线方程由标准式给出
	static bool getLineCrossPos( double A1, double B1, double C1,
		double A2, double B2, double C2, Pos2D * CrossPos );//Added by qzy!
	//pos1-pos2上的点ex,当offset>0时求在线前进右侧的对称点;否则求在左侧的对称点
	static int pointOffsetLine(const Pos2D &pos1, const Pos2D &pos2, const Pos2D& ex, double offset, Pos2D *result);
	//判断线与面是否有交点, 注意区别于across的情况
	static bool isLineIntersectsPolygon(const Pos2D& pos1, const Pos2D& pos2, const vector<Pos2D>&pos);
	//求线段与面的交点  lw
	static void isLineIntersectsPolygon(const Pos2DEx& pos1, const Pos2DEx& pos2, const vector<Pos2DEx>&pos,vector<Pos2DEx> &ret_pos);
	//判断点对象是否在框(面)中，p_pos为点坐标，pos为框的坐标集
	static bool isPointInArea(const Pos2D& p_pos,const vector<Pos2D>&pos);
	// 两线段是否相交
	static bool isSegmentIntersect(const Pos2D &pos00, const Pos2D &pos01, const Pos2D &pos10, const Pos2D &pos11);
	static int_32 findNearestPointOnLine( const Pos2D & pos, const Pos2D & start, const Pos2D & end , Pos2D * nearest_point ) ;
	//判断点是否在线的左边，不考虑方向
	static bool isLeftSide(double x1, double y1, double x2, double y2, double x, double y);
	//判断三个2D点是否成线,第三个点是否在线上?延长线上?线外?
	//                            +2
	//          -1  stdPos ------- 0 ------  endPos  +1
	//                            +2
	static int relativePointsLine( const Pos2D &stdPos , const Pos2D &endPos , const Pos2D &thdPos, double tolerance = frac);
	static bool isTheSamePosition(const Pos2D& l_pos, const Pos2D& r_pos);
	// 垂直方向平移一个线段（等距平行线）,offset >0 右侧
	//static int offsetSegment(const Pos2D &pos0, const Pos2D &pos1, double offset, Pos2D *out_pos0, Pos2D *out_pos1);
	//2直线是否平行
	//static bool isParallel(const Pos2D& start1, const Pos2D& end1, const Pos2D& start2, const Pos2D& end2);
	//若不相交，则返回false;同时:若平行，pos值被设为最大值；否则交点仍被求出并设置ret.by hqs
	//static bool lineCrossPoint(const Pos2D& p1, const Pos2D& p2, const Pos2D& p3, const Pos2D& p4, Pos2D* ret);
	//static bool isIntersect ( const Rect2D & left, const Rect2D & right ) ;
	//面和面相交，但不是包含关系
	//static bool polygonIntersectsPolygon(const vector<Pos2D>&lv_pos, const vector<Pos2D>&rv_pos);
	//线被面包含
	//static bool lineWithinPolygon(const vector<Pos2D>&lv_pos, const vector<Pos2D>&rv_pos);
	//两条LineString是否相交
	//static bool is2LineStringsIntersect(const vector<Pos2D>& l_pos, const vector<Pos2D>&r_pos);
	// 获取点到线的最短距离,输入，点，线的起点，终点，如果最近距离是垂直线段返回垂足
	// 如果最近距离是头节点或者尾节点，则返回头或者尾坐标
	//static double getDistancePointToLine( const Pos2D & pos, const Pos2D & start, 
										//const Pos2D & end , Pos2D * rt_point ) ;
	//计算过两个2d点的标准方程，如果两点重合会有除零错误！
	//static void calStdLineEquation( const Pos2D & startPos, const Pos2D & endPos,
		//double * A, double * B, double * C );//Added by qzy!
	//计算两条2D直线的交点，直线方程由标准式给出
	//static bool getLineCrossPos( double A1, double B1, double C1,
	//	double A2, double B2, double C2, Pos2D * CrossPos );//Added by qzy!

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////tmp 除了OK的 其他未做检查 不能使用
public:
	//loujt
	static bool isPos2DInRect(const Pos2DEx& pos,const Rect2DEx& rect);
	//loujt

	//todo
	static 	long polygon_clip( long src_sum, const Pos2DEx * src_pts, Pos2DEx * dest_pts, int x1, int y1, int x2, int y2 );

	//todo
	static z11::int_64 getDistanceSquare(const Pos2DEx & start, const Pos2DEx & end);
	//todo
	static int getDistance(const Pos2DEx & start, const Pos2DEx & end);
	//todo
	static int getDistance(const vector<Pos2DEx> & pos_array);
	//todo
	static int getDistance(const vector<Pos2DEx> & pos_array, const Fraction &param_start, const Fraction &param_end);
	// 线段上距离点最近的点
	static Pos2DEx NearestPointOnSegment(const Pos2DEx & aa, const Pos2DEx & pp, const Pos2DEx & qq );
	// 折线段上距离点最近/满足容忍值的第一个 的点，返回最近距离
	// param: 最近点的比例值，即整数部分表示第几折线段，小数部分表示该折线段上位置比例
	static int NearestPointOnSegmentString(Pos2DEx * pos, Fraction * param,const Pos2DEx & aa, const vector<Pos2DEx> & pos_array,
		int tolerance = 1/*0*/);
	static int NearestPointOnSegmentString(Pos2DEx * pos, Fraction * param,const Pos2DEx & aa, const vector<vector<Pos2DEx> > & pos_array,
		int tolerance = 1/*0*/);
	//
	static Pos2DEx getPosFromSegmentParam(const Pos2DEx & start_pos, const Pos2DEx & end_pos, const Fraction &param);

	// 点在线段上的比例位置，已假设点在线上了
	static Fraction PointPositionOnSegment(const Pos2DEx & aa, const Pos2DEx & pp, const Pos2DEx & qq );
	//计算两个二维点与某一坐标轴的角度（X or Y）
	static int calLineXAngle(const Pos2DEx &pos_start, const Pos2DEx &pos_end);
	static double calLineYAngle(const Pos2DEx &pos_start, const Pos2DEx &pos_end);
	//计算两点与正北方向所成角度
	static double calLineNAngle(const Pos2DEx & pos_start, const Pos2DEx & pos_end);

	// zgr,2008-01-18,这里的part是new的,请在外面删除,todo: 将控制点信息使用vector<Pos2DEx> 代替vector<Pos2DEx*>
	static void getPosesBetweenParam(vector<Pos2DEx> * part,const vector<Pos2DEx> & v_ctrl_pos, const Fraction &_start_param, const Fraction &_end_param);

	// zgr, 2008-01-18, 两条边pa-〉pb， pA->pB, 的夹角， 不考虑方向，ret(0~90)
	static int calIncludedAngle(const Pos2DEx & pa, const Pos2DEx & pb, const Pos2DEx & pA, const Pos2DEx & pB);
	// zgr, 2008-01-18, 两条边pa-〉pm->pB, 的夹角， 不考虑方向，ret(0~90)
	static int calIncludedAngle(const Pos2DEx & pa, const Pos2DEx & pm, const Pos2DEx & pb);
	// zgr, 2008-01-17, 计算曲线段上某个点的角度,以正北为轴,
	// 对于刚好是控制点的，取两端正北角度的平均值，起末点取单个方向的
	static double calCurvePointNAngle(const vector<Pos2DEx> & v_ctrl_pos, const Fraction &param);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//2008-03-20,王咚春，路口放大图所需接口
	//计算圆心角，x轴正向向量同start->end向量的夹角，逆时针走向，范围：[0,2*PI).若2点同返回0
	static bool isAnticlockwise(const Pos2DEx & p0, const Pos2DEx & p1, const Pos2DEx & p2);
	static bool isClockwise(const Pos2DEx & p0, const Pos2DEx & p1, const Pos2DEx & p2);

	static int getCentralAngleAgainstXAxis(const Pos2DEx& start, const Pos2DEx& end, int tolerance = 1/*frac*10.0*/);
	//求由圆心+半径+圆心角所确定的点
	static int_32 calculateCirclePoint(const Pos2DEx& center, int radius, int angle, Pos2DEx* result);
	//计算圆心角，P1-Origin-P2的夹角，逆时针走向，范围：[0,2*PI)若2点同返回0
	static int getCentralAngle(const Pos2DEx& origin, const Pos2DEx& p1, const Pos2DEx& p2, int tolerance = 1/*frac*10.0*/);

	static int TwoPointDistanceSquare(const Pos2DEx & pa, const Pos2DEx & pb);
	static int TwoPointDistance( const Pos2DEx & pa, const Pos2DEx & pb );

	//todo
	static bool isLineCross(const Pos2DEx &s1, const Pos2DEx &e1, const Pos2DEx &s2, const Pos2DEx &e2, Pos2DEx* CrossPos);//added by wdc
	//判断点是否在线段上，“-1”——不在，“0”——在中部，“1”——与startPos重合，
	//“2”——与endPos重合，“3”——三点重合 //Added by qzy!
	static int_32 pointOnSection( const Pos2DEx &startPos, const Pos2DEx &endPos, const Pos2DEx &thdPos, double tolerance );
	//求线和圆的交点//by wdc //相交和相切返回不同值
	static int_32 getIntersectionOfLineCirclePlus(const Pos2DEx& start, const Pos2DEx& end, const Pos2DEx& origin, int radius, vector<Pos2DEx>* result);
	//求半径相等两圆的交点//by wdc
	static int_32 getIntersectionOfCircleCircle(const Pos2DEx& o1, const Pos2DEx& o2, int r, vector<Pos2DEx>* result);
	//求半径不等两圆的交点//by wdc
	static int_32 getIntersectionOfCircleCirclePlus(const Pos2DEx& o1, const Pos2DEx& o2, int r1, int r2, vector<Pos2DEx>* result);
	//求某点在线段上的垂足。假如垂足在线段外，垂足仍然被设置，但是return -1
	static int_32 getFootOfPerpendicular(const Pos2DEx& p1, const Pos2DEx& p2, const Pos2DEx& ex, Pos2DEx* result);

	// 垂直方向平移一个线段（等距平行线）,offset >0 右侧
	//todo
	static int offsetSegment(const Pos2DEx &pos0, const Pos2DEx &pos1, int offset, Pos2DEx *out_pos0, Pos2DEx *out_pos1);
	//2直线是否平行
	//todo
	static bool isParallel(const Pos2DEx& start1, const Pos2DEx& end1, const Pos2DEx& start2, const Pos2DEx& end2);
	//若不相交，则返回false;同时:若平行，pos值被设为最大值；否则交点仍被求出并设置ret.by hqs
	//todo
	static bool lineCrossPoint(const Pos2DEx& p1, const Pos2DEx& p2, const Pos2DEx& p3, const Pos2DEx& p4, Pos2DEx* ret);
	//static bool isIntersect ( const Rect2DEx & left, const Rect2DEx & right ) ;
	//面和面相交，但不是包含关系
	static bool polygonIntersectsPolygon(const vector<Pos2DEx>&lv_pos, const vector<Pos2DEx>&rv_pos);
	//线被面包含
	static bool lineWithinPolygon(const vector<Pos2DEx>&lv_pos, const vector<Pos2DEx>&rv_pos);
	//两条LineString是否相交
	static bool is2LineStringsIntersect(const vector<Pos2DEx>& l_pos, const vector<Pos2DEx>&r_pos);
	// 获取点到线的最短距离,输入，点，线的起点，终点，如果最近距离是垂直线段返回垂足
	// 如果最近距离是头节点或者尾节点，则返回头或者尾坐标
	static int getDistancePointToLine( const Pos2DEx & pos, const Pos2DEx & start, 
		const Pos2DEx & end , Pos2DEx * rt_point ) ;
	//计算过两个2d点的标准方程，如果两点重合会有除零错误！
	static void calStdLineEquation( const Pos2DEx & startPos, const Pos2DEx & endPos,
		int * A, int * B, int * C );//Added by qzy!
	//计算两条2D直线的交点，直线方程由标准式给出
	static bool getLineCrossPos( int A1, int B1, int C1,
		int A2, int B2, int C2, Pos2DEx * CrossPos );//Added by qzy!
	//pos1-pos2上的点ex,当offset>0时求在线前进右侧的对称点;否则求在左侧的对称点
	static int pointOffsetLine(const Pos2DEx &pos1, const Pos2DEx &pos2, const Pos2DEx& ex, int offset, Pos2DEx *result);
	//判断线与面是否有交点, 注意区别于across的情况
	static bool isLineIntersectsPolygon(const Pos2DEx& pos1, const Pos2DEx& pos2, const vector<Pos2DEx>&pos);
	//static void isLineIntersectsPolygon(const Pos2DEx& pos1, const Pos2DEx& pos2, const vector<Pos2DEx>&pos, Pos2DEx& start_pos, Pos2DEx& end_pos);
	static void isSrcLineIntersectsPolygon(const ScreenPos2D& pos1, const ScreenPos2D& pos2, const vector<ScreenPos2D>&pos, ScreenPos2D& start_pos, ScreenPos2D& end_pos);

	//判断点对象是否在框(面)中，p_pos为点坐标，pos为框的坐标集
	static bool isPointInArea(const Pos2DEx& p_pos,const vector<Pos2DEx>&pos);
	// 两线段是否相交
	static bool isSegmentIntersect(const Pos2DEx &pos00, const Pos2DEx &pos01, const Pos2DEx &pos10, const Pos2DEx &pos11);
	static int_32 findNearestPointOnLine( const Pos2DEx & pos, const Pos2DEx & start, const Pos2DEx & end , Pos2DEx * nearest_point ) ;
	//判断点是否在线的左边，不考虑方向
	static bool isLeftSide(int x1, int y1, int x2, int y2, int x, int y);
	//判断三个2D点是否成线,第三个点是否在线上?延长线上?线外?
	//                            +2
	//          -1  stdPos ------- 0 ------  endPos  +1
	//                            +2
	static int relativePointsLine( const Pos2DEx &stdPos , const Pos2DEx &endPos , const Pos2DEx &thdPos, int tolerance = 1/*frac*/);
	static bool isTheSamePosition(const Pos2DEx& l_pos, const Pos2DEx& r_pos);

	//计算多边形形心   lw
	static bool calculateCentroidOfPolygon(const vector<Pos2DEx> &control_points,const int pos_num);
	static bool calculateCentroidOfPolygon(const vector<ScreenPos2D> &control_points,const int pos_num,ScreenPos2D &centroid_pos);
	//多边形逆时针旋转  lw
	static void rotationPolygon(vector<ScreenPos2D> &rotation_points,const vector<ScreenPos2D>& control_points,const int pos_num,double angle,const ScreenPos2D &centroid_pos);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


};
inline double SpatialMath::getDistanceSquare(const Pos2D & start, const Pos2D & end)
{
	double dis = pow((end.d0 - start.d0) , 2) + pow((end.d1 - start.d1) , 2);
	return dis;
}
inline double SpatialMath::getDistance(const Pos2D & start, const Pos2D & end)
{
	double dis;
	dis = sqrt(pow((end.d0 - start.d0) , 2) + pow((end.d1 - start.d1) , 2));
	return dis;
}

inline z11::int_64 SpatialMath::getDistanceSquare(const Pos2DEx & start, const Pos2DEx & end)
{
	z11::int_64 dis = z11::int_64(end.d0 - start.d0) * (end.d0 - start.d0) + z11::int_64(end.d1 - start.d1) * (end.d1 - start.d1);
	return dis;
}
inline int SpatialMath::getDistance(const Pos2DEx & start, const Pos2DEx & end)
{
	int dis;
	dis = (int)sqrt((double)(getDistanceSquare(start, end)));
	return dis;
}

}// end GIS_Embedded
#endif