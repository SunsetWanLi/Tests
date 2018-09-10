#include "StaticRTree.h"
#include "../Features/Data.h"
#include "../Features/Condense.h"
#include <string.h>

using namespace Embedded;
using namespace Embedded::SpatialIndex;

ISpatialIndex* SpatialIndex::returnStaticRTree(IStorageManager& sm, PropertySet& ps)
{
	ISpatialIndex* si = new StaticRTree(sm, ps);
	return si;
}

ISpatialIndex* SpatialIndex::createNewStaticRTree(
	IStorageManager& sm,
	id_type& indexIdentifier,
	uint_32 condenseStrategy
	)
{
	Variant var;
	PropertySet ps;

	var.m_varType = VT_LONG;
	var.m_val.lVal = condenseStrategy;
	ps.setProperty("CondenseStrategy", var);

	ISpatialIndex* ret = SpatialIndex::returnStaticRTree(sm, ps);

	var = ps.getProperty("IndexIdentifier");
	indexIdentifier = var.m_val.lVal;

	return ret;
}

ISpatialIndex* SpatialIndex::loadStaticRTree(IStorageManager& sm, id_type indexIdentifier)
{
	Variant var;
	PropertySet ps;

	var.m_varType = VT_LONG;
	var.m_val.lVal = static_cast<int_32>(indexIdentifier);
	ps.setProperty("IndexIdentifier", var);

	return SpatialIndex::returnStaticRTree(sm, ps);
}

StaticRTree::StaticRTree(IStorageManager& sm, PropertySet& ps) :
	m_pStorageManager(&sm),
	m_rootID(0),
	m_headerID(StorageManager::NewPage),
	m_maxDisplayCount(200),
	m_rectPool(1000),
	m_nodePool(100),
	m_condenseStrategy(NULL)
{
//#ifdef PTHREADS
//	pthread_rwlock_init(&m_rwLock, NULL);
//#else
//	m_rwLock = false;
//#endif

	Variant var = ps.getProperty("IndexIdentifier");
	if (var.m_varType != VT_EMPTY)
	{
		if (var.m_varType != VT_LONG) throw IllegalArgumentException(
			"StaticRTree: Property IndexIdentifier must be VT_LONG"
			);
		m_headerID = var.m_val.lVal;
		initOld(ps);
	}
	else
	{
		initNew(ps);
		var.m_varType = VT_LONG;
		var.m_val.lVal = static_cast<int_32>(m_headerID);
		ps.setProperty("IndexIdentifier", var);
	}
}

StaticRTree::~StaticRTree()
{
//#ifdef PTHREADS
//	pthread_rwlock_destroy(&m_rwLock);
//#endif

	setFirstDisplayScope();

	//storeHeader();
}

//
// ISpatialIndex interface
//

void StaticRTree::insertData(uint_32 len, const uint_8* pData, const IShape& shape, id_type objID, id_type* nodeID)
{
	throw NotSupportedException("Should never be called. ");
}

bool StaticRTree::deleteData(const IShape& shape, id_type shapeIdentifier)
{
	throw NotSupportedException("Should never be called. ");
}

void StaticRTree::containsWhatQuery(const IShape& query, IVisitor& v, bool isFine)
{
	throw NotSupportedException("Should never be called. ");
}

void StaticRTree::intersectsWithQuery(const IShape& query, IVisitor& v, bool isFine)
{
	rangeQuery(IntersectionQuery, query, v, isFine);
}

void StaticRTree::pointLocationQuery(const Point& query, IVisitor& v, bool isFine)
{
	throw NotSupportedException("Should never be called. ");
}

void StaticRTree::nearestNeighborQuery(uint_32 k, const IShape& query, IVisitor& v, INearestNeighborComparator& nnc)
{
	throw NotSupportedException("Should never be called. ");
}

void StaticRTree::nearestNeighborQuery(uint_32 k, const IShape& query, IVisitor& v)
{
	throw NotSupportedException("Should never be called. ");
}

void StaticRTree::selfJoinQuery(const IShape& s, IVisitor& v)
{
	throw NotSupportedException("Should never be called. ");
}

void StaticRTree::queryStrategy(IQueryStrategy& qs)
{
//#ifdef PTHREADS
//	SharedLock lock(&m_rwLock);
//#else
//	if (m_rwLock == false) m_rwLock = true;
//	else throw ResourceLockedException("queryStrategy: cannot acquire a shared lock");
//#endif
    m_rwLock.lock();

	id_type next = m_rootID;
	bool hasNext = true;

//	try
//	{
		while (hasNext)
		{
			NodePtr n = readNode(next);
			qs.getNextEntry(*n, next, hasNext);

			if (qs.shouldStoreEntry())
				writeNode(n.get());
		}

//#ifndef PTHREADS
//		m_rwLock = false;
//#endif
        m_rwLock.unlock();
//	}
//	catch (...)
//	{
//#ifndef PTHREADS
//		m_rwLock = false;
//#endif
//		throw;
//	}
}

bool StaticRTree::countIntersectsQuery(const IShape& query, ICountVisitor& v)
{
	std::stack<NodePtr> st;

	NodePtr root = readNode(m_rootID);
	if (root->m_children > 0 && query.intersectsShape(root->m_nodeMBR))
		st.push(root);

	while (! st.empty())
	{
		NodePtr n = st.top(); st.pop();

		if (n->isLeaf())
		{
			IShape* nodeMBR;
			n->getShape(&nodeMBR);
			bool bNodeIn = query.containsShape(*nodeMBR);
			delete nodeMBR;

			if (bNodeIn)
			{
				v.countNode(*n.get());
				if (v.overUpperLimit())
					return true;
			}
			else
			{
				for (uint_32 cChild = 0; cChild < n->m_children; cChild++)
				{
					if (query.intersectsShape(*(n->m_ptrMBR[cChild])))
					{
						v.countData(*n.get(), cChild, query);
						if (v.overUpperLimit())
							return true;
					}
				}
			}
		}
		else
		{
			for (uint_32 cChild = 0; cChild < n->m_children; cChild++)
			{
				if (query.intersectsShape(*(n->m_ptrMBR[cChild])))
					st.push(readNode(n->m_pIdentifier[cChild]));
			}
		}
	}

	return false;
}

void StaticRTree::getIndexProperties(PropertySet& out) const
{
	throw NotSupportedException("Should never be called. ");
}

bool StaticRTree::isIndexValid()
{
	throw NotSupportedException("Should never be called. ");
}

void StaticRTree::getStatistics(IStatistics** out) const
{
	throw NotSupportedException("Should never be called. ");
}

void StaticRTree::readData(id_type nodeID, id_type objID, IShape** out)
{
	throw NotSupportedException("Should never be called. ");
}

id_type StaticRTree::nextObjID()
{
	throw NotSupportedException("Should never be called. ");
}

void StaticRTree::setDisplayRegion(const Rect& scope)
{
	m_firstDisplayScope = scope;
}

const Rect& StaticRTree::getDisplayRegion()
{
	return m_firstDisplayScope;
}

void StaticRTree::readDataByAddress(id_type nodeID, uint_32 offset, IShape** s) const
{
	uint_32 dataLength;
	uint_8* buffer;

	try
	{
		m_pStorageManager->loadByteArray(nodeID, dataLength, &buffer);
	}
	catch (InvalidPageException& e)
	{
		std::cerr << e.what() << std::endl;
		//std::cerr << *this << std::endl;
		throw IllegalStateException("readTopoEdge: failed with InvalidPageException");
	}

	uint_8* ptr = buffer + offset;
	ptr += Node::MBR_SIZE + Node::EDGE_ID_SIZE;
	uint_32 len = 0;
	memcpy(&len, ptr, Node::EACH_CHILD_SIZE_SIZE);
	ptr += Node::EACH_CHILD_SIZE_SIZE;

	m_condenseStrategy->loadFromByteArray(s, ptr, len);

	delete[] buffer;
}

void StaticRTree::initNew(PropertySet& ps)
{
	Variant var;

	// node pool capacity
	var = ps.getProperty("NodePoolCapacity");
	if (var.m_varType != VT_EMPTY)
	{
		if (var.m_varType != VT_LONG)
			throw IllegalArgumentException("initNew: Property IndexPoolCapacity must be VT_LONG");

		m_nodePool.setCapacity(var.m_val.lVal);
	}

	// rect pool capacity
	var = ps.getProperty("RectPoolCapacity");
	if (var.m_varType != VT_EMPTY)
	{
		if (var.m_varType != VT_LONG)
			throw IllegalArgumentException("initNew: Property RectPoolCapacity must be VT_LONG");

		m_rectPool.setCapacity(var.m_val.lVal);
	}

	var = ps.getProperty("CondenseStrategy");
	if (var.m_varType != VT_EMPTY)
	{
		if (var.m_varType != VT_LONG)
			throw IllegalArgumentException("initNew: Property CondenseStrategy must be VT_LONG");

		CondenseStrategyType cs
			= static_cast<CondenseStrategyType>(var.m_val.lVal);
		switch (cs)
		{
		case CS_NO_OFFSET:
			m_condenseStrategy = new Features::NoOffsetCondenseStrategy;
			break;
		default:
			throw IllegalArgumentException("Unknown bulk condense strategy.");
			break;
		}
	}

	m_infiniteRect.makeInfinite();

	Node root(this, -1);
	m_rootID = writeNode(&root);

	storeHeader();
}

void StaticRTree::initOld(PropertySet& ps)
{
	loadHeader();

	// only some of the properties may be changed.
	// the rest are just ignored.

	Variant var;

	// index pool capacity
	var = ps.getProperty("NodePoolCapacity");
	if (var.m_varType != VT_EMPTY)
	{
		if (var.m_varType != VT_LONG)
			throw IllegalArgumentException("initOld: Property IndexPoolCapacity must be VT_LONG");

		m_nodePool.setCapacity(var.m_val.lVal);
	}

	// rect pool capacity
	var = ps.getProperty("RectPoolCapacity");
	if (var.m_varType != VT_EMPTY)
	{
		if (var.m_varType != VT_LONG)
			throw IllegalArgumentException("initOld: Property RectPoolCapacity must be VT_LONG");

		m_rectPool.setCapacity(var.m_val.lVal);
	}

	m_infiniteRect.makeInfinite();
}

void StaticRTree::storeHeader()
{
	const uint_32 headerSize =
		sizeof(id_type)	+							// m_rootID
		sizeof(Rect) +								// m_firstDisplayScope
		sizeof(uint_32)+ 							// m_condenseStrategy
		sizeof(uint_32)+								// firstAttrPage
		sizeof(ShapeType);								//m_shapeType


	uint_8* header = new uint_8[headerSize];
	uint_8* ptr = header;

	memcpy(ptr, &m_rootID, sizeof(id_type));
	ptr += sizeof(id_type);

	uint_32 lenRect = m_firstDisplayScope.getByteArraySize();
	if (m_firstDisplayScope.isEmpty())
	{
		memset(ptr, 0, lenRect);
	}
	else
	{
		uint_8* pRect; uint_32 lenRect;
		m_firstDisplayScope.storeToByteArray(&pRect, lenRect);
		memcpy(ptr, pRect, lenRect);
		delete[] pRect;
	}
	ptr += lenRect;

	uint_32 type = m_condenseStrategy->getType();
	memcpy(ptr, &type, sizeof(uint_32));
	ptr += sizeof(uint_32);

	ptr += sizeof(uint_32); // firstAttrPage

	memcpy(ptr, &m_shapeType, sizeof(ShapeType));
	ptr += sizeof(ShapeType);



	m_pStorageManager->storeByteArray(m_headerID, headerSize, header);

	delete[] header;
}

void StaticRTree::loadHeader()
{
	uint_32 headerSize;
	uint_8* header = 0;
	m_pStorageManager->loadByteArray(m_headerID, headerSize, &header);

	uint_8* ptr = header;

	memcpy(&m_rootID, ptr, sizeof(id_type));
	ptr += sizeof(id_type);

	uint_32 rectSize = m_firstDisplayScope.getByteArraySize();
	uint_8* pRect = new uint_8[rectSize];
	memcpy(pRect, ptr, rectSize);
	m_firstDisplayScope.loadFromByteArray(pRect);
	delete pRect;
	ptr += rectSize;

	uint_32 type;
	memcpy(&type, ptr, sizeof(uint_32));
	ptr += sizeof(uint_32);

	//uint_32 firstAttrPage;
	//memcpy(&firstAttrPage, ptr, sizeof(uint_32));
	ptr += sizeof(uint_32);

	memcpy(&m_shapeType, ptr, sizeof(ShapeType));
	ptr += sizeof(ShapeType);
	assert(m_shapeType == ST_POINT || m_shapeType == ST_LINE || m_shapeType == ST_POLYGON);

	CondenseStrategyType cs
		= static_cast<CondenseStrategyType>(type);
	switch (cs)
	{
	case CS_NO_OFFSET:
		m_condenseStrategy = new Features::NoOffsetCondenseStrategy;
		m_condenseStrategy->setShapeType(m_shapeType);
		break;
	default:
		throw IllegalArgumentException("Unknown bulk condense strategy.");
		break;
	}

	delete[] header;
}

void StaticRTree::rangeQuery(RangeQueryType type, const IShape& query, IVisitor& v, bool isFine)
{
//#ifdef PTHREADS
//	SharedLock lock(&m_rwLock);
//#else
//	if (m_rwLock == false) m_rwLock = true;
//	else throw ResourceLockedException("rangeQuery: cannot acquire a shared lock");
//#endif
    m_rwLock.lock();
//	try
//	{
		std::stack<NodePtr> st;

		NodePtr root = readNode(m_rootID);
		if (root->m_children > 0 && query.intersectsShape(root->m_nodeMBR))
			st.push(root);

		while (! st.empty())
		{
			NodePtr n = st.top(); st.pop();

			if (n->isLeaf())
			{
				v.visitNode(*n);

				IShape* nodeMBR;
				n->getShape(&nodeMBR);
				bool bNodeIn = dynamic_cast<Rect*>(const_cast<IShape*>(&query))
					&& query.containsShape(*nodeMBR);
				delete nodeMBR;

				if (bNodeIn)
				{
					v.visitData(*n);

					if (v.shouldCheckEachData())
					{
						for (uint_32 cChild = 0; cChild < n->m_children; cChild++)
						{
							Features::Data data = Features::Data(n->m_pDataLength[cChild], n->m_pData[cChild], *(n->m_ptrMBR[cChild]), n->m_pIdentifier[cChild], m_condenseStrategy);
							v.visitData(data, cChild);
						}
					}
				}
				else
				{
					for (uint_32 cChild = 0; cChild < n->m_children; cChild++)
					{
						bool b;

						if (bNodeIn)
						{
							b = true;
						}
						else
						{
							if (type == ContainmentQuery)
							{
								b = query.containsShape(*(n->m_ptrMBR[cChild]));
								if (isFine && !b)
								{
									IShape* pS;
									Features::Data(n->m_pDataLength[cChild], n->m_pData[cChild], *(n->m_ptrMBR[cChild]), n->m_pIdentifier[cChild], m_condenseStrategy).getShape(&pS);
									b = query.containsShape(*pS);
									delete pS;
								}
							}
							else
							{
								b = query.intersectsShape(*(n->m_ptrMBR[cChild]));
								if (isFine && b)
								{
									IShape* pS;
									Features::Data(n->m_pDataLength[cChild], n->m_pData[cChild], *(n->m_ptrMBR[cChild]), n->m_pIdentifier[cChild], m_condenseStrategy).getShape(&pS);
									b = query.intersectsShape(*pS);
									delete pS;
								}
							}
						}

						if (b)
						{
							Features::Data data = Features::Data(n->m_pDataLength[cChild], n->m_pData[cChild], *(n->m_ptrMBR[cChild]), n->m_pIdentifier[cChild], m_condenseStrategy);
							v.visitData(data, cChild);
						}
					}
				}

			}
			else
			{
				v.visitNode(*n);

				for (uint_32 cChild = 0; cChild < n->m_children; cChild++)
				{
					if (query.intersectsShape(*(n->m_ptrMBR[cChild]))) st.push(readNode(n->m_pIdentifier[cChild]));
				}
			}
		}

//#ifndef PTHREADS
//		m_rwLock = false;
//#endif
//	}
//	catch (...)
//	{
//#ifndef PTHREADS
//		m_rwLock = false;
//#endif
//		throw;
//	}
    m_rwLock.unlock();
}

id_type StaticRTree::writeNode(Node* n)
{
	uint_8* buffer;
	uint_32 dataLength;
	n->storeToByteArray(&buffer, dataLength);

	id_type page;
	if (n->m_identifier < 0)
		page = StorageManager::NewPage;
	else
		page = n->m_identifier;

	try
	{
		m_pStorageManager->storeByteArray(page, dataLength, buffer);
		delete[] buffer;
	}
	catch (InvalidPageException& e)
	{
		delete[] buffer;
		std::cerr << e.what() << std::endl;
		//std::cerr << *this << std::endl;
		throw IllegalStateException("writeNode: failed with InvalidPageException");
	}

	if (n->m_identifier < 0)
		n->m_identifier = page;

	return page;
}

NodePtr StaticRTree::readNode(id_type id)
{
	uint_32 dataLength;
	uint_8* buffer;

	try
	{
		m_pStorageManager->loadByteArray(id, dataLength, &buffer);
	}
	catch (InvalidPageException& e)
	{
		std::cerr << e.what() << std::endl;
		//std::cerr << *this << std::endl;
		throw IllegalStateException("readNode: failed with InvalidPageException");
	}

	try
	{
		NodePtr n = m_nodePool.acquire();

		if (n.get() == 0)
			n = NodePtr(new Node(this, -1), &m_nodePool);

		n->m_tree = this;
		n->m_identifier = id;
		n->m_bLeaf = false;
		n->loadFromByteArray(buffer);

		delete[] buffer;
		return n;
	}
	catch (...)
	{
		delete[] buffer;
		throw;
	}
}

void StaticRTree::deleteNode(Node* n)
{
	try
	{
		m_pStorageManager->deleteByteArray(n->m_identifier);
	}
	catch (InvalidPageException& e)
	{
		std::cerr << e.what() << std::endl;
		//std::cerr << *this << std::endl;
		throw IllegalStateException("deleteNode: failed with InvalidPageException");
	}
}

void StaticRTree::setFirstDisplayScope()
{
	if (m_firstDisplayScope.isEmpty())
		m_firstDisplayScope = readNode(m_rootID)->m_nodeMBR;

	while (true)
	{
		CountRoadVisitor vis(m_maxDisplayCount);
		if (countIntersectsQuery(m_firstDisplayScope, vis))
			m_firstDisplayScope.changeSize(0.5);
		else
			break;
	}
}
