
#ifndef GIS_Embeded_DATUM_H_
#define GIS_Embeded_DATUM_H_

#include "SpatialRefMeta.h"
#include "DatumType.h"


namespace GIS_Embedded{

//enum GCSType;

#pragma pack(push, 4)
struct CD_PrimeMeridian
{
	MC_Angle _greenwichLongitude; //the longitude of this prime meridian relative to the GreenWich Meridian.expressed in the related angular unit.
	
	CD_PrimeMeridian & operator = (const CD_PrimeMeridian &right)
	{
		_greenwichLongitude = right._greenwichLongitude;

		return *this;
	}

	bool operator == ( const CD_PrimeMeridian & right ) const
	{
		return ( _greenwichLongitude == right._greenwichLongitude); 
	}
};

struct CD_Ellipsoid
{
	//std::string _strName;
	MC_Length _semiMajorAxis;
	MC_Length _semiMinorAxis;
	double  _dPowFlattening;  //f = (a^2-b^2)/a^2

	CD_Ellipsoid & operator = (const CD_Ellipsoid &right)
	{
		//_strName = right._strName;
		_semiMajorAxis = right._semiMajorAxis;
		_semiMinorAxis = right._semiMinorAxis;
		_dPowFlattening = right._dPowFlattening;

		return *this;
	}
	
	bool operator == ( const CD_Ellipsoid & right ) const
	{
		return ( _semiMajorAxis == right._semiMajorAxis && _semiMinorAxis == right._semiMinorAxis
			&& _dPowFlattening == right._dPowFlattening); 
	}
};

//北京54坐标系基准椭球 克拉索夫斯基椭球(1940 Krassovsky)  a=6378245m b=6356863.018773m α=0.0033523298692 
//西安80坐标系基准椭球 1975年I.U.G.G推荐椭球(国际大地测量协会1975)　a=6378140m b=6356755.2881575m α=0.0033528131778　　　　　　　　　　 
//WGS-84 GPS 基准椭球  WGS-84椭球(GPS全球定位系统椭球、17届国际大地测量协会) a=6378137m b=6356752.3142451m α=0.00335281006247
struct CD_GeodesicDatum
{
	//std::string _strName;
	GCSType m_geo_crs_type;
	CD_PrimeMeridian _primeMeridian;
	CD_Ellipsoid _ellipsoid;

	CD_GeodesicDatum & operator = (const CD_GeodesicDatum &right)
	{
		//_strName = right._strName;
		m_geo_crs_type = right.m_geo_crs_type;
		_primeMeridian = right._primeMeridian;
		_ellipsoid = right._ellipsoid;

		return *this;
	}

	bool operator == ( const CD_GeodesicDatum & right ) const
	{
		return ( _primeMeridian == right._primeMeridian && _ellipsoid == right._ellipsoid); // type is not evaluated
	}
};
#pragma pack(pop)

}//namespace GIS_Embedded
#endif