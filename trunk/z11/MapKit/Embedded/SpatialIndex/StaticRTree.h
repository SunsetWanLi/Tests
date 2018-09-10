#ifndef _EMBEDDED_SPATIALINDEX_STATIC_RTREE_H_
#define _EMBEDDED_SPATIALINDEX_STATIC_RTREE_H_
#include "Utility.h"
#include "PointerPoolNode.h"
#include "../Tools/SpatialDataBase.h"
#include "../BasicType/Rect.h"
#include "System/Thread/MGMutex.h"
namespace Embedded
{
	namespace SpatialIndex
	{
		class StaticRTree : public ISpatialIndex
		{
		public:
			StaticRTree(IStorageManager&, PropertySet&);
			// String                   Value		Description
			// ----------------------------------------------
			// IndexIdentifier			VT_LONG		If specified an existing index will be openened from the supplied
			//										storage	manager with the given index id. Behaviour is unspecified
			//										if the index id or the storage manager are incorrect.
			// DataType					VT_LONG		One of Point, Line or Polygon
			// NodePoolCapacity			VT_LONG		Default is 100
			// RectPoolCapacity			VT_LONG		Default is 1000
			// PointPoolCapacity        VT_LONG		Default is 500
			// CondenseStrategy			VT_LONG		The method to condense geographic data.

			virtual ~StaticRTree();

		public:
			//
			// ISpatialIndex interface
			//

			virtual void insertData(uint_32 len, const uint_8* pData, const IShape& shape, id_type objID, id_type* nodeID);
			virtual bool deleteData(const IShape& shape, id_type shapeIdentifier);
			virtual void containsWhatQuery(const IShape& query, IVisitor& v, bool isFine);
			virtual void intersectsWithQuery(const IShape& query, IVisitor& v, bool isFine);
			virtual void pointLocationQuery(const Point& query, IVisitor& v, bool isFine);
			virtual void nearestNeighborQuery(uint_32 k, const IShape& query, IVisitor& v, INearestNeighborComparator& nnc);
			virtual void nearestNeighborQuery(uint_32 k, const IShape& query, IVisitor& v);
			virtual void selfJoinQuery(const IShape& s, IVisitor& v);
			virtual void queryStrategy(IQueryStrategy& qs);
			virtual bool countIntersectsQuery(const IShape& query, ICountVisitor& v);
			virtual void getIndexProperties(PropertySet& out) const;
			virtual bool isIndexValid();
			virtual void getStatistics(IStatistics** out) const;

			virtual void readData(id_type nodeID, id_type objID, IShape** out);
			virtual id_type nextObjID();
			virtual void setDisplayRegion(const Rect& scope);
			virtual const Rect& getDisplayRegion();
			virtual ShapeType getShapeType(){ return m_shapeType; }

			void readDataByAddress(id_type nodeID, uint_32 offset, IShape** out) const;

			const ICondenseStrategy* getCondenseStrategy() { return m_condenseStrategy; }

		private:
			void initNew(PropertySet&);
			void initOld(PropertySet& ps);
			void storeHeader();
			void loadHeader();

			void rangeQuery(RangeQueryType type, const IShape& query, IVisitor&, bool);

			id_type writeNode(Node*);
			NodePtr readNode(id_type id);
			void deleteNode(Node*);

			void setFirstDisplayScope();

		private:
			class CountRoadVisitor : public ICountVisitor
			{
			public:
				CountRoadVisitor(uint_32 limit) : m_upperLimit(limit), m_count(0) {}

				virtual void countNode(const INode& n) { m_count += n.getChildrenCount(); }
				virtual void countData(const INode& in, uint_32 iData, const IShape& query) { ++m_count; }
				virtual bool overUpperLimit() const { return m_count > m_upperLimit; }

			private:
				uint_32 m_upperLimit;
				uint_32 m_count;

			}; // CountRoadVisitor

		private:
			ShapeType m_shapeType;
			IStorageManager* m_pStorageManager;

			id_type m_rootID;
			id_type m_headerID;

			Rect m_infiniteRect;
			Rect m_firstDisplayScope;

			uint_32 m_maxDisplayCount;

			ICondenseStrategy* m_condenseStrategy;

			PointerPool<Rect> m_rectPool;
			PointerPool<Node> m_nodePool;
//#ifdef PTHREADS
//			pthread_rwlock_t m_rwLock;
//#else
//			bool m_rwLock;
//#endif
            z11::MGMutex m_rwLock;

			friend class Node;
			friend class PointBindLine;

		}; // StaticRTree

		ISpatialIndex* returnStaticRTree(IStorageManager& sm, PropertySet& ps);
		ISpatialIndex* createNewStaticRTree(
			IStorageManager& sm,
			id_type& indexIdentifier,
			uint_32 condenseStrategy
			);
		ISpatialIndex* loadStaticRTree(IStorageManager& sm, id_type indexIdentifier);
	}
}

#endif // _EMBEDDED_SPATIALINDEX_STATIC_RTREE_H_
