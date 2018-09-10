#include "HighNetwork.h"
#include "LowNetwork.h"
#include "BindPoint.h"
#include "HighNWSearching.h"
#include <string.h>
#include "System/Shared/MGLog.h"
using namespace Embedded;
using namespace Embedded::Network;

typedef uint_32 LOW_NODE_ID;
typedef uint_32 HIGH_NODE_ID;
typedef uint_32 ADDRESS;

static const uint_32 BEGIN_INDEX_PAGE	= 1;
// Entry
static const uint_32 E_START_LAYER_SIZE	= 2;
// layer1 index
static const uint_32 L1_PAGE_COUNT		= 1;
static const uint_32 L1_ITEM_SIZE		= sizeof(LOW_NODE_ID);
static const uint_32 L1_END_FLAG			= 0xffffffff;
// layer2 index
static const uint_32 L2_ITEM_SIZE		= sizeof(LOW_NODE_ID) + sizeof(ADDRESS);

HighNetwork::HighNetwork(IStorageManager* sm, const LowNetwork* singleNW,
							 const IPointBindLine* bind, const ILookUp* table, bool bPreLoadTable)
	: m_buffer(sm), m_singleNW(singleNW), m_bind(bind), m_table(table), m_bPreLoadTable(bPreLoadTable)
{
	uint_32 pageSize, indexPageCount, tablePageStart;
	parseHeader(pageSize, indexPageCount, tablePageStart);
	m_spatialIndex.initByParseStorage(m_buffer, pageSize, indexPageCount);
	m_comeInNodesLookUpTable.initByParseStorage(m_buffer, pageSize, tablePageStart);
}

HighNetwork::~ HighNetwork()
{
	delete m_singleNW;
}

double HighNetwork::computeShortestPath(SearchingInfo& info) const
{
	BindPoint s,e;
	{
		if (info.m_start.m_use_course)
		{
			BindPoint _s(info.m_start.m_realPos,info.m_start.m_course, m_bind, m_singleNW);
			s=_s;
		}
		else
		{

			BindPoint _s(info.m_start.m_realPos, m_bind, m_singleNW);
			s=_s;
		}
		if (info.m_end.m_use_course)
		{
			BindPoint _e(info.m_end.m_realPos,info.m_end.m_course, m_bind, m_singleNW);
			e=_e;
		}
		else
		{
			BindPoint _e(info.m_end.m_realPos, m_bind, m_singleNW);
			e=_e;
		}
	}
	info.m_start.m_bindPos = s.m_pos;
	info.m_end.m_bindPos = e.m_pos;

	std::vector<std::pair<NODE_ID, NEAREST_DIS> > route;

	HighNWSearching search(m_singleNW, this, m_table, m_bPreLoadTable);
	double dis = search.queryShortestRoute(s, e, &route);

	parseRouteResult(route, s, e, info);

	return dis;
}

void HighNetwork::getTopoNodeConnection(uint_32 nodeID, std::vector<M_CONN>* conns) const
{
	uint_32 page, offset;
	AdjacencyList::decodeTopoNodeID(nodeID, &page, &offset);

	uint_8* data;
	uint_32 len;
	m_buffer->loadByteArray(page, len, &data);

	uint_8* ptr = data + offset;
	AdjacencyList::fetchHighConnectionInfo(ptr, conns);

	delete[] data;
}

uint_32 HighNetwork::getTopoNodeLowID(uint_32 nodeID) const
{
	uint_32 page, offset;
	AdjacencyList::decodeTopoNodeID(nodeID, &page, &offset);

	uint_8* data;
	uint_32 len;
	m_buffer->loadByteArray(page, len, &data);

	uint_8* ptr = data + offset;
	uint_32 lowID = AdjacencyList::fetchSingleTopoNodeID(ptr);

	delete[] data;

	return lowID;
}

uint_32 HighNetwork::getCrossGridHighID(uint_32 expandLowID, const MapPos2D& dest0,
										 const MapPos2D& dest1, bool bDestSameGrid) const
{
	MapPos2D expand;
	m_singleNW->getTopoNodePos(expandLowID, &expand);
	// zj 如果返回0，表示扩展点与目标点在同一个叶子节点中，需要扩展点从最低层进行扩展
	// zj 如果返回非0，表示处在不同节点中，还可以进行高层扩展
	// zj 随着扩展点不断向目标点逼近，最后一定后在同一叶子节点中，也就是返回0，进行最低层扩展
	uint_32 layer0 = m_spatialIndex.getHighestLayerGridBetweenPos(expand, dest0);
	if (bDestSameGrid)
	{
		if (layer0 == 0)
			return 0;
		else
			return m_comeInNodesLookUpTable.mapSingleIDAndLayerToMultiID(expandLowID, layer0);
	}
	else
	{
		uint_32 layer1 = m_spatialIndex.getHighestLayerGridBetweenPos(expand, dest1);
		if (layer0 == 0 && layer1 == 0)
			return 0;
		else if (layer0 == 0)
			return m_comeInNodesLookUpTable.mapSingleIDAndLayerToMultiID(expandLowID, layer1);
		else if (layer1 == 0)
			return m_comeInNodesLookUpTable.mapSingleIDAndLayerToMultiID(expandLowID, layer0);
		else
			return m_comeInNodesLookUpTable.mapSingleIDAndLayerToMultiID(expandLowID, std::min(layer0, layer1));
	}
}

//
// class HighNetwork::SpatialIndex
//

void HighNetwork::SpatialIndex::
	initByParseStorage(IStorageManager* storage, uint_32 pageSize, uint_32 indexPageCount)
{
	uint_32 totLen = pageSize * indexPageCount;
	uint_8* totData = new uint_8[totLen];
	uint_8* ptr = totData;
	for (uint_32 i = 0; i < indexPageCount; ++i)
	{
		uint_8* data;
		uint_32 len;
		storage->loadByteArray(BEGIN_INDEX_PAGE + i, len, &data);
		assert(len == pageSize);
		memcpy(ptr, data, len);
		ptr += len;
		delete[] data;
	}

	ptr = totData;
	memcpy(m_scope.m_pLow, ptr, sizeof(double) * 2);
	ptr += sizeof(double) * 2;
	memcpy(m_scope.m_pHigh, ptr, sizeof(double) * 2);
	ptr += sizeof(double) * 2;

	//int leaf_num = 0;
	//int non_leaf_num = 1;
	//uint_8* start = ptr;
	m_root = new Grid;
	if ((*ptr & 0x80) != 0)
	{
		uint_8 test = *ptr;
		test <<= 1;
		uint_32 rem = 7;

		std::stack<std::pair<Grid*, uint_32> > buffer;
		buffer.push(std::make_pair(m_root, 0));
		while (!buffer.empty())
		{
			std::pair<Grid*, uint_32>& item = buffer.top();
			Grid* g = new Grid;
			item.first->m_child[item.second++] = g;
			if (item.second == 4)
				buffer.pop();
			if ((test & 0x80) != 0)
			{
				buffer.push(std::make_pair(g, 0));
				//non_leaf_num ++;
			}
			//else
			//	leaf_num ++;

			test <<= 1;
			--rem;
			if (rem == 0)
			{
				++ptr;
				test = *ptr;
				rem = 8;
			}
		}
	}

   // int size = ptr -start;

	delete[] totData;
}

uint_32 HighNetwork::SpatialIndex::
	getHighestLayerGridBetweenPos(const MapPos2D& locate, const MapPos2D& except) const
{
	uint_32 ret = 0;
	assert(m_scope.containsPoint(locate) && m_scope.containsPoint(except));

	Rect currScope(m_scope);
	Grid* currGrid(m_root);
	do
	{
		if (currGrid->isLeaf())
			return 0;

		MapPos2D center(currScope.center());

		if (locate.d0 < center.d0)
		{
			if (locate.d1 < center.d1)
			{
				currScope.m_pHigh[0] = center.d0;
				currScope.m_pHigh[1] = center.d1;
				currGrid = currGrid->m_child[2];
			}
			else
			{
				currScope.m_pLow[1] = center.d1;
				currScope.m_pHigh[0] = center.d0;
				currGrid = currGrid->m_child[0];
			}
		}
		else
		{
			if (locate.d1 < center.d1)
			{
				currScope.m_pLow[0] = center.d0;
				currScope.m_pHigh[1] = center.d1;
				currGrid = currGrid->m_child[3];
			}
			else
			{
				currScope.m_pLow[0] = center.d0;
				currScope.m_pLow[1] = center.d1;
				currGrid = currGrid->m_child[1];
			}
		}
		++ret;

	} while (AdjacencyList::isNodeInRect(except, currScope));

	return ret;
}

bool HighNetwork::SpatialIndex::
	isTwoPointInSameLeaf(const MapPos2D& p0, const MapPos2D& p1) const
{
	assert(m_scope.containsPoint(p0) && m_scope.containsPoint(p1));

	Rect currScope(m_scope);
	Grid* currGrid(m_root);
	do
	{
		if (currGrid->isLeaf())
			return true;

		MapPos2D center(currScope.center());

		if (p0.d0 < center.d0)
		{
			if (p0.d1 < center.d1)
			{
				currScope.m_pHigh[0] = center.d0;
				currScope.m_pHigh[1] = center.d1;
				currGrid = currGrid->m_child[2];
			}
			else
			{
				currScope.m_pLow[1] = center.d1;
				currScope.m_pHigh[0] = center.d0;
				currGrid = currGrid->m_child[0];
			}
		}
		else
		{
			if (p0.d1 < center.d1)
			{
				currScope.m_pLow[0] = center.d0;
				currScope.m_pHigh[1] = center.d1;
				currGrid = currGrid->m_child[3];
			}
			else
			{
				currScope.m_pLow[0] = center.d0;
				currScope.m_pLow[1] = center.d1;
				currGrid = currGrid->m_child[1];
			}
		}

	} while (AdjacencyList::isNodeInRect(p1, currScope));

	return false;
}

//
// class HighNetwork::ComeInNodesLookUpTable
//

void HighNetwork::ComeInNodesLookUpTable::
	initByParseStorage(IStorageManager* storage, uint_32 pageSize, uint_32 tablePageStart)
{
	m_storage = storage;

	m_pageStart = tablePageStart;

	uint_8* data;
	uint_32 len;
	storage->loadByteArray(tablePageStart, len, &data);
	uint_8* ptr = data;

	do
	{
		uint_32 maxSingleNodeIDInPage = 0;
		memcpy(&maxSingleNodeIDInPage, ptr, sizeof(L1_ITEM_SIZE));
		ptr += L1_ITEM_SIZE;
		assert(maxSingleNodeIDInPage > (m_index1.empty() ? 0 : m_index1.back()));
		m_index1.push_back(maxSingleNodeIDInPage);

	} while (m_index1.back() != L1_END_FLAG);
	m_index1.pop_back();

	delete[] data;
}

uint_32 HighNetwork::ComeInNodesLookUpTable::
	mapSingleIDAndLayerToMultiID(uint_32 singleID, uint_32 layer) const
{
	//计算得到singleID-Address 在哪个page中
	uint_32 index2Page = m_pageStart + L1_PAGE_COUNT + queryFromIndex1(singleID);
	//从这个Page中找到singleID对应的Address.
	uint_32 addr = queryFromIndex2(index2Page, singleID);
	if (addr == 0)
		return 0;
	else
		return queryFromData(addr, layer);
}

uint_32 HighNetwork::ComeInNodesLookUpTable::
	queryFromIndex1(uint_32 singleID) const
{
	std::vector<uint_32>::const_iterator itr
		= std::lower_bound(m_index1.begin(), m_index1.end(), singleID);
	assert(itr != m_index1.end());
	return static_cast<uint_32>(itr - m_index1.begin());
}

uint_32 HighNetwork::ComeInNodesLookUpTable::
	queryFromIndex2(uint_32 pageID, uint_32 singleID) const
{
	uint_8* data;
	uint_32 len;
	m_storage->loadByteArray(pageID, len, &data);
	assert(len > 0 && len % L2_ITEM_SIZE == 0);

	uint_8* ptr = data;
	uint_32 id = 0, addr = 0;
	int lower = 0, upper = len / L2_ITEM_SIZE - 1;
	while (lower <= upper)
	{
		int mid = ((lower + upper) >> 1);
		ptr = data + L2_ITEM_SIZE * mid;
		memcpy(&id, ptr, sizeof(LOW_NODE_ID));

		if (id == singleID)
		{
			ptr += sizeof(LOW_NODE_ID);
			memcpy(&addr, ptr, sizeof(ADDRESS));
			return addr;
		}
		else if (id < singleID)
			lower = mid + 1;
		else
			upper = mid - 1;
	}

	return 0;
}

uint_32 HighNetwork::ComeInNodesLookUpTable::
	queryFromData(uint_32 addr, uint_32 layer) const
{
	uint_32 page, offset;
	AdjacencyList::decodeTopoNodeID(addr, &page, &offset);

	uint_8* data;
	uint_32 len;
	m_storage->loadByteArray(page, len, &data);
	uint_8* ptr = data + offset;

	uint_32 startLayer = 0;
	memcpy(&startLayer, ptr, E_START_LAYER_SIZE);
	ptr += E_START_LAYER_SIZE;

	if (layer >= startLayer)
		ptr += sizeof(HIGH_NODE_ID) * (layer - startLayer);

	uint_32 multiNodeID = 0;
	memcpy(&multiNodeID, ptr, sizeof(HIGH_NODE_ID));

	delete[] data;

	return multiNodeID;
}

//
// Internal
//

void HighNetwork::parseRouteResult(const std::vector<std::pair<NODE_ID, NEAREST_DIS> >& route,
									 const BindPoint& s, const BindPoint& e, SearchingInfo& info) const
{
//	if (route.empty())
    if(route.size() <= 1)
	{
		if (s.m_roadID == e.m_roadID)
			info.m_roadIDs.push_back(s.m_roadID);
        else{
            info.m_roadIDs.push_back(s.m_roadID);
            info.m_roadIDs.push_back(e.m_roadID);
            info.is_adjacent = true;
        }
		return;
	}

	assert(route.size() > 1);

	assert(route.front().first == e.m_beginID || route.front().first == e.m_endID
		&& route.back().first == s.m_beginID || route.back().first == s.m_endID);
	if (route.front().first == e.m_beginID)
		info.m_end.m_bDir = true;
	else
		info.m_end.m_bDir = false;
	if (route.back().first == s.m_beginID)
		info.m_start.m_bDir = true;
	else
		info.m_start.m_bDir = false;

	info.m_roadIDs.push_back(s.m_roadID);

	uint_32 iCurrID = route.size() - 1;
	while (iCurrID != 0)
	{
		uint_32 sNodeID = route[iCurrID].first,
			eNodeID = route[iCurrID - 1].first;

		if (sNodeID == eNodeID)
		{
			if (route.size() == 2)
				break;
			else
				assert(0);
		}

		uint_32 nearestDis = route[iCurrID - 1].second - route[iCurrID].second;

		std::vector<S_CONN> conns;
		m_singleNW->getTopoNodeConnection(sNodeID, &conns);

		uint_32 nearbyDis = UINT32_MAX_1;
		uint_64 roadID;
		for (uint_32 i = 0; i < conns.size(); ++i)
		{
			const S_CONN& ct = conns[i];
			if (ct.nodeID != eNodeID)
				continue;
			else if (ct.length < nearbyDis)
			{
				nearbyDis = ct.length;
				roadID = ct.roadID;
			}
		}

		if (nearbyDis > nearestDis)
		{
			BetweenTwoNodeSearchingInfo part(sNodeID, eNodeID, nearestDis);
			m_singleNW->computeShortestPath(part);
			std::copy(part.m_roadIDs.begin(), part.m_roadIDs.end(), back_inserter(info.m_roadIDs));
		}
		else
		{
			assert(nearbyDis == nearestDis);
			info.m_roadIDs.push_back(roadID);
		}

		--iCurrID;
	}

	info.m_roadIDs.push_back(e.m_roadID);
}

void HighNetwork::parseHeader(uint_32& pageSize, uint_32& indexPageCount, uint_32& tablePageStart)
{
	uint_8* data;
	uint_32 len;
	m_buffer->loadByteArray(0, len, &data);
	uint_8* ptr = data;

	memcpy(&pageSize, ptr, sizeof(uint_32));
	ptr += sizeof(uint_32);
	memcpy(&indexPageCount, ptr, sizeof(uint_32));
	ptr += sizeof(uint_32);
	memcpy(&tablePageStart, ptr, sizeof(uint_32));
	ptr += sizeof(uint_32);

	delete[] data;
}

bool Embedded::Network::HighNetwork::bindPoint( const MapPos2D& pt,Network::BindPoint &bind_pt ) const
{

	BindPoint s(pt, m_bind, m_singleNW);
	bind_pt=s;
	return true;
}

bool Embedded::Network::HighNetwork::bindPoint( const MapPos2D& pt,double course,Network::BindPoint &bind_pt ) const
{

	BindPoint s(pt,course, m_bind, m_singleNW);
	bind_pt=s;
	return true;
}