
#include "CRT_ProjectionToProjection5480.h"
#include "CRTALG_ProjectionGauss.h"
#include "CRS_Factory.h"

namespace GIS_Embedded {

	CRT_ProjectionToProjection5480::CRT_ProjectionToProjection5480()
	{
		m_b54To80 = true;
		m_b80To54 = false;
		m_ellipsoid = CRS_Factory::createEllipse(ETC_KRASSOVSKY_1940);
		
		m_false_east = m_false_north = 0.0;	//TODO???
		m_OriginLatitude = 0;
		m_Scale_Factor = 1;
		m_central_meridian = 0;
	}

	CRT_ProjectionToProjection5480::CRT_ProjectionToProjection5480(const CRT_ProjectionToProjection5480& right)
	{
		m_ellipsoid = right.m_ellipsoid;
		m_DX = right.m_DX;
		m_DY = right.m_DY;
		m_x = right.m_x;
		m_y = right.m_y;
		m_b54To80 = right.m_b54To80;
		m_b80To54 = right.m_b80To54;

	}

	CRT_ProjectionToProjection5480::~CRT_ProjectionToProjection5480()
	{
	}


	CRT* CRT_ProjectionToProjection5480::copy() const 
	{
		return new CRT_ProjectionToProjection5480(*this);
	}

	bool CRT_ProjectionToProjection5480::operator ==(const CRT& right) const
	{
		const CRT_ProjectionToProjection5480* crt_prj = dynamic_cast<const CRT_ProjectionToProjection5480*>(&right);
		return crt_prj && m_ellipsoid == crt_prj->m_ellipsoid
			&&  m_DX == crt_prj->m_DX && m_DY == crt_prj->m_DY
			&&  m_x == crt_prj->m_x && m_y == crt_prj->m_y
			&&  m_b54To80 == crt_prj->m_b54To80 && m_b80To54 == crt_prj->m_b80To54;
	}



	int_32 CRT_ProjectionToProjection5480::trans(const GIS_Embedded::Pos3D& src, GIS_Embedded::Pos3D* dest) const
	{
		CRTALG_ProjectionGauss crt_alg;
		crt_alg._ellipsoid = m_ellipsoid;
 
		if(m_b54To80)
			crt_alg.projectFromProject(m_DX,m_DY,src.d0,src.d1,dest->d0,dest->d1,m_b54To80);
		else if( m_b80To54 )
			crt_alg.projectFromProject(m_DX,m_DY,src.d0,src.d1,dest->d0,dest->d1,m_b80To54);

		dest->d2 = 0;

		return 0;
	}

	int_32 CRT_ProjectionToProjection5480::calDXY(double x80,double y80,const GIS_Embedded::Pos3D& shift_val,const GIS_Embedded::Pos3D& posSource)
	{
		CRTALG_ProjectionGauss crt_alg;
		crt_alg._ellipsoid = this->m_ellipsoid;
		crt_alg.m_Central_Meridian = this->m_central_meridian;
		crt_alg.m_False_East = this->m_false_east;
		crt_alg.m_False_North = this->m_false_north;
		crt_alg.m_Scale_Factor = this->m_Scale_Factor;
		crt_alg.m_OriginLatitude = this->m_OriginLatitude;

		GIS_Embedded::Pos3D pos3d_geography = crt_alg.inverseToGeographic(posSource);

		GIS_Embedded::Pos3D pos_dest = crt_alg.getDXYFromGeography(x80,y80,shift_val,pos3d_geography);

		m_DX = pos_dest.d0;
		m_DY = pos_dest.d1;

		return 0;
	}

	int CRT_ProjectionToProjection5480::calDXDY(const GIS_Embedded::Pos3D& pos3d54,const GIS_Embedded::Pos3D& pos3d80)
	{
		m_DX = pos3d80.d0 - pos3d54.d0;
		m_DY = pos3d80.d1 - pos3d54.d1;

		return 0;
	}

	//int_32 CRT_ProjectionToProjection5480::packSize(const void* how_to) const
	//{
	//	return sizeof(PackHead) + 
	//		   sizeof(int_32) +
	//		   sizeof(double) * 4 +
	//		   sizeof(int_32) *2 +
	//		   sizeof(int_32);
	//}

	//int_32 CRT_ProjectionToProjection5480::pack(byte_8 **result,const void* how_to) const
	//{
	//	byte_8* temp;

	//	int_32 sz = packSize(how_to);
	//	
	//	*result = new byte_8[sz];

	//	return pack(*result,sz,&temp,how_to);
	//}

	//int_32 CRT_ProjectionToProjection5480::pack(byte_8* buffer, int_32 buffer_size,byte_8** end_pos, const void* how_to) const
	//{
	//	int_32 sz = packSize(how_to);

	//	if(sz > buffer_size)
	//	{
	//		*end_pos = buffer;
	//		return 0;
	//	}

	//	byte_8* p = buffer;

	//	PackHead* ph = (PackHead*)p;
	//	ph->byte_order = SYS_ENDIAN_TAG;
	//	ph->is_segment = false;
	//	ph->size = sz;
	//	ph->type = PRJ_GAUSS;

	//	p += sizeof(PackHead);

	//	*(int_32*)p = 0;
	//	p += sizeof(int_32);

	//	*(double*)p = m_DX;
	//	p += sizeof(double);

	//	*(double*)p = m_DY;
	//	p += sizeof(double);

	//	*(double*)p = m_x;
	//	p += sizeof(double);

	//	*(double*)p = m_y;
	//	p += sizeof(double);

	//	*(int_32*)p = m_b54To80 ? 1 : 0;
	//	p += sizeof(int_32);

	//	*(int_32*)p = m_b80To54 ? 1 : 0;
	//	p += sizeof(int_32);

	//	*(int_32*)p = sz;
	//	p += sizeof(int_32);

	//	*end_pos = p;

	//	return sz;
	//}


	//int_32 CRT_ProjectionToProjection5480::unpack(const byte_8* buffer,int_32 buf_size)
	//{
	//	const byte_8* p = buffer;

	//	p += sizeof(PackHead) + sizeof(int_32);

	//	m_ellipsoid = *(CD_Ellipsoid*)p;
	//	p += sizeof(CD_Ellipsoid);

	//	m_DX = *(double*)p;
	//	p += sizeof(double);

	//	m_DY = *(double*)p;
	//	p += sizeof(double);

	//	m_x = *(double*)p;
	//	p += sizeof(double);

	//	m_y = *(double*)p;
	//	p += sizeof(double);

	//	m_b54To80 = (*(double*)p) ? true : false;
	//	p += sizeof(int_32);

	//	m_b80To54 = (*(double*)p) ? true : false;
	//	p += sizeof(int_32);

	//	p += sizeof(int_32);

 //       return p - buffer;
	//}

}