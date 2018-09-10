#include "Buffer.h"

using namespace Embedded;
using namespace Embedded::StorageManager;

Buffer::Buffer(IStorageManager& sm, PropertySet& ps) :
	m_capacity(10),
	m_bWriteThrough(false),
	m_pStorageManager(&sm),
	m_hits(0)
{
	Variant var = ps.getProperty("Capacity");
	if (var.m_varType != VT_EMPTY)
	{
		if (var.m_varType != VT_ULONG) 
			throw IllegalArgumentException("Property Capacity must be VT_ULONG");
		m_capacity = var.m_val.ulVal;
	}

	var = ps.getProperty("WriteThrough");
	if (var.m_varType != VT_EMPTY)
	{
		if (var.m_varType != VT_BOOL) 
			throw IllegalArgumentException("Property WriteThrough must be VT_BOOL");
		m_bWriteThrough = var.m_val.blVal;
	}
}

Buffer::~Buffer()
{
	for (std::map<id_type, Entry*>::iterator it = m_buffer.begin(); it != m_buffer.end(); ++it)
	{
		Entry* e = (*it).second;
		id_type id = (*it).first;
		if (e->m_bDirty) 
			m_pStorageManager->storeByteArray(id, e->m_length, e->m_pData);
		delete e;
	}
}

//
// IStorageManager interface
//

void Buffer::loadByteArray(const id_type id, uint_32& len, uint_8** data)
{
	std::map<id_type, Entry*>::iterator it = m_buffer.find(id);

	if (it != m_buffer.end())
	{
		++m_hits;
		Entry* e = (*it).second;
		len = e->m_length;
		*data = new uint_8[len];
		memcpy(*data, e->m_pData, len);
	}
	else
	{
		m_pStorageManager->loadByteArray(id, len, data);
		Entry* e = new Entry(len, (const uint_8 *) *data);
		addEntry(id, e);
	}
}

void Buffer::storeByteArray(id_type& id, const uint_32 len, const uint_8* const data)
{
	throw NotSupportedException("Should Never be called");

	if (id == NewPage)
	{
		m_pStorageManager->storeByteArray(id, len, data);
		assert(m_buffer.find(id) == m_buffer.end());
		Entry* e = new Entry(len, data);
		addEntry(id, e);
	}
	else
	{
		if (m_bWriteThrough)
			m_pStorageManager->storeByteArray(id, len, data);

		Entry* e = new Entry(len, data);
		if (m_bWriteThrough == false) 
			e->m_bDirty = true;

		std::map<id_type, Entry*>::iterator it = m_buffer.find(id);
		if (it != m_buffer.end())
		{
			delete (*it).second;
			(*it).second = e;
			if (m_bWriteThrough == false) 
				++m_hits;
		}
		else
			addEntry(id, e);
	}
}

void Buffer::deleteByteArray(const id_type id)
{
	std::map<id_type, Entry*>::iterator it = m_buffer.find(id);
	if (it != m_buffer.end())
	{
		delete (*it).second;
		m_buffer.erase(it);
	}

	m_pStorageManager->deleteByteArray(id);
}

//void Buffer::getNodePageInfo(id_type id, uint_32& length, uint_32& pageSize) const
//{
//	m_pStorageManager->getNodePageInfo(id, length, pageSize);
//}

//
// IBuffer interface
//

void Buffer::clear()
{
	for (std::map<id_type, Entry*>::iterator it = m_buffer.begin(); it != m_buffer.end(); ++it)
	{
		if ((*it).second->m_bDirty)
		{
			id_type id = (*it).first;
			m_pStorageManager->storeByteArray(id, ((*it).second)->m_length, (const uint_8 *) ((*it).second)->m_pData);
		}

		delete (*it).second;
	}

	m_buffer.clear();
	m_hits = 0;
}

uint_32 Buffer::getHits()
{
	return m_hits;
}

