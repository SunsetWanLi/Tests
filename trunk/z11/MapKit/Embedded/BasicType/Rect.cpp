#include "Rect.h"
#include "Point.h"
#include "Line.h"
#include "../Tools/SpatialMath.h"
#include <string.h>

using namespace Embedded;

Rect::Rect()
	: m_pLow(NULL), m_pHigh(NULL)
{
}

Rect::Rect(const double* pLow, const double* pHigh)
{
	initialize(pLow, pHigh);
}

Rect::Rect(const Point& low, const Point& high)
{
	initialize(low.m_pCoords, high.m_pCoords);
}

Rect::Rect(const Point& center, const double& edge)
{
	initialize(center.m_pCoords, center.m_pCoords);
	double halfEdge = edge * 0.5;
	m_pLow[0]  = center.m_pCoords[0] - halfEdge;
	m_pLow[1]  = center.m_pCoords[1] - halfEdge;
	m_pHigh[0] = center.m_pCoords[0] + halfEdge;
	m_pHigh[1] = center.m_pCoords[1] + halfEdge;
}

Rect::Rect(const Rect& rect)
{
	initialize(rect.m_pLow, rect.m_pHigh);
}

Rect::Rect(const MapPos2D& leftLow, const MapPos2D& rightTop)
{
	initialize(leftLow, rightTop);
}

Rect::~Rect()
{
	delete[] m_pLow;
	delete[] m_pHigh;
}

Rect& Rect::operator=(const Rect& rect)
{
	if (this != &rect)
	{
		initialize();
		memcpy(m_pLow, rect.m_pLow, sizeof(double) * DIMENSION);
		memcpy(m_pHigh, rect.m_pHigh, sizeof(double) * DIMENSION);
	}

	return *this;
}

bool Rect::operator==(const Rect& rect) const
{
	for(uint_32 i = 0; i < DIMENSION; ++i)
	{
		if (
			m_pLow[i] < rect.m_pLow[i] - DOUBLE_TOLERANCE ||
			m_pLow[i] > rect.m_pLow[i] + DOUBLE_TOLERANCE ||
			m_pHigh[i] < rect.m_pHigh[i] - DOUBLE_TOLERANCE ||
			m_pHigh[i] > rect.m_pHigh[i] + DOUBLE_TOLERANCE
			)
			return false;
	}
	return true;
}

//
// IObject interface
//
Rect* Rect::clone()
{
	return new Rect(*this);
}

//
// ISerializable interface
//
uint_32 Rect::getByteArraySize() const
{
	return sizeof(double) * DIMENSION * 2;
}

void Rect::loadFromByteArray(const uint_8* data)
{
	initialize();
	memcpy(m_pLow, data, sizeof(double) * DIMENSION);
	data += sizeof(double) * DIMENSION;
	memcpy(m_pHigh, data, sizeof(double) * DIMENSION);
}

void Rect::storeToByteArray(uint_8** data, uint_32& length) const
{
	length = getByteArraySize();
	*data = new uint_8[length];
	uint_8* ptr = *data;

	memcpy(ptr, m_pLow, sizeof(double) * DIMENSION);
	ptr += sizeof(double) * DIMENSION;
	memcpy(ptr, m_pHigh, sizeof(double) * DIMENSION);
}

//
// IShape interface
//
bool Rect::intersectsShape(const IShape& s) const
{
	const Rect* pr = dynamic_cast<const Rect*>(&s);
	if (pr != 0)
		return intersectsRect(*pr);

	const Point* ppt = dynamic_cast<const Point*>(&s);
	if (ppt != 0)
		return containsPoint(*ppt);

	const Line* pl = dynamic_cast<const Line*>(&s);
	if (pl != 0)
		return intersectsLine(*pl);

	throw IllegalStateException(
		"Rect::intersectsShape: Not implemented yet!"
		);
}

bool Rect::containsShape(const IShape& s) const
{
	const Rect* pr = dynamic_cast<const Rect*>(&s);
	if (pr != 0) return containsRect(*pr);

	const Point* ppt = dynamic_cast<const Point*>(&s);
	if (ppt != 0) return containsPoint(*ppt);

	const Line* pl = dynamic_cast<const Line*>(&s);
	if (pl != 0) return containsLine(*pl);

	throw IllegalStateException(
		"Rect::containsShape: Not implemented yet!"
		);
}

bool Rect::touchesShape(const IShape& s) const
{
	const Rect* pr = dynamic_cast<const Rect*>(&s);
	if (pr != 0) return touchesRect(*pr);

	const Point* ppt = dynamic_cast<const Point*>(&s);
	if (ppt != 0) return touchesPoint(*ppt);

	throw IllegalStateException(
		"Rect::touchesShape: Not implemented yet!"
		);
}

void Rect::getCenter(Point& p) const
{
	p.initialize();
	for (uint_32 i = 0; i < DIMENSION; ++i)
		p.m_pCoords[i] = (m_pLow[i] + m_pHigh[i]) / 2.0;
}

void Rect::getMBR(Rect& r) const
{
	r = *this;
}

double Rect::getArea() const
{
	double area = 1.0;

	for (uint_32 i = 0; i < DIMENSION; ++i)
		area *= m_pHigh[i] - m_pLow[i];

	return area;
}

double Rect::getMinimumDistance(const IShape& s) const
{
	const Rect* pr = dynamic_cast<const Rect*>(&s);
	if (pr != 0)
		return getMinimumDistance(*pr);

	const Point* pp = dynamic_cast<const Point*>(&s);
	if (pp != 0)
		return getMinimumDistance(*pp);

	throw IllegalStateException(
		"Rect::getMinimumDistance: Not implemented yet!"
		);
}

bool Rect::intersectsRect(const Rect& r) const
{
	for (uint_32 i = 0; i < DIMENSION; ++i)
		if (m_pLow[i] > r.m_pHigh[i] || m_pHigh[i] < r.m_pLow[i])
			return false;
	return true;
}

bool Rect::intersectsLine(const Line& l) const
{
	uint_32 iLx0 = 0, iLy0 = 1;
	for (uint_32 i = 1; i < l.m_posNum; ++i)
	{
		uint_32 iLx1 = i * 2, iLy1 = i * 2 + 1;
		if (SpatialMath::isSegmentIntersectRect(
			l.m_pCoords[iLx0], l.m_pCoords[iLy0], l.m_pCoords[iLx1], l.m_pCoords[iLy1],
			m_pLow[0], m_pLow[1], m_pHigh[0], m_pHigh[1])
			)
			return true;
		iLx0 = iLx1; iLy0 = iLy1;
	}
	return false;
}

bool Rect::containsRect(const Rect& r) const
{
	for(uint_32 i = 0; i < DIMENSION; ++i)
		if (m_pLow[i] > r.m_pLow[i] || m_pHigh[i] < r.m_pHigh[i])
			return false;
	return true;
}

bool Rect::containsLine(const Line& l) const
{
	for (uint_32 i = 0; i < l.m_posNum; ++i)
	{
		uint_32 iX = i * 2, iY = i * 2 + 1;
		if (l.m_pCoords[iX] < m_pLow[0] || l.m_pCoords[iX] > m_pHigh[0]
		|| l.m_pCoords[iY] < m_pLow[1] || l.m_pCoords[iY] > m_pHigh[1])
			return false;
	}
	return true;
}

bool Rect::touchesRect(const Rect& r) const
{
	for (uint_32 i = 0; i < DIMENSION; ++i)
	{
		if (
			(m_pLow[i] >= r.m_pLow[i] - DOUBLE_TOLERANCE &&
			m_pLow[i] <= r.m_pLow[i] + DOUBLE_TOLERANCE) ||
			(m_pHigh[i] >= r.m_pHigh[i] - DOUBLE_TOLERANCE &&
			m_pHigh[i] <= r.m_pHigh[i] + DOUBLE_TOLERANCE)
			)
			return true;
	}
	return false;
}

double Rect::getMinimumDistance(const Rect& r) const
{
	double ret = 0.0;

	for (uint_32 i = 0; i < DIMENSION; ++i)
	{
		double x = 0.0;

		if (r.m_pHigh[i] < m_pLow[i])
			x = std::fabs(r.m_pHigh[i] - m_pLow[i]);
		else if (m_pHigh[i] < r.m_pLow[i])
			x = std::fabs(r.m_pLow[i] - m_pHigh[i]);

		ret += x * x;
	}

	return std::sqrt(ret);
}

bool Rect::containsPoint(const Point& p) const
{
	for(uint_32 i = 0; i < DIMENSION; ++i)
		if (p.getCoordinate(i) < m_pLow[i] || p.getCoordinate(i) > m_pHigh[i])
			return false;
	return true;
}

bool Rect::containsPoint(const MapPos2D& p) const
{
	if (p.d0 < m_pLow[0] || p.d0 > m_pHigh[0]
	 || p.d1 < m_pLow[1] || p.d1 > m_pHigh[1])
		 return false;
	return true;
}

bool Rect::touchesPoint(const Point& p) const
{
	for (uint_32 i = 0; i < DIMENSION; ++i)
	{
		if (
			(m_pLow[i] >= p.getCoordinate(i) - DOUBLE_TOLERANCE &&
			m_pLow[i] <= p.getCoordinate(i) + DOUBLE_TOLERANCE) ||
			(m_pHigh[i] >= p.getCoordinate(i) - DOUBLE_TOLERANCE &&
			m_pHigh[i] <= p.getCoordinate(i) + DOUBLE_TOLERANCE)
			)
			return true;
	}
	return false;
}

bool Rect::touchesPoint(const MapPos2D& p) const
{
	if (
		fabs(p.d0 - m_pLow[0]) <= DOUBLE_TOLERANCE ||
		fabs(p.d0 - m_pHigh[0]) <= DOUBLE_TOLERANCE ||
		fabs(p.d1 - m_pLow[1]) <= DOUBLE_TOLERANCE ||
		fabs(p.d1 - m_pHigh[1]) <= DOUBLE_TOLERANCE
		)
		return true;
	return false;
}

double Rect::getMinimumDistance(const Point& p) const
{
	double ret = 0.0;

	for (uint_32 i = 0; i < DIMENSION; ++i)
	{
		if (p.getCoordinate(i) < m_pLow[i])
			ret += std::pow(m_pLow[i] - p.getCoordinate(i), 2.0);
		else if (p.getCoordinate(i) > m_pHigh[i])
			ret += std::pow(p.getCoordinate(i) - m_pHigh[i], 2.0);
	}

	return std::sqrt(ret);
}

Rect Rect::getIntersectingRect(const Rect& r) const
{
	Rect ret;
	ret.makeInfinite();

	// check for intersection.
	// marioh: avoid function call since this is called billions of times.
	for (uint_32 cDim = 0; cDim < DIMENSION; cDim++)
	{
		if (m_pLow[cDim] > r.m_pHigh[cDim] || m_pHigh[cDim] < r.m_pLow[cDim])
			return ret;
	}

	for (uint_32 cDim = 0; cDim < DIMENSION; cDim++)
	{
		ret.m_pLow[cDim] = std::max(m_pLow[cDim], r.m_pLow[cDim]);
		ret.m_pHigh[cDim] = std::min(m_pHigh[cDim], r.m_pHigh[cDim]);
	}

	return ret;
}

double Rect::getIntersectingArea(const Rect& r) const
{
	double ret = 1.0;
	double f1, f2;

	for (uint_32 cDim = 0; cDim < DIMENSION; cDim++)
	{
		if (m_pLow[cDim] > r.m_pHigh[cDim] || m_pHigh[cDim] < r.m_pLow[cDim])
			return 0.0;

		f1 = std::max(m_pLow[cDim], r.m_pLow[cDim]);
		f2 = std::min(m_pHigh[cDim], r.m_pHigh[cDim]);
		ret *= f2 - f1;
	}

	return ret;
}

/*
* Returns the margin of a region. It is calcuated as the sum of  2^(d-1) * width, in each dimension.
* It is actually the sum of all edges, no matter what the dimensionality is.
*/
double Rect::getMargin() const
{
	double mul = std::pow(2.0, static_cast<double>(DIMENSION) - 1.0);
	double margin = 0.0;

	for (uint_32 i = 0; i < DIMENSION; i++)
	{
		margin += (m_pHigh[i] - m_pLow[i]) * mul;
	}

	return margin;
}

void Rect::combineRect(const Rect& r)
{
	if (isEmpty())
		makeInfinite();

	for (uint_32 cDim = 0; cDim < DIMENSION; cDim++)
	{
		m_pLow[cDim] = std::min(m_pLow[cDim], r.m_pLow[cDim]);
		m_pHigh[cDim] = std::max(m_pHigh[cDim], r.m_pHigh[cDim]);
	}
}

void Rect::combinePoint(const Point& p)
{
	for (uint_32 cDim = 0; cDim < DIMENSION; cDim++)
	{
		m_pLow[cDim] = std::min(m_pLow[cDim], p.m_pCoords[cDim]);
		m_pHigh[cDim] = std::max(m_pHigh[cDim], p.m_pCoords[cDim]);
	}
}

void Rect::combinePoint(const MapPos2D& pos)
{
	double tmp[2];
	tmp[0] = pos.d0;
	tmp[1] = pos.d1;

	combinePoint(Point(tmp));
}

void Rect::getCombinedRect(Rect& out, const Rect& in) const
{
	out = *this;
	out.combineRect(in);
}

void Rect::makeInfinite()
{
	initialize();
	for (uint_32 cIndex = 0; cIndex < DIMENSION; cIndex++)
	{
		m_pLow[cIndex] = DOUBLE_MAX;
		m_pHigh[cIndex] = -DOUBLE_MAX;
	}
}

void Rect::initialize(const double* pLow, const double* pHigh)
{
#ifndef NDEBUG
	for(uint_32 i = 0; i < DIMENSION; ++i)
	{
		if (pLow[i] > pHigh[i])
		{
			throw IllegalArgumentException(
				"Rect::initialize: Low point has larger coordinates than High point."
				);
		}
	}
#endif

	try
	{
		m_pLow = new double[DIMENSION];
		m_pHigh = new double[DIMENSION];
	}
	catch (...)
	{
		delete[] m_pLow;
		delete[] m_pHigh;
		throw;
	}

	memcpy(m_pLow, pLow, sizeof(double) * DIMENSION);
	memcpy(m_pHigh, pHigh, sizeof(double) * DIMENSION);
}

void Rect::initialize(const MapPos2D& leftLow, const MapPos2D& rightTop)
{
	double low[2], high[2];
	low[0] = leftLow.d0;
	low[1] = leftLow.d1;
	high[0] = rightTop.d0;
	high[1] = rightTop.d1;
	initialize(low, high);
}

void Rect::changeSize(const double& ratio)
{
	if (ratio < 0.0)
	{
		throw IllegalArgumentException(
			"Rect::changeSize: ratio should larger then 0. "
			);
	}

	if (!isEmpty())
	{
		for (uint_32 i = 0; i < DIMENSION; ++i)
		{
			double oldExtent     = m_pHigh[i] - m_pLow[i],
				   halfNewExtent = oldExtent * ratio * 0.5;
			m_pLow[i]  = m_pLow[i]  + 0.5 * oldExtent - halfNewExtent;
			m_pHigh[i] = m_pHigh[i] - 0.5 * oldExtent + halfNewExtent;
		}
	}
}

void Rect::changeSize(const double& xChange, const double& yChange)
{
	m_pLow[0]  -= xChange;
	m_pHigh[0] += xChange;
	m_pLow[1]  -= yChange;
	m_pHigh[1] += yChange;
}

void Rect::initialize()
{
	if (isEmpty())
	{
		try
		{
			m_pLow = new double[DIMENSION];
			m_pHigh = new double[DIMENSION];
		}
		catch (...)
		{
			delete[] m_pLow;
			delete[] m_pHigh;
			throw;
		}
	}
}
