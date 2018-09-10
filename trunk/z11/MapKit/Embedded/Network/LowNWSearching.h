#ifndef _EMBEDDED_NETWORK_SL_SINGLE_SRC_SEARCHING_H_
#define _EMBEDDED_NETWORK_SL_SINGLE_SRC_SEARCHING_H_
#include "Utility.h"

namespace Embedded
{
	namespace Network
	{
		class LowNWSearching
		{
		public:
			LowNWSearching(const LowNetwork* network, const ILookUp* table, bool bPreLoadTable) 
				: m_network(network), m_table(table), m_bPreLoadTable(bPreLoadTable) {}

			double queryShortestRoute(const BindPoint& from, const BindPoint& to, std::vector<uint_32>* routeNodeID);
			double queryShortestRoute(uint_32 from, uint_32 to, std::vector<uint_32>* routeNodeID, double knownDis = DOUBLE_MAX);

			double queryShortestDistance(uint_32 from, uint_32 to);

		private:
			double searchPathBetweenTwoNode(std::vector<uint_32>* pathNodeID, double knownDis = DOUBLE_MAX);
			double searchPathBetweenTwoPos(std::vector<uint_32>* pathNodeID);

			double searchDisBetweenTwoNode();

			void expand(VisitedNode* n);

		private:
			const LowNetwork* m_network;
			const ILookUp* m_table;
			bool m_bPreLoadTable;

			DestInfo m_destInfo;

			VisitedList m_visited;
			CandidateList m_candidate;
			
		}; // LowNWSearching
	}
}

#endif // _EMBEDDED_NETWORK_SL_SINGLE_SRC_SEARCHING_H_