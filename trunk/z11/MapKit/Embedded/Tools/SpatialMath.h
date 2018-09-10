#ifndef _EMBEDDED_SPATIAL_MATH_H_
#define _EMBEDDED_SPATIAL_MATH_H_
#include "Tools.h"
#include "../BasicType/Rect.h"
#include "../BasicType/Scope.h"

namespace Embedded
{
	class SpatialMath
	{
	public:

		//////////////////////////////////////////////////////////////////////////
		//
		// Test if satisfy some conditions, return type is bool.
		//
		//////////////////////////////////////////////////////////////////////////

		// To check if test at middle of bound0 and bound1
		static bool isBetween(double bound0, double bound1, double test) {
			if (bound0 < bound1)
				return test < bound1 + DOUBLE_TOLERANCE && test > bound0 - DOUBLE_TOLERANCE;
			else
				return test < bound0 + DOUBLE_TOLERANCE && test > bound1 - DOUBLE_TOLERANCE;
		}

		// To check if the segment intersect with the Rect.
		static bool isSegmentIntersectRect(double lx1, double ly1, double lx2, double ly2,
			double ax1, double ay1, double ax2, double ay2);
		static bool isSegmentIntersectRect(const MapPos2D& b, const MapPos2D& e, const MapScope& scope);

		// To check if the point in the rect.
		// x, y  : the point.
		// x1, y1: the left bottom point of rect
		// x2, y2: the right top point of rect
		// if bTouch is true, than return true when point on the edge of rect.
		static bool isPointInRect(double x, double y, double x1, double y1, double x2, double y2, bool bTouch = true);
		static bool isPointInRect(const MapPos2D& p, const MapScope& scope, bool bTouch = true);

		// To check if the 2 point overlapped with each other.
		static bool isTheSamePos(const MapPos2D& lhs, const MapPos2D& rhs, const double tolerance = DOUBLE_ZERO);

		// To check if the point is at left of the segment which direction is s -> e.
		// Attention: It can't handle the point on segment.
		//			  Before use it must make sure the point is not on the segment.
		static bool isPointAtSegmentLeft(const MapPos2D& p, const MapPos2D& s, const MapPos2D& e);

		// To check if segment(s0, e0) intersect segment(s1, e1).
		// If bConsiderCrossEndpoint is false, when an endpoint of a segment on the other segment, think they are not crossed.
		static bool isSegmentCross(const MapPos2D& s0, const MapPos2D& e0, const MapPos2D& s1, const MapPos2D& e1, bool bConsiderCrossEndpoint = true, const double tolerance = DOUBLE_ZERO);

		// To check if angle a-center-b is acute.
		static bool isAcuteAngle(const MapPos2D& a, const MapPos2D& center, const MapPos2D& b);

		//////////////////////////////////////////////////////////////////////////
		//
		// Compute some geographic value, return type is double.
		//
		//////////////////////////////////////////////////////////////////////////

		// Get the coordinate of a segment.
		static double findXOnSeg(double x1, double y1, double x2, double y2, double y);
		static double findYOnSeg(double x1, double y1, double x2, double y2, double x);
		static double findXOnSeg(const MapPos2D& b, const MapPos2D& e, double y);
		static double findYOnSeg(const MapPos2D& b, const MapPos2D& e, double x);

		// Get the distance of 2 point
		static double getDistance(double sx, double sy, double ex, double ey);
		static double getDistance(const MapPos2D& s, const MapPos2D& e); 
		static double getDistanceWithSquare(const MapPos2D& s, const MapPos2D& e);

		// Point is on the Line, get the distance of the point to the end of the Line.
		static double getDisOnLinePosToLineEnd(const MapPos2D& p, const std::vector<MapPos2D>& l, bool bStart);

		// Get distance p to s-e
		static double getDisPointToSegment(const MapPos2D& p, const MapPos2D& s, const MapPos2D& e);

		//////////////////////////////////////////////////////////////////////////
		//
		// Test through input value, output the multiple situations.
		// Return type is int, different value denote different situations.
		//
		//////////////////////////////////////////////////////////////////////////

		// 3 point can decide a circle, get its center.
		// return: [0] success, [-1] not success, 3 points on a segment.
		static int getCircumcenter(const MapPos2D& pos1, const MapPos2D& pos2, const MapPos2D& pos3, MapPos2D* center, double tolerance = DOUBLE_ZERO); 

		// To check if the point on the segment.
		// return: [-1] outside		[0] on the middle		[1] overlap with startPos
		//		    [2] overlap with endPos		[3] the 3 point are overlap each other.
		static int testPointOnSection(const MapPos2D& startPos, const MapPos2D& endPos, const MapPos2D& thdPos, double tolerance = DOUBLE_ZERO);

		// To check if the point in the triangle.
		// return: [0] p is on the edge t0-t1		[1] p is on the edge t1-t2
		//		   [2] p is on the edge t2-t0		[3] p is doublication with one point of triangle
		//		   [4] p is in the triangle			[-1] p is outside
		static int checkPosInTriangle(const MapPos2D& p, const MapPos2D& t0, const MapPos2D& t1, const MapPos2D& t2, double tolerance = DOUBLE_ZERO);

		//////////////////////////////////////////////////////////////////////////
		//
		// Get other value.
		//
		//////////////////////////////////////////////////////////////////////////

		// Get the max bound region of many points.
		static void getMBR(const std::vector<MapPos2D>& points, MapScope& mbr);

		// Get the cross point of two segment.
		// If they are not crossed, still compute the cross point unless they are parallel.
		// If they are crossed return true else return false.
		// If they are parallel, crossPoint set MAX and return false.
		static bool getSegmentCrossPoint(const MapPos2D& s0, const MapPos2D& e0, const MapPos2D& s1, const MapPos2D& e1, MapPos2D* crossPoint, bool bConsiderCrossEndpoint = true, const double tolerance = DOUBLE_ZERO);
		// Get the cross point of two segment.
		// If they are not crossed, direct return false withnot compute the cross point.
		static bool getSegmentCrossPointNoExtension(const MapPos2D& s0, const MapPos2D& e0, const MapPos2D& s1, const MapPos2D& e1, MapPos2D* crossPoint, bool bConsiderCrossEndpoint = true, const double tolerance = DOUBLE_ZERO);

		// Get the foot of out at line(s, e).
		// Is return -1 the foot is outside the line(s, e), return 0 the foot on the line(s, e).
		static int getFootOfPerpendicular(const MapPos2D& s, const MapPos2D& e, const MapPos2D& out, MapPos2D* foot);

		// Get the nearest position of Line to Point.
		static void getNearestPosOnLineToPoint(const MapPos2D& p, const std::vector<MapPos2D>& l, MapPos2D* nearest);

		// Get the position of Point on Line, use the form of double index.
		// If Point not on Line, return -1.
		static double getPointPositionOnLine(const MapPos2D& p, const std::vector<MapPos2D>& l);

		// Get part control pos of Line, which is the region from s to e.
		static void getPartCtlposOfLine(double s, double e, const std::vector<MapPos2D>& tot, std::vector<MapPos2D>* part);

		// Get the Point on Line which is specify by point position.
		static void getPosOnLineByPointPosition(const std::vector<MapPos2D>& l, double position, MapPos2D* p);

		// Get the center of many points
		static void getCenterOfPoints(const std::vector<MapPos2D>& pos, MapPos2D& center);

		// Get the intersection between segment and rect
		static void getIntersectionSegByRect(const MapPos2D& b, const MapPos2D& e, const MapScope& scope, std::vector<MapPos2D>& intersection);

		// Using for QuadTree, position from 0 to 3, indicate the position of the child
		// Return the Rect of position in original.
		// 0 1
		// 2 3
		static void getRectChildGrid(const Rect& src, uint_32 position, Rect& dest);

		// Using for QuadTree, position from 0 to 3, indicate the position of the child
		// Return the position of point in Rect.
		// 0 1
		// 2 3
		static uint_32 getPositionPosInRect(const Rect& parent, const MapPos2D& p);

		static double calculateCourse( const Pos2D &stdPos , const Pos2D &endPos);
	};

}

#endif // _EMBEDDED_SPATIAL_MATH_H_