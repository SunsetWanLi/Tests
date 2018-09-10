#ifndef _GIS_Embeded_CRTALG_GEOGRAPHY_TO_GEOGRAPHY_H_
#define _GIS_Embeded_CRTALG_GEOGRAPHY_TO_GEOGRAPHY_H_

#include "DatumType.h"
#include "Datum.h"

namespace GIS_Embedded 
{
	class CRTALG_GeographyToGeography {


	public:
		void geographyFromGeography(double B54,double L54,double dx,double dy,double dz,double& B80,double& L80) const; // 输入54地理坐标（B54, L54)，得到80地理坐标(B80,L80),dx,dy,dz为转换参数) ;

	public:
		CD_Ellipsoid _ellipsoid;
	};



}
#endif 