#include "Utility.h"
#include "BindPoint.h"
#include "LowNetwork.h"
#include "../Tools/StringTools.h"
#include <string.h>

using namespace Embedded;
using namespace Embedded::Network;

//
// class DestInfo
//

SearchingType NetworkSearchingType::searching_type = SINGLE_SEARCHING;

DestInfo::DestInfo()
	: m_bGridID(UINT32_MAX_1), m_eGridID(UINT32_MAX_1),
	m_bTable(NULL), m_eTable(NULL)
{
}

DestInfo::~DestInfo()
{
	if (m_bTable == m_eTable)
		delete m_bTable;
	else
	{
		delete m_bTable;
		delete m_eTable;
	}
}

void DestInfo::initByNodePos(const BindPoint& p, const ILookUp* table, bool bPreLoadTable)
{
	m_pos = p.m_pos;
	m_beginID = p.m_beginID; m_endID = p.m_endID;
	m_beginLen = p.m_beginLen; m_endLen = p.m_endLen;
	m_roadID = p.m_roadID;

	if (table != NULL)
	{
		m_bGridID = table->getGridID(p.m_beginPos);
		m_eGridID = table->getGridID(p.m_endPos);
		if (bPreLoadTable)
		{
			table->loadTable(m_bGridID, &m_bTable);
			if (m_bGridID == m_eGridID)
				m_eTable = m_bTable;
			else
				table->loadTable(m_eGridID, &m_eTable);
		}
	}
}

//一般是搜索子最短路径时使用
void DestInfo::initByNodeID(const LowNetwork* nw, uint_32 nodeID, const ILookUp* table, bool bPreLoadTable)
{
	nw->getTopoNodePos(nodeID, &m_pos);
	m_beginID = m_endID = nodeID;
	m_beginLen = m_endLen = 0.0;
	m_roadID = 0;

	if (table != NULL)
	{
		m_bGridID = m_eGridID = table->getGridID(m_pos);
		if (bPreLoadTable)
		{
			table->loadTable(m_bGridID, &m_bTable);
			m_eTable = m_bTable;
		}
	}
}
double DestInfo::getMinDisToHere(const ILookUp* lookUp, const MapPos2D& srcPos) const
{
	if (m_bTable != NULL)
	{
		if (m_beginID == m_endID)             //当搜索子最短路径时均为这种情况
		{
			assert(m_beginLen == 0 && m_endLen == 0);
			return lookUp->queryMinDistance(srcPos, m_bTable);
		}
		else if (m_bTable == m_eTable)
			return std::min(m_beginLen, m_endLen) + lookUp->queryMinDistance(srcPos, m_bTable);
		else
			return std::min(m_beginLen + lookUp->queryMinDistance(srcPos, m_bTable),
			m_endLen + lookUp->queryMinDistance(srcPos, m_eTable));
	}
	else
	{
		assert(m_bGridID != UINT32_MAX_1 || m_eGridID != UINT32_MAX_1);
		if (m_beginID == m_endID)
		{
			assert(m_beginLen == 0 && m_endLen == 0);
			return lookUp->queryMinDistance(srcPos, m_bGridID);
		}
		else if (m_bGridID == m_eGridID)
			return std::min(m_beginLen, m_endLen) + lookUp->queryMinDistance(srcPos, m_bGridID);
		else
			return std::min(m_beginLen + lookUp->queryMinDistance(srcPos, m_bGridID),
			m_endLen + lookUp->queryMinDistance(srcPos, m_eGridID));
	}
}
//
// class VisitedNode
//

VisitedNode::VisitedNode(uint_32 id, double from, const LowNetwork* network,
						 const ILookUp* table, const DestInfo& dest, VisitedNode* prev)
	: m_nodeID(id), m_highID(0), m_from(from), m_prev(prev)
{
	initToDis(network, table, dest);
}

VisitedNode::VisitedNode(uint_32 lowID, uint_32 highID, double from, const LowNetwork* network,
						 const ILookUp* table, const DestInfo& dest, VisitedNode* prev)
	: m_nodeID(lowID), m_highID(highID), m_from(from), m_prev(prev)
{
	initToDis(network, table, dest);
}

void VisitedNode::getShortestPath(std::vector<uint_32>* pathNodeIDs) const
{
	const VisitedNode* curr = this;
	do
	{
		pathNodeIDs->push_back(curr->m_nodeID);
		curr = curr->m_prev;
	} while (curr);
}

void VisitedNode::getShortestPath(std::vector<std::pair<NODE_ID, NEAREST_DIS> >* path) const
{
	const VisitedNode* curr = this;
	do
	{
		path->push_back(std::make_pair(curr->m_nodeID, static_cast<uint_32>(curr->m_from)));
		curr = curr->m_prev;
	} while (curr);
}

//int hits = 0, misses = 0;
void VisitedNode::initToDis(const LowNetwork* network, const ILookUp* table,
							const DestInfo& dest)
{

	if (dest.beSameNodeID(m_nodeID))           //m_nodeID 一个起点的ID，dest时包括两个终点的ID(s)
		m_to = dest.getEndingDis(m_nodeID);
	else
	{
		MapPos2D pos;
		network->getTopoNodePos(m_nodeID, &pos);
		double lineDis = dest.getMinDisToHere(pos);//绑定位置到 此起点的直线距离

		if (table == NULL || !dest.beHasLookUpTableData())
			m_to = lineDis;                        //没有预处理快查表时，就用直线距离作启发值
		else
		{										 //有预处理快查表时，用处理值作启发值更准确
			double tableDis = dest.getMinDisToHere(table, pos);
			if (tableDis < lineDis || tableDis == DOUBLE_MAX)
				/*++misses,*/m_to = lineDis;					//当点所在位置刚好背离另一个区域时，可能有tableDis<lineDis此时tableDis不准确
			else
				/*++hits,*/m_to = tableDis;
		}
	}
}

void Network::fetchLayersName(IStorageManager* sm, std::vector<z11::MGWString>& names)
{
	uint_32 len;
	uint_8* data;
	sm->loadByteArray(0, len, &data);

	uint_8* ptr = data;
	ptr += sizeof(double) * 2 * 2 + sizeof(uint_32) * 4;

	uint_32 layerNum;
	memcpy(&layerNum, ptr, sizeof(uint_32));
	ptr += sizeof(uint_32);

	names.clear();
	names.reserve(layerNum);
	for (uint_32 i = 0; i < layerNum; ++i)
	{
		names.push_back(StringTools::readWString(ptr, len));
		ptr += len;
	}

	delete[] data;
}

void Network::noSearchPath(const BindPoint& from, const BindPoint& to, std::vector<uint_32>* routeNodeID)
{
	if (from.m_roadID == to.m_roadID)
		;
	else if (from.m_beginID == to.m_beginID)
	{
		routeNodeID->push_back(from.m_beginID);
		routeNodeID->push_back(from.m_beginID);
	}
	else if (from.m_beginID == to.m_endID)
	{
		routeNodeID->push_back(from.m_beginID);
		routeNodeID->push_back(from.m_beginID);
	}
	else if (from.m_endID == to.m_beginID)
	{
		routeNodeID->push_back(from.m_endID);
		routeNodeID->push_back(from.m_endID);
	}
	else if (from.m_endID == to.m_endID)
	{
		routeNodeID->push_back(from.m_endID);
		routeNodeID->push_back(from.m_endID);
	}
	else
		assert(0);
}
