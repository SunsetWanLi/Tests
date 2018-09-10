
#include "CRTALG_GeographyToGeography.h"
#include "CRS_Factory.h"

namespace GIS_Embedded {
	void CRTALG_GeographyToGeography::geographyFromGeography(double B54,double L54,double dx,double dy,double dz, double& B80, double& L80) const
	{//54 到80的经纬度转换
			
		/*const double fSemiMajor_80 = 6378140; 
		const double fSemiMinor_80 = 6356863.018773;*/ 

		CD_Ellipsoid ellipse80 = CRS_Factory::createEllipse(ETC_IAG_1975); // 54地理坐标系
		const double fSemiMajor_80 = ellipse80._semiMajorAxis.getStandardValue();//80椭球长半径
		const double fSemiMinor_80 = ellipse80._semiMinorAxis.getStandardValue();//80椭球短半径

		double fSemiMajor, fPowFlat, M, W, N;
		
		//double dx, dy, dz; //两椭球参心坐标系的差值

		double dA; //IAG与克氏椭球长半径平方之差
		double dE; //IAG与克氏椭球第一偏心率平方之差

		
		fPowFlat = _ellipsoid._dPowFlattening; //54第一偏心率的平方
		fSemiMajor = _ellipsoid._semiMajorAxis.getStandardValue(); // 54长半轴
		
		dA =  pow(fSemiMajor_80,2) - pow(_ellipsoid._semiMajorAxis.getStandardValue(),2);
		dE =  pow(_ellipsoid._semiMajorAxis.getStandardValue()/_ellipsoid._semiMinorAxis.getStandardValue(),2) 
			- pow(fSemiMajor_80/fSemiMinor_80,2);

		W = sqrt(1 - fPowFlat*pow(sin(B54),2)); // sqrt(1- (esinB)^2)
		M = fSemiMajor*(1 - pow(fPowFlat,2))/pow(W,3); // 
		N = fSemiMajor/W; 

		double dB = (-dx*sin(B54)*cos(L54) - dy*sin(B54)*sin(L54) + dz*cos(B54) 
			+ sin(B54)*cos(B54)*(pow(fPowFlat,2))*dA/W + N*(2 - pow(fPowFlat*sin(B54),2))*dE/(2*pow(W,2)))/M;

		double dL = -(dx*sin(L54) - dy*cos(L54))/(N*cos(B54));

		B80 = B54 + dB;
		L80 = L54 + dL;
		
		return ;
	}

}