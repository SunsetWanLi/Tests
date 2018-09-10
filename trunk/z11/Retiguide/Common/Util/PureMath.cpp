
#include <cstdlib>
using namespace std;

#include "PureMath.h"
#include "../Types/Fraction.h"
#ifndef _I32_MAX
#define _I32_MIN    (-2147483647 - 1) /* minimum signed 32 bit value */
#define _I32_MAX    2147483647 /* maximum signed 32 bit value */
#define _UI32_MAX   0xffffffffu /* maximum unsigned 32 bit value */
#endif

#ifndef DBL_MAX
#define DBL_MAX 1.7976931348623158e+308
#endif

#ifndef max
#define max(a, b) ((a < b) ? b : a)
#endif

#ifndef min
#define min(a, b) ((b < a) ? b : a)
#endif

namespace GIS_Embedded
{
static const double PM_DOUBLE_ZERO = 1.7e-10;
	/*
	* (x1, y1) & (x2, y2) define the line, now we have a point on line with coordinate y, what is the x?
	*/
double PureMath::FindXOnLine(double x1, double y1, double x2, double y2, double y)
{
	if(y1 == y2) 
		return min(x1,x2);
	return (y - y2) * (x1 - x2) / (y1 - y2) + x2;
}
/*
	* (x1, y1) & (x2, y2) define the line, now we have a point on line with coordinate x, what is the y?
	*/
double PureMath::FindYOnLine(double x1, double y1, double x2, double y2, double x)
{
	if(x1 == x2) 
		return min(y1,y2);
	return (x - x2) * (y1 - y2) / (x1 - x2) + y2;
}

bool PureMath::isPointInRect(double x, double y, double x1, double y1, double x2, double y2)
{
	double l, r, t, b;

	if(x1 < x2)
		l = x1, r = x2;
	else
		l = x2, r = x1;

	if(y1 < y2)
		b = y1, t = y2;
	else
		b = y2, t = y1;

	return l < x && r > x && b < y && t > y;
}

void PureMath::calStdLineEquation(double xa, double ya, double xb, double yb,
									 double * A, double * B, double * C )
{//已知两个点，计算直线的标准方程：Ax+By+Cz=0
	//两点式方程为：(y-ya)/(yb-ya) = (x-xa)/(xb-xa) =>
	//系数为：A = yb - ya, B = xa - xb, 
	//c = ya( xb - xa ) - xa ( yb- ya ) = -ya( xa - xb ) - xa( yb - ya ) = ya*(-B) - xa*A
	*A = yb - ya;
	*B = xa - xb;
	*C = ya * ( -(*B) ) - xa * (*A);
}

//点是否在直线上方(不包含3点共线)
bool PureMath::isPointAboveLine(double x, double y, double xa, double ya, double xb, double yb)
{
	if(xa == xb || ya == yb)
	{
		//if( abs(start.d1 - end.d1) < DOUBLE_ZERO)	//no matter horizontal or vertical
		return y > max(ya, yb);
	}

	double A = 0, B = 0, C = 0;
	calStdLineEquation(xa, ya, xb, yb, &A, &B, &C);

	bool flag1 = A*B < 0,
		flag2 = A*x+B*y+C<0;
	if( (flag1&&flag2) || (!flag1 && !flag2) )
		return true;
	return false;
}

bool PureMath::isPointLeftSideLine(double x, double y, double xa, double ya, double xb, double yb)
{
	if(xa == xb && ya == yb)
		return x < xa || (x == xa && y > ya);

	if(xa == xb || xa == x) // (xa,ya)->(xb,yb) 垂直x轴 \\ (xa,ya)->(x,y) 垂直x轴
		return x < xb;
	
	return (y - ya)/(x - xa) > (yb-ya)/(xb-xa); // 通过比较斜率来比较左右
}

void PureMath::moveLineToPos(double xa,double ya, double xb, double yb, double x, double y,
							 double * new_xa, double *new_ya, double * new_xb, double *new_yb)
{
	double foot_x = x, foot_y = y;
	getFootOfPerpendicular(xa, ya, xb, yb, x,y, &foot_x, &foot_y);

	double delta_x = x - foot_x, delta_y = y - foot_y;

	*new_xa = xa + delta_x;
	*new_ya = ya + delta_y;
	*new_xb = xb + delta_x;
	*new_yb = yb + delta_y;
}

void PureMath::getFootOfPerpendicular(double xa,double ya,double xb, double yb, 
									  double x, double y,double * fx, double *fy)
{
	if(xa == xb && ya == yb)
	{
		*fx = xa;
		*fy = ya;
		return ;
	}

	double A = 0, B = 0, C = 0;
	calStdLineEquation(xa, ya, xb, yb, &A, &B, &C);

	*fx = ( A * A * x + B * B * xa + A * B * ( y - ya ) )
				/ ( A * A + B * B );
	if(xa == xb)
	{
		*fy = y;
	}
	else
	{
		*fy = FindYOnLine( xa, ya, xb, yb, *fx );
	}
}

/*
	*	to fine whether the line has at lease some part in the area(rectangle)
	*/
/* 求交算法描述；分别用四位表示：左右上下 
*  1010     0010     0110                                                         
*        ----------                                                             
*  1000  |  0000  |  0100                                                      
*        ----------                                                            
*  1001     0001     0101                                                       
*  直线的两个点各有一个4位的数，求交非1，肯定不相交，                                                                              
*  求与无1，肯定相交，有1，即判断和1位处表示的线求交。
*/
// 2001-03-03; 改成严格匹配。要求rect是有序的。而且不是退化的，
bool PureMath::LineOverArea(double lx1, double ly1, double lx2, double ly2,
							double ax1, double ay1, double ax2, double ay2)
{

//		return LineOverArea_Rough(lx1, ly1,  lx2, ly2,
//							   ax1,  ay1,  ax2,  ay2);
	unsigned char typeS, typeE;
	typeS = typeE = 0;
	if( lx1 < ax1 )      // 左：1000
		typeS |= 0x8; 
	else if( lx1 > ax2 ) // 右：0100
		typeS |= 0x4; 
	if( ly1 < ay1 )      // 下：0001
		typeS |= 0x1; 
	else if( ly1 > ay2 ) // 上：0010
		typeS |= 0x2; 
		
	if( lx2 < ax1 )      // 左：1000
		typeE |= 0x8; 
	else if( lx2 > ax2 ) // 右：0100
		typeE |= 0x4; 
	if( ly2 < ay1 )      // 下：0001
		typeE |= 0x1; 
	else if( ly2 > ay2 ) // 上：0010
		typeE |= 0x2; 

	unsigned char cmp;
	cmp = typeS & typeE;
	if( cmp != 0 ) // 求与非0，肯定在外面，不相交。
		return false;
	cmp = typeS | typeE;
	if( cmp == 0 ) // 求交全0，肯定在里面，必相交。
		return true;
	
	// 比较四条边
	if( cmp & 0x8 ) // 1000，左边
	{
		double cross_y;
		cross_y = FindYOnLine( lx1, ly1, lx2, ly2, ax1 );
		if( cross_y >= ay1 && cross_y <= ay2 )
			return true;
	}
	if( cmp & 0x4 ) // 0100，右边
	{
		double cross_y;
		cross_y = FindYOnLine( lx1, ly1, lx2, ly2, ax2 );
		if( cross_y >= ay1 && cross_y <= ay2 )
			return true;
	}
	if( cmp & 0x1 ) // 0001，下边
	{
		double cross_x;
		cross_x = FindXOnLine( lx1, ly1, lx2, ly2, ay1 );
		if( cross_x >= ax1 && cross_x <= ax2 )
			return true;
	}
	if( cmp & 0x2 ) // 0010，上边
	{
		double cross_x;
		cross_x = FindXOnLine( lx1, ly1, lx2, ly2, ay2 );
		if( cross_x >= ax1 && cross_x <= ax2 )
			return true;
	}

	return false;
}

//  is the
bool PureMath::PointInArea(double x, double y, double x1, double y1, double x2, double y2)
{
	double l, r, t, b;

	if(x1 < x2)
		l = x1, r = x2;
	else
		l = x2, r = x1;

	if(y1 < y2)
		b = y1, t = y2;
	else
		b = y2, t = y1;

	return l < x && r > x && b < y && t > y;
}

double PureMath::getDistancePointToLine(double x, double y, double xa, double ya, double xb, double yb)
{
	if(xa == xb && ya == yb) // 若直線兩個點相等
		return sqrt(pow(x - xa, 2) + pow(y - ya, 2));

	double A = 0, B = 0, C = 0;
	calStdLineEquation(xa, ya, xb, yb, &A, &B, &C);
	double ret = fabs(A * x + B * y + C) / (sqrt(A*A + B*B));
	return ret;
}

//// 2001-03-03; 改成严格匹配。要求rect是有序的。而且不是退化的，
//bool PureMath::LineOverArea(double lx1, double ly1, double lx2, double ly2,
//							double ax1, double ay1, double ax2, double ay2)
//{
//
////		return LineOverArea_Rough(lx1, ly1,  lx2, ly2,
////							   ax1,  ay1,  ax2,  ay2);
//	unsigned char typeS, typeE;
//	typeS = typeE = 0;
//	if( lx1 < ax1 )      // 左：1000
//		typeS |= 0x8; 
//	else if( lx1 > ax2 ) // 右：0100
//		typeS |= 0x4; 
//	if( ly1 < ay1 )      // 下：0001
//		typeS |= 0x1; 
//	else if( ly1 > ay2 ) // 上：0010
//		typeS |= 0x2; 
//		
//	if( lx2 < ax1 )      // 左：1000
//		typeE |= 0x8; 
//	else if( lx2 > ax2 ) // 右：0100
//		typeE |= 0x4; 
//	if( ly2 < ay1 )      // 下：0001
//		typeE |= 0x1; 
//	else if( ly2 > ay2 ) // 上：0010
//		typeE |= 0x2; 
//
//	unsigned char cmp;
//	cmp = typeS & typeE;
//	if( cmp != 0 ) // 求与非0，肯定在外面，不相交。
//		return false;
//	cmp = typeS | typeE;
//	if( cmp == 0 ) // 求交全0，肯定在里面，必相交。
//		return true;
//	
//	// 比较四条边
//	if( cmp & 0x8 ) // 1000，左边
//	{
//		double cross_y;
//		cross_y = FindYOnLine( lx1, ly1, lx2, ly2, ax1 );
//		if( cross_y >= ay1 && cross_y <= ay2 )
//			return true;
//	}
//	if( cmp & 0x4 ) // 0100，右边
//	{
//		double cross_y;
//		cross_y = FindYOnLine( lx1, ly1, lx2, ly2, ax2 );
//		if( cross_y >= ay1 && cross_y <= ay2 )
//			return true;
//	}
//	if( cmp & 0x1 ) // 0001，下边
//	{
//		double cross_x;
//		cross_x = FindXOnLine( lx1, ly1, lx2, ly2, ay1 );
//		if( cross_x >= ax1 && cross_x <= ax2 )
//			return true;
//	}
//	if( cmp & 0x2 ) // 0010，上边
//	{
//		double cross_x;
//		cross_x = FindXOnLine( lx1, ly1, lx2, ly2, ay2 );
//		if( cross_x >= ax1 && cross_x <= ax2 )
//			return true;
//	}
//
//	return false;
//}

bool PureMath::AreaOverlap(double x1, double y1, double x2, double y2,
							double x3, double y3, double x4, double y4)
{
	double l1, r1, t1, b1,
		l2, r2, t2, b2;

	// 1st do some sort...
	if(x1 <= x2)
		l1 = x1, r1 = x2;
	else
		l1 = x2, r1 = x1;

	if(y1 <= y2)
		b1 = y1, t1 = y2;
	else
		b1 = y2, t1 = y1;

	if(x3 <= x4)
		l2 = x3, r2 = x4;
	else
		l2 = x4, r2 = x3;

	if(y3 <= y4)
		b2 = y3, t2 = y4;
	else
		b2 = y4, t2 = y3;

	// compare the value
	return l1 <= r2 && r1 >= l2 && b1 <= t2 && t1 >= b2;
}

//若不相交，则返回false;同时:若平行，x,y被设为最大值；否则交点仍被求出并设置为x,y.by hqs
bool PureMath::lineCrossPoint(double x1, double y1, double x2, double y2, 
		double x3, double y3, double x4, double y4, double *x, double *y)
{
	//2个顶点重合， by hqs 2007-2-4
	if(fabs(x1 - x3) < PM_DOUBLE_ZERO && fabs(y1 - y3) < PM_DOUBLE_ZERO )
	{
		*x = x1;
		*y = y1;
		return true;
	}
	if(fabs(x1 - x4) < PM_DOUBLE_ZERO && fabs(y1 - y4) < PM_DOUBLE_ZERO ) 
	{
		*x = x1;
		*y = y1;
		return true;
	}
	if(fabs(x2 - x3) < PM_DOUBLE_ZERO && fabs(y2 - y3) < PM_DOUBLE_ZERO )
	{
		*x = x2;
		*y = y2;
		return true;
	}
	if(fabs(x2 - x4) < PM_DOUBLE_ZERO && fabs(y2 - y4) < PM_DOUBLE_ZERO ) 
	{
		*x = x2;
		*y = y2;
		return true;
	}

	//是否平行？ 分母 < 0
	if( fabs((y4 - y3)*(x2 - x1) - (y2 - y1)*(x4 - x3)) < PM_DOUBLE_ZERO /*|| sqrt((x3-x2)*(x3-x2) + (y3-y2)*(y3-y2)) < 3*/)	//comment by hqs 2007-2-2
	{
		/**x = (x2 + x3)/2, *y = (y2 + y3)/2;*/
		*x = DBL_MAX, *y = DBL_MAX;
		return false;
	}
	*x = ((x2*y1 - x1*y2)*(x4 - x3) - (x4*y3 - x3*y4)*(x2 - x1))
		/
		((y4 - y3)*(x2 - x1) - (y2 - y1)*(x4 - x3));

	*y = ((y2*x1 - y1*x2)*(y4 - y3) - (y4*x3 - y3*x4)*(y2 - y1))
		/
		((x4 - x3)*(y2 - y1) - (x2 - x1)*(y4 - y3));
	if( ! LineCross(x1, y1, x2, y2, x3, y3, x4, y4) )	//不相交，但仍然求出交点
		return false;
	//patched by hqs 2007-2-2
	double min_x = /*x2 < x3 ? x2 : x3;*/x1 < x2 ? x1 : x2;
	double max_x = /*x2 < x3 ? x3 : x2;*/x1 < x2 ? x2 : x1;
	double min_y = /*y2 < y3 ? y2 : y3;*/y1 < y2 ? y1 : y2;
	double max_y = /*y2 < y2 ? y3 : y2;*/y1 < y2 ? y2 : y1;

	if(*x < min_x)
		*x = min_x;
	else if(*x > max_x)
		*x = max_x;

	if(*y < min_y)
		*y = min_y;
	else if(*y > max_y)
		*y = max_y;

	return true;
}

bool PureMath::LineCross(double xa1, double ya1, double xa2, double ya2,
							double xb1, double yb1, double xb2, double yb2)
{
	if(	(fabs(xa1 - xb1) < PM_DOUBLE_ZERO && fabs(ya1 - yb1) < PM_DOUBLE_ZERO )
		||	(fabs(xa1 - xb2) < PM_DOUBLE_ZERO && fabs(ya1 - yb2) < PM_DOUBLE_ZERO ) 
		||	(fabs(xa2 - xb1) < PM_DOUBLE_ZERO && fabs(ya2 - yb1) < PM_DOUBLE_ZERO )
		||	(fabs(xa2 - xb2) < PM_DOUBLE_ZERO && fabs(ya2 - yb2) < PM_DOUBLE_ZERO ) )
		return /*false*/true;	//by hqs 2007-2-2

	return LeftSide(xa1, ya1, xa2, ya2, xb1, yb1)
		!= LeftSide(xa1, ya1, xa2, ya2, xb2, yb2)
		&& LeftSide(xb1, yb1, xb2, yb2, xa1, ya1)
		!= LeftSide(xb1, yb1, xb2, yb2, xa2, ya2);
}

/*    (x1, y1)
	*     \
	*	   \   . (x, y)			<----------- true				
	*       \
	*      (x2, y2)
	*/
bool PureMath::LeftSide(double x1, double y1, double x2, double y2, double x, double y)
{
		return (y - y1) * (x2 - x1) > (x - x1) * (y2 - y1);
}

bool PureMath::LineOverArea_Rough(double lx1, double ly1, double lx2, double ly2,
		double ax1, double ay1, double ax2, double ay2)
{
	return AreaOverlap( lx1, ly1, lx2, ly2,
		ax1, ay1, ax2, ay2);
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int_32 PureMath::FindXOnLine(int_32 x1, int_32 y1, int_32 x2, int_32 y2, int_32 y)
{
	if(y1 == y2) 
		return min(x1,x2);
	return (y - y2) * (x1 - x2) / (y1 - y2) + x2;
}
/*
* (x1, y1) & (x2, y2) define the line, now we have a point on line with coordinate x, what is the y?
*/
int_32 PureMath::FindYOnLine(int_32 x1, int_32 y1, int_32 x2, int_32 y2, int_32 x)
{
	if(x1 == x2) 
		return min(y1,y2);
	return (x - x2) * (y1 - y2) / (x1 - x2) + y2;
}

bool PureMath::isPointInRect(int_32 x, int_32 y, int_32 x1, int_32 y1, int_32 x2, int_32 y2)
{
	int_32 l, r, t, b;

	if(x1 < x2)
		l = x1, r = x2;
	else
		l = x2, r = x1;

	if(y1 < y2)
		b = y1, t = y2;
	else
		b = y2, t = y1;

	return l < x && r > x && b < y && t > y;
}

void PureMath::calStdLineEquation(int_32 xa, int_32 ya, int_32 xb, int_32 yb,
								  int_32 * A, int_32 * B, int_32 * C )
{//已知两个点，计算直线的标准方程：Ax+By+Cz=0
	//两点式方程为：(y-ya)/(yb-ya) = (x-xa)/(xb-xa) =>
	//系数为：A = yb - ya, B = xa - xb, 
	//c = ya( xb - xa ) - xa ( yb- ya ) = -ya( xa - xb ) - xa( yb - ya ) = ya*(-B) - xa*A
	*A = yb - ya;
	*B = xa - xb;
	*C = ya * ( -(*B) ) - xa * (*A);
}

//点是否在直线上方(不包含3点共线)
bool PureMath::isPointAboveLine(int_32 x, int_32 y, int_32 xa, int_32 ya, int_32 xb, int_32 yb)
{
	if(xa == xb || ya == yb)
	{
		//if( abs(start.d1 - end.d1) < DOUBLE_ZERO)	//no matter horizontal or vertical
		return y > max(ya, yb);
	}

	int_32 A = 0, B = 0, C = 0;
	calStdLineEquation(xa, ya, xb, yb, &A, &B, &C);

	bool flag1 = A*B < 0,
		flag2 = A*x+B*y+C<0;
	if( (flag1&&flag2) || (!flag1 && !flag2) )
		return true;
	return false;
}

bool PureMath::isPointLeftSideLine(int_32 x, int_32 y, int_32 xa, int_32 ya, int_32 xb, int_32 yb)
{
	if(xa == xb && ya == yb)
		return x < xa || (x == xa && y > ya);

	if(xa == xb || xa == x) // (xa,ya)->(xb,yb) 垂直x轴 \\ (xa,ya)->(x,y) 垂直x轴
		return x < xb;

	Fraction fraction1(y - ya, x - xa);
	Fraction fraction2(yb - ya, xb - xa);

	return fraction1 > fraction2; // 通过比较斜率来比较左右
}

void PureMath::moveLineToPos(int_32 xa,int_32 ya, int_32 xb, int_32 yb, int_32 x, int_32 y,
							 int_32 * new_xa, int_32 *new_ya, int_32 * new_xb, int_32 *new_yb)
{
	int_32 foot_x = x, foot_y = y;
	getFootOfPerpendicular(xa, ya, xb, yb, x,y, &foot_x, &foot_y);

	int_32 delta_x = x - foot_x, delta_y = y - foot_y;

	*new_xa = xa + delta_x;
	*new_ya = ya + delta_y;
	*new_xb = xb + delta_x;
	*new_yb = yb + delta_y;
}

void PureMath::getFootOfPerpendicular(int_32 xa,int_32 ya,int_32 xb, int_32 yb, 
									  int_32 x, int_32 y,int_32 * fx, int_32 *fy)
{
	if(xa == xb && ya == yb)
	{
		*fx = xa;
		*fy = ya;
		return ;
	}

	int_32 A = 0, B = 0, C = 0;
	calStdLineEquation(xa, ya, xb, yb, &A, &B, &C);

	*fx = ( A * A * x + B * B * xa + A * B * ( y - ya ) )
		/ ( A * A + B * B );
	if(xa == xb)
	{
		*fy = y;
	}
	else
	{
		*fy = FindYOnLine( xa, ya, xb, yb, *fx );
	}
}

/*
*	to fine whether the line has at lease some part in the area(rectangle)
*/
/* 求交算法描述；分别用四位表示：左右上下 
*  1010     0010     0110                                                         
*        ----------                                                             
*  1000  |  0000  |  0100                                                      
*        ----------                                                            
*  1001     0001     0101                                                       
*  直线的两个点各有一个4位的数，求交非1，肯定不相交，                                                                              
*  求与无1，肯定相交，有1，即判断和1位处表示的线求交。
*/
// 2001-03-03; 改成严格匹配。要求rect是有序的。而且不是退化的，
bool PureMath::LineOverArea(int_32 lx1, int_32 ly1, int_32 lx2, int_32 ly2,
							int_32 ax1, int_32 ay1, int_32 ax2, int_32 ay2)
{

	//		return LineOverArea_Rough(lx1, ly1,  lx2, ly2,
	//							   ax1,  ay1,  ax2,  ay2);
	unsigned char typeS, typeE;
	typeS = typeE = 0;
	if( lx1 < ax1 )      // 左：1000
		typeS |= 0x8; 
	else if( lx1 > ax2 ) // 右：0100
		typeS |= 0x4; 
	if( ly1 < ay1 )      // 下：0001
		typeS |= 0x1; 
	else if( ly1 > ay2 ) // 上：0010
		typeS |= 0x2; 

	if( lx2 < ax1 )      // 左：1000
		typeE |= 0x8; 
	else if( lx2 > ax2 ) // 右：0100
		typeE |= 0x4; 
	if( ly2 < ay1 )      // 下：0001
		typeE |= 0x1; 
	else if( ly2 > ay2 ) // 上：0010
		typeE |= 0x2; 

	unsigned char cmp;
	cmp = typeS & typeE;
	if( cmp != 0 ) // 求与非0，肯定在外面，不相交。
		return false;
	cmp = typeS | typeE;
	if( cmp == 0 ) // 求交全0，肯定在里面，必相交。
		return true;

	// 比较四条边
	if( cmp & 0x8 ) // 1000，左边
	{
		int_32 cross_y;
		cross_y = FindYOnLine( lx1, ly1, lx2, ly2, ax1 );
		if( cross_y >= ay1 && cross_y <= ay2 )
			return true;
	}
	if( cmp & 0x4 ) // 0100，右边
	{
		int_32 cross_y;
		cross_y = FindYOnLine( lx1, ly1, lx2, ly2, ax2 );
		if( cross_y >= ay1 && cross_y <= ay2 )
			return true;
	}
	if( cmp & 0x1 ) // 0001，下边
	{
		int_32 cross_x;
		cross_x = FindXOnLine( lx1, ly1, lx2, ly2, ay1 );
		if( cross_x >= ax1 && cross_x <= ax2 )
			return true;
	}
	if( cmp & 0x2 ) // 0010，上边
	{
		int_32 cross_x;
		cross_x = FindXOnLine( lx1, ly1, lx2, ly2, ay2 );
		if( cross_x >= ax1 && cross_x <= ax2 )
			return true;
	}

	return false;
}

//  is the
bool PureMath::PointInArea(int_32 x, int_32 y, int_32 x1, int_32 y1, int_32 x2, int_32 y2)
{
	int_32 l, r, t, b;

	if(x1 < x2)
		l = x1, r = x2;
	else
		l = x2, r = x1;

	if(y1 < y2)
		b = y1, t = y2;
	else
		b = y2, t = y1;

	return l < x && r > x && b < y && t > y;
}

int_32 PureMath::getDistancePointToLine(int_32 x, int_32 y, int_32 xa, int_32 ya, int_32 xb, int_32 yb)
{
	if(xa == xb && ya == yb) // 若直線兩個點相等
		return int_32(sqrt(0.0+(x - xa) * (x - xa)) + (y - ya) * (y - ya));

	int_32 A = 0, B = 0, C = 0;
	calStdLineEquation(xa, ya, xb, yb, &A, &B, &C);
	int_32 ret = (int_32)(abs(A * x + B * y + C) / (sqrt(0.0+A*A + B*B)));
	return ret;
}

//// 2001-03-03; 改成严格匹配。要求rect是有序的。而且不是退化的，
//bool PureMath::LineOverArea(int_32 lx1, int_32 ly1, int_32 lx2, int_32 ly2,
//							int_32 ax1, int_32 ay1, int_32 ax2, int_32 ay2)
//{
//
////		return LineOverArea_Rough(lx1, ly1,  lx2, ly2,
////							   ax1,  ay1,  ax2,  ay2);
//	unsigned char typeS, typeE;
//	typeS = typeE = 0;
//	if( lx1 < ax1 )      // 左：1000
//		typeS |= 0x8; 
//	else if( lx1 > ax2 ) // 右：0100
//		typeS |= 0x4; 
//	if( ly1 < ay1 )      // 下：0001
//		typeS |= 0x1; 
//	else if( ly1 > ay2 ) // 上：0010
//		typeS |= 0x2; 
//		
//	if( lx2 < ax1 )      // 左：1000
//		typeE |= 0x8; 
//	else if( lx2 > ax2 ) // 右：0100
//		typeE |= 0x4; 
//	if( ly2 < ay1 )      // 下：0001
//		typeE |= 0x1; 
//	else if( ly2 > ay2 ) // 上：0010
//		typeE |= 0x2; 
//
//	unsigned char cmp;
//	cmp = typeS & typeE;
//	if( cmp != 0 ) // 求与非0，肯定在外面，不相交。
//		return false;
//	cmp = typeS | typeE;
//	if( cmp == 0 ) // 求交全0，肯定在里面，必相交。
//		return true;
//	
//	// 比较四条边
//	if( cmp & 0x8 ) // 1000，左边
//	{
//		int_32 cross_y;
//		cross_y = FindYOnLine( lx1, ly1, lx2, ly2, ax1 );
//		if( cross_y >= ay1 && cross_y <= ay2 )
//			return true;
//	}
//	if( cmp & 0x4 ) // 0100，右边
//	{
//		int_32 cross_y;
//		cross_y = FindYOnLine( lx1, ly1, lx2, ly2, ax2 );
//		if( cross_y >= ay1 && cross_y <= ay2 )
//			return true;
//	}
//	if( cmp & 0x1 ) // 0001，下边
//	{
//		int_32 cross_x;
//		cross_x = FindXOnLine( lx1, ly1, lx2, ly2, ay1 );
//		if( cross_x >= ax1 && cross_x <= ax2 )
//			return true;
//	}
//	if( cmp & 0x2 ) // 0010，上边
//	{
//		int_32 cross_x;
//		cross_x = FindXOnLine( lx1, ly1, lx2, ly2, ay2 );
//		if( cross_x >= ax1 && cross_x <= ax2 )
//			return true;
//	}
//
//	return false;
//}

bool PureMath::AreaOverlap(int_32 x1, int_32 y1, int_32 x2, int_32 y2,
						   int_32 x3, int_32 y3, int_32 x4, int_32 y4)
{
	int_32 l1, r1, t1, b1,
		l2, r2, t2, b2;

	// 1st do some sort...
	if(x1 <= x2)
		l1 = x1, r1 = x2;
	else
		l1 = x2, r1 = x1;

	if(y1 <= y2)
		b1 = y1, t1 = y2;
	else
		b1 = y2, t1 = y1;

	if(x3 <= x4)
		l2 = x3, r2 = x4;
	else
		l2 = x4, r2 = x3;

	if(y3 <= y4)
		b2 = y3, t2 = y4;
	else
		b2 = y4, t2 = y3;

	// compare the value
	return l1 <= r2 && r1 >= l2 && b1 <= t2 && t1 >= b2;
}

//若不相交，则返回false;同时:若平行，x,y被设为最大值；否则交点仍被求出并设置为x,y.by hqs
bool PureMath::lineCrossPoint(int_32 x1, int_32 y1, int_32 x2, int_32 y2, 
							  int_32 x3, int_32 y3, int_32 x4, int_32 y4, int_32 *x, int_32 *y)
{
	//2个顶点重合， by hqs 2007-2-4
	if(x1 == x3 && y1 == y3)
	{
		*x = x1;
		*y = y1;
		return true;
	}
	if(x1 == x4 && y1 == y4) 
	{
		*x = x1;
		*y = y1;
		return true;
	}
	if(x2 == x3 && y2 == y3)
	{
		*x = x2;
		*y = y2;
		return true;
	}
	if(x2 == x4 && y2 == y4) 
	{
		*x = x2;
		*y = y2;
		return true;
	}

	//是否平行？ 分母 < 0
	if( (y4 - y3)*(x2 - x1) == (y2 - y1)*(x4 - x3) /*|| sqrt((x3-x2)*(x3-x2) + (y3-y2)*(y3-y2)) < 3*/)	//comment by hqs 2007-2-2
	{
		/**x = (x2 + x3)/2, *y = (y2 + y3)/2;*/
		*x = _I32_MAX, *y = _I32_MAX;
		return false;
	}
	*x = (((int_64)x2*y1 - (int_64)x1*y2)*(x4 - x3) - ((int_64)x4*y3 - (int_64)x3*y4)*(x2 - x1))
		/
		((int_64)(y4 - y3)*(x2 - x1) - (int_64)(y2 - y1)*(x4 - x3));

	*y = (((int_64)y2*x1 - (int_64)y1*x2)*(y4 - y3) - ((int_64)y4*x3 - (int_64)y3*x4)*(y2 - y1))
		/
		((int_64)(x4 - x3)*(y2 - y1) - (int_64)(x2 - x1)*(y4 - y3));
	if( ! LineCross(x1, y1, x2, y2, x3, y3, x4, y4) )	//不相交，但仍然求出交点
		return false;
	//patched by hqs 2007-2-2
	int_32 min_x = /*x2 < x3 ? x2 : x3;*/x1 < x2 ? x1 : x2;
	int_32 max_x = /*x2 < x3 ? x3 : x2;*/x1 < x2 ? x2 : x1;
	int_32 min_y = /*y2 < y3 ? y2 : y3;*/y1 < y2 ? y1 : y2;
	int_32 max_y = /*y2 < y2 ? y3 : y2;*/y1 < y2 ? y2 : y1;

	if(*x < min_x)
		*x = min_x;
	else if(*x > max_x)
		*x = max_x;

	if(*y < min_y)
		*y = min_y;
	else if(*y > max_y)
		*y = max_y;

	return true;
}

bool PureMath::LineCross(int_32 xa1, int_32 ya1, int_32 xa2, int_32 ya2,
						 int_32 xb1, int_32 yb1, int_32 xb2, int_32 yb2)
{
	if(		(xa1 == xb1 && ya1 == yb1)
		||	(xa1 == xb2 && ya1 == yb2) 
		||	(xa2 == xb1 && ya2 == yb1)
		||	(xa2 == xb2 && ya2 == yb2) )
		return /*false*/true;	//by hqs 2007-2-2

	return LeftSide(xa1, ya1, xa2, ya2, xb1, yb1)
		!= LeftSide(xa1, ya1, xa2, ya2, xb2, yb2)
		&& LeftSide(xb1, yb1, xb2, yb2, xa1, ya1)
		!= LeftSide(xb1, yb1, xb2, yb2, xa2, ya2);
}

/*    (x1, y1)
*     \
*	   \   . (x, y)			<----------- true				
*       \
*      (x2, y2)
*/
bool PureMath::LeftSide(int_32 x1, int_32 y1, int_32 x2, int_32 y2, int_32 x, int_32 y)
{
	return (y - y1) * (x2 - x1) > (x - x1) * (y2 - y1);
}

bool PureMath::LineOverArea_Rough(int_32 lx1, int_32 ly1, int_32 lx2, int_32 ly2,
								  int_32 ax1, int_32 ay1, int_32 ax2, int_32 ay2)
{
	return AreaOverlap( lx1, ly1, lx2, ly2,
		ax1, ay1, ax2, ay2);
}

double PureMath::AreaOfTriangle(int_32 x1,int_32 y1,int_32 x2,int_32 y2,int_32 x3,int_32 y3)
{
	double len_a = sqrt(calDistanceSquare(x1,y1,x2,y2));
	long len_h = getDistancePointToLine(x3,y3,x1,y1,x2,y2);
	return len_a*len_h/2;
}


}