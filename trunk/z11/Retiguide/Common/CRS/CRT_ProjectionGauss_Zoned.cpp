
#include "CRT_ProjectionGauss_Zoned.h"
#include "CRTALG_ProjectionGauss.h"



namespace GIS_Embedded {

	//默认为3度投影带
	CRT_ProjectionGauss_Zoned::CRT_ProjectionGauss_Zoned()
		:m_b3du(true)
		,m_b6du(false)
	{
		
	}

	CRT_ProjectionGauss_Zoned::CRT_ProjectionGauss_Zoned(const CRT_ProjectionGauss_Zoned& right)
	{
		
		this->m_ellipsoid = right.m_ellipsoid;
		this->m_b3du = right.m_b3du;
		this->m_b6du = right.m_b6du;
		
	}

	CRT_ProjectionGauss_Zoned::~CRT_ProjectionGauss_Zoned()
	{

	}

	CRT* CRT_ProjectionGauss_Zoned::copy() const
	{
		return new CRT_ProjectionGauss_Zoned(*this);
	}


	bool CRT_ProjectionGauss_Zoned::operator ==(const CRT& right) const
	{
		const CRT_ProjectionGauss_Zoned* crt_prj_gauss_zoned = 
			dynamic_cast<const CRT_ProjectionGauss_Zoned*>(&right);
		return crt_prj_gauss_zoned
			&&this->m_ellipsoid == crt_prj_gauss_zoned->m_ellipsoid;
			
	}

		

	CRT* CRT_ProjectionGauss_Zoned::createInverseCRT() const
	{
		return new CRT_ProjectionGauss_Zoned_Inverse(*this);
	}

	int_32 CRT_ProjectionGauss_Zoned::trans(const Pos3D &src, Pos3D *dest) const
	{
		CRTALG_ProjectionGauss crt_alg;
		crt_alg._ellipsoid = m_ellipsoid;

		GIS_Embedded::Pos3D pos(src);

		if( m_b3du )
		{
			*dest = crt_alg.project3DuZone(pos);
		}else {
			*dest = crt_alg.project6DuZone(pos);
		}

		return 0;
	}

	


	CRT_ProjectionGauss_Zoned_Inverse::CRT_ProjectionGauss_Zoned_Inverse()
	{
	}

	CRT_ProjectionGauss_Zoned_Inverse::CRT_ProjectionGauss_Zoned_Inverse(const CRT_ProjectionGauss_Zoned_Inverse& right)
		:m_prj_gauss_zoned(right.m_prj_gauss_zoned)
	{
	}

	CRT_ProjectionGauss_Zoned_Inverse::CRT_ProjectionGauss_Zoned_Inverse(const CRT_ProjectionGauss_Zoned& right)
		:m_prj_gauss_zoned(right)
	{
	}

	CRT* CRT_ProjectionGauss_Zoned_Inverse::copy() const
	{
		return new CRT_ProjectionGauss_Zoned_Inverse(*this);
	}

	bool CRT_ProjectionGauss_Zoned_Inverse::operator == (const CRT& crt) const
	{
		const CRT_ProjectionGauss_Zoned_Inverse* right = dynamic_cast<const CRT_ProjectionGauss_Zoned_Inverse*>(&crt);

		return right && m_prj_gauss_zoned == right->m_prj_gauss_zoned;
	}

	CRT* CRT_ProjectionGauss_Zoned_Inverse::createInverseCRT() const
	{
		return new CRT_ProjectionGauss_Zoned(m_prj_gauss_zoned);
	}


	int_32 CRT_ProjectionGauss_Zoned_Inverse::trans(const Pos3D& src,Pos3D* dest) const
	{
		CRTALG_ProjectionGauss crt_alg;
		crt_alg._ellipsoid = m_prj_gauss_zoned.m_ellipsoid;
		
		GIS_Embedded::Pos3D pos(src);

		if(m_prj_gauss_zoned.m_b3du) // 3度
		{
			crt_alg.inverse3DuZoneToGeographic(pos);
		}
		else if(m_prj_gauss_zoned.m_b6du) // 6度
		{
			crt_alg.inverse6DuZoneToGeographic(pos);
		}

		return 0;
	}



	//int_32 CRT_ProjectionGauss_Zoned::packSize(const void *how_to ) const
	//{
	//	return 
	//	sizeof(PackHead) +
	//	sizeof(int_32) +
	//	sizeof(CD_Ellipsoid) +
	//	//sizeof(double) * 5 + // m_OriginLatitude, m_Scale_Factor, m_false_east, m_false_north, m_central_meridian
	//	sizeof(int_32) + // m_b3du
	//	sizeof(int_32);

	//	return 0;
	//}
	//int_32 CRT_ProjectionGauss_Zoned::pack(byte_8 *buffer, int_32 buffer_size, byte_8 **end_pos, const void *how_to ) const
	//{
	//	int_32 sz = packSize(how_to);
	//	if(sz > buffer_size)
	//	{
	//		*end_pos = buffer;
	//		return 0;
	//	}

	//	byte_8 *p = buffer;

	//	PackHead *ph = (PackHead*)p;
	//	ph->byte_order = SYS_ENDIAN_TAG;
	//	ph->is_segment = false;
	//	ph->size = sz;
	//	ph->type = CRT_PRJ_GAUSS_ZONED;

	//	p += sizeof(PackHead);

	//	*(int_32*)p = 0;
	//	p += sizeof(int_32);

	//	*(CD_Ellipsoid*)p = m_ellipsoid;
	//	p += sizeof(CD_Ellipsoid);

	//	*(int_32*)p = m_b3du ? 1 : 0;
	//	p += sizeof(int_32);

	//	*(int_32*)p = m_b6du ? 1 : 0;
	//	p += sizeof(int_32);

	//	*(int_32*)p = sz;
	//	p += sizeof(int_32);

	//	*end_pos = p;

	//	return sz;
	//			
	//}

	//int_32 CRT_ProjectionGauss_Zoned::pack(byte_8 **result, const void *how_to )const
	//{
	//	byte_8 *temp;
	//	int_32 sz = packSize(how_to);

	//	*result = new byte_8[sz];
	//	return pack(*result, sz, &temp, how_to);
	//}

	//int_32 CRT_ProjectionGauss_Zoned::unpack(const byte_8 *buffer, int_32 buf_size )
	//{
	//	const byte_8 *p = buffer;
	//	p += sizeof(PackHead) + sizeof(int_32);

	//	m_ellipsoid = *(CD_Ellipsoid*)p;
	//	p += sizeof(CD_Ellipsoid);

	//	m_b3du = (*(int_32*)p) ? true : false ;
	//	p += sizeof(int_32);

	//	m_b6du = (*(int_32*)p) ? true : false ;
	//	p += sizeof(int_32);

	//	p += sizeof(int_32);

	//	return p - buffer;
	//	
	//}

	
	
	
	/**/





}