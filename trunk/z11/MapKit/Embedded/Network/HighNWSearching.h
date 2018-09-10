#ifndef _EMBEDDED_NETWORK_HIGH_NW_SEARCHING_H_
#define _EMBEDDED_NETWORK_HIGH_NW_SEARCHING_H_
#include "Utility.h"
#include "AdjacencyList.h"

namespace Embedded
{
	namespace Network
	{
		class HighNetwork;

		class HighNWSearching
		{
		public:
			HighNWSearching(const LowNetwork* singleNW, const HighNetwork* multiNW,
				const ILookUp* table, bool bPreLoadTable);

			double queryShortestDistance(uint_32 from, uint_32 to);
			double queryShortestRoute(const BindPoint& from, const BindPoint& to, 
				std::vector<std::pair<NODE_ID, NEAREST_DIS> >* route);

		private:
			double searchDisBetweenTwoNode();
			double searchPathBetweenTwoPos(std::vector<std::pair<NODE_ID, NEAREST_DIS> >* route);

			void expand(VisitedNode* n);

			//template<class T>
			//void expand(const std::vector<T>& conns, VisitedNode* n);

			void expand(const std::vector<S_CONN>& conns, VisitedNode* n);
			void expand(const std::vector<M_CONN>& conns, VisitedNode* n);

			uint_32 getExpandNodeLowID(uint_32 prevHighID, uint_32 expandFetchedID) const;
			uint_32 getExpandNodeHighID(uint_32 prevHighID, uint_32 expandFetchedID,
				uint_32 expandLowID) const;

			uint_32 getExpandNodeLowID0(uint_32 expandFetchedID) const; //zj when prevHighID=0
			uint_32 getExpandNodeLowID1(uint_32 expandFetchedID) const; //zj when prevHighID!=0
			uint_32 getExpandNodeHighID0( uint_32 expandFetchedID,uint_32 expandLowID) const;//zj when prevHighID=0
			uint_32 getExpandNodeHighID1( uint_32 expandFetchedID,uint_32 expandLowID) const;//zj when prevHighID!=0

		private:
			const LowNetwork* m_singleNW;
			const HighNetwork* m_multiNW;

			MapPos2D m_destPos0, m_destPos1;
			bool m_bDestSameGrid;

			const ILookUp* m_table;
			bool m_bPreLoadTable;

			DestInfo m_destInfo;

			VisitedList m_visited;
			CandidateList m_candidate;

		}; // HighNWSearching
	}
}

#endif // _EMBEDDED_NETWORK_HIGH_NW_SEARCHING_H_