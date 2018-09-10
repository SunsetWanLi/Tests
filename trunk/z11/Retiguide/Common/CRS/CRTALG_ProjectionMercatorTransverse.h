#ifndef GIS_Embeded_CRTALG_PROJECTIONMERCATOR_H_
#define GIS_Embeded_CRTALG_PROJECTIONMERCATOR_H_
#include "basic_gm_type.h"
#include "extern.h"
#include "Datum.h"

namespace GIS_Embedded 
{
;
	
class CRTALG_ProjectionMercatorTransverse
{
public:
	~CRTALG_ProjectionMercatorTransverse()
	{
	};

	void SetCentralMeridian(double centralMeridian);

	double GetCentralMeridian();

	void SetOriginLatitude( double originLat ); //设置中央子午线

	double GetOriginLatitude( ); //返回中央子午线

	void SetScaleFactor( double scale_factor); //设置尺度变换比例

	double GetScaleFactor();		//返回尺度变换比例


	CRTALG_ProjectionMercatorTransverse* copy() const;

	GIS_Embedded::Pos3D projectFromGeographic( const GIS_Embedded::Pos3D& posSource) const;

	GIS_Embedded::Pos3D inverseToGeographic( const GIS_Embedded::Pos3D& posSource ) const;
public:
	double m_OriginLatitude;	//原点纬度
	double m_Scale_Factor;		//尺度变换比例
	double m_False_East;
	double m_False_North;
	CD_Ellipsoid _ellipsoid;
	double m_Central_Meridian;	//中央子午线
};


}//namespace GIS_Embedded

#endif //GIS_Embeded_CRT_PROJECTIONMERCATOR_H_