
#include "CRT_ProjectionMercatorTransverse.h"
#include "CRTALG_ProjectionMercatorTransverse.h"

namespace GIS_Embedded
{
;

CRT_ProjectionMercatorTransverse::CRT_ProjectionMercatorTransverse()
{
	m_false_east = 0.0;
	m_false_north = 0.0;	//TODO???
	m_OriginLatitude = 0.0;
	m_Scale_Factor = 0.0;
}

CRT_ProjectionMercatorTransverse::CRT_ProjectionMercatorTransverse(const CRT_ProjectionMercatorTransverse &right)
{
	m_ellipsoid = right.m_ellipsoid;
	m_central_meridian = right.m_central_meridian;
	m_false_east = right.m_false_east;
	m_false_north = right.m_false_north;
	m_OriginLatitude = right.m_OriginLatitude;
	m_Scale_Factor = right.m_Scale_Factor;
}

CRT_ProjectionMercatorTransverse::~CRT_ProjectionMercatorTransverse()
{}

CRT *CRT_ProjectionMercatorTransverse::copy() const
{
	return new CRT_ProjectionMercatorTransverse(*this);
}

bool CRT_ProjectionMercatorTransverse::operator ==(const CRT &crt) const
{
	const CRT_ProjectionMercatorTransverse *right = dynamic_cast<const CRT_ProjectionMercatorTransverse*>(&crt);
	return right && m_ellipsoid == right->m_ellipsoid
		&& m_false_east == right->m_false_east 
		&& m_false_north == right->m_false_north
		&& m_central_meridian == right->m_central_meridian
		&& m_OriginLatitude == right->m_OriginLatitude
		&& m_Scale_Factor == right->m_Scale_Factor;
}

CRT *CRT_ProjectionMercatorTransverse::createInverseCRT() const
{
	return new CRT_ProjectionMercatorTransverse_Inverse(*this);
}

int_32 CRT_ProjectionMercatorTransverse::trans(const Pos3D &src, Pos3D *dest) const
{
	CRTALG_ProjectionMercatorTransverse crt_alg;
	crt_alg._ellipsoid = m_ellipsoid;
	crt_alg.m_False_East = m_false_east;
	crt_alg.m_False_North = m_false_north;
	crt_alg.m_Central_Meridian = m_central_meridian;
	crt_alg.m_OriginLatitude = m_OriginLatitude;
	crt_alg.m_Scale_Factor = m_Scale_Factor;

	*dest = crt_alg.projectFromGeographic(src);
	//// X和Y是倒的
	swap(dest->d0, dest->d1);

	return 0;
}

CRT_ProjectionMercatorTransverse_Inverse::CRT_ProjectionMercatorTransverse_Inverse()
{}

CRT_ProjectionMercatorTransverse_Inverse::CRT_ProjectionMercatorTransverse_Inverse(const CRT_ProjectionMercatorTransverse_Inverse &right ) 
: m_prj_mercator(right.m_prj_mercator)
{}

CRT_ProjectionMercatorTransverse_Inverse::CRT_ProjectionMercatorTransverse_Inverse(const CRT_ProjectionMercatorTransverse &prj_mercator)
: m_prj_mercator(prj_mercator)
{}

CRT_ProjectionMercatorTransverse_Inverse::~CRT_ProjectionMercatorTransverse_Inverse()
{}

CRT *CRT_ProjectionMercatorTransverse_Inverse::copy() const
{
	return new CRT_ProjectionMercatorTransverse_Inverse(*this);
}

bool CRT_ProjectionMercatorTransverse_Inverse::operator ==(const CRT &crt) const
{
	const CRT_ProjectionMercatorTransverse_Inverse *right = dynamic_cast<const CRT_ProjectionMercatorTransverse_Inverse*>(&crt);
	return right && m_prj_mercator == right->m_prj_mercator;
}

CRT *CRT_ProjectionMercatorTransverse_Inverse::createInverseCRT() const
{
	return new CRT_ProjectionMercatorTransverse(m_prj_mercator);
}

int_32 CRT_ProjectionMercatorTransverse_Inverse::trans(const Pos3D &src, Pos3D *dest) const
{
	CRTALG_ProjectionMercatorTransverse crt_alg;
	crt_alg._ellipsoid = m_prj_mercator.m_ellipsoid;
	crt_alg.m_False_East = m_prj_mercator.m_false_east;
	crt_alg.m_False_North = m_prj_mercator.m_false_north;
	crt_alg.m_Central_Meridian = m_prj_mercator.m_central_meridian;
	crt_alg.m_OriginLatitude = m_prj_mercator.m_OriginLatitude;
	crt_alg.m_Scale_Factor = m_prj_mercator.m_Scale_Factor;

	Pos3D pos;
	pos.d0 = src.d1, pos.d1 = src.d0, pos.d2 = src.d2;

	*dest = crt_alg.inverseToGeographic(pos);

	return 0;
}

//int_32 CRT_ProjectionMercatorTransverse::packSize(const void *how_to) const
//{
//	return 
//		sizeof(PackHead) +
//		sizeof(int_32) +
//		sizeof(CD_Ellipsoid) +
//		sizeof(double) * 5 + // m_OriginLatitude (??), m_Scale_Factor, m_false_east, m_false_north, m_central_meridian
//		sizeof(int_32);
//}
//
//int_32 CRT_ProjectionMercatorTransverse::pack(byte_8 **result, const void *how_to) const
//{
//	byte_8 *temp;
//	int_32 sz = packSize(how_to);
//
//	*result = new byte_8[sz];
//	return pack(*result, sz, &temp, how_to);
//}
//
//int_32 CRT_ProjectionMercatorTransverse::pack(byte_8 *buffer, int_32 buffer_size, byte_8 **end_pos, const void *how_to) const
//{
//	int_32 sz = packSize(how_to);
//	if(sz > buffer_size)
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
//	ph->type = PTC_CRT_PRJ_MERCATOR_TRANSVERSE;
//
//	p += sizeof(PackHead);
//
//	*(int_32*)p = 0;
//	p += sizeof(int_32);
//
//	*(CD_Ellipsoid*)p = m_ellipsoid;
//	p += sizeof(CD_Ellipsoid);
//
//
//	*(double*)p = m_false_east;
//	p += sizeof(double);
//
//	*(double*)p = m_false_north;
//	p += sizeof(double);
//
//	*(double*)p = m_central_meridian;
//	p += sizeof(double);
//
//	*(double*)p = m_OriginLatitude;
//	p += sizeof(double);
//
//	*(double*)p = m_Scale_Factor;
//	p += sizeof(double);
//
//	*(int_32*)p = sz;
//	p += sizeof(int_32);
//
//
//
//	*end_pos = p;
//
//	return sz;
//}
//
//int_32 CRT_ProjectionMercatorTransverse::unpack(const byte_8 *buffer, int_32 buf_size) 
//{
//	const byte_8 *p = buffer;
//	p += sizeof(PackHead) + sizeof(int_32);
//
//	m_ellipsoid = *(CD_Ellipsoid*)p;
//	p += sizeof(CD_Ellipsoid);
//
//
//	m_false_east = *(double*)p;
//	p += sizeof(double);
//
//	m_false_north = *(double*)p;
//	p += sizeof(double);
//
//	m_central_meridian = *(double*)p;
//	p += sizeof(double);
//
//	m_OriginLatitude = *(double*)p;
//	p += sizeof(double);
//
//	m_Scale_Factor = *(double*)p;
//	p += sizeof(double);
//
//	p += sizeof(int_32);
//	return p - buffer;
//}

}// GIS_Embedded
