
#include "CRTALG_ProjectionMercatorTransverse.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288 
#endif
namespace GIS_Embedded{


void CRTALG_ProjectionMercatorTransverse::SetCentralMeridian(double centralMeridian)
{
	m_Central_Meridian = centralMeridian;
}

double CRTALG_ProjectionMercatorTransverse::GetCentralMeridian()
{
	return m_Central_Meridian;
}
void CRTALG_ProjectionMercatorTransverse::SetOriginLatitude(double originLat)
{
	m_OriginLatitude = originLat;
}

double CRTALG_ProjectionMercatorTransverse::GetOriginLatitude()
{
	return m_OriginLatitude;
}

void CRTALG_ProjectionMercatorTransverse::SetScaleFactor( double scale_factor)
{
	m_Scale_Factor = scale_factor;
}

double CRTALG_ProjectionMercatorTransverse::GetScaleFactor()
{
	return m_Scale_Factor;
}

CRTALG_ProjectionMercatorTransverse* CRTALG_ProjectionMercatorTransverse::copy() const
{
	CRTALG_ProjectionMercatorTransverse* crt = new CRTALG_ProjectionMercatorTransverse;
	
	crt->_ellipsoid = this->_ellipsoid;

	crt->m_False_East = this->m_False_East;
	crt->m_False_North = this->m_False_North;

	crt->m_OriginLatitude = this->m_OriginLatitude;
	crt->m_Scale_Factor = this->m_Scale_Factor;
	return crt;
}

GIS_Embedded::Pos3D CRTALG_ProjectionMercatorTransverse::projectFromGeographic( const GIS_Embedded::Pos3D& posSource) const
{//对极点不适合
	GIS_Embedded::Pos3D pos;
	double dX, dY;
	double dB, dL;
	
	dL = posSource.d0;
	dB = posSource.d1*M_PI/180;
	if(dL >= m_Central_Meridian-180)//西半球
	{
		dL = dL - m_Central_Meridian;
	}
	else 
	{
		dL = 360 - m_Central_Meridian + dL;
	}
	dL *= M_PI/180;

	double fMajor, dR0;
	
	fMajor = _ellipsoid._semiMajorAxis.getStandardValue();//长半轴
	dR0 = fMajor; //视地球为一个圆球，半径为长半轴

	dX = dR0*atan(tan(dB)/cos(dL) );
	dY = dR0*log((1+cos(dB)*sin(dL))/(1-cos(dB)*sin(dL)) )/2;

	pos.d0 = (dX - dR0*m_OriginLatitude*M_PI/180) * m_Scale_Factor;//平移到原点和缩放比例
	pos.d1 = dY * m_Scale_Factor;
	pos.d0 += m_False_North;
	pos.d1 += m_False_East;
	pos.d2 = posSource.d2;
	
	return pos;
}

GIS_Embedded::Pos3D CRTALG_ProjectionMercatorTransverse::inverseToGeographic( const GIS_Embedded::Pos3D& posSource ) const
{
	GIS_Embedded::Pos3D pos = posSource;

	double dX, dY, dB, dL;
	double fMajor, dR0;
	
	fMajor = _ellipsoid._semiMajorAxis.getStandardValue();//长半轴
	dR0 = fMajor; //视地球为一个圆球，半径为长半轴

	//去除偏移量,恢复换来比例,将原点移到赤道线上
	dX = (posSource.d0 - m_False_North)/m_Scale_Factor + dR0*m_OriginLatitude*M_PI/180;
	dY = (posSource.d1 - m_False_East)/m_Scale_Factor;

	double e, dsinLcosB, powtg, powSinB;
	e = exp(2*dY/dR0);
	dsinLcosB = (e - 1)/(e + 1);
	powtg = pow( tan(dX/dR0), 2);
	powSinB = powtg*(1-pow(dsinLcosB, 2) )/(1+powtg);

	dB = asin(sqrt(powSinB) )*180/M_PI;
	dL = asin(dsinLcosB/sqrt(1-powSinB ) ) * 180/M_PI;

	if(dL < 180 - m_Central_Meridian ) 
	{
		dL += m_Central_Meridian;
	}
	else
	{
		dL = dL = m_Central_Meridian + dL - 360;;
	}	

	if(dX < 0)//南纬度X小于零
		dB = -dB;

	pos.d0 = dL;
	pos.d1 = dB;

	return pos;
}




}//namespace GIS_Embedded
