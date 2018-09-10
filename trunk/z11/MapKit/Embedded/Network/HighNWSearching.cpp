#include "HighNWSearching.h"
#include "LowNetwork.h"
#include "HighNetwork.h"
#include "BindPoint.h"
#include "AdjacencyList.h"
#include "../Application/Pathfinding.h"

using namespace Embedded;
using namespace Embedded::Network;

HighNWSearching::HighNWSearching(const LowNetwork* singleNW, const HighNetwork* multiNW, 
										   const ILookUp* table, bool bPreLoadTable)
	: m_singleNW(singleNW), m_multiNW(multiNW), m_table(table), m_bPreLoadTable(bPreLoadTable)
{
}

double HighNWSearching::queryShortestDistance(uint_32 from, uint_32 to)
{
	if (from == to)
		return 0;
	if (!m_singleNW->getConnectionSetID(from) == m_singleNW->getConnectionSetID(to))
		return DOUBLE_MAX;

	m_destInfo.initByNodeID(m_singleNW, to, m_table, m_bPreLoadTable);
	MapPos2D destPos;
	m_singleNW->getTopoNodePos(to, &destPos);
	m_destPos0 = destPos;
	m_destPos1 = destPos;
	m_bDestSameGrid = true;

	m_visited.clear();
	m_candidate.clear();

	VisitedNode* b = new VisitedNode(from, 0.0, m_singleNW, m_table, m_destInfo, NULL);
	m_visited.push(b);
	m_candidate.push(b);

	return searchDisBetweenTwoNode();

}

double HighNWSearching::queryShortestRoute(const BindPoint& from, const BindPoint& to, 
												std::vector<std::pair<NODE_ID, NEAREST_DIS> >* route)
{
	double ret = DOUBLE_MAX;
//
//	if (from.m_roadID == to.m_roadID || from.m_beginID == to.m_beginID ||
//		from.m_beginID == to.m_endID || from.m_endID == to.m_beginID || from.m_endID == to.m_endID)
	if (from.m_roadID == to.m_roadID)
    {
//		Utility::noSearchPath(from, to, routeNodeID);
		// todo: should compute the dis in the polyline
        if(from.m_beginLen <= to.m_beginLen)
        {
            ret = to.m_beginLen - from.m_beginLen;
        }
	}
	else if (m_singleNW->getConnectionSetID(from.m_beginID) == m_singleNW->getConnectionSetID(to.m_beginID))
	{
		m_destInfo.initByNodePos(to, m_table, m_bPreLoadTable);
		m_destPos0 = to.m_beginPos;
		m_destPos1 = to.m_endPos;
		m_bDestSameGrid = m_multiNW->isTwoPointInSameLeaf(m_destPos0, m_destPos1);

		m_visited.clear();
		m_candidate.clear();

		VisitedNode *b = new VisitedNode(from.m_beginID, from.m_beginLen, m_singleNW, m_table, m_destInfo, NULL),
			*e = new VisitedNode(from.m_endID, from.m_endLen, m_singleNW, m_table, m_destInfo, NULL);
        if (NetworkSearchingType::searching_type == BOTH_SEARCHING) {
            m_visited.push(b);
            m_visited.push(e);
            m_candidate.push(b);
            m_candidate.push(e);
        } else {
            delete b;
            m_visited.push(e);
            m_candidate.push(e);
        }

		ret = searchPathBetweenTwoPos(route);
	}

	return ret;
}

double HighNWSearching::searchDisBetweenTwoNode()
{
	while (!m_candidate.empty())
	{
		VisitedNode* opt = m_candidate.top(); m_candidate.pop();
		if (m_destInfo.beSameNodeID(opt->m_nodeID))
			return opt->m_from;
		else
			expand(opt);
	}

	return DOUBLE_MAX;
}

double HighNWSearching::searchPathBetweenTwoPos(std::vector<std::pair<NODE_ID, NEAREST_DIS> >* route)
{
	VisitedNode* lastResult = NULL;
	double lastDis;

	while (!m_candidate.empty())
	{
		VisitedNode* opt = m_candidate.top(); m_candidate.pop();

		// debug
		//if (opt->m_nodeID == 500324)
		//	int zz = 0;
		//
		if (lastResult != NULL && opt->m_from + opt->m_to > lastDis)
		{
			lastResult->getShortestPath(route);
			return lastDis;
		}
		else if (m_destInfo.beSameNodeID(opt->m_nodeID))
		{
			if (lastResult != NULL)
			{
				double currDis = opt->m_from + opt->m_to;
				if (currDis < lastDis)
				{
					opt->getShortestPath(route);
					return currDis;
				}
				else
				{
					lastResult->getShortestPath(route);
					return lastDis;
				}
			}
			else
			{
				lastResult = opt;
				lastDis = lastResult->m_from + lastResult->m_to;
				expand(opt);
			}
		}
		else
			expand(opt);
	}

	return DOUBLE_MAX;
}
//template< > void HighNWSearching::expand(const std::vector<Embedded::Network::S_CONN>& conns, VisitedNode* n);
//template< > void HighNWSearching::expand(const std::vector<Embedded::Network::M_CONN>& conns, VisitedNode* n);
void HighNWSearching::expand(VisitedNode* n)
{
	if (n->m_highID == 0) // zj 可能是本身就不是一个带高级节点的节点，也可能是本身带高级节点，但是不需要进行高层扩展（当此节点与目标节点在一个叶子节点Grid中时）
	{
		std::vector<S_CONN> conns;
		m_singleNW->getTopoNodeConnection(n->m_nodeID, &conns, true);
		expand(conns, n);
	}
	else
	{
		std::vector<M_CONN> conns;
		m_multiNW->getTopoNodeConnection(n->m_highID, &conns);
		expand(conns, n);
	}
}

//template<class T>
//void HighNWSearching::expand(const std::vector<T>& conns, VisitedNode* n)
//{
//	for (uint_32 i = 0; i < conns.size(); ++i)
//	{
//		const T& ct = conns[i];
//
//		uint_32 expandLowID = getExpandNodeLowID(n->m_highID, ct.nodeID);
//
//		PTR_VISITED pVisited = m_visited.find(expandLowID);
//		if (m_visited.exist(pVisited))
//		{
//			double newFrom = n->m_from + ct.length;
//			if (newFrom < (*pVisited)->m_from)
//				m_candidate.update(*pVisited, newFrom, n);
//		}
//		else
//		{
//			uint_32 expandHighID = getExpandNodeHighID(n->m_highID, ct.nodeID, expandLowID);
//
//			VisitedNode* newNode = new VisitedNode(expandLowID, expandHighID, 
//				n->m_from + ct.length, m_singleNW, m_table, m_destInfo, n);
//			m_visited.push(newNode);
//			m_candidate.push(newNode);
//		}
//	}
//}

/*template< > */
void HighNWSearching::expand(const std::vector<S_CONN>& conns, VisitedNode* n)
{
	for (uint_32 i = 0; i < conns.size(); ++i)
	{
		const S_CONN& ct = conns[i];
		uint_32 expandLowID = getExpandNodeLowID0(ct.nodeID);
        
        if (NetworkSearchingType::searching_type == SINGLE_SEARCHING) {
            Line* line = NULL;
            
            //Features::MapLayer::getLineByAddress(ct.roadID, &line);
            Pathfinding::getInstance()->getLineByAddress(ct.roadID, &line);
            std::vector<MapPos2D> ctlpos;
            line->getPoints(&ctlpos);
            delete_and_set_null(line);
            
            MapPos2D begin = ctlpos.front();
            MapPos2D end = ctlpos.back();
            
            uint_32 b_id = m_singleNW->queryTopoNodeID(begin);
            uint_32 e_id = m_singleNW->queryTopoNodeID(end);
            if(e_id == n->m_nodeID)
                continue;
        }
        
		PTR_VISITED pVisited = m_visited.find(expandLowID);
		if (m_visited.exist(pVisited))
		{
			double newFrom = n->m_from + ct.length;
			if (newFrom < (*pVisited)->m_from)
				m_candidate.update(*pVisited, newFrom, n);
		}
		else
		{
			uint_32 expandHighID = getExpandNodeHighID0(ct.nodeID, expandLowID);

			VisitedNode* newNode = new VisitedNode(expandLowID, expandHighID, 
				n->m_from + ct.length, m_singleNW, m_table, m_destInfo, n);
			m_visited.push(newNode);
			m_candidate.push(newNode);
		}
        

	}
}

//template<>
void HighNWSearching::expand(const std::vector<M_CONN>& conns, VisitedNode* n)
{
	for (uint_32 i = 0; i < conns.size(); ++i)
	{
		const M_CONN& ct = conns[i];
		uint_32 expandLowID = getExpandNodeLowID1(ct.nodeID);

		PTR_VISITED pVisited = m_visited.find(expandLowID);
		if (m_visited.exist(pVisited))
		{
			double newFrom = n->m_from + ct.length;
			if (newFrom < (*pVisited)->m_from)
				m_candidate.update(*pVisited, newFrom, n);
		}
		else
		{
			uint_32 expandHighID = getExpandNodeHighID1(ct.nodeID, expandLowID);

			VisitedNode* newNode = new VisitedNode(expandLowID, expandHighID, 
				n->m_from + ct.length, m_singleNW, m_table, m_destInfo, n);
			m_visited.push(newNode);
			m_candidate.push(newNode);
		}

	}
}

//zj 始终都得到最低层的ID
uint_32 HighNWSearching::getExpandNodeLowID(uint_32 prevHighID, uint_32 expandFetchedID) const
{
	if (prevHighID == 0)
	{
		AdjacencyList::eraseNodeIDFlag(expandFetchedID);
		return expandFetchedID;
	}
	else
	{
		if (AdjacencyList::isSetNodeIDFlag(expandFetchedID))
			return m_multiNW->getTopoNodeLowID(expandFetchedID);
		else
			return expandFetchedID;
	}
}
//zj 始终都得到最低层的ID
uint_32 HighNWSearching::getExpandNodeLowID0(uint_32 expandFetchedID) const
{
	AdjacencyList::eraseNodeIDFlag(expandFetchedID);
	return expandFetchedID;	
}
//zj 始终都得到最低层的ID
uint_32 HighNWSearching::getExpandNodeLowID1(uint_32 expandFetchedID) const
{
	if (AdjacencyList::isSetNodeIDFlag(expandFetchedID))
		return m_multiNW->getTopoNodeLowID(expandFetchedID);
	else
		return expandFetchedID;
	
}
//zj 始终都得到的ID可能是不同高层的ID，没有高层ID时返回0
uint_32 HighNWSearching::getExpandNodeHighID(uint_32 prevHighID, uint_32 expandFetchedID,
												 uint_32 expandLowID) const
{
	if (prevHighID != 0 && AdjacencyList::isSetNodeIDFlag(expandFetchedID))
		return expandFetchedID;
	else if (prevHighID == 0 && !AdjacencyList::isSetNodeIDFlag(expandFetchedID))
		return 0;
	else
		return m_multiNW->getCrossGridHighID(expandLowID, m_destPos0, m_destPos1, m_bDestSameGrid);
}
//zj 始终都得到的ID可能是不同高层的ID,没有高层ID时返回0
uint_32 HighNWSearching::getExpandNodeHighID0(uint_32 expandFetchedID, uint_32 expandLowID) const
{
	if (!AdjacencyList::isSetNodeIDFlag(expandFetchedID))
		return 0;
	else
		return m_multiNW->getCrossGridHighID(expandLowID, m_destPos0, m_destPos1, m_bDestSameGrid);//从一个非高级节点连接到一个高级节点
}
//zj 始终都得到的ID可能是不同高层的ID
uint_32 HighNWSearching::getExpandNodeHighID1(uint_32 expandFetchedID, uint_32 expandLowID) const
{
	if (AdjacencyList::isSetNodeIDFlag(expandFetchedID))
		return expandFetchedID;	
	else
		return m_multiNW->getCrossGridHighID(expandLowID, m_destPos0, m_destPos1, m_bDestSameGrid);//代表跨区了
}
