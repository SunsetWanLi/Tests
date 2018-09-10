#ifndef GIS_Embeded_CRTALG_PROJECTIONMERCATOR_H_
#define GIS_Embeded_CRTALG_PROJECTIONMERCATOR_H_
#include "basic_gm_type.h"
#include "extern.h"
#include "Datum.h"

namespace GIS_Embedded 
{
;
	
class CRTALG_ProjectionMercator
{
public:
	~CRTALG_ProjectionMercator()
	{
	};

	void SetCentralMeridian(double centralMeridian);

	double GetCentralMeridian();

	void SetStandardParallel1(double standard_p1);

	double GetStandardParallel1();

	CRTALG_ProjectionMercator* copy() const;

	GIS_Embedded::Pos3D projectFromGeographic( const GIS_Embedded::Pos3D& posSource) const;

	GIS_Embedded::Pos3D inverseToGeographic( const GIS_Embedded::Pos3D& posSource ) const;
public:
	double m_Standard_Parallel1;//伸缩变形控制 = l*cos(m_Standard_Parallel1)
	double m_False_East;
	double m_False_North;
	double m_Central_Meridian;	//中央子午线
	CD_Ellipsoid _ellipsoid;
};


}//namespace GIS_Embedded

#endif //GIS_Embeded_CRT_PROJECTIONMERCATOR_H_