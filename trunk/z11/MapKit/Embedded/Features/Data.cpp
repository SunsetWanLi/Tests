#include "Data.h"
#include <string.h>

using namespace Embedded;
using namespace Embedded::Features;

Data::Data(uint_32 len, uint_8* pData, Rect& r, id_type id, const ICondenseStrategy* strategy)
	: m_id(id), m_rect(r), m_pData(NULL), m_dataLength(len), m_strategy(strategy)
{
	if (m_dataLength > 0)
	{
		m_pData = new uint_8[m_dataLength];
		memcpy(m_pData, pData, m_dataLength);
	}
}

Data::Data(const IShape* s, id_type id, const ICondenseStrategy* strategy)
	: m_id(id), m_pData(NULL), m_dataLength(0), m_strategy(strategy)
{
	m_strategy->storeToByteArray(s, &m_pData, m_dataLength);
	s->getMBR(m_rect);
}

Data::~Data()
{
	delete[] m_pData;
}

Data* Data::clone()
{
	return new Data(m_dataLength, m_pData, m_rect, m_id, m_strategy);
}

id_type Data::getIdentifier() const
{
	return m_id;
}

void Data::getShape(IShape** out) const
{
	if (m_dataLength == 0)
		*out = new Rect(m_rect);
	else
		getCertainShape(out);
}

void Data::getData(uint_32& len, uint_8** data) const
{
	len = m_dataLength;
	*data = 0;

	if (m_dataLength > 0)
	{
		*data = new uint_8[m_dataLength];
		memcpy(*data, m_pData, m_dataLength);
	}
}

uint_32 Data::getByteArraySize() const
{
	return
		sizeof(id_type) +
		sizeof(uint_32) +
		m_dataLength +
		m_rect.getByteArraySize();
}

void Data::loadFromByteArray(const uint_8* ptr)
{
	memcpy(&m_id, ptr, sizeof(id_type));
	ptr += sizeof(id_type);

	delete[] m_pData;
	m_pData = 0;

	memcpy(&m_dataLength, ptr, sizeof(uint_32));
	ptr += sizeof(uint_32);

	if (m_dataLength > 0)
	{
		m_pData = new uint_8[m_dataLength];
		memcpy(m_pData, ptr, m_dataLength);
		ptr += m_dataLength;
	}

	m_rect.loadFromByteArray(ptr);
}

void Data::storeToByteArray(uint_8** data, uint_32& len) const
{
	len = getByteArraySize();

	*data = new uint_8[len];
	uint_8* ptr = *data;

	memcpy(ptr, &m_id, sizeof(id_type));
	ptr += sizeof(id_type);
	memcpy(ptr, &m_dataLength, sizeof(uint_32));
	ptr += sizeof(uint_32);

	if (m_dataLength > 0)
	{
		memcpy(ptr, m_pData, m_dataLength);
		ptr += m_dataLength;
	}

	uint_32 rectsize;
	uint_8* rectdata = 0;
	m_rect.storeToByteArray(&rectdata, rectsize);

	memcpy(ptr, rectdata, rectsize);
	delete[] rectdata;
	// ptr += rectsize;
}

void Data::getCertainShape(IShape** s) const
{
	m_strategy->loadFromByteArray(s, m_pData, m_dataLength);
}
