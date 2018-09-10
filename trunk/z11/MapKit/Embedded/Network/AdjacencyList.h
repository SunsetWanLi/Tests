#ifndef _EMBEDDED_NETWORK_ADJACENCY_LIST_H_
#define _EMBEDDED_NETWORK_ADJACENCY_LIST_H_
#include "Utility.h"

namespace Embedded
{
	namespace Network
	{
		struct S_CONN
		{
			uint_32 nodeID; //zj 最高位为0，表示此节点没有对应的HighID, 最高位为1表示有对应的HighID，但它本身并不是HighID.
			uint_64 roadID;
			uint_32 length;
			int getFrc(){return 1;} ;
		}; // S_CONN

		struct M_CONN
		{
			uint_32 nodeID; // zj 最高位为0，表示此id为最低层的id, 即LowID， 最高位为1表示对应的HighID，本身即是。当连接的点是另一个区中的时，会是LowID（也即跨区）
			uint_32 length;
		}; // Connection

		class AdjacencyList
		{
		public:
			static void decodeTopoEdgeID(uint_64 id, uint_32* layerID, id_type* nodeID, uint_32* offset);
			static uint_32 encodeTopoNodeID(uint_32 page, uint_32 offset);
			static void decodeTopoNodeID(uint_32 id, uint_32* page, uint_32* offset);
			static uint_32 decodeTopoEdgeLength(uint_32 storage);

			// Low
			static uint_32 fetchLowNodeCount(const uint_8* pageStart);
			static uint_32 fetchLowConnectionSetID(const uint_8* itemStart);
			static void fetchLowCoordinate(const uint_8* itemStart, MapPos2D* pos);
			static void fetchLowConnectionInfo(const uint_8* itemStart, std::vector<S_CONN>* conns);
			static uint_32 computeLowDataOffset(const MapPos2D& p, uint_8* data);

			static void eraseNodeIDFlag(uint_32& _id) { _id &= TOPO_NODE_GAP; }
			static bool isSetNodeIDFlag(uint_32 id) { return (id & TOPO_NODE_CARRY) != 0; }

			// High
			static void fetchHighConnectionInfo(const uint_8* itemStart, std::vector<M_CONN>* conns);
			static uint_32 fetchSingleTopoNodeID(const uint_8* itemStart);
			static bool isNodeInRect(const MapPos2D& p, const Rect& r);

		private:
			// Node
			static const uint_32 TOPO_NODE_GAP				= 0x7fffffff;
			static const uint_32 TOPO_NODE_CARRY				= 0x80000000;

			static const uint_32 GRID_SIZE_BIT;
			static const uint_32 OFFSET_SIZE_BIT;

			// Edge
			static const uint_32 EDGE_LENGTH_CARRY			= 0x8000;
			static const uint_32 EDGE_LENGTH_ENDLESS			= 0xffff;
			static const uint_32 EDGE_LENGTH_CONDENSE_BITS	= 10;

			static const uint_32 GRID_ID_SIZE				= 5;
			static const uint_32	DATA_OFFSET_SIZE			= 2;
			static const uint_64 LAYER_ID_MASK				= 0xff00000000000000ULL;
			static const uint_64 GRID_ID_MASK				= 0x00ffffffffff0000ULL;
			static const uint_64 DATA_OFFSET_MASK			= 0x000000000000ffffULL;

			// Low
			static const uint_32 L_COORD_SIZE				= 4;
			static const uint_32 L_CONNECTION_ID_SIZE		= 2;
			static const uint_32 L_DATA_COUNT_SIZE			= 2;
			static const uint_32 L_NODE_ID_SIZE				= 4;
			static const uint_32 L_EDGE_ID_SIZE				= 8;
			static const uint_32 L_EDGE_LENGTH_SIZE			= 2;
			static const uint_32 L_CONNECTION_ITEM_SIZE		= L_NODE_ID_SIZE + L_EDGE_ID_SIZE + L_EDGE_LENGTH_SIZE;

			// High
			static const uint_32 H_DATA_COUNT_SIZE			= 2;
			static const uint_32 H_SINGLE_TOPO_NODE_ID_SIZE	= 4;
			static const uint_32 H_EDGE_COUNT_SIZE			= 1;
			static const uint_32 H_MULTI_TOPO_NODE_ID_SIZE	= 4;
			static const uint_32 H_EDGE_LENGTH_SIZE			= 4;
			static const uint_32 H_CONNECTION_ITEM_SIZE		= H_MULTI_TOPO_NODE_ID_SIZE + H_EDGE_LENGTH_SIZE;

		}; // AdjacencyList
	}
}

#endif // _EMBEDDED_NETWORK_ADJACENCY_LIST_H_
