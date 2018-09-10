#include "Node.h"
#include "Utility.h"
#include "StaticRTree.h"
#include "../Features/Data.h"
#include <string.h>

using namespace Embedded;
using namespace Embedded::SpatialIndex;

//
// IObject interface
//

IObject* Node::clone()
{
	throw NotSupportedException("Node::clone should never be called.");
}

//
// ISerializable interface
//

uint_32 Node::getByteArraySize() const
{
	return
		CHILDREN_COUNT_SIZE		+	// m_children
		MBR_SIZE				+	// m_nodeMBR
		m_totalDataLength		+
		m_children * (
			MBR_SIZE					+	// m_ptrMBR
			EDGE_ID_SIZE				+	// m_pIdentifier
			EACH_CHILD_SIZE_SIZE			// m_pDataLength
		);
}


void Node::loadFromByteArray(const uint_8* ptr)
{
	m_nodeMBR.makeInfinite();

	memcpy(&m_children, ptr, CHILDREN_COUNT_SIZE);
	ptr += CHILDREN_COUNT_SIZE;

	for (uint_32 cChild = 0; cChild < m_children; cChild++)
	{
		m_ptrMBR[cChild] = getRectPtr();
		(m_ptrMBR[cChild])->makeInfinite();

		readMBRFromByte(*m_ptrMBR[cChild].get(), ptr);

		m_pIdentifier[cChild] = 0;
		memcpy(&(m_pIdentifier[cChild]), ptr, EDGE_ID_SIZE);
		ptr += EDGE_ID_SIZE;

		m_pDataLength[cChild] = 0;
		memcpy(&(m_pDataLength[cChild]), ptr, EACH_CHILD_SIZE_SIZE);
		ptr += EACH_CHILD_SIZE_SIZE;

		if (m_pDataLength[cChild] > 0)
		{
			m_bLeaf = true;
			m_totalDataLength += m_pDataLength[cChild];
			m_pData[cChild] = new uint_8[m_pDataLength[cChild]];
			memcpy(m_pData[cChild], ptr, m_pDataLength[cChild]);
			ptr += m_pDataLength[cChild];
		}
		else
		{
			m_pData[cChild] = 0;
		}
	}

	readMBRFromByte(m_nodeMBR, ptr);
}


void Node::storeToByteArray(uint_8** data, uint_32& len) const
{
	len = getByteArraySize();

	*data = new uint_8[len];
	uint_8* ptr = *data;

	memcpy(ptr, &m_children, CHILDREN_COUNT_SIZE);
	ptr += CHILDREN_COUNT_SIZE;

	for (uint_32 cChild = 0; cChild < m_children; cChild++)
	{
		writeMBRToByte(*m_ptrMBR[cChild].get(), ptr);

		memcpy(ptr, &(m_pIdentifier[cChild]), EDGE_ID_SIZE);
		ptr += EDGE_ID_SIZE;

		memcpy(ptr, &(m_pDataLength[cChild]), EACH_CHILD_SIZE_SIZE);
		ptr += EACH_CHILD_SIZE_SIZE;

		if (m_pDataLength[cChild] > 0)
		{
			m_bLeaf = true;
			memcpy(ptr, m_pData[cChild], m_pDataLength[cChild]);
			ptr += m_pDataLength[cChild];
		}
	}

	// store the node MBR for efficiency. This increases the node size a little bit.
	writeMBRToByte(m_nodeMBR, ptr);

	assert(len == ptr - *data);
}

//
// SpatialIndex::IEntry interface
//

id_type Node::getIdentifier() const
{
	return m_identifier;
}

void Node::getShape(IShape** out) const
{
	*out = new Rect(m_nodeMBR);
}

//
// SpatialIndex::INode interface
//

uint_32 Node::getChildrenCount() const
{
	return m_children;
}

id_type Node::getChildIdentifier(uint_32 index) const
{
	if (index < 0 || index >= m_children)
		throw IndexOutOfBoundsException(index);

	return m_pIdentifier[index];
}

void Node::setChildIdentifier(uint_32 index, id_type id)
{
	throw IllegalStateException("This should never be called. ");
}

void Node::getChildShape(id_type id, IShape** out) const
{
	throw IllegalStateException("This should never be called. ");
}

void Node::getChildShape(uint_32 index, IShape** out)  const
{
	assert(index < m_children);

	if (isIndex() || index >= m_children)
	{
		*out = NULL;
		return;
	}

	Features::Data d(m_pDataLength[index], m_pData[index], *m_ptrMBR[index], m_pIdentifier[index], m_tree->m_condenseStrategy);
	d.getShape(out);
}

uint_32 Node::getLevel() const
{
	throw IllegalStateException("This should never be called. ");
}

bool Node::isIndex() const
{
	return !m_bLeaf;
}

bool Node::isLeaf() const
{
	return m_bLeaf;
}

RectPtr Node::getRectPtr() const
{
	return m_tree->m_rectPool.acquire();
}

uint_32 Node::getIndexByteArraySize(uint_32 cChild)
{
	return
		CHILDREN_COUNT_SIZE		+	// m_children
		MBR_SIZE				+	// m_nodeMBR
		cChild * (
			MBR_SIZE					+	// m_ptrMBR
			EDGE_ID_SIZE				+	// m_pIdentifier
			EACH_CHILD_SIZE_SIZE			// m_pDataLength
		);
}

//
// Internal
//

Node::Node(StaticRTree* pTree, id_type id) :
	m_identifier(id),
	m_children(0),
	m_pData(NULL),
	m_ptrMBR(NULL),
	m_pIdentifier(NULL),
	m_pDataLength(NULL),
	m_tree(pTree),
	m_totalDataLength(0),
	m_bLeaf(false)
{
	m_nodeMBR.makeInfinite();

	try
	{
		m_pDataLength = new uint_32[NODE_CAPACITY + 1];
		m_pData = new uint_8*[NODE_CAPACITY + 1];
		m_ptrMBR = new RectPtr[NODE_CAPACITY + 1];
		m_pIdentifier = new id_type[NODE_CAPACITY + 1];
	}
	catch (...)
	{
		delete[] m_pDataLength;
		delete[] m_pData;
		delete[] m_ptrMBR;
		delete[] m_pIdentifier;
		throw;
	}
}

Node::~Node()
{
	if (m_pData != 0)
	{
		for (uint_32 cChild = 0; cChild < m_children; cChild++)
		{
			if (m_pData[cChild] != 0) delete[] m_pData[cChild];
		}

		delete[] m_pData;
	}

	delete[] m_pDataLength;
	delete[] m_ptrMBR;
	delete[] m_pIdentifier;
}

void Node::insertEntry(uint_32 dataLength, uint_8* pData, const Rect& mbr, id_type id)
{
	m_pDataLength[m_children] = dataLength;
	m_pData[m_children] = pData;
	m_ptrMBR[m_children] = getRectPtr();
	*(m_ptrMBR[m_children]) = mbr;
	m_pIdentifier[m_children] = id;

	m_totalDataLength += dataLength;
	++m_children;

	m_nodeMBR.combineRect(mbr);
}

void Node::readMBRFromByte(Rect& r, const uint_8*& ptr)
{
	int low[2], high[2];
	memcpy(low, ptr, MBR_SIZE >> 1);
	ptr += MBR_SIZE >> 1;
	memcpy(high, ptr, MBR_SIZE >> 1);
	ptr += MBR_SIZE >> 1;

	r.m_pLow[0] = low[0];
	r.m_pLow[1] = low[1];
	r.m_pHigh[0] = high[0];
	r.m_pHigh[1] = high[1];
}

void Node::writeMBRToByte(const Rect& r, uint_8*& ptr)
{
	int low[2], high[2];
	low[0] = static_cast<int>(r.m_pLow[0] + 0.5);
	low[1] = static_cast<int>(r.m_pLow[1] + 0.5);
	high[0] = static_cast<int>(r.m_pHigh[0] + 0.5);
	high[1] = static_cast<int>(r.m_pHigh[1] + 0.5);

	memcpy(ptr, low, MBR_SIZE >> 1);
	ptr += MBR_SIZE >> 1;
	memcpy(ptr, high, MBR_SIZE >> 1);
	ptr += MBR_SIZE >> 1;
}
