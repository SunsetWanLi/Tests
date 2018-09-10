#ifndef _GLOBAL_CRT_H_
#define _GLOBAL_CRT_H_

#include "CRS_if.h"
#include "../Types/basic_types.h"
namespace GIS_Embedded
{

	class GlobalCrt
	{
	public:
		GlobalCrt();
		~GlobalCrt(){}

		int geoToPrj(double geo_x, double geo_y, long *prj_x, long *prj_y);
		int geoToPrj(double geo_x, double geo_y, int_32 *prj_x, int_32 *prj_y);
		int prjToGeo(int_32 prj_x, int_32 prj_y, double *geo_x, double *geo_y);
		int prjToGeo(const Pos2D&prj,Pos2D &geo);
	private:
		static CRT* m_crt;
		static CRT* m_inverse_crt;
	};

}

#endif