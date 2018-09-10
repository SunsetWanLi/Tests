
#include "CRT_Conversion.h"
#include "ComputeEquation.h"
#include "CRS.h"
#include <assert.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288 
#endif
namespace GIS_Embedded
{

CRT_Conversion::CRT_Conversion()
{
}

bool CRT_Conversion::operator == (const CRT &right) const
{
	const CRT_Conversion * _right = dynamic_cast<const CRT_Conversion *>(&right);
	if(!_right)
		return false;
	return _ellipsoid == _right->_ellipsoid;
}

//将经纬度坐标转换为空间直角坐标
GIS_Embedded::Pos3D CRT_Conversion::convertFromGeographicToGeocentric( const GIS_Embedded::Pos3D& posSource) const
{
	GIS_Embedded::Pos3D posTarget;

	double dRadiusN; //椭圆半径
	double dEllipMajor,dEllipMinor,dInvertF; //长轴,知轴,第一偏心率的平方
	double dLatitude,dLongitude,dHeight; //纬度,经度(弧度)
	

	dLongitude = posSource.d0*M_PI/180; //转成了弧度
	dLatitude = posSource.d1*M_PI/180;
	dHeight = posSource.d2;

	dEllipMajor = _ellipsoid._semiMajorAxis._dValue ;
	dEllipMinor = _ellipsoid._semiMinorAxis._dValue ;
	dInvertF = _ellipsoid._dPowFlattening ;
	dRadiusN = dEllipMajor/sqrt(1-dInvertF*pow(sin(dLatitude),2) );
	
	posTarget.d0 = (dRadiusN + dHeight)*cos(dLatitude)*cos(dLongitude);
	posTarget.d1 = (dRadiusN + dHeight)*cos(dLatitude)*sin(dLongitude);
	posTarget.d2 = (dRadiusN*(1-dInvertF) + dHeight)*sin(dLatitude);

	return posTarget;
}

//将空间直角坐标转换为经纬度坐标
GIS_Embedded::Pos3D CRT_Conversion::convertFromGeocentricToGeographic( const GIS_Embedded::Pos3D& posSource) const
{
	GIS_Embedded::Pos3D pos;
	double dEllipMajor,dEllipMinor,dInvertF; //长轴,知轴,扁率
	double dX,dY,dZ; 
	double dLatitude,dLongitude,dHeight;     //纬度,经度

	dX = posSource.d0;
	dY = posSource.d1;
	dZ = posSource.d2;

	dEllipMajor = _ellipsoid._semiMajorAxis._dValue ;
	dEllipMinor = _ellipsoid._semiMinorAxis._dValue ;
	dInvertF = _ellipsoid._dPowFlattening ;

	dLongitude = atan2(dY,dX)*180/M_PI;


	//下面利用迭代计算的方式计算 纬度B 高度H 
	double dB0,dB1;  //计算中用到的中间变量
	int    iCount = 0;  //dBSign纬度的符号
	double dFenZi,dFenMu,dN,dXY, dHeight0;

	dXY = pow(dX,2) + pow(dY,2);

	if(dZ >= 0)
	{
		dB0 = 0.7853;          //取初始纬度为45度
	}
	else
	{
		dB0 = -0.7853;          //取初始纬度为45度
	}
	dN = dEllipMajor/sqrt(1 - dInvertF*sin(dB0)*sin(dB0));//dN 为中间变量
	dHeight0 = dZ/sin(dB0) - dN*(1-dInvertF);
	dB1 = dB0;	dHeight = dHeight0;
	do {
		iCount++;		dB0 = dB1;		dHeight0 = dHeight;

		if(fabs(dB0) <= 0.000001)
		{
			dB1 = dB0;
			dHeight = sqrt(dXY)/cos(dB1) - dN;//*(1 - dInFPow2);
		}
		else
		{
			//dFenZi = dZ + dN*dInvertF*sin(dB0);
			dFenZi = dZ*(dN +dHeight0);
			dFenMu = sqrt(dXY)*(dN*(1-dInvertF)+dHeight0);
			dB1 = atan2(dFenZi,dFenMu);
		}
		dN = dEllipMajor/sqrt(1 - dInvertF*sin(dB1)*sin(dB1));//dN0为中间变量
		dHeight = dZ/sin(dB1) - dN*(1-dInvertF);
	} while(( fabs(dB1 - dB0) >= 1e-9 || fabs(dHeight - dHeight0) >= 0.001 ) && iCount <= 1000);//纬度误差1e-5秒
																								//高度误差0.001米
	dLatitude = 180*dB1/M_PI;

	pos.d0 = dLongitude;
	pos.d1 = dLatitude;
	pos.d2 = dHeight;

	return pos;
}

int_32 CRT_Conversion::trans(const Pos3D &src, Pos3D *dest) const
{
	if(m_is_geocentric_to_geographic)
		*dest = convertFromGeocentricToGeographic(src);
	else
		*dest = convertFromGeographicToGeocentric(src);

	return 0;
}

CRT *CRT_Conversion::copy() const
{
	return new CRT_Conversion(*this);
}

}//GIS_Embedded
