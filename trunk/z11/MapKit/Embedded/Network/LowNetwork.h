#ifndef _EMBEDDED_NETWORK_LOW_NETWORK_H_
#define _EMBEDDED_NETWORK_LOW_NETWORK_H_
#include "AdjacencyList.h"
#include "../BasicType/Rect.h"

namespace Embedded
{
	namespace Network
	{
		class BindPoint;

		class LowNetwork : public INetwork
		{
		public:
			LowNetwork(IStorageManager* sm, const IPointBindLine* bind, 
				const ILookUp* table, bool bPreLoadTable);
			LowNetwork(IStorageManager* sm);
			virtual ~LowNetwork() {}

			double computeShortestPath(BetweenTwoNodeSearchingInfo& info) const;
			virtual double computeShortestPath(SearchingInfo& info) const;

			uint_32 queryTopoNodeID(const MapPos2D& p) const;
			void getTopoNodePos(uint_32 nodeID, MapPos2D* pos) const;

			uint_32 getConnectionSetID(uint_32 nodeID) const;
			void getTopoNodeConnection(uint_32 nodeID, std::vector<S_CONN>* conns, bool bUsedInMLSearch = false) const;

		private:
			void parseHeader();
			
			void parseRouteResult(const std::vector<uint_32>& nodeIDs, BetweenTwoNodeSearchingInfo& info) const;
			void parseRouteResult(const std::vector<uint_32>& nodeIDs, const BindPoint& s, 
				const BindPoint& e, SearchingInfo& info) const;

			void retrieveIndex(const MapPos2D& p, std::vector<uint_32>* IDs) const;
			void retrieveLeaf(const MapPos2D& p, uint_32 page, uint_32 offset, const Rect& s, std::vector<uint_32>* IDs) const;

		private:
			IStorageManager* m_buffer;

			const IPointBindLine* m_bind;

			const ILookUp* m_table;
			bool m_bPreLoadTable;

			Rect m_scope;
			bool m_rootBeLeaf;
			uint_32 m_firstLeafPage, m_firstIndexPage;

			friend class LowNWQuadTree;

		}; // LowNetwork
	}
}

#endif // _EMBEDDED_NETWORK_LOW_NETWORK_H_
