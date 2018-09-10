#include "LowNWQuadTree.h"
#include "LowNetwork.h"
#include "AdjacencyList.h"
#include "../Tools/SpatialMath.h"
#include <string.h>

using namespace Embedded;
using namespace Embedded::Network;

//
// class LowNWNode
//

LowNWNode::LowNWNode(const Rect& scope, uint_32 count, bool bLeaf)
	: m_scope(scope), m_topoNodeCount(count), m_bLeaf(bLeaf)
{
}

//
// class LowNWLeaf
//

LowNWLeaf::LowNWLeaf(const Rect& scope, uint_32 count)
	: LowNWNode(scope, count, true)
{
}

void LowNWLeaf::queryAdjacencyListID(const MapPos2D& p, const uint_8* data, std::vector<uint_32>* IDs)
{
	uint_32 currID;
	memcpy(&currID, data, GRID_ID_SIZE);
	data += GRID_ID_SIZE;

	uint_32 verSize = 0;
	memcpy(&verSize, data, PARTITION_SIZE);
	data += PARTITION_SIZE;
	for (uint_32 i = 0; i < verSize; ++i)
	{
		double verPartition;
		memcpy(&verPartition, data, sizeof(double));
		data += sizeof(double);

		uint_32 horSize = 0;
		memcpy(&horSize, data, PARTITION_SIZE);
		data += PARTITION_SIZE;

		if (p.d0 > verPartition)
		{
			data += horSize * sizeof(double);
			currID += (horSize + 1);
		}
		else
		{
			for (uint_32 j = 0; j < horSize; ++j)
			{
				double horPartition;
				memcpy(&horPartition, data, sizeof(double));
				data += sizeof(double);
				if (p.d1 > horPartition)
					++currID;
				else
				{
					IDs->push_back(currID);
					if (p.d1 == horPartition)
						IDs->push_back(currID + 1);
					currID += horSize - j;
					data += (horSize - j) * sizeof(double);
					break;
				}
			}

			IDs->push_back(currID);

			++currID;
			if (p.d0 != verPartition)
				return;
		}
	}

	uint_32 horSize = 0;
	memcpy(&horSize, data, PARTITION_SIZE);
	data += PARTITION_SIZE;
	for (uint_32 i = 0; i < horSize; ++i)
	{
		double horPartition;
		memcpy(&horPartition, data, sizeof(double));
		data += sizeof(double);
		if (p.d1 > horPartition)
			++currID;
		else
		{
			IDs->push_back(currID);
			if (p.d1 == horPartition)
				IDs->push_back(currID + 1);
			return;
		}
	}
	IDs->push_back(currID);
}

void LowNWLeaf::getAllAdjacencyListIDs(const uint_8* data, uint_32& startID, uint_32& num)
{
	startID = 0;
	memcpy(&startID, data, GRID_ID_SIZE);
	data += GRID_ID_SIZE;

	num = 0;
	uint_32 verSize = 0;
	memcpy(&verSize, data, PARTITION_SIZE);
	data += PARTITION_SIZE;
	for (uint_32 i = 0; i < verSize; ++i)
	{
		data += sizeof(double);

		uint_32 horSize = 0;
		memcpy(&horSize, data, PARTITION_SIZE);
		data += PARTITION_SIZE;
		num += horSize + 1;

		data += sizeof(double) * horSize;
	}

	uint_32 horSize = 0;
	memcpy(&horSize, data, PARTITION_SIZE);
	//data += PARTITION_SIZE;
	num += horSize + 1;
}

//
// class LowNWIndex
//

LowNWIndex::LowNWIndex(const Rect& scope, uint_32 count)
	: LowNWNode(scope, count, false)
{
}

LowNWIndex::~LowNWIndex()
{
	for (uint_32 i = 0; i < 4; ++i)
		delete m_child[i];
}

//
// class LowNWQuadTree
//

LowNWQuadTree::LowNWQuadTree(IStorageManager& sm)
	: m_network(new LowNetwork(&sm)), m_scope(m_network->m_scope)
{
	build();
}

LowNWQuadTree::~LowNWQuadTree()
{
	delete m_root;
	delete m_network;
}

void LowNWQuadTree::build()
{
	if (m_network->m_rootBeLeaf)
	{
		uint_8* data;
		uint_32 len;
		m_network->m_buffer->loadByteArray(m_network->m_firstLeafPage, len, &data);

		LowNWLeaf* l = new LowNWLeaf(m_network->m_scope, 0);
		LowNWLeaf::getAllAdjacencyListIDs(data + LOW_LEAF_COUNT_SIZE, l->m_startALPage, l->m_pageCount);
		delete[] data;

		l->m_topoNodeCount = 0;
		for (uint_32 i = 0; i < l->m_pageCount; ++i)
		{
			m_network->m_buffer->loadByteArray(l->m_startALPage + i, len, &data);
			l->m_topoNodeCount += AdjacencyList::fetchLowNodeCount(data);
			delete[] data;
		}

		m_root = l;
	}
	else
	{
		m_root = new LowNWIndex(m_network->m_scope, 0);
		std::queue<std::pair<LowNWIndex*, ADDR> > queIndexes;
		queIndexes.push(std::make_pair(dynamic_cast<LowNWIndex*>(m_root), std::make_pair(m_network->m_firstIndexPage, 0)));

		while (!queIndexes.empty())
		{
			std::pair<LowNWIndex*, ADDR> item = queIndexes.front(); queIndexes.pop();

			uint_8* data;
			uint_32 len;
			m_network->m_buffer->loadByteArray(item.second.first, len, &data);
			uint_8* ptr = data + item.second.second;

			Rect childScope(item.first->m_scope);
			for (uint_32 i = 0; i < 4; ++i)
			{
				SpatialMath::getRectChildGrid(item.first->m_scope, i, childScope);

				uint_32 id, count;
				memcpy(&id, ptr, LOW_PM_NODE_ID_SIZE);
				ptr += LOW_PM_NODE_ID_SIZE;
				memcpy(&count, ptr, LOW_PM_NODE_COUNT_SIZE);
				ptr += LOW_PM_NODE_COUNT_SIZE;

				ADDR addr;
				AdjacencyList::decodeTopoNodeID(id, &addr.first, &addr.second);

				if (addr.first < m_network->m_firstIndexPage)
				{
					if (addr.first == 0 && addr.second == 0)
					{
						LowNWLeaf* l = new LowNWLeaf(m_network->m_scope, 0);
						l->m_startALPage = 0;
						l->m_pageCount = 0;
						item.first->m_child[i] = l;
					}
					else
					{
						uint_8* data;
						uint_32 len;
						m_network->m_buffer->loadByteArray(addr.first, len, &data);
						LowNWLeaf* l = new LowNWLeaf(childScope, count);
						LowNWLeaf::getAllAdjacencyListIDs(data + addr.second, l->m_startALPage, l->m_pageCount);
						item.first->m_child[i] = l;
						delete[] data;
					}
				}
				else
				{
					LowNWIndex* tmp = new LowNWIndex(childScope, count);
					item.first->m_child[i] = tmp;
					queIndexes.push(std::make_pair(tmp, addr));
				}
			}

			delete[] data;
		}

		for (uint_32 i = 0; i < 4; ++i)
			m_root->m_topoNodeCount += dynamic_cast<LowNWIndex*>(m_root)->m_child[i]->m_topoNodeCount;
	}
}
