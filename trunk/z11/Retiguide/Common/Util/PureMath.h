#ifndef COMMON_PURE_MATH_H_
#define COMMON_PURE_MATH_H_
#include <math.h>
#include "../Types/basic_types.h"

namespace GIS_Embedded
{
class PureMath
{

public:
	static double FindXOnLine(double x1, double y1, double x2, double y2, double x);
	static double FindYOnLine(double x1, double y1, double x2, double y2, double y);
	static bool isPointInRect(double x, double y, double x1, double y1, double x2, double y2);
	static bool isRectInRect(double left_x1, double left_y1, double left_x2, double left_y2,
		double right_x1, double right_y1, double right_x2, double right_y2);
	static double calDistanceSquare(double x1,double y1, double x2, double y2);

	//计算过两个2d点的标准方程
	static void calStdLineEquation( double xa, double ya, double xb, double yb,
		double * A, double * B, double * C );//Added by qzy!

	//点是否在直线上方(不包含3点共线), point:(x, y), line: (xa, ya)->(xb, yb)
	static bool isPointAboveLine(double x, double y,double xa, double ya, double xb, double yb);
	// zgr, 计算点到直线的距离 point:(x, y), line: (xa, ya)->(xb, yb)
	static double getDistancePointToLine(double x, double y, double xa, double ya, double xb, double yb);

	// zgr, 点是否在线的左方，考虑方向(xa,ya)->(xb,yb)
	static bool isPointLeftSideLine(double x, double y, double xa, double ya, double xb, double yb);
	// zgr, 将直线移动到点(x,y)上得到两个新的直线上的点，根据原线垂直方向平移
    static void moveLineToPos(double xa,double ya, double xb, double yb, double x, double y,
		double * new_xa, double *new_ya, double * new_xb, double *new_yb);
	// zgr,計算垂足，點到直线的垂足，(xa,ya)->(xb,yb)表示直线，不表示线段
	static void getFootOfPerpendicular(double xa,double ya,double xb, double yb, 
									  double x, double y,double * fx, double *fy);

	static bool LineOverArea(double lx1, double ly1, double lx2, double ly2,
		double ax1, double ay1, double ax2, double ay2);

	static bool PointInArea(double x, double y,
		double x1, double y1, double x2, double y2);
	// input : [left, right)
	template<class T>
	static bool between(const T& input, const T& left, const T& right)
	{
		return left <= input && input < right;
	}
	static bool LineOverArea_Rough(double lx1, double ly1, double lx2, double ly2,
		double ax1, double ay1, double ax2, double ay2);
	static bool AreaOverlap(double x1, double y1, double x2, double y2,
		double x3, double y3, double x4, double y4);
	//若不相交，则返回false;同时:若平行，x,y被设为最大值；否则交点仍被求出并设置为x,y.by hqs
	static bool lineCrossPoint(double x1, double y1, double x2, double y2, 
		double x3, double y3, double x4, double y4, double *x, double *y);
	static bool LineCross(	double xa1, double ya1, double xa2, double ya2,
							double xb1, double yb1, double xb2, double yb2);
	static bool LeftSide(double x1, double y1, double x2, double y2, double x, double y);



public:
	static int_32 FindXOnLine(int_32 x1, int_32 y1, int_32 x2, int_32 y2, int_32 x);
	static int_32 FindYOnLine(int_32 x1, int_32 y1, int_32 x2, int_32 y2, int_32 y);
	static bool isPointInRect(int_32 x, int_32 y, int_32 x1, int_32 y1, int_32 x2, int_32 y2);
	static bool isRectInRect(int_32 left_x1, int_32 left_y1, int_32 left_x2, int_32 left_y2,
		int_32 right_x1, int_32 right_y1, int_32 right_x2, int_32 right_y2);
	//static int_64 calDistanceSquare(int_32 x1,int_32 y1, int_32 x2, int_32 y2);

	//计算过两个2d点的标准方程
	static void calStdLineEquation( int_32 xa, int_32 ya, int_32 xb, int_32 yb,
		int_32 * A, int_32 * B, int_32 * C );//Added by qzy!

	//点是否在直线上方(不包含3点共线), point:(x, y), line: (xa, ya)->(xb, yb)
	static bool isPointAboveLine(int_32 x, int_32 y,int_32 xa, int_32 ya, int_32 xb, int_32 yb);
	// zgr, 计算点到直线的距离 point:(x, y), line: (xa, ya)->(xb, yb)
	static int_32 getDistancePointToLine(int_32 x, int_32 y, int_32 xa, int_32 ya, int_32 xb, int_32 yb);

	// zgr, 点是否在线的左方，考虑方向(xa,ya)->(xb,yb)
	static bool isPointLeftSideLine(int_32 x, int_32 y, int_32 xa, int_32 ya, int_32 xb, int_32 yb);
	// zgr, 将直线移动到点(x,y)上得到两个新的直线上的点，根据原线垂直方向平移
	static void moveLineToPos(int_32 xa,int_32 ya, int_32 xb, int_32 yb, int_32 x, int_32 y,
		int_32 * new_xa, int_32 *new_ya, int_32 * new_xb, int_32 *new_yb);
	// zgr,計算垂足，點到直线的垂足，(xa,ya)->(xb,yb)表示直线，不表示线段
	static void getFootOfPerpendicular(int_32 xa,int_32 ya,int_32 xb, int_32 yb, 
		int_32 x, int_32 y,int_32 * fx, int_32 *fy);

	static bool LineOverArea(int_32 lx1, int_32 ly1, int_32 lx2, int_32 ly2,
		int_32 ax1, int_32 ay1, int_32 ax2, int_32 ay2);

	static bool PointInArea(int_32 x, int_32 y,
		int_32 x1, int_32 y1, int_32 x2, int_32 y2);

	static bool LineOverArea_Rough(int_32 lx1, int_32 ly1, int_32 lx2, int_32 ly2,
		int_32 ax1, int_32 ay1, int_32 ax2, int_32 ay2);
	static bool AreaOverlap(int_32 x1, int_32 y1, int_32 x2, int_32 y2,
		int_32 x3, int_32 y3, int_32 x4, int_32 y4);
	//若不相交，则返回false;同时:若平行，x,y被设为最大值；否则交点仍被求出并设置为x,y.by hqs
	static bool lineCrossPoint(int_32 x1, int_32 y1, int_32 x2, int_32 y2, 
		int_32 x3, int_32 y3, int_32 x4, int_32 y4, int_32 *x, int_32 *y);
	static bool LineCross(	int_32 xa1, int_32 ya1, int_32 xa2, int_32 ya2,
		int_32 xb1, int_32 yb1, int_32 xb2, int_32 yb2);
	static bool LeftSide(int_32 x1, int_32 y1, int_32 x2, int_32 y2, int_32 x, int_32 y);

	//lw 三角形面积
	static double AreaOfTriangle(int_32 x1,int_32 y1,int_32 x2,int_32 y2,int_32 x3,int_32 y3);

};

inline double PureMath::calDistanceSquare(double x1,double y1, double x2, double y2)
{
	return pow(fabs(x1-x2),2) + pow(fabs(y1-y2),2);
}

//inline int_64 PureMath::calDistanceSquare(int_32 x1,int_32 y1, int_32 x2, int_32 y2)
//{
//	return (x1-x2) * (x1-x2) + (y1-y2) * (y1-y2);
//}
}
#endif