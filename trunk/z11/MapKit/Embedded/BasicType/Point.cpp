#include "Point.h"
#include "Line.h"
#include "Rect.h"
#include "../Tools/SpatialMath.h"
#include <string.h>

using namespace Embedded;

Point::Point()
	: m_pCoords(NULL)
{
}

Point::Point(const MapPos2D& p)
{
	double coords[2];
	coords[0] = p.d0;
	coords[1] = p.d1;
	initialize(coords);
}

Point::Point(const double* pCoords)
{
	initialize(pCoords);
}

Point::Point(const Point& p)
{
	initialize(p.m_pCoords);
}

Point::~Point()
{
	delete[] m_pCoords;
}

Point& Point::operator=(const Point& p)
{
	if (this != &p)
		memcpy(m_pCoords, p.m_pCoords, DIMENSION * sizeof(double));
	return *this;
}

bool Point::operator==(const Point& p) const
{
	for(uint_32 i = 0; i < DIMENSION; ++i)
	{
		if (
			m_pCoords[i] < p.m_pCoords[i] - DOUBLE_TOLERANCE ||
			m_pCoords[i] > p.m_pCoords[i] + DOUBLE_TOLERANCE
			)
			return false;
	}
	return true;
}

//
// IObject interface
//
Point* Point::clone()
{
	return new Point(*this);
}

//
// ISerializable interface
//
uint_32 Point::getByteArraySize() const
{
	return DIMENSION * sizeof(double);
}

void Point::loadFromByteArray(const uint_8* data)
{
	if (NULL == m_pCoords)
	{
		try
		{
			m_pCoords = new double[DIMENSION];
		}
		catch (...)
		{
			delete[] m_pCoords;
			throw;
		}
	}
	memcpy(m_pCoords, data, getByteArraySize());
}

void Point::storeToByteArray(uint_8** data, uint_32& length) const
{
	length = getByteArraySize();
	*data = new uint_8[length];
	memcpy(*data, m_pCoords, length);
}

//
// IShape interface
//

bool Point::intersectsShape(const IShape& s) const
{
	const Rect* pr = dynamic_cast<const Rect*>(&s);
	if (pr != 0)
		return pr->containsPoint(*this);

	const Point* pp = dynamic_cast<const Point*>(&s);
	if (pp != 0)
		return *pp == *this;

	throw IllegalStateException(
		"Point::intersectsShape: Not implemented yet!"
		);
}

bool Point::containsShape(const IShape& s) const
{
	return false;
}

bool Point::touchesShape(const IShape& s) const
{
	const Point* pp = dynamic_cast<const Point*>(&s);
	if (pp != 0)
		return *this == *pp;

	const Rect* pr = dynamic_cast<const Rect*>(&s);
	if (pr != 0)
		return pr->touchesPoint(*this);

	throw IllegalStateException(
		"Point::touchesShape: Not implemented yet!"
		);
}

void Point::getCenter(Point& p) const
{
	p = *this;
}

void Point::getMBR(Rect& out) const
{
	out = Rect(m_pCoords, m_pCoords);
}

double Point::getArea() const
{
	return 0.0;
}

double Point::getMinimumDistance(const IShape& s) const
{
	const Point* pp = dynamic_cast<const Point*>(&s);
	if (pp != 0)
		return getMinimumDistance(*pp);

	const Rect* pr = dynamic_cast<const Rect*>(&s);
	if (pr != 0)
		return pr->getMinimumDistance(*this);

	const Line* pl = dynamic_cast<const Line*>(&s);
	if (pl)
		return getMinimumDistance(*pl);

	throw IllegalStateException(
		"Point::getMinimumDistance: Not implemented yet!"
		);
}

double Point::getMinimumDistance(const Point& p) const
{
	double ret = 0.0;

	for (uint_32 i = 0; i < DIMENSION; ++i)
		ret += std::pow(m_pCoords[i] - p.m_pCoords[i], 2.0);

	return std::sqrt(ret);
}

double Point::getMinimumDistance(const Line& l) const
{
	std::vector<MapPos2D> ctlpos;
	l.getPoints(&ctlpos);
	MapPos2D point(m_pCoords[0], m_pCoords[1]);

	MapPos2D nearest;
	SpatialMath::getNearestPosOnLineToPoint(point, ctlpos, &nearest);

	return SpatialMath::getDistance(point, nearest);
}

double Point::getCoordinate(uint_32 index) const
{
	if (index < 0 || index >= DIMENSION)
		throw IndexOutOfBoundsException(index);
	return m_pCoords[index];
}

void Point::getPoint(Embedded::MapPos2D *pos) const
{
	assert(pos && m_pCoords);
	pos->d0 = m_pCoords[0];
	pos->d1 = m_pCoords[1];
}

void Point::getPoint(ScreenPos2D *pos) const
{
	assert(pos && m_pCoords);
	pos->d0 = m_pCoords[0];
	pos->d1 = m_pCoords[1];
}

void Point::initialize(const double* pCoords)
{
	try
	{
		m_pCoords = new double[DIMENSION];
	}
	catch (...)
	{
		delete[] m_pCoords;
		throw;
	}

	memcpy(m_pCoords, pCoords, DIMENSION * sizeof(double));
}

void Point::initialize()
{
	if (isEmpty())
	{
		try
		{
			m_pCoords = new double[DIMENSION];
		}
		catch (...)
		{
			delete[] m_pCoords;
			throw;
		}
	}
}
