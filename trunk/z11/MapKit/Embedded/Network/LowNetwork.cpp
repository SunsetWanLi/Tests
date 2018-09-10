#include "LowNetwork.h"
#include "BindPoint.h"
#include "LowNWSearching.h"
#include "AdjacencyList.h"
#include "LowNWQuadTree.h"
#include <string.h>

using namespace Embedded;
using namespace Embedded::Network;

LowNetwork::LowNetwork(IStorageManager* sm, const IPointBindLine* bind,
							 const ILookUp* table, bool bPreLoadTable)
	: m_buffer(sm), m_bind(bind), m_table(table), m_bPreLoadTable(bPreLoadTable)
{
	m_scope.makeInfinite();
	parseHeader();
}

LowNetwork::LowNetwork(IStorageManager* sm)
	: m_buffer(sm), m_bind(NULL), m_table(NULL), m_bPreLoadTable(false)
{
	m_scope.makeInfinite();
	parseHeader();
}

double LowNetwork::computeShortestPath(BetweenTwoNodeSearchingInfo& info) const
{
	LowNWSearching search(this, m_table, m_bPreLoadTable);

	std::vector<uint_32> routeNodeIDs;
	double dis = search.queryShortestRoute(info.m_start, info.m_end, &routeNodeIDs, info.m_knownDis);

	parseRouteResult(routeNodeIDs, info);

	return dis;
}

double LowNetwork::computeShortestPath(SearchingInfo& info) const
{
	BindPoint s(info.m_start.m_realPos, m_bind, this),
		e(info.m_end.m_realPos, m_bind, this);
	info.m_start.m_bindPos = s.m_pos;
	info.m_end.m_bindPos = e.m_pos;
	std::vector<uint_32> routeNodeIDs;

	LowNWSearching search(this, m_table, m_bPreLoadTable);
	double dis = search.queryShortestRoute(s, e, &routeNodeIDs);

	parseRouteResult(routeNodeIDs, s, e, info);

	return dis;
}

uint_32 LowNetwork::queryTopoNodeID(const MapPos2D& p) const
{
	std::vector<uint_32> IDs;
	retrieveIndex(p, &IDs);

	for (uint_32 i = 0; i < IDs.size(); ++i)
	{
		uint_8* data;
		uint_32 len;
		m_buffer->loadByteArray(IDs.at(i), len, &data);

		uint_32 offset = AdjacencyList::computeLowDataOffset(p, data);
		delete[] data;
		if (offset != 0)
			return AdjacencyList::encodeTopoNodeID(IDs.at(i), offset);
	}

	assert(0);
	throw IllegalStateException("LowNetwork::queryTopoNodeID: Can't find pos.");
}

void LowNetwork::getTopoNodePos(uint_32 nodeID, MapPos2D* pos) const
{
	uint_32 page, offset;
	AdjacencyList::decodeTopoNodeID(nodeID, &page, &offset);

	uint_8* data;
	uint_32 len;
	m_buffer->loadByteArray(page, len, &data);

	uint_8* ptr = data + offset;
	AdjacencyList::fetchLowCoordinate(ptr, pos);

	delete[] data;
}

uint_32 LowNetwork::getConnectionSetID(uint_32 nodeID) const
{
	uint_32 page, offset;
	AdjacencyList::decodeTopoNodeID(nodeID, &page, &offset);

	uint_8* data;
	uint_32 len;
	m_buffer->loadByteArray(page, len, &data);

	uint_8* ptr = data + offset;
	uint_32 id = AdjacencyList::fetchLowConnectionSetID(ptr);

	delete[] data;
	return id;
}

void LowNetwork::getTopoNodeConnection(uint_32 nodeID, std::vector<S_CONN>* conns,
										  bool bUsedInMLSearch /*= false*/) const
{
	uint_32 page, offset;
	AdjacencyList::decodeTopoNodeID(nodeID, &page, &offset);

	uint_8* data;
	uint_32 len;
	m_buffer->loadByteArray(page, len, &data);

	uint_8* ptr = data + offset;
	AdjacencyList::fetchLowConnectionInfo(ptr, conns);

	delete[] data;

	if (!bUsedInMLSearch)
	{
		for (uint_32 i = 0; i < conns->size(); ++i)
			AdjacencyList::eraseNodeIDFlag((*conns)[i].nodeID);
	}
}

void LowNetwork::parseHeader()
{
	uint_8* data;
	uint_32 len;
	m_buffer->loadByteArray(0, len, &data);
	uint_8* ptr = data;

	memcpy(m_scope.m_pLow, ptr, sizeof(double) * 2);
	ptr += sizeof(double) * 2;
	memcpy(m_scope.m_pHigh, ptr, sizeof(double) * 2);
	ptr += sizeof(double) * 2;

	uint_32 bLeaf = 0;
	memcpy(&bLeaf, ptr, sizeof(uint_32));
	ptr += sizeof(uint_32);
	m_rootBeLeaf = bLeaf == 0 ? false : true;

	memcpy(&m_firstLeafPage, ptr, sizeof(uint_32));
	ptr += sizeof(uint_32);
	memcpy(&m_firstIndexPage, ptr, sizeof(uint_32));
	ptr += sizeof(uint_32);

	delete[] data;
}

void LowNetwork::parseRouteResult(const std::vector<uint_32>& nodeIDs,
									 BetweenTwoNodeSearchingInfo& info) const
{
	if (nodeIDs.size() < 2)
		return;

	uint_32 iCurrID = nodeIDs.size() - 1;
	do
	{
		uint_32 sNodeID = nodeIDs[iCurrID],
			eNodeID = nodeIDs[iCurrID - 1];

		if (sNodeID == eNodeID)
		{
			if (nodeIDs.size() == 2)
				break;
			else
				assert(0);
		}

		std::vector<S_CONN> conns;
		getTopoNodeConnection(sNodeID, &conns);
		double dis = DOUBLE_MAX;
		uint_64 roadID;
		for (uint_32 i = 0; i < conns.size(); ++i)
		{
			const S_CONN& ct = conns[i];
			if (ct.nodeID != eNodeID)
				continue;
			else if (ct.length < dis)
			{
				dis = ct.length;
				roadID = ct.roadID;
			}
		}

		assert(dis != DOUBLE_MAX);
		info.m_roadIDs.push_back(roadID);

		--iCurrID;

	} while (iCurrID != 0);
}

void LowNetwork::parseRouteResult(const std::vector<uint_32>& nodeIDs, const BindPoint& s, const BindPoint& e,
									 SearchingInfo& info) const
{
	if (nodeIDs.size() <= 1)
	{
		if (s.m_roadID == e.m_roadID)
			info.m_roadIDs.push_back(s.m_roadID);
		return;
	}

	assert(nodeIDs.size() > 1);

	assert(nodeIDs.front() == e.m_beginID || nodeIDs.front() == e.m_endID
		&& nodeIDs.back() == s.m_beginID || nodeIDs.back() == s.m_endID);
	if (nodeIDs.front() == e.m_beginID)
		info.m_end.m_bDir = true;
	else
		info.m_end.m_bDir = false;
	if (nodeIDs.back() == s.m_beginID)
		info.m_start.m_bDir = true;
	else
		info.m_start.m_bDir = false;

	info.m_roadIDs.push_back(s.m_roadID);

	uint_32 iCurrID = nodeIDs.size() - 1;
	while (iCurrID != 0)
	{
		uint_32 sNodeID = nodeIDs[iCurrID],
			eNodeID = nodeIDs[iCurrID - 1];

		if (sNodeID == eNodeID)
		{
			if (nodeIDs.size() == 2)
				break;
			else
				assert(0);
		}

		std::vector<S_CONN> conns;
		getTopoNodeConnection(sNodeID, &conns);
		double dis = DOUBLE_MAX;
		uint_64 roadID;
		for (uint_32 i = 0; i < conns.size(); ++i)
		{
			const S_CONN& ct = conns[i];
			if (ct.nodeID != eNodeID)
				continue;
			else if (ct.length < dis)
			{
				dis = ct.length;
				roadID = ct.roadID;
			}
		}

		assert(dis != DOUBLE_MAX);
		info.m_roadIDs.push_back(roadID);

		--iCurrID;
	}

	info.m_roadIDs.push_back(e.m_roadID);
}

void LowNetwork::retrieveIndex(const MapPos2D& p, std::vector<uint_32>* IDs) const
{
	if (m_rootBeLeaf)
		return retrieveLeaf(p, m_firstLeafPage, LOW_LEAF_COUNT_SIZE, m_scope, IDs);

	uint_8* data;
	uint_32 len;
	m_buffer->loadByteArray(m_firstIndexPage, len, &data);
	uint_8* ptr = data;

	Rect scope = m_scope;
	while (true)
	{
		MapPos2D center(scope.center());
		uint_32 position;
		if (p.d0 < center.d0)
		{
			if (p.d1 < center.d1)
			{
				position = 2;
				scope.m_pHigh[0] = center.d0;
				scope.m_pHigh[1] = center.d1;
			}
			else
			{
				position = 0;
				scope.m_pLow[1] = center.d1;
				scope.m_pHigh[0] = center.d0;
			}
		}
		else
		{
			if (p.d1 < center.d1)
			{
				position = 3;
				scope.m_pLow[0] = center.d0;
				scope.m_pHigh[1] = center.d1;
			}
			else
			{
				position = 1;
				scope.m_pLow[0] = center.d0;
				scope.m_pLow[1] = center.d1;
			}
		}

		ptr += (LOW_PM_NODE_ID_SIZE + LOW_PM_NODE_COUNT_SIZE) * position;
		uint_32 indexID;
		memcpy(&indexID, ptr, LOW_PM_NODE_ID_SIZE);
		//ptr += sizeof(LOW_PM_NODE_ID_SIZE);
		uint_32 test_size;
		ptr += sizeof(LOW_PM_NODE_ID_SIZE);
		memcpy(&test_size, ptr, LOW_PM_NODE_COUNT_SIZE);


		delete[] data;

		uint_32 page, offset;
		AdjacencyList::decodeTopoNodeID(indexID, &page, &offset);

		if (page < m_firstIndexPage)
			return retrieveLeaf(p, page, offset, scope, IDs);
		else
		{
			m_buffer->loadByteArray(page, len, &data);
			ptr = data + offset;
		}
	}

	delete[] data;
	assert(0);
	throw IllegalStateException("LowNetwork::retrieveIndex: Can't find leaf.");
}

void LowNetwork::retrieveLeaf(const MapPos2D& p, uint_32 page, uint_32 offset, const Rect& s, std::vector<uint_32>* IDs) const
{
	assert(s.containsPoint(p) || s.touchesPoint(p));

	uint_8* data;
	uint_32 len;
	m_buffer->loadByteArray(page, len, &data);
	uint_8* ptr = data + offset;

	LowNWLeaf::queryAdjacencyListID(p, ptr, IDs);

	delete[] data;
}
