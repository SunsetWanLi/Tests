#ifndef _EMBEDDED_NETWORK_HIGH_NETWORK_H_
#define _EMBEDDED_NETWORK_HIGH_NETWORK_H_
#include "AdjacencyList.h"

namespace Embedded
{
	namespace Network
	{
		typedef uint_32 NODE_ID;
		typedef uint_32 NEAREST_DIS;

		class BindPoint;
		class LowNetwork;

		class HighNetwork : public INetwork
		{
		public:
			HighNetwork(IStorageManager* sm, const LowNetwork* singleNW, 
				const IPointBindLine* bind, const ILookUp* table, bool bPreLoadTable);
			virtual ~HighNetwork();

			virtual double computeShortestPath(SearchingInfo& info) const;
			virtual bool bindPoint(const MapPos2D& pt,Network::BindPoint &bind_pt) const;
			virtual bool bindPoint(const MapPos2D& s,double course,Network::BindPoint &bind_pt) const;

			void getTopoNodeConnection(uint_32 nodeID, std::vector<M_CONN>* conns) const;
			uint_32 getTopoNodeLowID(uint_32 nodeID) const;

			uint_32 getCrossGridHighID(uint_32 expandLowID, const MapPos2D& dest0, 
				const MapPos2D& dest1, bool bDestSameGrid) const;

			bool isTwoPointInSameLeaf(const MapPos2D& p0, const MapPos2D& p1) const {
				return m_spatialIndex.isTwoPointInSameLeaf(p0, p1);
			}

		private:
			class SpatialIndex
			{
			public:
				SpatialIndex() { m_scope.makeInfinite(); }
				~SpatialIndex() { delete m_root; }

				void initByParseStorage(IStorageManager* storage, uint_32 pageSize, 
					uint_32 indexPageCount);

				uint_32 getHighestLayerGridBetweenPos(const MapPos2D& locate, 
					const MapPos2D& except) const;

				bool isTwoPointInSameLeaf(const MapPos2D& p0, const MapPos2D& p1) const;

			private:
				class Grid
				{
				public:
					Grid() {
						for (uint_32 i = 0; i < 4; ++i)
							m_child[i] = NULL;
					}
					~Grid() {
						for (uint_32 i = 0; i < 4; ++i)
							delete m_child[i];
					}

					bool isLeaf() const { return m_child[0] == NULL; }

				public:
					Grid* m_child[4];
				}; // Grid

			private:
				Rect m_scope;
				Grid* m_root;

			}; // SpatialIndex

			class ComeInNodesLookUpTable
			{
			public:
				void initByParseStorage(IStorageManager* storage, uint_32 pageSize, 
					uint_32 tablePageStart);

				uint_32 mapSingleIDAndLayerToMultiID(uint_32 singleID, uint_32 layer) const;

			private:
				uint_32 queryFromIndex1(uint_32 singleID) const;
				uint_32 queryFromIndex2(uint_32 pageID, uint_32 singleID) const;
				uint_32 queryFromData(uint_32 addr, uint_32 layer) const;

			private:
				IStorageManager* m_storage;

				std::vector<uint_32> m_index1;

				uint_32 m_pageStart;

			}; // ComeInNodesLookUpTable

		private:
			void parseHeader(uint_32& pageSize, uint_32& indexPageCount, uint_32& tablePageStart);

			void parseRouteResult(const std::vector<std::pair<NODE_ID, NEAREST_DIS> >& route, 
				const BindPoint& s, const BindPoint& e, SearchingInfo& info) const;

		private:
			IStorageManager* m_buffer;

			const IPointBindLine* m_bind;

			const ILookUp* m_table;
			bool m_bPreLoadTable;

			const LowNetwork* m_singleNW;

			SpatialIndex m_spatialIndex;

			ComeInNodesLookUpTable m_comeInNodesLookUpTable;

		}; // HighNetwork
	}
}

#endif // _EMBEDDED_NETWORK_HIGH_NETWORK_H_