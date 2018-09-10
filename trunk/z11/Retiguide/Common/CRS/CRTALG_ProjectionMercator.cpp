
#include "CRTALG_ProjectionMercator.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288 
#endif
namespace GIS_Embedded{

void CRTALG_ProjectionMercator::SetCentralMeridian(double centralMeridian)
{
	m_Central_Meridian = centralMeridian;
}

double CRTALG_ProjectionMercator::GetCentralMeridian()
{
	return m_Central_Meridian;
}

void CRTALG_ProjectionMercator::SetStandardParallel1(double standard_p1)
{
	m_Standard_Parallel1 = standard_p1;
}

double CRTALG_ProjectionMercator::GetStandardParallel1()
{
	return m_Standard_Parallel1;
}

CRTALG_ProjectionMercator* CRTALG_ProjectionMercator::copy() const
{
	CRTALG_ProjectionMercator* crt = new CRTALG_ProjectionMercator;
	
	crt->_ellipsoid = this->_ellipsoid;
	crt->m_Central_Meridian = this->m_Central_Meridian;
	crt->m_False_East = this->m_False_East;
	crt->m_False_North = this->m_False_North;
	crt->m_Standard_Parallel1 = this->m_Standard_Parallel1;

	return crt;
}

GIS_Embedded::Pos3D CRTALG_ProjectionMercator::projectFromGeographic( const GIS_Embedded::Pos3D& posSource) const
{//对极点不适合

	GIS_Embedded::Pos3D pos;
	double dX, dY;
	double dB, dL;
	dL = posSource.d0;
	if(dL >= m_Central_Meridian-180)//西半球
	{
		dL = dL - m_Central_Meridian;
	}
	else 
	{
		dL = 360 - m_Central_Meridian + dL;
	}
	dL *= M_PI/180;

	if(posSource.d1 <= -89.9)//-90， log(0)->负无穷，避免！
		dB = -89.9*M_PI/180;	
	else if(posSource.d1 >= 89.9)//90, tan(90)->正无穷，避免！
		dB = 89.9*M_PI/180;
	else
		dB = posSource.d1*M_PI/180;

	double m_OriginLatitude = 0;

	double fMajor, fMinor, fPowFlat,dN0,dR0;

	double originLat = m_OriginLatitude*M_PI/180;


	fMajor = _ellipsoid._semiMajorAxis.getStandardValue();//长半轴
	fMinor = _ellipsoid._semiMinorAxis.getStandardValue();//短半轴
	fPowFlat = _ellipsoid._dPowFlattening ; //第一偏心率的平方
	dN0 = fMajor/sqrt(1 - fPowFlat*pow(sin(originLat), 2) );
	dR0 = dN0*cos(originLat);

	double q1 = log(tan(M_PI/4 + dB/2) );
	double fPowFlate = sqrt(fPowFlat);
	double q2 = fPowFlate /2 * log( (1 + fPowFlate* sin(dB) ) / (1 - fPowFlate * sin (dB) )  );
	double q = q1 - q2;

	dX = dR0*q;
	dY = dR0*dL;
	pos.d0 = dX * cos(m_Standard_Parallel1*M_PI/180) + m_False_North;
	pos.d1 = dY * cos(m_Standard_Parallel1*M_PI/180) + m_False_East;
	pos.d2 = posSource.d2;

	return pos;

	//////////////////////////////////////////////////////////////////////////
	////Below is the cold before Modified by LY 2010-3-11, zj//////////////////
	//////////////////////////////////////////////////////////////////////////

	//GIS_Embedded::Pos3D pos;
	//double dX, dY;
	//double dB, dL;
	//dL = posSource.d0;
	//if(dL >= m_Central_Meridian-180)//西半球
	//{
	//	dL = dL - m_Central_Meridian;
	//}
	//else 
	//{
	//	dL = 360 - m_Central_Meridian + dL;
	//}
	//dL *= PI/180;

	//if(posSource.d1 <= -89.9)//-90， log(0)->负无穷，避免！
	//	dB = -89.9*PI/180;	
	//else if(posSource.d1 >= 89.9)//90, tan(90)->正无穷，避免！
	//	dB = 89.9*PI/180;
	//else
	//	dB = posSource.d1*PI/180;

	//double fMajor, fMinor, dR0;
	////double originLat = m_OriginLatitude*PI/180;
	//
	//fMajor = _ellipsoid._semiMajorAxis.getStandardValue();//长半轴
	//fMinor = _ellipsoid._semiMinorAxis.getStandardValue();//短半轴
	////fPowFlat = _ellipsoid._dPowFlattening ; //第一偏心率的平方
	////dN0 = fMajor/sqrt(1 - fPowFlat*pow(sin(originLat), 2) );
	//dR0 = (fMajor+fMinor)/2/*dN0*cos(originLat)*/;

	//dX = dR0*log(tan(PI/4 + dB/2 ) );
	//dY = dR0*dL;
	//pos.d0 = dX * cos(m_Standard_Parallel1*PI/180) + m_False_North;
	//pos.d1 = dY * cos(m_Standard_Parallel1*PI/180) + m_False_East;
	//pos.d2 = posSource.d2;
	//
	//return pos;
}

GIS_Embedded::Pos3D CRTALG_ProjectionMercator::inverseToGeographic( const GIS_Embedded::Pos3D& posSource ) const
{
	GIS_Embedded::Pos3D pos;

	double dB, dL, dX, dY;
	double fMajor, fMinor, fPowFlat, dN0,dR0;
	double m_OriginLatitude = 0;
	double originLat = m_OriginLatitude*M_PI/180;
	dX = dY = 0;
	if(cos(m_Standard_Parallel1*M_PI/180) != 0)
	{
		dX = (posSource.d1 - m_False_North) / cos(m_Standard_Parallel1*M_PI/180);
		dY = (posSource.d0 -m_False_East) / cos(m_Standard_Parallel1*M_PI/180);
	}

	fMajor = _ellipsoid._semiMajorAxis.getStandardValue();//长半轴
	fMinor = _ellipsoid._semiMinorAxis.getStandardValue();//短半轴
	fPowFlat = _ellipsoid._dPowFlattening ; //第一偏心率的平方
	dN0 = fMajor/sqrt(1 - fPowFlat*pow(sin(originLat), 2) );

	dR0 = dN0*cos(originLat);

	dL = dX/dR0 * 180/M_PI;
	dB = 0;

	double E = exp(1.0);
	double fPowFlat_sqrt = sqrt(fPowFlat);//第一偏心率
	double part1 = pow(E,(-dY / dR0));
	for(int i = 0; i < 20; ++i)
	{		
		double part2 = pow(E,(fPowFlat_sqrt/2) * log( (1 - fPowFlat_sqrt* sin(dB) ) / (1 + fPowFlat_sqrt * sin (dB))));
		dB = M_PI/2 - 2 * atan( part1 * part2 );
	}
	dB *= 180 / M_PI;

	if(dL < 180 - m_Central_Meridian ) 
	{
		dL += m_Central_Meridian;
	}
	else
	{
		dL = m_Central_Meridian + dL - 360;
	}

	pos.d0 = dL;
	pos.d1 = dB;
	pos.d2 = posSource.d2;
	return pos;

	//////////////////////////////////////////////////////////////////////////
	////Below is the cold before Modified by LY 2010-3-11, zj//////////////////
	//////////////////////////////////////////////////////////////////////////

	//GIS_Embedded::Pos3D pos;

	//double dB, dL, dX, dY;
	//double fMajor, fMinor, dR0;
	////double originLat = m_OriginLatitude*PI/180;
	//dX = dY = 0;
	//if(cos(m_Standard_Parallel1*PI/180) != 0)
	//{
	//	dX = (posSource.d1 - m_False_North) / cos(m_Standard_Parallel1*PI/180);
	//	dY = (posSource.d0 -m_False_East) / cos(m_Standard_Parallel1*PI/180);
	//}

	//fMajor = _ellipsoid._semiMajorAxis.getStandardValue();//长半轴
	//fMinor = _ellipsoid._semiMinorAxis.getStandardValue();//短半轴
	////fPowFlat = _ellipsoid._dPowFlattening ; //第一偏心率的平方
	////dN0 = fMajor/sqrt(1 - fPowFlat*pow(sin(originLat), 2) );

	//dR0 = (fMajor+fMinor)/2/*dN0*cos(originLat)*/;

	//dL = dX/dR0 * 180/PI;
	//dB = 2*( atan(exp(dY/dR0) ) - PI/4 ) * 180/PI;

	//if(dL < 180 - m_Central_Meridian ) 
	//{
	//	dL += m_Central_Meridian;
	//}
	//else
	//{
	//	dL = m_Central_Meridian + dL - 360;
	//}

	//pos.d0 = dL;
	//pos.d1 = dB;
	//pos.d2 = posSource.d2;
	//return pos;
}




}//namespace GIS_Embedded
