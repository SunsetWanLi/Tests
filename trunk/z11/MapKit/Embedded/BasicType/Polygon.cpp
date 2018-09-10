#include "Polygon.h"
#include "Rect.h"
#include <string.h>

namespace Embedded
{

Polygon::Polygon()
	: m_posNum(0), m_pCoords(NULL)
{
}

Polygon::Polygon(uint_32 posNum, const double* pCoords)
	: m_posNum(posNum)
{
	m_pCoords = new double[m_posNum * DIMENSION];
	memcpy(m_pCoords, pCoords, DIMENSION * sizeof(double) * m_posNum);
}

Polygon::Polygon(const Polygon& l)
	: m_posNum(l.m_posNum)
{
	m_pCoords = new double[m_posNum * DIMENSION];
	memcpy(m_pCoords, l.m_pCoords, DIMENSION * sizeof(double) * m_posNum);
	m_attr = l.m_attr;
}

Polygon::~Polygon()
{
	delete[] m_pCoords;
}

Polygon& Polygon::operator=(const Polygon& l)
{
	if (this != &l)
	{
		if(m_posNum != l.m_posNum)
		{
			m_posNum = l.m_posNum;
			delete [] m_pCoords;
			m_pCoords = new double[DIMENSION*m_posNum];
		}
		memcpy(m_pCoords, l.m_pCoords, DIMENSION * sizeof(double) * m_posNum);
		m_attr = l.m_attr;
	}
	return *this;
}

bool Polygon::operator==(const Polygon& l) const
{

	if (m_posNum != l.m_posNum)
		return false;

	uint_32 cursor = 0;
	for (uint_32 j = 0; j < l.m_posNum; ++j)
	{
		if (
			m_pCoords[cursor * 2] < l.m_pCoords[cursor * 2] - DOUBLE_TOLERANCE ||
			m_pCoords[cursor * 2] > l.m_pCoords[cursor * 2] + DOUBLE_TOLERANCE ||
			m_pCoords[cursor * 2 + 1] < l.m_pCoords[cursor * 2 + 1] - DOUBLE_TOLERANCE ||
			m_pCoords[cursor * 2 + 1] > l.m_pCoords[cursor * 2 + 1] + DOUBLE_TOLERANCE
			)
			return false;
		++cursor;
	}

	return true;
}

//
// IObject interface
//
Polygon* Polygon::clone()
{
	return new Polygon(*this);
}

//
// ISerializable interface
//
uint_32 Polygon::getByteArraySize() const
{
	return sizeof(uint_8) +                 //type ST_POLYGON
		sizeof(uint_32)				  //m_posNum
		+ sizeof(double) * DIMENSION * m_posNum;

}

void Polygon::loadFromByteArray(const uint_8* data)
{
	data += sizeof(uint_8);	// type
	memcpy(&m_posNum, data, sizeof(uint_32));
	data += sizeof(uint_32);
	if(m_pCoords) delete[] m_pCoords;
	m_pCoords = new double[ m_posNum*DIMENSION ];
	memcpy(m_pCoords, data, m_posNum*DIMENSION*sizeof(double));
	// data += tmpSize;
}

void Polygon::storeToByteArray(uint_8** data, uint_32& length) const
{
	length = getByteArraySize();
	*data = new uint_8[length];
	uint_8* ptr = *data;

	ShapeType type = ST_POLYGON;
	memcpy(ptr, &type, sizeof(uint_8));
	ptr += sizeof(uint_8);

	memcpy(ptr, &m_posNum, sizeof(uint_32));
	ptr += sizeof(uint_32);
	uint_32 tmpSize = sizeof(double) * DIMENSION *  m_posNum;
	memcpy(ptr, m_pCoords, tmpSize);
	// ptr += tmpSize;
}

void Polygon::getPoints(std::vector<MapPos2D>* points) const
{
	/*points->reserve(m_posNum);
	for (uint_32 i = 0; i < m_posNum; ++i)
		points->push_back(MapPos2D(m_pCoords[i * 2], m_pCoords[i * 2 + 1]));*/
	assert(sizeof(MapPos2D) == 2 * sizeof(double));
	points->resize(m_posNum);
	memcpy(&points->at(0), m_pCoords, m_posNum*sizeof(MapPos2D));

}

void Polygon::getPoints(std::vector<ScreenPos2D> *points) const
{
	points->reserve(m_posNum);
	for (uint_32 i = 0; i < m_posNum; ++i)
		points->push_back(ScreenPos2D((long)m_pCoords[i * 2], (long)m_pCoords[i * 2 + 1]));
}

//
// IShape interface
//
bool Polygon::intersectsShape(const IShape& in) const
{
	throw NotSupportedException("Have no time to finish it now. ");
}

bool Polygon::containsShape(const IShape& in) const
{
	throw NotSupportedException("Have no time to finish it now. ");
}

bool Polygon::touchesShape(const IShape& in) const
{
	throw NotSupportedException("Have no time to finish it now. ");
}

void Polygon::getCenter(Point& out) const
{
	throw NotSupportedException("Have no time to finish it now. ");
}

void Polygon::getMBR(Rect& out) const
{
	out.makeInfinite();

	for (uint_32 i = 0; i < m_posNum; ++i)
	{
		uint_32 iX = i * 2, iY = i * 2 + 1;

		if (m_pCoords[iX] > out.m_pHigh[0])
			out.m_pHigh[0] = m_pCoords[iX];
		if (m_pCoords[iX] < out.m_pLow[0])
			out.m_pLow[0] = m_pCoords[iX];

		if (m_pCoords[iY] > out.m_pHigh[1])
			out.m_pHigh[1] = m_pCoords[iY];
		if (m_pCoords[iY] < out.m_pLow[1])
			out.m_pLow[1] = m_pCoords[iY];
	}

	//throw Tools::NotSupportedException("Have no time to finish it now. ");
}

double Polygon::getArea() const
{
	throw NotSupportedException("Have no time to finish it now. ");
}

double Polygon::getMinimumDistance(const IShape& in) const
{
	throw NotSupportedException("Have no time to finish it now. ");
}



uint_32 Polygon::posNum() const
{
	return m_posNum;
}

}
