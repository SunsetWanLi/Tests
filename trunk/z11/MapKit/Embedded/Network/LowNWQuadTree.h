#ifndef _EMBEDDED_NETWORK_LOW_NW_QUADTREE_H_
#define _EMBEDDED_NETWORK_LOW_NW_QUADTREE_H_
#include "../Tools/SpatialDataBase.h"
#include "../BasicType/Rect.h"

namespace Embedded
{	
	namespace Network
	{
		class LowNetwork;
		class LowNWQuadTree;

		static const uint_32 LOW_PM_NODE_ID_SIZE		= 4;
		static const uint_32 LOW_PM_NODE_COUNT_SIZE	= 4;
		static const uint_32 LOW_LEAF_COUNT_SIZE		= 1;

		class LowNWNode
		{
		public:
			LowNWNode(const Rect& scope, uint_32 count, bool bLeaf);
			virtual ~LowNWNode() {}

		public:
			bool m_bLeaf;
			uint_32 m_topoNodeCount;
			Rect m_scope;

		}; // LowNWNode

		class LowNWLeaf : public LowNWNode
		{
		public:
			LowNWLeaf(const Rect& scope, uint_32 count);
			virtual ~LowNWLeaf() {}

			static void queryAdjacencyListID(const MapPos2D& p, const uint_8* data, std::vector<uint_32>* IDs);
			static void getAllAdjacencyListIDs(const uint_8* data, uint_32& startID, uint_32& num);

		private:
			static const uint_32 GRID_ID_SIZE	= 4;
			static const uint_32 PARTITION_SIZE	= 1;

		public:
			uint_32 m_startALPage;
			uint_32 m_pageCount;

		}; // LowNWLeaf

		class LowNWIndex : public LowNWNode
		{
		public:
			LowNWIndex(const Rect& scope, uint_32 count);
			virtual ~LowNWIndex();

		public:
			// 0 1
			// 2 3
			LowNWNode* m_child[4];

		}; // LowNWIndex

		class LowNWQuadTree
		{
		public:
			LowNWQuadTree(IStorageManager& sm);
			~LowNWQuadTree();

		private:
			void build();

		private:
			LowNetwork* m_network;

			LowNWNode* m_root;
			const Rect& m_scope;

			typedef uint_32 PAGE_ID;
			typedef uint_32 OFFSET;
			typedef std::pair<PAGE_ID, OFFSET> ADDR;

		}; // LowNWQuadTree
	}
}

#endif // _EMBEDDED_NETWORK_LOW_NW_QUADTREE_H_