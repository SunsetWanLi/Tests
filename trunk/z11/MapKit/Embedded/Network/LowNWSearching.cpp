#include "LowNWSearching.h"
#include "LowNetwork.h"
#include "BindPoint.h"

using namespace Embedded;
using namespace Embedded::Network;

double LowNWSearching::queryShortestRoute(const BindPoint& from, const BindPoint& to, std::vector<uint_32>* routeNodeID)
{
	double ret = DOUBLE_MAX;

//	if (from.m_roadID == to.m_roadID || from.m_beginID == to.m_beginID ||
//		from.m_beginID == to.m_endID || from.m_endID == to.m_beginID || from.m_endID == to.m_endID)
    if (from.m_roadID == to.m_roadID)
	{
		//noSearchPath(from, to, routeNodeID);
		// todo: should compute the dis in the polyline
		ret = 0;
	}
	else if (m_network->getConnectionSetID(from.m_beginID) == m_network->getConnectionSetID(to.m_beginID))
	{
		m_destInfo.initByNodePos(to, m_table, m_bPreLoadTable);
		m_visited.clear();
		m_candidate.clear();

		VisitedNode *b = new VisitedNode(from.m_beginID, from.m_beginLen, m_network, m_table, m_destInfo, NULL),
			*e = new VisitedNode(from.m_endID, from.m_endLen, m_network, m_table, m_destInfo, NULL);
		m_visited.push(b);
		m_visited.push(e);
		m_candidate.push(b);
		m_candidate.push(e);

		ret = searchPathBetweenTwoPos(routeNodeID);
	}

	return ret;
}

double LowNWSearching::queryShortestRoute(uint_32 from, uint_32 to, std::vector<uint_32>* routeNodeID,
												double knownDis /*= DOUBLE_MAX*/)
{
	if (from == to)
		return 0;
	if (!m_network->getConnectionSetID(from) == m_network->getConnectionSetID(to))
		return DOUBLE_MAX;

	m_destInfo.initByNodeID(m_network, to, m_table, m_bPreLoadTable);
	m_visited.clear();
	m_candidate.clear();

	VisitedNode* b = new VisitedNode(from, 0.0, m_network, m_table, m_destInfo, NULL);
	m_visited.push(b);
	m_candidate.push(b);

	return searchPathBetweenTwoNode(routeNodeID, knownDis);
}

double LowNWSearching::queryShortestDistance(uint_32 from, uint_32 to)
{
	if (from == to)
		return 0;
	if (!m_network->getConnectionSetID(from) == m_network->getConnectionSetID(to))
		return DOUBLE_MAX;

	m_destInfo.initByNodeID(m_network, to, m_table, m_bPreLoadTable);
	m_visited.clear();
	m_candidate.clear();

	VisitedNode* b = new VisitedNode(from, 0.0, m_network, m_table, m_destInfo, NULL);
	m_visited.push(b);
	m_candidate.push(b);

	return searchDisBetweenTwoNode();
}

double LowNWSearching::searchPathBetweenTwoNode(std::vector<uint_32>* pathNodeID,
													  double knownDis /*= DOUBLE_MAX*/)
{
	while (!m_candidate.empty())
	{
		VisitedNode* opt = m_candidate.top(); m_candidate.pop();
		if (m_destInfo.beSameNodeID(opt->m_nodeID))
		{
			opt->getShortestPath(pathNodeID);
			return opt->m_from;
		}
		else
		{
			if (knownDis != DOUBLE_MAX && opt->m_from + opt->m_to > knownDis)
				continue;
			expand(opt);
		}
	}

	return DOUBLE_MAX;
}

double LowNWSearching::searchPathBetweenTwoPos(std::vector<uint_32>* pathNodeIDs)
{
	VisitedNode* lastResult = NULL;
	double lastDis;

	while (!m_candidate.empty())
	{
		VisitedNode* opt = m_candidate.top(); m_candidate.pop();

		if (lastResult != NULL && opt->m_from + opt->m_to > lastDis)
		{
			lastResult->getShortestPath(pathNodeIDs);
			//// debug
			//std::ofstream fout("c:/result_3_13.txt", std::ios_base::app);
			//fout << " [expend size]:" << m_visited.size() << std::endl;
			////
			return lastDis;
		}
		else if (m_destInfo.beSameNodeID(opt->m_nodeID))
		{
			if (lastResult != NULL)
			{
				double currDis = opt->m_from + opt->m_to;
				if (currDis < lastDis)
				{
					opt->getShortestPath(pathNodeIDs);
					//// debug
					//std::ofstream fout("c:/result_3_13.txt", std::ios_base::app);
					//fout << " [expend size]:" << m_visited.size() << std::endl;
					////
					return currDis;
				}
				else
				{
					lastResult->getShortestPath(pathNodeIDs);
					//// debug
					//std::ofstream fout("c:/result_3_13.txt", std::ios_base::app);
					//fout << " [expend size]:" << m_visited.size() << std::endl;
					////
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

double LowNWSearching::searchDisBetweenTwoNode()
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

void LowNWSearching::expand(VisitedNode* n)
{
	std::vector<S_CONN> conns;
	m_network->getTopoNodeConnection(n->m_nodeID, &conns);
	for (uint_32 i = 0; i < conns.size(); ++i)
	{
		const S_CONN& ct = conns.at(i);

		PTR_VISITED pVisited = m_visited.find(ct.nodeID);
		if (m_visited.exist(pVisited))
		{
			double newFrom = n->m_from + ct.length;
			if (newFrom < (*pVisited)->m_from)
				m_candidate.update(*pVisited, newFrom, n);
		}
		else
		{
			VisitedNode* newNode = new VisitedNode(ct.nodeID, n->m_from + ct.length, 
				m_network, m_table, m_destInfo, n);
			m_visited.push(newNode);
			m_candidate.push(newNode);
		}
	}
}
