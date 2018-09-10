#include "GlobalCrt.h"

namespace GIS_Embedded
{
	CRT* GlobalCrt::m_crt = NULL;
	CRT* GlobalCrt::m_inverse_crt = NULL;

	GlobalCrt::GlobalCrt()
	{
		if(m_crt == NULL && m_inverse_crt == NULL)
		{
			CRS * crs1 = CRS_Factory::createCRS_Geography(GCS_WGS_1984);
			CRS_Geography *crs_geo = dynamic_cast<CRS_Geography *>(crs1);
			CRS * crs2 = CRS_Factory::createCRS_Project(*crs_geo, PRJ_MERCATOR);
			CRT_Factory::createCRT(*crs1, *crs2, &m_crt);
			m_inverse_crt = m_crt->createInverseCRT();
			delete crs1;
			delete crs2;
		}
	}

	int GlobalCrt::geoToPrj(double geo_x, double geo_y, long *prj_x, long *prj_y)
	{
		Pos2D src, dest;
		src.d0 = geo_x;
		src.d1 = geo_y;
		m_crt->trans(src, &dest);
		*prj_x = (long)dest.d0;
		*prj_y = (long)dest.d1;
		return 0;

	}
	int GlobalCrt::geoToPrj(double geo_x, double geo_y, int_32 *prj_x, int_32 *prj_y)
	{
		Pos2D src, dest;
		src.d0 = geo_x;
		src.d1 = geo_y;
		m_crt->trans(src, &dest);
		*prj_x = (int_32)dest.d0;
		*prj_y = (int_32)dest.d1;
		return 0;
	}	

	//int GlobalCrt::geoToPrj(double geo_x, double geo_y, float *prj_x, float *prj_y)
	//{
	//	Pos2D src, dest;
	//	src.d0 = geo_x;
	//	src.d1 = geo_y;
	//	m_crt->trans(src, &dest);
	//	*prj_x = dest.d0;
	//	*prj_y = dest.d1;
	//	return 0;
	//}	

	int GlobalCrt::prjToGeo(int_32 prj_x, int_32 prj_y, double *geo_x, double *geo_y)
	{
		Pos2D src, dest;
		src.d0 = prj_x;
		src.d1 = prj_y;
		m_inverse_crt->trans(src, &dest);
		*geo_x = dest.d0;
		*geo_y = dest.d1;
		return 0;
	}

	int GlobalCrt::prjToGeo( const Pos2D&prj,Pos2D&geo )
	{
		return prjToGeo(prj.d0,prj.d1,&geo.d0,&geo.d1);
	}
}