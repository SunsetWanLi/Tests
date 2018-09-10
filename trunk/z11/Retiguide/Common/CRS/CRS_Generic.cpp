
#include "CRS_Generic.h"
#include "CRT.h"
#include "CRS_Factory.h"
//#include "CRSUnpacker.h"

namespace GIS_Embedded
{
;

CRS_Generic::CRS_Generic()
{
	m_ref_crs = NULL;
	m_ref_crt = NULL;
	m_coord_system = CRS_Factory::createCoordinateSystem(UTC_STD, 3, CS_CARTESIAN);
}

CRS_Generic::CRS_Generic(const CRS_Generic &right)
{
	m_ref_crs = right.m_ref_crs ? right.m_ref_crs->copy() : NULL;
	m_ref_crt = right.m_ref_crt ? right.m_ref_crt->copy() : NULL;
	m_coord_system = right.m_coord_system;
}

CRS_Generic::~CRS_Generic()
{
	delete m_ref_crs;
	delete m_ref_crt;
}

CRS *CRS_Generic::getRefCRS() const
{
	return m_ref_crs;
}

CRT *CRS_Generic::getRefCRT() const
{
	return m_ref_crt;
}

CRS *CRS_Generic::copy() const
{
	return new CRS_Generic(*this);
}

bool CRS_Generic::operator ==(const CRS &crs) const
{
	const CRS_Generic *right = dynamic_cast<const CRS_Generic*>(&crs);

	if(!right)
		return false;
	if(!m_ref_crs && !m_ref_crt && !right->m_ref_crs && !right->m_ref_crt)
	{
		return true;
	}

	if(		(!m_ref_crs && right->m_ref_crs) || (m_ref_crs && !right->m_ref_crs)
		||	(!m_ref_crt && right->m_ref_crt) || (m_ref_crt && !right->m_ref_crt))
	{
		return false;
	}

	return right && *m_ref_crs == *right->m_ref_crs
		&& *m_ref_crt == *right->m_ref_crt
		&& m_coord_system == right->m_coord_system;
}

bool CRS_Generic::isCompound() const
{
	return m_ref_crs != NULL;
}

void CRS_Generic::setRefCRS(const CRS *ref_crs)
{
	delete m_ref_crs;
	m_ref_crs = ref_crs->copy();
}

void CRS_Generic::setRefCRT(const CRT *ref_crt)
{
	delete m_ref_crt;
	m_ref_crt = ref_crt->copy();
}

//int_32 CRS_Generic::packSize(const void *how_to) const
//{
//	int_32 ret = sizeof(PackHead) + sizeof(int_32) 
//		+ sizeof(int_32) // ref_crs is valid
//		+ sizeof(int_32) // ref_crt is valid
//		+ sizeof(int_32);// end_check
//
//	if(m_ref_crs && dynamic_cast<Packable*>(m_ref_crs))
//	{
//		ret += dynamic_cast<Packable*>(m_ref_crs)->packSize();
//	}
//	if(m_ref_crt && dynamic_cast<Packable*>(m_ref_crt))
//	{
//		ret += dynamic_cast<Packable*>(m_ref_crt)->packSize();
//	}
//
//	return ret;
//}
//
//int_32 CRS_Generic::pack(byte_8 *buffer, int_32 buffer_size, byte_8 **end_pos, const void *how_to)const
//{
//	int_32 sz = packSize(how_to);
//
//	if(buffer_size < sz)
//	{
//		*end_pos = buffer;
//		return 0;
//	}
//
//	byte_8 *p = buffer;
//
//	PackHead *ph = (PackHead*)p;
//	ph->byte_order = SYS_ENDIAN_TAG;
//	ph->is_segment = false;
//	ph->size = sz;
//	ph->type = PTC_CRS_GENERIC;
//	p += sizeof(PackHead);
//
//	// howto
//	*(int_32*)p = 0;
//	p += sizeof(int_32);
//
//	// ref_crs?
//	*(int_32*)p = m_ref_crs == NULL ? 0 : 1;
//	p += sizeof(int_32);
//
//	// ref_crt?
//	*(int_32*)p = m_ref_crt == NULL ? 0 : 1;
//	p += sizeof(int_32);
//
//	// ref_crs
//	if(m_ref_crs)
//		p += dynamic_cast<Packable*>(m_ref_crs)->pack(p, sz - (p - buffer), end_pos, how_to);
//
//	// ref_crt
//	if(m_ref_crt)
//		p += dynamic_cast<Packable*>(m_ref_crt)->pack(p, sz - (p - buffer), end_pos, how_to);
//
//	//
//	*(int_32*)p = sz;
//	p += sizeof(int_32);
//
//	*end_pos = p;
//	return sz;
//}
//
//int_32 CRS_Generic::pack(byte_8 **result, const void *how_to) const 
//{
//	int_32 sz = packSize(how_to);
//
//	byte_8 *temp;
//	*result = new byte_8[sz];
//
//	return pack(*result, sz, &temp, how_to);
//}

//int_32 CRS_Generic::unpack(const byte_8 *buffer, int_32 buf_size)
//{
//	const PackHead *ph = (const PackHead*)buffer;
//	if(buf_size != PACK_IGNORE_BUF_SIZE && ph->size > buf_size)
//	{
//		return -1;
//	}
//	const byte_8 *p = buffer + sizeof(PackHead) + sizeof(int_32);
//
//	int_32 have_ref_crs, have_ref_crt;
//	have_ref_crs = *(const int_32*)p;
//	p += sizeof(int_32);
//	have_ref_crt = *(const int_32*)p;
//	p += sizeof(int_32);
//
//	if(have_ref_crs)
//	{
//		// unpack crs
//		Packable *temp;
//		uint_32 len;
//		CRS_Unpacker::create(p, &temp, &len);
//
//		assert(dynamic_cast<CRS*>(temp));
//		m_ref_crs = dynamic_cast<CRS*>(temp);
//		p += len;
//	}
//	else
//		m_ref_crs = NULL;
//
//	if(have_ref_crt)
//	{
//		// unpack crt
//		Packable *temp;
//		uint_32 len;
//		CRS_Unpacker::create(p, &temp, &len);
//
//		assert(dynamic_cast<CRT*>(temp));
//		m_ref_crt = dynamic_cast<CRT*>(temp);
//		p += len;
//	}
//	else
//		m_ref_crt = NULL;
//
//	assert(*(int_32*)p == ((const PackHead*)buffer)->size);
//	p += sizeof(int_32);
//
//	return p - buffer;
//}
//
}// GIS_Embedded
