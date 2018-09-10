#include "QuadtreeTable.h"
#include "LookUpTable.h"
#include "../Tools/SpatialMath.h"
#include <string.h>
#include "System/Shared/MGLog.h"
using namespace Embedded;
using namespace Embedded::Network;

QuadTreeTable::QuadTreeTable(IStorageManager& storage, uint_32 pageSize)
	: m_buffer(&storage), m_pageSize(pageSize)
{
	loadHeader();
}

QuadTreeTable::~QuadTreeTable()
{
	delete m_root;
}

double QuadTreeTable::queryMinDistance(const MapPos2D& sNodePos, uint_32 dGridID) const
{
	return LookUpTable::queryMinDistance(m_buffer, m_dataStartPage, m_gridNum,
		getGridID(sNodePos), dGridID, m_pageSize);
}

void QuadTreeTable::loadTable(uint_32 destGridID, uint_8** data) const
{
	return LookUpTable::loadTable(m_buffer, m_dataStartPage, m_gridNum, destGridID, data, m_pageSize);
}

double QuadTreeTable::queryMinDistance(const MapPos2D& src, const uint_8* table) const
{
	return LookUpTable::queryMinDistance(getGridID(src), table);
}

uint_32 QuadTreeTable::getGridID(const MapPos2D& pos) const
{
	Node* curr = m_root;
	MapScope scope(m_mapScope);
	while (true)
	{
		assert(SpatialMath::isPointInRect(pos, scope));
		if (curr->isLeaf())
			return curr->m_id;
		else
		{
			double cx = 0.5 * (scope.m_xMin + scope.m_xMax),
				cy = 0.5 * (scope.m_yMin + scope.m_yMax);
			if (pos.d0 < cx)
			{
				if (pos.d1 < cy)
				{
					curr = curr->m_child[2];
					scope.m_xMax = cx; scope.m_yMax = cy;
				}
				else
				{
					curr = curr->m_child[0];
					scope.m_xMax = cx; scope.m_yMin = cy;
				}
			}
			else
			{
				if (pos.d1 < cy)
				{
					curr = curr->m_child[3];
					scope.m_xMin = cx; scope.m_yMax = cy;
				}
				else
				{
					curr = curr->m_child[1];
					scope.m_xMin = cx; scope.m_yMin = cy;
				}
			}
		}
	}

	assert(0);
	throw IllegalStateException("QuadTreeTable::getGridID: Didn't find.");
	return UINT32_MAX_1;
}

//
// class QuadTreeTable::Node
//

QuadTreeTable::Node::Node(uint_8** ptr)
{
	uint_32 id = 0;
	memcpy(&id, *ptr, QuadTreeTable::NODE_ID_SIZE);
	*ptr += QuadTreeTable::NODE_ID_SIZE;

	if (id == QuadTreeTable::NODE_ID_ENDLESS)
	{
		//non_leaf_num ++;
		m_id = UINT32_MAX_1;
		for (uint_32 i = 0; i < 4; ++i)
			m_child[i] = new Node(ptr);
	}
	else
	{
		//all_leaf_num ++;
		m_id = id;
		for (uint_32 i = 0; i < 4; ++i)
			m_child[i] = NULL;
	}
}

QuadTreeTable::Node::~Node()
{
	for (uint_32 i = 0; i < 4; ++i)
		delete m_child[i];
}

//
// Internal
//

void QuadTreeTable::loadHeader()
{
	uint_32 headerSize;
	uint_8* header = 0;
	m_buffer->loadByteArray(HEADER_PAGE, headerSize, &header);

	uint_8* ptr = header;

	memcpy(&m_gridNum, ptr, sizeof(uint_32));
	ptr += sizeof(uint_32);
	memcpy(&m_mapScope.m_xMin, ptr, sizeof(double));
	ptr += sizeof(double);
	memcpy(&m_mapScope.m_yMin, ptr, sizeof(double));
	ptr += sizeof(double);
	memcpy(&m_mapScope.m_xMax, ptr, sizeof(double));
	ptr += sizeof(double);
	memcpy(&m_mapScope.m_yMax, ptr, sizeof(double));
	ptr += sizeof(double);
	memcpy(&m_dataStartPage, ptr, sizeof(uint_32));
	ptr += sizeof(uint_32);

	delete[] header;

	uint_32 indexSize = ( m_dataStartPage - 1 ) * m_pageSize;
	uint_8* indexData = new uint_8[indexSize];
	ptr = indexData;
	for (uint_32 i = 1; i < m_dataStartPage; ++i)
	{
		uint_32 pageSize=0;
		uint_8* pageData = 0;
		m_buffer->loadByteArray(i, pageSize, &pageData);

		uint_8* ptrSrc = pageData;
		z11::MGLogD("page size=%d %d",pageSize,m_pageSize);
		memcpy(ptr, ptrSrc, m_pageSize);
		ptr += m_pageSize;

		delete[] pageData;
	}

	ptr = indexData;
	m_root = new Node(&ptr);

	//int non = Node::non_leaf_num;
	//int leaf = Node::all_leaf_num;
	//int all = (ptr-indexData)/NODE_ID_SIZE;

	delete[] indexData;
}

//int QuadTreeTable::Node::non_leaf_num = 0;
//int QuadTreeTable::Node::all_leaf_num = 0;
