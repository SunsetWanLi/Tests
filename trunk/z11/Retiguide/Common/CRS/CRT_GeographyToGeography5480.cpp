
#include "CRT_GeographyToGeography5480.h"
#include "CRTALG_GeographyToGeography.h"

#include "CRS_Factory.h"
#include "CRS_Geography.h"
#include "CRS_Project.h"
#include "CRT_Factory.h"
#include "CRT_ProjectionToProjection5480.h"



namespace GIS_Embedded {
	CRT_GeographyToGeography5480::CRT_GeographyToGeography5480()
	{
	}

	CRT_GeographyToGeography5480::CRT_GeographyToGeography5480(const CRT_GeographyToGeography5480& right)
	{
		m_ellipsoid = right.m_ellipsoid;
		m_delta_x = right.m_delta_x;
		m_delta_y = right.m_delta_y;
		m_delta_z = right.m_delta_z;

	}

	CRT_GeographyToGeography5480::~CRT_GeographyToGeography5480()
	{
	}


	CRT* CRT_GeographyToGeography5480::copy() const
	{
		return new CRT_GeographyToGeography5480(*this);
	}


	bool CRT_GeographyToGeography5480::operator ==(const CRT& right)const
	{
		const CRT_GeographyToGeography5480* crt_geo = dynamic_cast<const CRT_GeographyToGeography5480*>(&right);

		return crt_geo && this->m_ellipsoid == crt_geo->m_ellipsoid
			&& this->m_delta_x == crt_geo->m_delta_x && this->m_delta_y == crt_geo->m_delta_y && this->m_delta_z == crt_geo->m_delta_z;
	}		
		


	// src(B54,L54,0), (*dest)(B80,L80,0)
	int_32 CRT_GeographyToGeography5480::trans(const GIS_Embedded::Pos3D& src, GIS_Embedded::Pos3D* dest) const
	{
		
		GIS_Embedded::Pos3D posPrj54;
		GIS_Embedded::Pos3D posPrj80;

		CRT* crt = NULL;

		CRS* crs_src = CRS_Factory::createCRS_Geography(ETC_KRASSOVSKY_1940);

		//assert( crs_src != NULL );
		
		CRS_Geography* crs_geograpy = dynamic_cast<CRS_Geography*>(crs_src);
		
		CRS* crs_dest = CRS_Factory::createCRS_Project(*crs_geograpy);

		//assert( crs_dest != NULL );

		CRT_Factory::createCRT(*crs_src,*crs_dest,&crt);

		crt->trans(src,&posPrj54); //54地理-->54投影

        CRT_ProjectionToProjection5480 crt_prj5480;
		
		//设置偏移量
		crt_prj5480.m_DX = m_DX;
		crt_prj5480.m_DY = m_DY;

		crt_prj5480.trans(posPrj54,&posPrj80); // 54投影-->80投影

		CRT* crt_gauss_inverse = crt->createInverseCRT();
		
		crt_gauss_inverse->trans(posPrj80,dest); //80投影-->80地理

		delete crs_src;
		delete crs_dest;
		delete crt;
		delete crt_gauss_inverse;



		/*CRTALG_GeographyToGeography crt_alg;
		crt_alg._ellipsoid = m_ellipsoid;

		crt_alg.geographyFromGeography(src.d0,src.d1,m_delta_x,m_delta_y,m_delta_z,dest->d0,dest->d1);
		dest->d2 = 0;*/

		return 0;
	}

	//int_32 CRT_GeographyToGeography5480::packSize(const void* how_to) const
	//{
	//	return sizeof(PackHead) +
	//		   sizeof(int_32) +
	//		   sizeof(CD_Ellipsoid) +
	//		   sizeof(double) * 3 + // m_delta_x,m_delta_y, m_delta_z;
	//		   sizeof(int_32);
	//}


	//int_32 CRT_GeographyToGeography5480::pack(byte_8 ** result, const void* how_to) const
	//{
	//	byte_8* temp;

	//	int_32 sz = packSize(how_to);

	//	*result = new byte_8[sz];
	//	
	//	return pack(*result,sz,&temp,how_to);
	//}

	//int_32 CRT_GeographyToGeography5480::pack(byte_8* buffer, int_32 buffer_size,byte_8** end_pos,const void* how_to) const
	//{
	//	int_32 sz = packSize(how_to);
	//	if( sz > buffer_size )
	//	{
	//		*end_pos = buffer;
	//		return 0;
	//	}

	//	byte_8* p = buffer;

	//	PackHead* ph = (PackHead*)p;
	//	ph->byte_order = SYS_ENDIAN_TAG;
	//	ph->is_segment = false;
	//	ph->size = sz;
	//	ph->type = PTC_CRT_PRJ_GAUSS;

	//	p += sizeof(PackHead);

	//	*(int_32*)p = 0;

	//	*(CD_Ellipsoid*)p = m_ellipsoid;
	//	p += sizeof(CD_Ellipsoid);

	//	*(double*)p = m_delta_x;
	//	p += sizeof(double);


	//	*(double*)p = m_delta_y;
	//	p += sizeof(double);

	//	*(double*)p = m_delta_z;
	//	p += sizeof(double);

	//	*(int_32*)p = sz;
	//	p += sizeof(int_32);

	//	*end_pos = p;

	//	return sz;
	//}


	//int_32 CRT_GeographyToGeography5480::unpack(const byte_8* buffer,int_32 buf_size)
	//{
	//	const byte_8* p = buffer;

	//	p += sizeof(PackHead) + sizeof(int_32);

	//	m_ellipsoid = *(CD_Ellipsoid*)p;
	//	p += sizeof(CD_Ellipsoid);

	//	m_delta_x = *(double*)p;
	//	p += sizeof(double);

	//	m_delta_y = *(double*)p;
	//	p += sizeof(double);

	//	m_delta_z = *(double*)p;
	//	p += sizeof(double);

	//	p += sizeof( int_32 );

	//	return p -buffer;

	//}

}
