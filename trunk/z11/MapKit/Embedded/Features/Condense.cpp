#include "Condense.h"
#include "../BasicType/Point.h"
#include "../BasicType/Line.h"
#include "../BasicType/Polygon.h"
#include <string.h>

using namespace Embedded;
using namespace Embedded::Features;

//
// class NoOffsetCondenseStrategy
//

uint_32 NoOffsetCondenseStrategy::allShapeDataSize(const INode* n) const
{
	uint_32 ret = 0;

	if (n->isIndex())
		return ret;

	for (uint_32 cChild = 0; cChild < n->getChildrenCount(); cChild++)
	{
		IShape* s;
		n->getChildShape(cChild, &s);
		ret += dataSize(s);
		delete s;
	}

	return ret;
}

uint_32 NoOffsetCondenseStrategy::dataSize(const IShape* s) const
{
	throw NotSupportedException("Should Never be called");
	const Point* point = dynamic_cast<const Point*>(s);
	if (point != NULL)
		return 2 * COORDS_SIZE;

	const Line* line = dynamic_cast<const Line*>(s);
	if (line != NULL)
		return TYPE_SIZE +ATTR_ADDR_SIZE+ POINT_COUNT_SIZE + line->posNum() * 2 * COORDS_SIZE;

	const Polygon* poly = dynamic_cast<const Polygon*>(s);
	if (poly != NULL)
		return /*TYPE_SIZE +*/ poly->posNum() * 2 * COORDS_SIZE + POINT_COUNT_SIZE ;

	throw NotSupportedException(
		"NoOffsetCondenseStrategy::dataSize: new type. "
		);
}

void NoOffsetCondenseStrategy::loadFromByteArray(IShape** s, const uint_8* data, uint_32 length) const
{
	const uint_8* ptr = data;
	if(m_shapeType == ST_POINT)
	{
		int x, y;
		memcpy(&x, ptr, COORDS_SIZE);
		ptr += COORDS_SIZE;
		memcpy(&y, ptr, COORDS_SIZE);
		ptr += COORDS_SIZE;

		double coords[2];
		coords[0] = x;
		coords[1] = y;
		Point* point = new Point(coords);
		point->m_attr.loadFromBuf(ptr);
		*s = point;
		return;

	}
	else if(m_shapeType == ST_LINE)
	{
		ShapeType shapeType = ST_POINT;
		memcpy(&shapeType, ptr, TYPE_SIZE);
		ptr += TYPE_SIZE;

		uint_32 attrAddr;
		memcpy(&attrAddr, ptr, ATTR_ADDR_SIZE);
		ptr += ATTR_ADDR_SIZE;

		Line* line = new Line(attrAddr);

		memcpy(&line->m_posNum, ptr, POINT_COUNT_SIZE);
		ptr += POINT_COUNT_SIZE;

		try
		{
			line->m_pCoords = new double[Line::DIMENSION * line->m_posNum];
		}
		catch (...)
		{
			delete[] line->m_pCoords;
			throw;
		}

		for (uint_32 i = 0; i < line->m_posNum; ++i)
		{
			int x, y;
			memcpy(&x, ptr, COORDS_SIZE);
			ptr += COORDS_SIZE;
			memcpy(&y, ptr, COORDS_SIZE);
			ptr += COORDS_SIZE;

			line->m_pCoords[i * 2]		= x;
			line->m_pCoords[i * 2 + 1]	= y;
		}

		assert(ptr - data == length);
		*s = line;
		return;

	}
	else if(m_shapeType == ST_POLYGON)
	{
		Polygon* poly = new Polygon();

		unsigned short posNum;
		memcpy(&posNum, ptr, POINT_COUNT_SIZE);
		ptr += POINT_COUNT_SIZE;
		poly->m_posNum = posNum;


		try
		{
			poly->m_pCoords = new double[posNum * Polygon::DIMENSION];
		}
		catch (...)
		{
			delete[] poly->m_pCoords;
			throw;
		}

		uint_32 cursor = 0;
		for (uint_32 j = 0; j < posNum; ++j)
		{
			int x, y;
			memcpy(&x, ptr, COORDS_SIZE);
			ptr += COORDS_SIZE;
			memcpy(&y, ptr, COORDS_SIZE);
			ptr += COORDS_SIZE;

			poly->m_pCoords[cursor * 2]		= x;
			poly->m_pCoords[cursor * 2 + 1] = y;

			++cursor;
		}

		//assert(ptr - data == length);
		poly->m_attr.loadFromBuf(ptr);
		*s = poly;
		return;

	}
	else
	{
		assert(0);
	}

	return;
	//const uint_8* ptr = data;

	//if (length == COORDS_SIZE * 2)
	//{
	//	int x, y;
	//	memcpy(&x, ptr, COORDS_SIZE);
	//	ptr += COORDS_SIZE;
	//	memcpy(&y, ptr, COORDS_SIZE);
	//	//ptr += COORDS_SIZE;

	//	double coords[2];
	//	coords[0] = x;
	//	coords[1] = y;
	//	*s = new Point(coords);
	//
	//	return;
	//}

	//if (length == COORDS_SIZE * 3)
	//{
	//	throw IllegalStateException("Should never be called.");
	//}

	//ShapeType shapeType = ST_POINT;
	//memcpy(&shapeType, ptr, TYPE_SIZE);
	//ptr += TYPE_SIZE;

	//if (shapeType == ST_LINE)
	//{
	//	uint_32 attrAddr;
	//	memcpy(&attrAddr, ptr, ATTR_ADDR_SIZE);
	//	ptr += ATTR_ADDR_SIZE;

	//	Line* line = new Line(attrAddr);
	//	memcpy(&line->m_posNum, ptr, POINT_COUNT_SIZE);
	//	ptr += POINT_COUNT_SIZE;

	//	try
	//	{
	//		line->m_pCoords = new double[Line::DIMENSION * line->m_posNum];
	//	}
	//	catch (...)
	//	{
	//		delete[] line->m_pCoords;
	//		throw;
	//	}

	//	for (uint_32 i = 0; i < line->m_posNum; ++i)
	//	{
	//		int x, y;
	//		memcpy(&x, ptr, COORDS_SIZE);
	//		ptr += COORDS_SIZE;
	//		memcpy(&y, ptr, COORDS_SIZE);
	//		ptr += COORDS_SIZE;

	//		line->m_pCoords[i * 2]		= x;
	//		line->m_pCoords[i * 2 + 1]	= y;
	//	}

	//	assert(ptr - data == length);
	//	*s = line;
	//	return;
	//}
	//else if (shapeType == ST_POLYGON)
	//{
	//	Polygon* poly = new Polygon();

	//	poly->m_ringNum = 0;
	//	memcpy(&poly->m_ringNum, ptr, RING_COUNT_SIZE);
	//	ptr += RING_COUNT_SIZE;

	//	try
	//	{
	//		poly->m_posNum = new uint_32[poly->m_ringNum];
	//	}
	//	catch (...)
	//	{
	//		delete[] poly->m_posNum;
	//		throw;
	//	}

	//	uint_32 posCount = 0;
	//	for (uint_32 i = 0; i < poly->m_ringNum; ++i)
	//	{
	//		poly->m_posNum[i] = 0;
	//		memcpy(&poly->m_posNum[i], ptr, POINT_COUNT_SIZE);
	//		ptr += POINT_COUNT_SIZE;
	//		posCount += poly->m_posNum[i];
	//	}

	//	try
	//	{
	//		poly->m_pCoords = new double[posCount * Polygon::DIMENSION];
	//	}
	//	catch (...)
	//	{
	//		delete[] poly->m_pCoords;
	//		throw;
	//	}

	//	uint_32 cursor = 0;
	//	for (uint_32 i = 0; i < poly->m_ringNum; ++i)
	//	{
	//		for (uint_32 j = 0; j < poly->m_posNum[i]; ++j)
	//		{
	//			int x, y;
	//			memcpy(&x, ptr, COORDS_SIZE);
	//			ptr += COORDS_SIZE;
	//			memcpy(&y, ptr, COORDS_SIZE);
	//			ptr += COORDS_SIZE;

	//			poly->m_pCoords[cursor * 2]		= x;
	//			poly->m_pCoords[cursor * 2 + 1] = y;

	//			++cursor;
	//		}
	//	}

	//	assert(ptr - data == length);
	//	*s = poly;
	//	return;
	//}

	//assert(0);
	//throw NotSupportedException(
	//	"NoOffsetCondenseStrategy::loadFromByteArray: new type. "
	//	);
}

void NoOffsetCondenseStrategy::storeToByteArray(const IShape* s, uint_8** data, uint_32& length) const
{
	throw NotSupportedException("Should Never be called");

	/*length = dataSize(s);
	*data = new uint_8[length];
	uint_8* ptr = *data;

	if(m_shapeType == ST_POINT)
	{
		const Point* point = dynamic_cast<const Point*>(s);
		assert(point);
		int x = static_cast<int>(point->m_pCoords[0] + 0.5),
			y = static_cast<int>(point->m_pCoords[1] + 0.5);
		memcpy(ptr, &x, COORDS_SIZE);
		ptr += COORDS_SIZE;
		memcpy(ptr, &y, COORDS_SIZE);
		ptr += COORDS_SIZE;
		point->m_attr.storeToByteArray(ptr, point->m_attr.getByteArraySize());
		return;

	}
	else if(m_shapeType == ST_LINE)
	{
		const Line* line = dynamic_cast<const Line*>(s);
		assert(line);
		uint_8 type = ST_LINE;
		memcpy(ptr, &type, TYPE_SIZE);
		ptr += TYPE_SIZE;

		memcpy(ptr, &line->m_attrAddr, ATTR_ADDR_SIZE);
		ptr += ATTR_ADDR_SIZE;

		assert(line->m_posNum <= MAX_POINT_COUNT);
		memcpy(ptr, &line->m_posNum, POINT_COUNT_SIZE);
		ptr += POINT_COUNT_SIZE;

		for (uint_32 i = 0; i < line->m_posNum; ++i)
		{
			int x = static_cast<int>(line->m_pCoords[i * 2] + 0.5),
				y = static_cast<int>(line->m_pCoords[i * 2 + 1] + 0.5);
			memcpy(ptr, &x, COORDS_SIZE);
			ptr += COORDS_SIZE;
			memcpy(ptr, &y, COORDS_SIZE);
			ptr += COORDS_SIZE;
		}

		assert(ptr - *data == length);
		return;

	}
	else if(m_shapeType == ST_POLYGON)
	{
		const Polygon* poly = dynamic_cast<const Polygon*>(s);
		assert(poly);
		uint_8 type = ST_POLYGON;
		memcpy(ptr, &type, TYPE_SIZE);
		ptr += TYPE_SIZE;

		assert(poly->m_ringNum <= MAX_RING_COUNT);
		unsigned char ringNum = poly->m_ringNum;
		memcpy(ptr, &ringNum, RING_COUNT_SIZE);
		ptr += RING_COUNT_SIZE;

		for (uint_32 i = 0; i < poly->m_ringNum; ++i)
		{
			assert(poly->m_posNum[i] <= MAX_POINT_COUNT);
			unsigned short posNum = poly->m_posNum[i];
			memcpy(ptr, &posNum, POINT_COUNT_SIZE);
			ptr += POINT_COUNT_SIZE;
		}

		uint_32 cursor = 0;
		for (uint_32 i = 0; i < poly->m_ringNum; ++i)
		{
			for (uint_32 j = 0; j < poly->m_posNum[i]; ++j)
			{
				int x = static_cast<int>(poly->m_pCoords[cursor * 2] + 0.5),
					y = static_cast<int>(poly->m_pCoords[cursor * 2 + 1] + 0.5);
				memcpy(ptr, &x, COORDS_SIZE);
				ptr += COORDS_SIZE;
				memcpy(ptr, &y, COORDS_SIZE);
				ptr += COORDS_SIZE;
				++cursor;
			}
		}

		assert(ptr - *data == length);
		return;

	}
	else
	{
		assert(0);
	}
	throw Tools::NotSupportedException(
		"NoOffsetCondenseStrategy::storeToByteArray: new type. "
		);*/

}

////
//// class CertaintyOffsetCondenseStrategy
////
//
//uint_32 CertaintyOffsetCondenseStrategy::allShapeDataSize(const INode* n) const
//{
//	uint_32 ret = 0;
//
//	if (n->isIndex())
//		return ret;
//
//	for (uint_32 cChild = 0; cChild < n->getChildrenCount(); cChild++)
//	{
//		IShape* s;
//		n->getChildShape(cChild, &s);
//		ret += dataSize(s);
//		delete s;
//	}
//
//	return ret;
//}
//
//uint_32 CertaintyOffsetCondenseStrategy::dataSize(const IShape* s) const
//{
//	const Point* point = dynamic_cast<const Point*>(s);
//	if (point != NULL)
//		return 2 * COORDS_SIZE;
//
//	const Line* line = dynamic_cast<const Line*>(s);
//	if (line != NULL)
//		return TYPE_SIZE + POINT_COUNT_SIZE + line->posNum() * 2 * COORDS_SIZE;
//
//	const Polygon* poly = dynamic_cast<const Polygon*>(s);
//	if (poly != NULL)
//		return TYPE_SIZE + poly->posNum() * 2 * POINT_COUNT_SIZE + RING_COUNT_SIZE + POINT_COUNT_SIZE * poly->m_ringNum;
//
//	throw NotSupportedException(
//		"NoOffsetCondenseStrategy::dataSize: new type. "
//		);
//}
//
//void CertaintyOffsetCondenseStrategy::loadFromByteArray(IShape** s, const uint_8* data, uint_32 length) const
//{
//	const uint_8* ptr = data;
//
//	if (length == COORDS_SIZE * 2)
//	{
//		int x, y;
//		memcpy(&x, ptr, COORDS_SIZE);
//		ptr += COORDS_SIZE;
//		memcpy(&y, ptr, COORDS_SIZE);
//		//ptr += COORDS_SIZE;
//
//		double coords[2];
//		coords[0] = x;
//		coords[1] = y;
//		*s = new Point(coords);
//
//		return;
//	}
//
//	if (length == COORDS_SIZE * 3)
//	{
//		throw IllegalStateException("Should never be called.");
//	}
//
//	ShapeType shapeType = ST_POINT;
//	memcpy(&shapeType, ptr, TYPE_SIZE);
//	ptr += TYPE_SIZE;
//
//	if (shapeType == ST_LINE)
//	{
//		Line* line = new Line();
//
//		memcpy(&line->m_posNum, ptr, POINT_COUNT_SIZE);
//		ptr += POINT_COUNT_SIZE;
//
//		try
//		{
//			line->m_pCoords = new double[Line::DIMENSION * line->m_posNum];
//		}
//		catch (...)
//		{
//			delete[] line->m_pCoords;
//			throw;
//		}
//
//		for (uint_32 i = 0; i < line->m_posNum; ++i)
//		{
//			int x, y;
//			memcpy(&x, ptr, COORDS_SIZE);
//			ptr += COORDS_SIZE;
//			memcpy(&y, ptr, COORDS_SIZE);
//			ptr += COORDS_SIZE;
//
//			line->m_pCoords[i * 2]		= x;
//			line->m_pCoords[i * 2 + 1]	= y;
//		}
//
//		assert(ptr - data == length);
//		*s = line;
//		return;
//	}
//	else if (shapeType == ST_POLYGON)
//	{
//		Polygon* poly = new Polygon();
//
//		poly->m_ringNum = 0;
//		memcpy(&poly->m_ringNum, ptr, RING_COUNT_SIZE);
//		ptr += RING_COUNT_SIZE;
//
//		try
//		{
//			poly->m_posNum = new uint_32[poly->m_ringNum];
//		}
//		catch (...)
//		{
//			delete[] poly->m_posNum;
//			throw;
//		}
//
//		uint_32 posCount = 0;
//		for (uint_32 i = 0; i < poly->m_ringNum; ++i)
//		{
//			poly->m_posNum[i] = 0;
//			memcpy(&poly->m_posNum[i], ptr, POINT_COUNT_SIZE);
//			ptr += POINT_COUNT_SIZE;
//			posCount += poly->m_posNum[i];
//		}
//
//		try
//		{
//			poly->m_pCoords = new double[posCount * Polygon::DIMENSION];
//		}
//		catch (...)
//		{
//			delete[] poly->m_pCoords;
//			throw;
//		}
//
//		uint_32 cursor = 0;
//		for (uint_32 i = 0; i < poly->m_ringNum; ++i)
//		{
//			for (uint_32 j = 0; j < poly->m_posNum[i]; ++j)
//			{
//				int x, y;
//				memcpy(&x, ptr, COORDS_SIZE);
//				ptr += COORDS_SIZE;
//				memcpy(&y, ptr, COORDS_SIZE);
//				ptr += COORDS_SIZE;
//
//				poly->m_pCoords[cursor * 2]		= x;
//				poly->m_pCoords[cursor * 2 + 1] = y;
//
//				++cursor;
//			}
//		}
//
//		assert(ptr - data == length);
//		*s = poly;
//		return;
//	}
//
//	assert(0);
//	throw NotSupportedException(
//		"NoOffsetCondenseStrategy::loadFromByteArray: new type. "
//		);
//}
//
//void CertaintyOffsetCondenseStrategy::storeToByteArray(const IShape* s, uint_8** data, uint_32& length) const
//{
//	length = dataSize(s);
//	*data = new uint_8[length];
//	uint_8* ptr = *data;
//
//	const Point* point = dynamic_cast<const Point*>(s);
//	if (point != NULL)
//	{
//		int x = static_cast<int>(point->m_pCoords[0] + 0.5),
//			y = static_cast<int>(point->m_pCoords[1] + 0.5);
//		memcpy(ptr, &x, COORDS_SIZE);
//		ptr += COORDS_SIZE;
//		memcpy(ptr, &y, COORDS_SIZE);
//		//ptr += COORDS_SIZE;
//		return;
//	}
//
//	const Line* line = dynamic_cast<const Line*>(s);
//	if (line != NULL)
//	{
//		uint_8 type = ST_LINE;
//		memcpy(ptr, &type, TYPE_SIZE);
//		ptr += TYPE_SIZE;
//
//		assert(line->m_posNum <= MAX_POINT_COUNT);
//		memcpy(ptr, &line->m_posNum, POINT_COUNT_SIZE);
//		ptr += POINT_COUNT_SIZE;
//
//		for (uint_32 i = 0; i < line->m_posNum; ++i)
//		{
//			int x = static_cast<int>(line->m_pCoords[i * 2] + 0.5),
//				y = static_cast<int>(line->m_pCoords[i * 2 + 1] + 0.5);
//			memcpy(ptr, &x, COORDS_SIZE);
//			ptr += COORDS_SIZE;
//			memcpy(ptr, &y, COORDS_SIZE);
//			ptr += COORDS_SIZE;
//		}
//
//		assert(ptr - *data == length);
//		return;
//	}
//
//	const Polygon* poly = dynamic_cast<const Polygon*>(s);
//	if (poly != NULL)
//	{
//		uint_8 type = ST_POLYGON;
//		memcpy(ptr, &type, TYPE_SIZE);
//		ptr += TYPE_SIZE;
//
//		assert(poly->m_ringNum <= RING_COUNT_SIZE);
//		memcpy(ptr, &poly->m_ringNum, RING_COUNT_SIZE);
//		ptr += RING_COUNT_SIZE;
//
//		for (uint_32 i = 0; i < poly->m_ringNum; ++i)
//		{
//			assert(poly->m_posNum[i] <= POINT_COUNT_SIZE);
//			memcpy(ptr, &poly->m_posNum[i], POINT_COUNT_SIZE);
//			ptr += POINT_COUNT_SIZE;
//		}
//
//		uint_32 cursor = 0;
//		for (uint_32 i = 0; i < poly->m_ringNum; ++i)
//		{
//			for (uint_32 j = 0; j < poly->m_posNum[i]; ++j)
//			{
//				int x = static_cast<int>(poly->m_pCoords[cursor * 2] + 0.5),
//					y = static_cast<int>(poly->m_pCoords[cursor * 2 + 1] + 0.5);
//				memcpy(ptr, &x, COORDS_SIZE);
//				ptr += COORDS_SIZE;
//				memcpy(ptr, &y, COORDS_SIZE);
//				ptr += COORDS_SIZE;
//				++cursor;
//			}
//		}
//
//		assert(ptr - *data == length);
//		return;
//	}
//
//	throw NotSupportedException(
//		"NoOffsetCondenseStrategy::storeToByteArray: new type. "
//		);
//}
