#include "AdjacencyList.h"
#include <string.h>

using namespace Embedded;
using namespace Embedded::Network;

const uint_32 AdjacencyList::OFFSET_SIZE_BIT				= 16;//static_cast<uint_32>(8 * 1.5);
const uint_32 AdjacencyList::GRID_SIZE_BIT				= sizeof(uint_32)*8-OFFSET_SIZE_BIT;//static_cast<uint_32>(8 * 2.5);

void AdjacencyList::decodeTopoEdgeID(uint_64 id, uint_32* layerID, id_type* nodeID, uint_32* offset)
{
	*layerID = static_cast<uint_32>((id & LAYER_ID_MASK) >> (GRID_ID_SIZE + DATA_OFFSET_SIZE) * 8);
	*nodeID = (id & GRID_ID_MASK) >> (DATA_OFFSET_SIZE * 8);
	*offset = static_cast<uint_32>(id & DATA_OFFSET_MASK);
}
uint_32 AdjacencyList::encodeTopoNodeID(uint_32 page, uint_32 offset)
{
	assert(page <= (1 << GRID_SIZE_BIT) - 1
		&& offset <= (1 << OFFSET_SIZE_BIT) - 1);
	return page << OFFSET_SIZE_BIT | offset;
}

void AdjacencyList::decodeTopoNodeID(uint_32 id, uint_32* page, uint_32* offset)
{
	*page = (id & TOPO_NODE_GAP) >> OFFSET_SIZE_BIT;
	*offset = (id << GRID_SIZE_BIT) >> GRID_SIZE_BIT;
}

uint_32 AdjacencyList::decodeTopoEdgeLength(uint_32 storage)
{
	if (storage == EDGE_LENGTH_ENDLESS)
		return EDGE_LENGTH_ENDLESS;
	else if (storage & EDGE_LENGTH_CARRY)
		return (storage ^ EDGE_LENGTH_CARRY) << EDGE_LENGTH_CONDENSE_BITS;
	else
		return storage;
}

uint_32 AdjacencyList::fetchLowNodeCount(const uint_8* pageStart)
{
	uint_32 c = 0;
	memcpy(&c, pageStart, L_DATA_COUNT_SIZE);
	return c;
}

uint_32 AdjacencyList::fetchLowConnectionSetID(const uint_8* itemStart)
{
	itemStart += L_COORD_SIZE * 2;

	uint_32 id = 0;
	memcpy(&id, itemStart, L_CONNECTION_ID_SIZE);

	return id;
}

void AdjacencyList::fetchLowCoordinate(const uint_8* itemStart, MapPos2D* pos)
{
	int x = 0, y = 0;
	memcpy(&x, itemStart, L_COORD_SIZE);
	itemStart += L_COORD_SIZE;
	memcpy(&y, itemStart, L_COORD_SIZE);

	pos->d0 = x;
	pos->d1 = y;
}

void AdjacencyList::fetchLowConnectionInfo(const uint_8* itemStart, std::vector<S_CONN>* conns)
{
	itemStart += L_COORD_SIZE * 2 + L_CONNECTION_ID_SIZE;

	uint_32 cEdge = 0;
	memcpy(&cEdge, itemStart, L_DATA_COUNT_SIZE);
	itemStart += L_DATA_COUNT_SIZE;

	conns->reserve(cEdge);
	for (uint_32 i = 0; i < cEdge; ++i)
	{
		S_CONN ct;
		memcpy(&ct.nodeID, itemStart, L_NODE_ID_SIZE);
		itemStart += L_NODE_ID_SIZE;
		memcpy(&ct.roadID, itemStart, L_EDGE_ID_SIZE);
		itemStart += L_EDGE_ID_SIZE;

		uint_32 len = 0;
		memcpy(&len, itemStart, L_EDGE_LENGTH_SIZE);
		itemStart += L_EDGE_LENGTH_SIZE;
		ct.length = decodeTopoEdgeLength(len);

		conns->push_back(ct);
	}
}

uint_32 AdjacencyList::computeLowDataOffset(const MapPos2D& p, uint_8* data)
{
	uint_8* start = data;
	uint_32 dataCount = 0;
	memcpy(&dataCount, data, L_DATA_COUNT_SIZE);
	data += L_DATA_COUNT_SIZE;

	int x, y;
	x = static_cast<int>(p.d0 + 0.5);
	y = static_cast<int>(p.d1 + 0.5);

	for (uint_32 i = 0; i < dataCount; ++i)
	{
		int cx, cy;
		memcpy(&cx, data, L_COORD_SIZE);
		data += L_COORD_SIZE;
		memcpy(&cy, data, L_COORD_SIZE);
		data += L_COORD_SIZE;

		if (cx == x && cy == y)
		{
			return data - start - L_COORD_SIZE * 2;
		}
		else
		{
			data += L_CONNECTION_ID_SIZE;
			uint_32 edgeCount = 0;
			memcpy(&edgeCount, data, L_DATA_COUNT_SIZE);
			data += L_DATA_COUNT_SIZE;
			data += edgeCount * L_CONNECTION_ITEM_SIZE;
		}
	}

	return 0;
}

void AdjacencyList::fetchHighConnectionInfo(const uint_8* itemStart, std::vector<M_CONN>* conns)
{
	itemStart += H_SINGLE_TOPO_NODE_ID_SIZE;

	uint_32 cEdge = 0;
	memcpy(&cEdge, itemStart, H_EDGE_COUNT_SIZE);
	itemStart += H_EDGE_COUNT_SIZE;

	conns->reserve(cEdge);
	for (uint_32 i = 0; i < cEdge; ++i)
	{
		M_CONN ct;
		memcpy(&ct.nodeID, itemStart, H_MULTI_TOPO_NODE_ID_SIZE);
		itemStart += H_MULTI_TOPO_NODE_ID_SIZE;
		memcpy(&ct.length, itemStart, H_EDGE_LENGTH_SIZE);
		itemStart += H_EDGE_LENGTH_SIZE;

		conns->push_back(ct);
	}
}

uint_32 AdjacencyList::fetchSingleTopoNodeID(const uint_8* itemStart)
{
	uint_32 ret = 0;
	memcpy(&ret, itemStart, H_SINGLE_TOPO_NODE_ID_SIZE);
	return ret;
}

bool AdjacencyList::isNodeInRect(const MapPos2D& p, const Rect& r)
{
	return p.d0 >= r.m_pLow[0] && p.d0 < r.m_pHigh[0]
	&& p.d1 >= r.m_pLow[1] && p.d1 < r.m_pHigh[1];
}
