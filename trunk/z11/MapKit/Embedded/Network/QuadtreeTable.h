#ifndef _EMBEDDED_NETWORK_QUADTREE_TABLE_H_
#define _EMBEDDED_NETWORK_QUADTREE_TABLE_H_
#include "../BasicType/Scope.h"

namespace Embedded
{
	namespace Network
	{
		class QuadTreeTable : public ILookUp
		{
		public:
			QuadTreeTable(IStorageManager& storage, uint_32 pageSize);
			virtual ~QuadTreeTable();

			virtual double queryMinDistance(const MapPos2D& sNodePos, uint_32 dGridID) const;
			virtual void loadTable(uint_32 destGridID, uint_8** data) const;
			virtual double queryMinDistance(const MapPos2D& src, const uint_8* table) const;
			virtual uint_32 getGridID(const MapPos2D& pos) const;

		public:
			static const uint_32 HEADER_PAGE		= 0;

			static const uint_32 NODE_ID_SIZE	= 2;
			static const uint_32 NODE_ID_GAP		= 0x7fff;
			static const uint_32 NODE_ID_CARRY	= 0x8000;
			static const uint_32 NODE_ID_ENDLESS	= 0xffff;

		private:
			class Node
			{
			public:
				Node(uint_8** ptr);
				~Node();

				bool isLeaf() const { return m_child[0] == NULL; }

			public:
				Node* m_child[4];
				uint_32 m_id;

				//static int non_leaf_num;
				//static int all_leaf_num;

			}; // Node

		private:
			void loadHeader();

		private:
			IStorageManager* m_buffer;

			uint_32 m_pageSize;
			uint_32 m_gridNum;

			MapScope m_mapScope;
			uint_32 m_dataStartPage;
			Node* m_root;

		}; // QuadTreeTable
	}
}

#endif // _EMBEDDED_NETWORK_QUADTREE_TABLE_H_
