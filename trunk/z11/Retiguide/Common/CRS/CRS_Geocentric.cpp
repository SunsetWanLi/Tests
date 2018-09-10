
#include "CRS_Geocentric.h"
#include "CRS_Factory.h"

namespace GIS_Embedded
{
;

CRS_Geocentric::CRS_Geocentric()
{
	m_datum = CRS_Factory::createDatum();
	m_coord_system = CRS_Factory::createCoordinateSystem(UTC_STD, 3, CS_CARTESIAN);
}

CRS_Geocentric::CRS_Geocentric(const CRS_Geocentric &right)
{
	m_datum = right.m_datum;
	m_coord_system = right.m_coord_system;
}

CRS *CRS_Geocentric::copy() const
{
	return new CRS_Geocentric(*this);
}

bool CRS_Geocentric::operator ==(const CRS &crs) const
{
	const CRS_Geocentric *right = dynamic_cast<const CRS_Geocentric *>(&crs);

	return right && m_datum == right->m_datum && m_coord_system == right->m_coord_system;
}

//int_32 CRS_Geocentric::packSize(const void *how_to) const
//{
//	return sizeof(PackHead)
//		+ sizeof(int_32) // how to
//		+ sizeof(CD_GeodesicDatum)
//		+ sizeof(CoordinateSystem)
//		+ sizeof(int_32)//end check
//		;	
//}
//
//int_32 CRS_Geocentric::pack(byte_8 *buffer, int_32 buffer_size, byte_8 **end_pos, const void *how_to) const
//{
//	int_32 sz = packSize();
//	if(buffer_size < sz)
//		return -1;
//
//	byte_8 *current = buffer;
//
//	PackHead *ph = (PackHead*)current;
//	ph->byte_order = SYS_ENDIAN_TAG;
//	ph->is_segment = false;
//	ph->size = sz;
//	ph->type = PTC_CRS_GEOCENTRIC;
//	current += sizeof(PackHead);
//
//	*(int_32*)current = 0;
//	current += sizeof(int_32);
//
//	*(CD_GeodesicDatum*)current = m_datum;
//	current += sizeof(CD_GeodesicDatum);
//
//	*(CoordinateSystem*)current = m_coord_system;
//	current += sizeof(CoordinateSystem);
//
//	*(int_32*)current = sz;
//	current += sizeof(int_32);
//
//	*end_pos = current;
//
//	assert(current - buffer == sz);
//
//	return sz;
//}
//
//int_32 CRS_Geocentric::pack(byte_8 **result, const void *how_to) const
//{
//	int_32 sz = packSize();
//	*result = new byte_8[sz];
//	byte_8 *pos;
//	return pack(*result, sz, &pos, how_to);
//}
//
//int_32 CRS_Geocentric::unpack(const byte_8 *buffer, int_32 buf_size)
//{
//	int_32 sz = packSize();
//
//	if(buf_size != PACK_IGNORE_BUF_SIZE && sz > buf_size)
//	{
//		assert(0);
//		return -1;
//	}
//
//	const byte_8 *current = buffer + sizeof(PackHead) + sizeof(int_32);
//	m_datum = *(CD_GeodesicDatum*)current;
//	current += sizeof(CD_GeodesicDatum);
//
//	m_coord_system = *(CoordinateSystem*)current;
//	current += sizeof(CoordinateSystem);
//
//	assert(*(int_32*)current == sz);
//
//	return sz;
//}


}