#ifndef _EMBEDDED_SPATIALINDEX_NODE_H_
#define _EMBEDDED_SPATIALINDEX_NODE_H_
#include "../Tools/SpatialDataBase.h"
#include "../BasicType/Rect.h"

namespace Embedded
{
	namespace SpatialIndex
	{

		class StaticRTree;
		class Node;

		typedef PoolPointer<Node> NodePtr;

		class Node : public INode
		{
		public:
			virtual ~Node();

			//
			// IObject interface
			//
			virtual IObject* clone();

			//
			// ISerializable interface
			//
			virtual uint_32 getByteArraySize() const;
			virtual void loadFromByteArray(const uint_8* data);
			virtual void storeToByteArray(uint_8** data, uint_32& len) const;

			//
			// SpatialIndex::IEntry interface
			//
			virtual id_type getIdentifier() const;
			virtual void getShape(IShape** out) const;

			//
			// SpatialIndex::INode interface
			//
			virtual uint_32 getChildrenCount() const;
			virtual id_type getChildIdentifier(uint_32 index)  const;
			virtual void setChildIdentifier(uint_32 index, id_type id);
			virtual void getChildShape(id_type id, IShape** out)  const;
			virtual void getChildShape(uint_32 index, IShape** out)  const;
			virtual uint_32 getLevel() const;
			virtual bool isIndex() const;
			virtual bool isLeaf() const;

			RectPtr getRectPtr() const;

			static uint_32 getIndexByteArraySize(uint_32 cChild);

		public:
			// Road ID struct:
			static const uint_32 EDGE_ID_SIZE			= 8;

			// Node info
			static const uint_32 CHILDREN_COUNT_SIZE		= 2;
			static const uint_32 MBR_SIZE				= 2 * 2 * 4;

			// Each child info
			static const uint_32 EACH_CHILD_SIZE_SIZE	= 3;

		private:
			uint_32 m_children;
				// The number of children pointed by this node.

			Rect m_nodeMBR;
				// The minimum bounding Rect enclosing all data contained in the node.

			uint_8** m_pData;
				// The data stored in the node.

			RectPtr* m_ptrMBR;
				// The corresponding data MBRs.

			id_type* m_pIdentifier;
				// The corresponding data identifiers.

			uint_32* m_pDataLength;
				// The length of each child. 
				// No value is null when it is leaf node.

		private:
			Node(StaticRTree* pTree, id_type id);

			void insertEntry(uint_32 dataLength, uint_8* pData, const Rect& mbr, id_type id);

			static void readMBRFromByte(Rect& r, const uint_8*& ptr);
			static void writeMBRToByte(const Rect& r, uint_8*& ptr);

		private:
			id_type m_identifier;
				// The unique ID of this node.

			mutable bool m_bLeaf;
				// The Node is Index or Leaf.

			StaticRTree* m_tree;
				// Point to SpatialIndex

			uint_32 m_totalDataLength;

			friend class PointerPool<Node>;
			friend class StaticRTree;
			friend class PointBindLine;

		}; // Node
	}
}

#endif // _EMBEDDED_SPATIALINDEX_NODE_H_