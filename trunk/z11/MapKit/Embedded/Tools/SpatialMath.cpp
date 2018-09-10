#include "SpatialMath.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288 
#endif
using namespace Embedded;

bool SpatialMath::isSegmentIntersectRect(double lx1, double ly1, double lx2, double ly2,
										 double ax1, double ay1, double ax2, double ay2)
{
	unsigned char typeS, typeE;
	typeS = typeE = 0;
	if ( lx1 < ax1 )      // left:  1000
		typeS |= 0x8; 
	else if ( lx1 > ax2 ) // right: 0100
		typeS |= 0x4; 
	if ( ly1 < ay1 )      // down:  0001
		typeS |= 0x1; 
	else if ( ly1 > ay2 ) // up:    0010
		typeS |= 0x2; 

	if ( lx2 < ax1 )      // left:  1000
		typeE |= 0x8; 
	else if ( lx2 > ax2 ) // right: 0100
		typeE |= 0x4; 
	if ( ly2 < ay1 )      // down:  0001
		typeE |= 0x1; 
	else if ( ly2 > ay2 ) // up:    0010
		typeE |= 0x2; 

	unsigned char comp;
	comp = typeS & typeE;
	if ( comp != 0 )		// must be outside, so must intersect
		return false;
	comp = typeS | typeE;
	if ( comp == 0 )		// must be inside, so must not intersect
		return true;

	// test each edge
	if ( comp & 0x8 )		// 1000, left edge
	{
		double cross_y;
		cross_y = findYOnSeg( lx1, ly1, lx2, ly2, ax1 );
		if ( cross_y >= ay1 && cross_y <= ay2 )
			return true;
	}
	if ( comp & 0x4 )		// 0100, right edge
	{
		double cross_y;
		cross_y = findYOnSeg( lx1, ly1, lx2, ly2, ax2 );
		if ( cross_y >= ay1 && cross_y <= ay2 )
			return true;
	}
	if ( comp & 0x1 )		// 0001, down edge
	{
		double cross_x;
		cross_x = findXOnSeg( lx1, ly1, lx2, ly2, ay1 );
		if ( cross_x >= ax1 && cross_x <= ax2 )
			return true;
	}
	if ( comp & 0x2 )		// 0010, up edge
	{
		double cross_x;
		cross_x = findXOnSeg( lx1, ly1, lx2, ly2, ay2 );
		if ( cross_x >= ax1 && cross_x <= ax2 )
			return true;
	}

	return false;
}

bool SpatialMath::isSegmentIntersectRect(const MapPos2D& b, const MapPos2D& e, const MapScope& scope)
{
	return isSegmentIntersectRect(b.d0, b.d1, e.d0, e.d1, scope.m_xMin, scope.m_yMin, scope.m_xMax, scope.m_yMax);
}

bool SpatialMath::isPointInRect(double x, double y, double x1, double y1, double x2, double y2, bool bTouch/* = true*/)
{
	double l, r, t, b;

	if (x1 < x2)
		l = x1, r = x2;
	else
		l = x2, r = x1;

	if (y1 < y2)
		b = y1, t = y2;
	else
		b = y2, t = y1;

	if (bTouch)
	{
		return x >= l - DOUBLE_TOLERANCE && x <= r + DOUBLE_TOLERANCE
			&& y >= b - DOUBLE_TOLERANCE && y <= t + DOUBLE_TOLERANCE;	
	}
	else
	{
		return x > l + DOUBLE_TOLERANCE && x < r - DOUBLE_TOLERANCE
			&& y > b + DOUBLE_TOLERANCE && y < t - DOUBLE_TOLERANCE;	
	}
}

bool SpatialMath::isPointInRect(const MapPos2D& p, const MapScope& scope, bool bTouch/* = true*/)
{
	return isPointInRect(p.d0, p.d1, scope.m_xMin, scope.m_yMin, scope.m_xMax, scope.m_yMax);
}

bool SpatialMath::isTheSamePos(const MapPos2D& lhs, const MapPos2D& rhs, const double tolerance/* = DOUBLE_ZERO*/)
{
	return fabs(lhs.d0 - rhs.d0) <= tolerance
		&& fabs(lhs.d1 - rhs.d1) <= tolerance;
}

bool SpatialMath::isPointAtSegmentLeft(const MapPos2D& p, const MapPos2D& s, const MapPos2D& e)
{
	return (p.d1 - s.d1) * (e.d0 - s.d0) - (p.d0 - s.d0) * (e.d1 - s.d1) > DOUBLE_TOLERANCE;
}

bool SpatialMath::isSegmentCross(const MapPos2D& s0, const MapPos2D& e0, 
								 const MapPos2D& s1, const MapPos2D& e1,
								 bool bConsiderCrossEndpoint/* = true*/,
								 const double tolerance/* = DOUBLE_ZERO*/)
{
	MapScope r0(s0, e0), r1(s1, e1);
	if (!r0.isIntersect(r1))
		return false;

	if (testPointOnSection(s1, e1, s0, tolerance) != -1 || testPointOnSection(s1, e1, e0, tolerance) != -1 || 
		testPointOnSection(s0, e0, s1, tolerance) != -1 || testPointOnSection(s0, e0, e1, tolerance) != -1)
	{
		if (bConsiderCrossEndpoint)
			return true;
		else
			return false;
	}

	return isPointAtSegmentLeft(s0, s1, e1) != isPointAtSegmentLeft(e0, s1, e1)
		&& isPointAtSegmentLeft(s1, s0, e0) != isPointAtSegmentLeft(e1, s0, e0);
}

bool SpatialMath::isAcuteAngle(const MapPos2D& a, const MapPos2D& center, const MapPos2D& b)
{
	return (a.d0 - center.d0) * (a.d0 - center.d0) + (a.d1 - center.d1) * (a.d1 - center.d1)
		+ (b.d0 - center.d0) * (b.d0 - center.d0) + (b.d1 - center.d1) * (b.d1 - center.d1)
		- (a.d0 - b.d0) * (a.d0 - b.d0) - (a.d1 - b.d1) * (a.d1 - b.d1) > 0;
}

//
// (x1, y1) & (x2, y2) define the segment, now we have a point on segment with coordinate y, what is the x?
//
double SpatialMath::findXOnSeg(double x1, double y1, double x2, double y2, double y)
{
	if (y1 == y2) 
		return std::min(x1, x2);
	return (y - y2) * (x1 - x2) / (y1 - y2) + x2;
}

//
// (x1, y1) & (x2, y2) define the segment, now we have a point on segment with coordinate x, what is the y?
//
double SpatialMath::findYOnSeg(double x1, double y1, double x2, double y2, double x)
{
	if (x1 == x2) 
		return std::min(y1, y2);
	return (x - x2) * (y1 - y2) / (x1 - x2) + y2;
}

double SpatialMath::findXOnSeg(const MapPos2D& b, const MapPos2D& e, double y)
{
	return findXOnSeg(b.d0, b.d1, e.d0, e.d1, y);
}

double SpatialMath::findYOnSeg(const MapPos2D& b, const MapPos2D& e, double x)
{
	return findYOnSeg(b.d0, b.d1, e.d0, e.d1, x);
}

double SpatialMath::getDistance(double sx, double sy, double ex, double ey)
{
	return sqrt(pow((sx - ex), 2) + pow((sy - ey), 2));
}

double SpatialMath::getDistance(const MapPos2D& s, const MapPos2D& e)
{
	return sqrt(getDistanceWithSquare(s, e));
}

double SpatialMath::getDistanceWithSquare(const MapPos2D& s, const MapPos2D& e)
{
	return pow((e.d0 - s.d0), 2) + pow((e.d1 - s.d1), 2);
}

double SpatialMath::getDisOnLinePosToLineEnd(const MapPos2D& p, const std::vector<MapPos2D>& l, bool bStart)
{
	uint_32 iStart, iEnd;
	if (bStart)
	{
		iStart = 0;
		iEnd = 1;
	}
	else
	{
		iStart = l.size() - 1;
		iEnd = iStart - 1;
	}

	double ret = 0.0;
	bool bFinish = false;
	while (!bFinish)
	{
		int state = testPointOnSection(l.at(iStart), l.at(iEnd), p);
		switch (state)
		{
		case -1:
			ret += getDistance(l.at(iStart), l.at(iEnd));
			if (bStart)
			{
				++iStart;
				++iEnd;
				if (iEnd == l.size())
				{
					assert(0);
					bFinish = true;
				}
			}
			else
			{
				--iStart;
				--iEnd;
				if (iStart == 0)
				{
					assert(0);
					bFinish = true;
				}
			}
			break;
		case 0:
			ret += getDistance(l.at(iStart), p);
			bFinish = true;
			break;
		case 1: case 3:
			bFinish = true;
			break;
		case 2:
			ret += getDistance(l.at(iStart), l.at(iEnd));
			bFinish = true;
			break;
		default:
			assert(0);
			bFinish = true;
			break;
		}
	}

	return ret;
}

double SpatialMath::getDisPointToSegment(const MapPos2D& p, const MapPos2D& s, const MapPos2D& e)
{
	if (!isAcuteAngle(p, s, e))
		return getDistance(p, s);
	else if (!isAcuteAngle(p, e, s))
		return getDistance(p, e);
	else
		return fabs((s.d0 - p.d0) * (e.d1 - p.d1) - (s.d1 - p.d1) * (e.d0 - p.d0)) /
			SpatialMath::getDistance(s, e);
}

int SpatialMath::getCircumcenter(const MapPos2D& pos1, const MapPos2D& pos2, 
								 const MapPos2D& pos3, MapPos2D* center, double tolerance/* = DOUBLE_ZERO*/)
{
	if (testPointOnSection(pos1, pos2, pos3, tolerance) != -1)
		return -1;

	double x1, x2, x3, y1, y2, y3;
	double a, b, c, d, e, f;
	x1 = pos1.d0; y1 = pos1.d1;
	x2 = pos2.d0; y2 = pos2.d1;
	x3 = pos3.d0; y3 = pos3.d1;

	a = 2 * (x2 - x1);
	b = 2 * (y2 - y1);
	c = x2 * x2 + y2 * y2 - x1 * x1 - y1 * y1;
	d = 2 * (x3 - x2);
	e = 2 * (y3 - y2);
	f = x3 * x3 + y3 * y3 - x2 * x2 - y2 * y2;

	center->d0 = (b * f - e * c) / (b * d - e * a);
	center->d1 = (d * c - a * f) / (b * d - e * a);

	return 0;
}

int SpatialMath::testPointOnSection(const MapPos2D& startPos, const MapPos2D& endPos, 
									const MapPos2D& thdPos, double tolerance/* = DOUBLE_ZERO*/)
{
	assert(tolerance > 0.0);
	const double llx = thdPos.d0 - tolerance / 2.0;
	const double lly = thdPos.d1 - tolerance / 2.0;
	const double rtx = thdPos.d0 + tolerance / 2.0;
	const double rty = thdPos.d1 + tolerance / 2.0;

	if (!isSegmentIntersectRect(
		startPos.d0, startPos.d1, endPos.d0, endPos.d1, llx, lly, rtx, rty) 
		) 
		return -1;

	int resultA = 0;
	int resultB = 0;
	if (isPointInRect(startPos.d0, startPos.d1, llx, lly, rtx, rty, false))
		resultA = 1;
	if (isPointInRect(endPos.d0, endPos.d1, llx, lly, rtx, rty, false))
		resultB = 2;

	return resultA + resultB;
}

int SpatialMath::checkPosInTriangle(const MapPos2D& p, const MapPos2D& t0, 
									const MapPos2D& t1, const MapPos2D& t2,
									double tolerance/* = DOUBLE_ZERO*/)
{
	if (isTheSamePos(p, t0, tolerance) || isTheSamePos(p, t1, tolerance) || isTheSamePos(p, t2, tolerance))
		return 3;
	if (testPointOnSection(t0, t1, p, tolerance) == 0)
		return 0;
	if (testPointOnSection(t1, t2, p, tolerance) == 0)
		return 1;
	if (testPointOnSection(t2, t0, p, tolerance) == 0)
		return 2;

	if (isPointAtSegmentLeft(p, t0, t1) != isPointAtSegmentLeft(p, t0, t2)
	&& isPointAtSegmentLeft(p, t1, t2) != isPointAtSegmentLeft(p, t1, t0)
	&& isPointAtSegmentLeft(p, t2, t1) != isPointAtSegmentLeft(p, t2, t0))
		return 4;
	else 
		return -1;
}

void SpatialMath::getMBR(const std::vector<MapPos2D>& points, MapScope& mbr)
{
	if (points.empty())
	{
		mbr.setNull();
		return;
	}

	mbr.m_xMin = mbr.m_xMax = points.front().d0;
	mbr.m_yMin = mbr.m_yMax = points.front().d1;

	for (uint_32 i = 0; i < points.size(); ++i)
	{
		if (points.at(i).d0 < mbr.m_xMin)
			mbr.m_xMin = points.at(i).d0;
		if (points.at(i).d0 > mbr.m_xMax)
			mbr.m_xMax = points.at(i).d0;
		if (points.at(i).d1 < mbr.m_yMin)
			mbr.m_yMin = points.at(i).d1;
		if (points.at(i).d1 > mbr.m_yMax)
			mbr.m_yMax = points.at(i).d1;
	}
}

bool SpatialMath::getSegmentCrossPoint(const MapPos2D& s0, const MapPos2D& e0, 
									   const MapPos2D& s1, const MapPos2D& e1, 
									   MapPos2D* crossPoint, 
									   bool bConsiderCrossEndpoint/* = true*/,
									   const double tolerance/* = DOUBLE_ZERO*/)
{
	// If one point on other segment ?
	int flag;

	flag = testPointOnSection(s1, e1, s0, tolerance);
	if (0 == flag)
		*crossPoint = s0;
	else if (1 == flag || 3 == flag)
		*crossPoint = s1;
	else if (2 == flag)
		*crossPoint = e1;
	if (flag != -1)
		return bConsiderCrossEndpoint;

	flag = testPointOnSection(s1, e1, e0, tolerance);
	if (0 == flag)
		*crossPoint = s0;
	else if (1 == flag || 3 == flag)
		*crossPoint = s1;
	else if (2 == flag)
		*crossPoint = e1;
	if (flag != -1)
		return bConsiderCrossEndpoint;

	flag = testPointOnSection(s0, e0, s1, tolerance);
	if (0 == flag)
		*crossPoint = s0;
	else if (1 == flag || 3 == flag)
		*crossPoint = s1;
	else if (2 == flag)
		*crossPoint = e1;
	if (flag != -1)
		return bConsiderCrossEndpoint;

	flag = testPointOnSection(s0, e0, e1, tolerance);
	if (0 == flag)
		*crossPoint = s0;
	else if (1 == flag || 3 == flag)
		*crossPoint = s1;
	else if (2 == flag)
		*crossPoint = e1;
	if (flag != -1)
		return bConsiderCrossEndpoint;

	// If they are parallel ?
	double denominatorX = (e1.d1 - s1.d1) * (e0.d0 - s0.d0) - (e0.d1 - s0.d1) * (e1.d0 - s1.d0),
		   denominatorY = (e1.d0 - s1.d0) * (e0.d1 - s0.d1) - (e0.d0 - s0.d0) * (e1.d1 - s1.d1);
	if (fabs(denominatorX) < DOUBLE_TOLERANCE || fabs(denominatorY) < DOUBLE_TOLERANCE)
	{
		crossPoint->d0 = DOUBLE_MAX;
		crossPoint->d1 = DOUBLE_MAX;
		return false;
	}

	// Must have a cross point, including extension line.
	crossPoint->d0 = ( (e0.d0 * s0.d1 - s0.d0 * e0.d1) * (e1.d0 - s1.d0) - (e1.d0 * s1.d1 - s1.d0 * e1.d1) * (e0.d0 - s0.d0) ) / denominatorX;
	crossPoint->d1 = ( (e0.d1 * s0.d0 - s0.d1 * e0.d0) * (e1.d1 - s1.d1) - (e1.d1 * s1.d0 - s1.d1 * e1.d0) * (e0.d1 - s0.d1) ) / denominatorY;

	// Cross or extension cross ?
	if (isPointAtSegmentLeft(s0, s1, e1) != isPointAtSegmentLeft(e0, s1, e1) && 
		isPointAtSegmentLeft(s1, s0, e0) != isPointAtSegmentLeft(e1, s0, e0))
		return true;
	else
		return false;
}

bool SpatialMath::getSegmentCrossPointNoExtension(const MapPos2D& s0, const MapPos2D& e0, 
												  const MapPos2D& s1, const MapPos2D& e1, 
												  MapPos2D* crossPoint, 
												  bool bConsiderCrossEndpoint/* = true*/, 
												  const double tolerance/* = DOUBLE_ZERO*/)
{
	if (!isSegmentCross(s0, e0, s1, e1, bConsiderCrossEndpoint, tolerance))
		return false;

	double denominatorX = (e1.d1 - s1.d1) * (e0.d0 - s0.d0) - (e0.d1 - s0.d1) * (e1.d0 - s1.d0),
		   denominatorY = (e1.d0 - s1.d0) * (e0.d1 - s0.d1) - (e0.d0 - s0.d0) * (e1.d1 - s1.d1);

	assert (fabs(denominatorX) > DOUBLE_TOLERANCE && fabs(denominatorY) > DOUBLE_TOLERANCE);
	crossPoint->d0 = ( (e0.d0 * s0.d1 - s0.d0 * e0.d1) * (e1.d0 - s1.d0) - (e1.d0 * s1.d1 - s1.d0 * e1.d1) * (e0.d0 - s0.d0) ) / denominatorX;
	crossPoint->d1 = ( (e0.d1 * s0.d0 - s0.d1 * e0.d0) * (e1.d1 - s1.d1) - (e1.d1 * s1.d0 - s1.d1 * e1.d0) * (e0.d1 - s0.d1) ) / denominatorY;
	return true;
}

int SpatialMath::getFootOfPerpendicular(const MapPos2D& s, const MapPos2D& e, const MapPos2D& out, MapPos2D* foot)
{
	double	dx = e.d0 - s.d0,
			dy = e.d1 - s.d1;
	double  dxSquare = dx * dx,
			dySquare = dy * dy;

	if (dxSquare + dySquare < DOUBLE_TOLERANCE)
	{
		*foot = s;
		return -1;
	}

	foot->d0 = (dxSquare * out.d0 + dySquare * s.d0 + dx * dy * (out.d1 - s.d1)) / (dxSquare + dySquare);
	if (s.d0 == e.d0)
	{
		foot->d1 = out.d1;
	}
	else
	{
		foot->d1 = findYOnSeg(s, e, foot->d0);
	}

	if (isBetween(s.d0, e.d0, foot->d0) && isBetween(s.d1, e.d1, foot->d1))
		return 0;
	else
		return -1;
}

void SpatialMath::getNearestPosOnLineToPoint(const MapPos2D& p, const std::vector<MapPos2D>& l, MapPos2D* nearest)
{
	uint_32 iNearestPos = 0;
	double nearestDis = getDistance(p, l.at(iNearestPos));
	for (uint_32 i = 1; i < l.size(); ++i)
	{
		double dis = getDistance(p, l.at(i));
		if (dis < nearestDis)
		{
			nearestDis = dis;
			iNearestPos = i;
		}
	}
	*nearest = l.at(iNearestPos);

	if (iNearestPos != l.size() - 1)
	{
		MapPos2D foot;
		if (getFootOfPerpendicular(l.at(iNearestPos), l.at(iNearestPos + 1), p, &foot) == 0)
		{
			double dis = getDistance(foot, p);
			if (dis < nearestDis)
			{
				nearestDis = dis;
				*nearest = foot;
			}
		}
	}
	
	if (iNearestPos != 0)
	{
		MapPos2D foot;
		if (getFootOfPerpendicular(l.at(iNearestPos), l.at(iNearestPos - 1), p, &foot) == 0)
		{
			double dis = getDistance(foot, p);
			if (dis < nearestDis)
				*nearest = foot;
		}
	}
}

double SpatialMath::getPointPositionOnLine(const MapPos2D& p, const std::vector<MapPos2D>& l)
{
	if (l.size() < 2)
	{
		assert(0);
		return -1;
	}

	for (uint_32 i = 0; i < l.size() - 1; ++i)
	{
		int state = testPointOnSection(l.at(i), l.at(i + 1), p);
		if (state == -1)
			continue;
		else if (state == 0)
			return i + getDistance(l.at(i), p) / getDistance(l.at(i), l.at(i + 1));
		else if (state == 1 || state == 3)
			return i;
		else if (state == 2)
			return i + 1;
	}

	return -1;
}

void SpatialMath::getPartCtlposOfLine(double s, double e, const std::vector<MapPos2D>& tot, std::vector<MapPos2D>* part)
{
	assert(s < tot.size() && e < tot.size() && s >= 0 && e >= 0 && s <= e);
	assert(tot.size() > 1);

	MapPos2D start, end;
	getPosOnLineByPointPosition(tot, s, &start);
	getPosOnLineByPointPosition(tot, e, &end);

	part->push_back(start);
	uint_32 sIndex = static_cast<uint_32>(ceil(s));
	while (sIndex < e)
		part->push_back(tot.at(sIndex++));
	part->push_back(end);
}

void SpatialMath::getPosOnLineByPointPosition(const std::vector<MapPos2D>& l, double position, MapPos2D* p)
{
	assert(position <= l.size() - 1 && position >= 0);

	uint_32 index = static_cast<uint_32>(floor(position));
	if (static_cast<double>(index) == position)
		*p = l.at(index);
	else
	{
		assert(index < l.size() - 1);
		double part = position - index;
		p->d0 = ( l.at(index + 1).d0 - l.at(index).d0 ) * part + l.at(index).d0;
		p->d1 = ( l.at(index + 1).d1 - l.at(index).d1 ) * part + l.at(index).d1;
	}
}

void SpatialMath::getCenterOfPoints(const std::vector<MapPos2D>& pos, MapPos2D& center)
{
	center.d0 = 0.0; center.d1 = 0.0;
	if (pos.empty())
		return;

	for (uint_32 i = 0; i < pos.size(); ++i)
	{
		center.d0 += pos[i].d0;
		center.d1 += pos[i].d1;
	}

	center.d0 /= pos.size();
	center.d1 /= pos.size();
}

void SpatialMath::getIntersectionSegByRect(const MapPos2D& b, const MapPos2D& e, const MapScope& scope, std::vector<MapPos2D>& intersection)
{
	unsigned char typeS, typeE;
	typeS = typeE = 0;
	if ( b.d0 < scope.m_xMin )      // left:  1000
		typeS |= 0x8; 
	else if ( b.d0 > scope.m_xMax ) // right: 0100
		typeS |= 0x4; 
	if ( b.d1 < scope.m_yMin )      // down:  0001
		typeS |= 0x1; 
	else if ( b.d1 > scope.m_yMax ) // up:    0010
		typeS |= 0x2; 

	if ( e.d0 < scope.m_xMin )      // left:  1000
		typeE |= 0x8; 
	else if ( e.d0 > scope.m_xMax ) // right: 0100
		typeE |= 0x4; 
	if ( e.d1 < scope.m_yMin )      // down:  0001
		typeE |= 0x1; 
	else if ( e.d1 > scope.m_yMax ) // up:    0010
		typeE |= 0x2; 

	unsigned char comp;
	comp = typeS & typeE;
	if ( comp != 0 )		// must be outside, so must intersect
		return;
	comp = typeS | typeE;
	if ( comp == 0 )		// must be inside, so must not intersect
		return;

	// test each edge
	if ( comp & 0x8 )		// 1000, left edge
	{
		double cross_y;
		cross_y = findYOnSeg( b, e, scope.m_xMin );
		if ( cross_y >= scope.m_yMin && cross_y <= scope.m_yMax )
			intersection.push_back(MapPos2D(scope.m_xMin, cross_y));
	}
	if ( comp & 0x4 )		// 0100, right edge
	{
		double cross_y;
		cross_y = findYOnSeg( b, e, scope.m_xMax );
		if ( cross_y >= scope.m_yMin && cross_y <= scope.m_yMax )
			intersection.push_back(MapPos2D(scope.m_xMax, cross_y));
	}
	if ( comp & 0x1 )		// 0001, down edge
	{
		double cross_x;
		cross_x = findXOnSeg( b, e, scope.m_yMin );
		if ( cross_x >= scope.m_xMin && cross_x <= scope.m_xMax )
			intersection.push_back(MapPos2D(cross_x, scope.m_yMin));
	}
	if ( comp & 0x2 )		// 0010, up edge
	{
		double cross_x;
		cross_x = findXOnSeg( b, e, scope.m_yMax );
		if ( cross_x >= scope.m_xMin && cross_x <= scope.m_xMax )
			intersection.push_back(MapPos2D(cross_x, scope.m_yMax));
	}
}

void SpatialMath::getRectChildGrid(const Rect& src, uint_32 position, Rect& dest)
{
	assert(&src != &dest && position <= 3 && position >= 0);
	switch (position)
	{
	case 0:
		dest.m_pLow[0]	= src.m_pLow[0];
		dest.m_pLow[1]	= 0.5 * (src.m_pLow[1] + src.m_pHigh[1]);
		dest.m_pHigh[0] = 0.5 * (src.m_pLow[0] + src.m_pHigh[0]);
		dest.m_pHigh[1]	= src.m_pHigh[1];
		break;
	case 1:
		dest.m_pLow[0]	= 0.5 * (src.m_pLow[0] + src.m_pHigh[0]);
		dest.m_pLow[1]	= 0.5 * (src.m_pLow[1] + src.m_pHigh[1]);
		dest.m_pHigh[0]	= src.m_pHigh[0];
		dest.m_pHigh[1]	= src.m_pHigh[1];
		break;
	case 2:
		dest.m_pLow[0]	= src.m_pLow[0];
		dest.m_pLow[1]	= src.m_pLow[1];
		dest.m_pHigh[0] = 0.5 * (src.m_pLow[0] + src.m_pHigh[0]);
		dest.m_pHigh[1] = 0.5 * (src.m_pLow[1] + src.m_pHigh[1]);
		break;
	case 3:
		dest.m_pLow[0]	= 0.5 * (src.m_pLow[0] + src.m_pHigh[0]);
		dest.m_pLow[1]	= src.m_pLow[1];
		dest.m_pHigh[0]	= src.m_pHigh[0];
		dest.m_pHigh[1] = 0.5 * (src.m_pLow[1] + src.m_pHigh[1]);
		break;
	}
}

uint_32 SpatialMath::getPositionPosInRect(const Rect& parent, const MapPos2D& p)
{
	MapPos2D center(parent.center());
	assert(parent.containsPoint(p));
	if (p.d0 < center.d0)
	{
		if (p.d1 < center.d1)
			return 2;
		else
			return 0;
	}
	else
	{
		if (p.d1 < center.d1)
			return 3;
		else
			return 1;
	}
}

double Embedded::SpatialMath::calculateCourse( const Pos2D &stdPos , const Pos2D &endPos )
{
	double course=90-atan2((endPos.d1-stdPos.d1),(endPos.d0-stdPos.d0))/M_PI*180;
	if (course<0)
	{
		course+=360;
	}
	assert(course>=0&&course<=360);
	return course;
}