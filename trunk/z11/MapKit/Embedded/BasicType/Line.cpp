#include "Line.h"
#include "Rect.h"
#include "../Tools/SpatialMath.h"
#include "../Application/AttributeMgr.h"
#include <string.h>
namespace Embedded
{
Line::Line(uint_32 attrAddr)
	: m_pCoords(NULL), m_posNum(0), m_attr(NULL), m_attrAddr(attrAddr)
{
}

Line::Line(const double* pCoords, uint_32 size, uint_32 attrAddr)
	: m_posNum(size), m_attr(NULL), m_attrAddr(attrAddr)
{
	initialize(pCoords);
}

Line::Line(const std::vector<MapPos2D>& line, uint_32 attrAddr)
	: m_posNum(line.size()), m_attr(NULL), m_attrAddr(attrAddr)
{
	assert(m_posNum > 1);
	initialize(line);
}

Line::Line(const Line& l)
	: m_posNum(l.m_posNum), m_attr(NULL), m_attrAddr(l.m_attrAddr)
{
	initialize(l.m_pCoords);
}

Line::~Line()
{
    delete_and_set_null(m_attr);
	delete[] m_pCoords;
}

Line& Line::operator=(const Line& l)
{
	if (this != &l)
	{
		m_attrAddr = l.m_attrAddr;
		checkRoom(l.m_posNum);
		m_posNum = l.m_posNum;
		memcpy(m_pCoords, l.m_pCoords, DIMENSION * sizeof(double) * m_posNum);
	}
	return *this;
}

bool Line::operator==(const Line& l) const
{
	if (l.m_posNum != m_posNum)
		return false;
	for (uint_32 i = 0; i < m_posNum; ++i)
	{
		if (
			m_pCoords[i * 2] < l.m_pCoords[i * 2] - DOUBLE_TOLERANCE ||
			m_pCoords[i * 2] > l.m_pCoords[i * 2] + DOUBLE_TOLERANCE ||
			m_pCoords[i * 2 + 1] < l.m_pCoords[i * 2 + 1] - DOUBLE_TOLERANCE ||
			m_pCoords[i * 2 + 1] > l.m_pCoords[i * 2 + 1] + DOUBLE_TOLERANCE
			)
			return false;
	}
	return true;
}

//
// IObject interface
//
Line* Line::clone()
{
	return new Line(*this);
}

//
// ISerializable interface
//

uint_32 Line::getByteArraySize() const
{
	return
		sizeof(uint_8) +							// type
		sizeof(uint_32) +						// attrAddr
		sizeof(uint_32) +						// pos num
		sizeof(double) * DIMENSION * m_posNum;
}

void Line::loadFromByteArray(const uint_8* data)
{
	data += sizeof(uint_8);	// type
	memcpy(&m_attrAddr, data, sizeof(uint_32));
	data += sizeof(uint_32);
	memcpy(&m_posNum, data, sizeof(uint_32));
	data += sizeof(uint_32);
	initialize();
	memcpy(m_pCoords, data, sizeof(double) * DIMENSION * m_posNum);
}

void Line::storeToByteArray(uint_8** data, uint_32& length) const
{
	length = getByteArraySize();
	*data = new uint_8[length];
	uint_8* ptr = *data;

	ShapeType type = ST_LINE;
	memcpy(ptr, &type, sizeof(uint_8));
	ptr += sizeof(uint_8);
	memcpy(ptr, &m_attrAddr, sizeof(uint_32));
	ptr += sizeof(uint_32);
	memcpy(ptr, &m_posNum, sizeof(uint_32));
	ptr += sizeof(uint_32);
	memcpy(ptr, m_pCoords, sizeof(double) * DIMENSION * m_posNum);
	// ptr += sizeof(double) * DIMENSION * m_posNum;

}

//
// IShape interface
//

bool Line::intersectsShape(const IShape& s) const
{
	const Rect* pr = dynamic_cast<const Rect*>(&s);
	if (pr != 0)
		return pr->intersectsLine(*this);

	//const Point* ppt = dynamic_cast<const Point*>(&s);
	//if (ppt != 0)
	//	return intersectsPoint(*ppt);

	//const Line* pl = dynamic_cast<const Line*>(&s);
	//if (pl != 0)
	//	return intersectsLine(*pl);

	throw IllegalStateException(
		"Line::intersectsShape: Not implemented yet!"
		);
}

bool Line::containsShape(const IShape& in) const
{
	throw NotSupportedException("Have no time to finish it now. ");
}

bool Line::touchesShape(const IShape& in) const
{
	throw NotSupportedException("Have no time to finish it now. ");
}

void Line::getCenter(Point& out) const
{
	throw NotSupportedException("Have no time to finish it now. ");
}

void Line::getMBR(Rect& out) const
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
}

double Line::getArea() const
{
	throw NotSupportedException("Have no time to finish it now. ");
}

double Line::getMinimumDistance(const IShape& in) const
{
	throw NotSupportedException("Have no time to finish it now. ");
}

void Line::initialize(uint_32 posNum, const double* pCoords)
{
	if (m_posNum != 0)
		throw IllegalStateException("Line::initialize: m_posNum != 0, only use to empty Line. ");
	m_posNum = posNum;
	initialize(pCoords);
}

void Line::getBothEndsPoint(MapPos2D* s, MapPos2D* e) const
{
	getPoint(0, s);
	getPoint(m_posNum - 1, e);
}

void Line::getPoint(uint_32 index, MapPos2D* point) const
{
	if (index >= m_posNum)
		throw IndexOutOfBoundsException(index);
	point->d0 = m_pCoords[index * 2];
	point->d1 = m_pCoords[index * 2 + 1];
}

void Line::getPoints(std::vector<MapPos2D>* points) const
{
	/*points->reserve(m_posNum);
	for (uint_32 i = 0; i < m_posNum; ++i)
		points->push_back(MapPos2D(m_pCoords[i * 2], m_pCoords[i * 2 + 1]));*/
	assert(sizeof(MapPos2D) == 2 * sizeof(double));
	points->resize(m_posNum);
	memcpy(&points->at(0), m_pCoords, m_posNum*sizeof(MapPos2D));
}
void Line::getPoints(std::vector<ScreenPos2D> *points) const
{
	points->clear();
	points->reserve(m_posNum);
	for (uint_32 i = 0; i < m_posNum; ++i)
		points->push_back(ScreenPos2D((long)m_pCoords[i * 2], (long)m_pCoords[i * 2 + 1]));
}

void Line::moveControlPoi(uint_32 pointIndex, const MapPos2D& newPos)
{
	if (pointIndex >= m_posNum)
		throw IndexOutOfBoundsException(pointIndex);
	m_pCoords[pointIndex * 2] = newPos.d0;
	m_pCoords[pointIndex * 2 + 1] = newPos.d1;
}

double Line::getLength() const
{
	double ret = 0.0;
	for (uint_32 i = 0; i < m_posNum - 1; ++i)
	{
		ret += SpatialMath::getDistance(
			m_pCoords[i * 2], m_pCoords[i * 2 + 1], m_pCoords[i * 2 + 2], m_pCoords[i * 2 + 3]
		);
	}
	return ret;
}

const z11::MGWString& Line::getName() const
{
	if(!m_attr)
	{
		Attribute* attr = 0;
		if(m_attrAddr < MAXVALIDATTRADDRESS)
			AttributeMgr::getInstance()->getLineAttribute(m_attrAddr, &attr);
		else
			attr = new Attribute();
        Attribute** ptr = (Attribute**)(&m_attr);
		*ptr = attr;
	}
	return m_attr->getName();
}

//void Line::getAttribute()
//{
//	if(!m_attr)
//		m_attr = new Attribute();
//	if(_attrSource && m_attrAddr < MAXVALIDATTRADDRESS)
//	{
//		id_type page = m_attrAddr >> 12;
//		uint_32 leng, offset = m_attrAddr & 0x00000fff;
//		uint_8* data;
//		_attrSource->loadByteArray(page, leng, &data);
//		m_attr->loadFromBuf(data+offset);
//		delete[] data;
//	}
//
//}

void Line::initialize(const double* pCoords)
{
	initialize();
	memcpy(m_pCoords, pCoords, DIMENSION * sizeof(double) * m_posNum);
}

void Line::initialize(const std::vector<MapPos2D>& line)
{
	initialize();
	memcpy(m_pCoords, &line.front(), DIMENSION * sizeof(double) * m_posNum);
}

void Line::initialize()
{
	try
	{
		m_pCoords = new double[DIMENSION * m_posNum];
	}
	catch (...)
	{
		delete[] m_pCoords;
		throw;
	}
}

void Line::checkRoom(uint_32 size)
{
	if (m_posNum != size)
	{
		delete[] m_pCoords;
		m_pCoords = new double[DIMENSION * size];
	}
}

//IStorageManager* Line::_attrSource = NULL;
}
