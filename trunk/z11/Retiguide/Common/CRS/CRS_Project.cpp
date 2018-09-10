
// CRS_Project.cpp

#include "CRS_Project.h"
#include "CRT_Projection.h"
#include "CRS_Geography.h"
//#include "CRSUnpacker.h"

namespace GIS_Embedded
{
;

CRS_Project::CRS_Project()
{
	m_ref_crs = NULL;
	m_ref_crt = NULL;
}

CRS_Project::CRS_Project(const CRS_Project &right)
{
	m_ref_crs = right.m_ref_crs ? dynamic_cast<CRS_Geography*>(right.m_ref_crs->copy()) : NULL;
	m_ref_crt = right.m_ref_crt ? dynamic_cast<CRT_Projection*>(right.m_ref_crt->copy()): NULL;
	m_coord_system = right.m_coord_system;
}

CRS_Project::~CRS_Project()
{
	delete m_ref_crs;
	delete m_ref_crt;
}

CRS *CRS_Project::getRefCRS() const
{
	return m_ref_crs;
}

CRT *CRS_Project::getRefCRT() const
{
	return m_ref_crt;
}

void CRS_Project::setRefCRS(const CRS_Geography *ref_crs)
{
	delete m_ref_crs;
	m_ref_crs = dynamic_cast<CRS_Geography*>(ref_crs->copy());
}

void CRS_Project::setRefCRT(const CRT_Projection *ref_crt)
{
	delete m_ref_crt;
	m_ref_crt = dynamic_cast<CRT_Projection*>(ref_crt->copy());
}

CRS *CRS_Project::copy()const
{
	return new CRS_Project(*this);
}

bool CRS_Project::operator ==(const CRS &crs) const
{
	const CRS_Project *right = dynamic_cast<const CRS_Project*>(&crs);

	return right && right->m_coord_system == m_coord_system 
		   // both pointers to be null------  || no pointers to be null-------  && content the same       
		&& ((!right->m_ref_crs && !m_ref_crs) || (right->m_ref_crs && m_ref_crs && *(right->m_ref_crs) == *m_ref_crs))
		&& ((!right->m_ref_crt && !m_ref_crt) || (right->m_ref_crt && m_ref_crt && *(right->m_ref_crt) == *m_ref_crt));
}

//int_32 CRS_Project::packSize(const void *how_to) const
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
//int_32 CRS_Project::pack(byte_8 **result, const void *how_to) const
//{
//	int_32 sz = packSize(how_to);
//
//	byte_8 *temp;
//	*result = new byte_8[sz];
//
//	return pack(*result, sz, &temp, how_to);
//}
//
//int_32 CRS_Project::pack(byte_8 *buffer, int_32 buffer_size, byte_8 **end_pos, const void *how_to) const
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
//	ph->type = PTC_CRS_PROJECT;
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
//
//}
//
//int_32 CRS_Project::unpack(const byte_8 *buffer, int_32 buf_size)
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
//		assert(dynamic_cast<CRS_Geography*>(temp));
//		m_ref_crs = dynamic_cast<CRS_Geography*>(temp);
//		p += len;
//	}
//
//	if(have_ref_crt)
//	{
//		// unpack crt
//		Packable *temp;
//		uint_32 len;
//		CRS_Unpacker::create(p, &temp, &len);
//
//		assert(dynamic_cast<CRT_Projection*>(temp));
//		m_ref_crt = dynamic_cast<CRT_Projection*>(temp);
//		p += len;
//	}
//
//	assert(*(int_32*)p == ((const PackHead*)buffer)->size);
//	p += sizeof(int_32);
//
//	return p - buffer;
//}
//
}//namespace