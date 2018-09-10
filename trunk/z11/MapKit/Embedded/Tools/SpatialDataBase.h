#ifndef _EMBEDDED_SPATIALDATABASE_H_
#define _EMBEDDED_SPATIALDATABASE_H_
#include "Tools.h"
#include "../BasicType/BasicGMType.h"

//GRF 11-10-17
//# if !HAVE_BZERO
//#  define bzero(d, n) memset((d), 0, (n))
//# endif

namespace Embedded
{
	class Point;
	class Line;
	class Rect;

	namespace Network
	{
		class BindPoint;
	}

	typedef int_64 id_type;
	typedef PoolPointer<Rect> RectPtr;
	typedef PoolPointer<Point> PointPtr;

	enum CondenseStrategyType
	{
		CS_NO_OFFSET,
		CS_CERTAINTY_OFFSET,
		CS_UNCERTAINTY_OFFSET
	};

	enum ShapeType
	{
		ST_POINT = 0x0,
		ST_LINE,
		ST_REGION,
		ST_POLYGON,
		ST_UNKNOWN
	};

	//
	// Interfaces
	//

	class IShape : public virtual ISerializable
	{
	public:
		virtual bool intersectsShape(const IShape& in) const = 0;
		virtual bool containsShape(const IShape& in) const = 0;
		virtual bool touchesShape(const IShape& in) const = 0;
		virtual void getCenter(Point& out) const = 0;
		virtual void getMBR(Rect& out) const = 0;
		virtual double getArea() const = 0;
		virtual double getMinimumDistance(const IShape& in) const = 0;
		virtual const z11::MGWString& getName() const = 0;
		virtual ~IShape() {}
	}; // IShape

	class IEntry : public IObject
	{
	public:
		virtual id_type getIdentifier() const = 0;
		virtual void getShape(IShape** out) const = 0;
		virtual ~IEntry() {}
	}; // IEntry

	class INode : public IEntry, public ISerializable
	{
	public:
		virtual uint_32 getChildrenCount() const = 0;
		virtual id_type getChildIdentifier(uint_32 index) const = 0;
		virtual void setChildIdentifier(uint_32 index, id_type id) = 0;
		virtual void getChildShape(id_type id, IShape** out) const = 0;
		virtual void getChildShape(uint_32 index, IShape** out) const = 0;
		virtual uint_32 getLevel() const = 0;
		virtual bool isIndex() const = 0;
		virtual bool isLeaf() const = 0;
		virtual ~INode() {}
	}; // INode

	class IData : public IEntry
	{
	public:
		virtual void getData(uint_32& len, uint_8** data) const = 0;
		virtual ~IData() {}
	}; // IData

	class INearestNeighborComparator
	{
	public:
		virtual double getMinimumDistance(const IShape& query, const IShape& entry) = 0;
		virtual double getMinimumDistance(const IShape& query, const IData& data) = 0;
		virtual ~INearestNeighborComparator() {}
	}; // INearestNeighborComparator

	class IStorageManager
	{
	public:
		virtual void loadByteArray(const id_type id, uint_32& len, uint_8** data) = 0;
		virtual void storeByteArray(id_type& id, const uint_32 len, const uint_8* const data) = 0;
		virtual void deleteByteArray(const id_type id) = 0;
		virtual uint_32 getPage0Capacity() = 0;
		virtual ~IStorageManager() {}
	}; // IStorageManager

	class IVisitor
	{
	public:
		virtual bool shouldCheckEachData() const = 0;
		virtual void visitNode(const INode& in) = 0;
		virtual void visitData(const IData& in, uint_32 dataIndex) = 0;
		virtual void visitData(std::vector<const IData*>& v) = 0;
		virtual void visitData(const INode& in) = 0;
		virtual ~IVisitor() {}
	}; // IVisitor

	class ICountVisitor
	{
	public:
		virtual void countNode(const INode& in) = 0;
		virtual void countData(const INode& in, uint_32 iData, const IShape& query) = 0;
		virtual bool overUpperLimit() const = 0;
		virtual ~ICountVisitor() {}
	}; // ICountVisitor

	class IQueryStrategy
	{
	public:
		virtual void getNextEntry(const IEntry& previouslyFetched, id_type& nextEntryToFetch, bool& bFetchNextEntry) = 0;
		virtual bool shouldStoreEntry() = 0;
		virtual ~IQueryStrategy() {}
	}; // IQueryStrategy

	class IStatistics
	{
	public:
		virtual uint_32 getReads() const = 0;
		virtual uint_32 getWrites() const = 0;
		virtual uint_32 getNumberOfNodes() const = 0;
		virtual uint_32 getNumberOfData() const = 0;
		virtual ~IStatistics() {}
	}; // IStatistics

	class ISpatialIndex
	{
	public:
		virtual void insertData(uint_32 len, const uint_8* pData, const IShape& shape, id_type objID, id_type* nodeID) = 0;
		virtual bool deleteData(const IShape& shape, id_type shapeIdentifier) = 0;
		virtual void containsWhatQuery(const IShape& query, IVisitor& v, bool isFine)  = 0;
		virtual void intersectsWithQuery(const IShape& query, IVisitor& v, bool isFine) = 0;
		virtual void pointLocationQuery(const Point& query, IVisitor& v, bool isFine) = 0;
		virtual void nearestNeighborQuery(uint_32 k, const IShape& query, IVisitor& v, INearestNeighborComparator& nnc) = 0;
		virtual void nearestNeighborQuery(uint_32 k, const IShape& query, IVisitor& v) = 0;
		virtual void selfJoinQuery(const IShape& s, IVisitor& v) = 0;
		virtual void queryStrategy(IQueryStrategy& qs) = 0;
		virtual bool countIntersectsQuery(const IShape& query, ICountVisitor& v) = 0;
		virtual void getIndexProperties(PropertySet& out) const = 0;
		virtual bool isIndexValid() = 0;
		virtual void getStatistics(IStatistics** out) const = 0;

		virtual void readData(id_type nodeID, id_type objID, IShape** out) = 0;
		virtual id_type nextObjID() = 0;
		virtual void setDisplayRegion(const Rect& scope) = 0;
		virtual const Rect& getDisplayRegion() = 0;
		virtual ShapeType getShapeType() = 0;

		virtual ~ISpatialIndex() {}
	}; // ISpatialIndex

	class ICondenseStrategy
	{
	public:
		// Only the IShape data size, not including the Node info.
		virtual uint_32 getType() const = 0;
		virtual uint_32 allShapeDataSize(const INode* n) const = 0;
		virtual uint_32 dataSize(const IShape* s) const = 0;
		virtual void loadFromByteArray(IShape** s, const uint_8* data, uint_32 length) const = 0;
		virtual void storeToByteArray(const IShape* s, uint_8** data, uint_32& length) const = 0;
		virtual void setShapeType(ShapeType st) = 0;

		virtual ~ICondenseStrategy() {}
	}; // ICondenseStrategy

	class ILookUp
	{
	public:
		virtual double queryMinDistance(const MapPos2D& sNodePos, uint_32 dGridID) const = 0;

		virtual void loadTable(uint_32 destGridID, uint_8** data) const = 0;
		virtual double queryMinDistance(const MapPos2D& src, const uint_8* table) const = 0;
		virtual uint_32 getGridID(const MapPos2D& pos) const = 0;

		virtual ~ILookUp() {}
	}; // ILookUp

	class IPointBindLine
	{
	public:
		virtual void bind(const MapPos2D& p, Line** line, uint_64* id) const = 0;
		virtual void bind(const MapPos2D& p, double course,Line** line, uint_64* id) const = 0;
		virtual ~IPointBindLine() {}
	}; // IPointBindLine

	struct SearchingInfo
	{
		struct LocationInfo
		{
			MapPos2D m_realPos;
			MapPos2D m_bindPos;
			bool m_bDir;
			
			LocationInfo(const MapPos2D& pos) : m_realPos(pos),m_use_course(false),m_course(0) {}
			LocationInfo(const MapPos2D& pos,double _course):m_realPos(pos),m_use_course(true),m_course(_course){}
			bool m_use_course;
			double m_course;
		}; // LocationInfo


		LocationInfo m_start, m_end;
		std::vector<uint_64> m_roadIDs;

		SearchingInfo(const MapPos2D& s, const MapPos2D& e) : m_start(s), m_end(e) {is_adjacent = false;}
		SearchingInfo(const MapPos2D& s,double s_course,const MapPos2D& e,double e_course):m_start(s,s_course),m_end(e,e_course)
		{
			if (s_course<0)//[0,360]
			{
				m_start.m_use_course=false;
			}
			if (e_course<0)//[0,360]
			{
				m_end.m_use_course=false;
			}
            is_adjacent = false;
			
		}
        bool is_adjacent;

	}; // SearchingInfo

	class INetwork
	{
	public:
		virtual double computeShortestPath(SearchingInfo& info) const = 0;
		virtual bool bindPoint(const MapPos2D& s,Network::BindPoint &bind_pt) const
		{
			return false;
		}
		virtual bool bindPoint(const MapPos2D& s,double course,Network::BindPoint &bind_pt) const
		{
			return false;
		}
		virtual ~INetwork() {}
	}; // INetwork

	namespace StorageManager
	{
		enum StorageManagerConstants
		{
			EmptyPage = -0x1,
			NewPage = -0x1,
			HeaderPage = 0
		};

		class IBuffer : public IStorageManager
		{
		public:
			virtual uint_32 getHits() = 0;
			virtual void clear() = 0;
			virtual ~IBuffer() {}
		}; // IBuffer

		extern IStorageManager* returnStaticDiskStorageManager(PropertySet& in);
		extern IStorageManager* createNewStaticDiskStorageManager(const z11::MGWString& baseName, uint_32 pageSize);
		extern IStorageManager* loadStaticDiskStorageManager(const z11::MGWString& baseName);

		//
		extern IStorageManager* returnDiskStorageManager(PropertySet& in);
		extern IStorageManager* createNewDiskStorageManager(const z11::MGWString& baseName, uint_32 pageSize);
		extern IStorageManager* loadDiskStorageManager(const z11::MGWString& baseName);


		extern IBuffer* returnFIFOBuffer(IStorageManager& ind, PropertySet& in);
		extern IBuffer* createNewFIFOBuffer(IStorageManager& in, uint_32 capacity, bool bWriteThrough);
	}
}

#endif // _EMBEDDED_SPATIALDATABASE_H_

#define PUBLISH_PAGE_SIZE_SHIFT 16
#define PUBLISH_PAGE_SIZE (1<<PUBLISH_PAGE_SIZE_SHIFT)
#define PUBLISH_PAGE_SIZE_MASK (PUBLISH_PAGE_SIZE-1)