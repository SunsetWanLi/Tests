
#include "basic_gm_type.h"
#include "CRTALG_ProjectionGauss.h"
#include "CRTALG_GeographyToGeography.h"
#include "ComputeEquation.h"
#include "CRS_Factory.h"
//#include "CRS.h"
#include <assert.h>
#include <iostream>
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288 
#endif
namespace GIS_Embedded{

ProjectXY CRTALG_ProjectionGauss::_projectGauss[18][7] =
{	{{0.0,0.0,184.3,185.5},{0.0,55660.5,184.3,185.5},{0.0,111325.2,184.3,185.6},{0.0,166998.5,184.4,185.6},{0.0,222684.6,184.4,185.7},{0.0,278387.8,184.5,185.7},{0.0,334112.4,184.5,185.7}},  //1  0
	{{552885.7,0.0,184.3,184.8},{552906.8,55449.8,184.4,184.7},{552970.1,110901.8,184.6,184.5},{553075.5,166358.3,184.8,184.5},{553223.3,221821.7,184.9,184.3},{553413.3,277294.0,185.1,184.2},{553645.6,332777.7,185.4,184.1}},         //2 5
	{{1105855.3,0.0,184.4,182.7},{1105896.9,54824.2,184.6,182.5},{1106021.5,109675.5,185.0,182.2},{1106229.4,164580.9,185.2,181.9},{1106520.6,219567.6,185.6,181.5},{1106895.5,274662.9,185.9,180.9},{1107354.2,329894.3,186.3,179.9}},  //3 10
	{{1658990.4,0.0,184.4,179.2},{1659051.1,53775.3,184.8,178.8},{1659233.3,107550.6,185.3,178.4},{1659537.0,161325.9,185.7,178.0},{1659962.4,215101.3,186.1,177.5},{1660509.5,268876.8,186.6,177.0},{1661178.5,322652.3,187.1,176.5}},  //4 15
	{{2212367.3,0.0,184.5,174.4},{2212445.4,52323.7,185.0,173.8},{2212679.7,104647.9,185.5,173.3},{2213070.2,156973.3,186.1,172.8},{2213617.1,209300.5,186.7,172.2},{2214320.6,261630.0,187.2,171.5},{2215180.7,313962.4,187.9,170.9}},  //5 20
	{{2766055.5,0.0,184.6,168.2},{2766148.5,50469.6,185.3,167.4},{2766427.7,100906.2,185.9,165.9},{2766892.9,151277.1,186.5,162.9},{2767543.9,201549.1,187.1,158.1},{2768380.4,251689.3,187.8,150.6},{2769402.2,301664.6,188.4,140.0}},  //6 25
	{{3320114.9,0.0,184.8,160.8},{3320220.2,48243.6,185.5,160.0},{3320536.0,96490.0,186.2,159.2},{3321062.4,144742.0,186.9,158.4},{3321799.6,193002.2,187.6,157.6},{3322747.8,241273.5,188.3,156.8},{3323907.2,289558.7,189.1,156.0}},  //7  30
	{{3874594.7,0.0,184.9,152.1},{3874708.9,45644.8,185.7,151.2},{3875051.7,91293.5,186.4,150.3},{3875623.0,136950.2,187.2,149.4},{3876423.2,182618.9,187.9,148.4},{3877452.3,228303.4,188.8,147.6},{3878710.7,274007.9,189.6,146.6}},  //8  35
	{{4429531.1,0.0,185.1,142.3},{4429650.8,42697.4,185.9,141.3},{4430010.1,85397.4,186.7,140.2},{4430609.1,128102.6,187.4,139.2},{4431447.7,170815.6,188.3,138.2},{4432526.4,213539.1,189.1,137.2},{4433845.3,256275.7,189.9,136.1}},  //9  40
	{{4984946.7,0.0,185.2,131.4},{4985068.3,39423.4,186.1,130.3},{4985433.3,78846.9,186.8,129.1},{4986041.5,118270.3,187.7,128.0},{4986893.3,157693.8,188.4,126.7},{4987988.5,197117.3,189.3,125.5},{4989327.6,236540.8,190.0,124.2}},  //10 45
	{{5540849.6,0.0,185.4,119.5},{5540969.4,35848.2,186.2,118.2},{5541328.9,71697.8,187.0,117.1},{5541928.2,107550.6,187.7,115.8},{5542767.2,143408.0,188.6,114.6},{5543846.2,179271.6,193.4,113.3},{5545165.4,215142.9,190.1,112.0}},  //11 50
	{{6097233.2,0.0,185.5,106.7},{6097347.5,31997.3,186.3,105.3},{6097690.6,63996.0,187.1,104.0},{6098262.6,95997.4,187.8,102.7},{6099063.3,128002.9,188.6,101.3},{6100093.1,160013.8,189.3,100.0},{6101352.1,192031.6,190.0,98.7}},  //12 55
	{{6654075.9,0.0,185.7,93.0},{6654181.4,27900.1,186.3,91.6},{6654497.6,55800.7,187.1,90.2},{6655024.8,83702.4,187.8,88.8},{6655763.0,111605.7,188.4,87.4},{6656712.1,139511.2,189.1,85.9},{6657872.4,167419.2,189.8,84.6}},  //13 60
	{{7211342.5,0.0,185.8,78.6},{7211435.8,23587.2,186.4,77.1},{7211715.6,47171.1,187.0,75.4},{7212181.9,70748.3,187.7,73.4},{7212834.8,94315.4,188.2,70.9},{7213674.0,117869.2,188.8,67.6},{7214699.6,141406.2,189.3,63.6}},  //14 65
	{{7768984.4,0.0,185.9,63.6},{7769062.4,19093.3,186.5,62.1},{7769297.5,38186.6,187.0,60.6},{7769689.0,57280.0,187.4,59.0},{7770237.0,76373.4,187.9,57.5},{7770941.7,95467.0,188.4,56.0},{7771803.0,114560.7,188.8,54.4}},  //15 70
	{{8326941.5,0.0,186.0,48.2},{8327002.4,14451.0,186.4,46.6},{8327185.1,28902.0,186.8,45.0},{8327489.6,43353.0,187.2,43.5},{8327916.0,57804.0,187.0,41.9},{8328464.2,72255.0,187.9,40.4},{8329134.2,86706.1,188.3,38.7}},  //16 75
	{{8885144.0,0.0,186.1,32.3},{8885185.7,9696.8,186.4,30.7},{8885310.7,19393.7,186.6,29.1},{8885519.0,29090.9,186.9,27.5},{8885810.7,38788.6,187.1,25.9},{8886185.8,48486.8,187.3,24.3},{8886644.2,58185.8,187.5,22.6}},  //17 80
	{{9443514.6,0.0,186.1,16.2},{9443535.7,4867.3,186.3,14.6},{9443599.2,9734.5,186.4,13.0},{9443705.0,14601.8,186.5,11.4},{9443853.1,19469.1,186.6,9.7},{9444043.5,24336.3,186.7,8.2},{9444276.2,29203.6,186.0,6.5}}   //18 85
};

void CRTALG_ProjectionGauss::SetOriginLatitude(double originLat)
{
	m_OriginLatitude = originLat;
}

double CRTALG_ProjectionGauss::GetOriginLatitude()
{
	return m_OriginLatitude;
}

void CRTALG_ProjectionGauss::SetScaleFactor( double scale_factor)
{
	m_Scale_Factor = scale_factor;
}

double CRTALG_ProjectionGauss::GetScaleFactor()
{
	return m_Scale_Factor;
}

CRTALG_ProjectionGauss* CRTALG_ProjectionGauss::copy() const
{
	CRTALG_ProjectionGauss* crt = new CRTALG_ProjectionGauss;

	crt->_ellipsoid = this->_ellipsoid;//in common
//	crt->m_projectType = this->m_projectType;
	crt->m_Central_Meridian = this->m_Central_Meridian;
	crt->m_False_East = this->m_False_East;
	crt->m_False_North = this->m_False_North;

	crt->m_OriginLatitude = this->m_OriginLatitude;
	crt->m_Scale_Factor = this->m_Scale_Factor;

	return crt;
}

double CRTALG_ProjectionGauss::getPrimeArcLength(double dB) const
{
	//以下五个常系数只是克拉索夫斯基椭球有意义，对于其它椭球的投影需另行设置
	//double C0 = 111133.0046 , C1 = 16038.528 , C2 = 16.833 , C3 = 0.022 , C4 = 0.00003 ;
	double C0, C1, C2, C3, C4, fPowFlat ,  fSemiMajor , dS;

	fPowFlat = _ellipsoid._dPowFlattening ; //第一偏心率的平方
	fSemiMajor =  _ellipsoid._semiMajorAxis.getStandardValue();//长半轴

	C0 = 1 + 3*fPowFlat/4 + 45*pow(fPowFlat,2)/64+175*pow(fPowFlat,3)/256+11025*pow(fPowFlat,4)/16384 ;
	C1 = 3*fPowFlat/8+15*pow(fPowFlat,2)/32+525*pow(fPowFlat,3)/1024+2205*pow(fPowFlat,4)/4096;
	C2 = 15*pow(fPowFlat,2)/256+105*pow(fPowFlat,3)/1024+2205*pow(fPowFlat,4)/16384 ;
	C3 = 35*pow(fPowFlat,3)/3072+105*pow(fPowFlat, 4)/4096 ;
	C4 = 315*pow(fPowFlat, 4)/131072;

	dS = fSemiMajor*(1-fPowFlat)*(C0*dB - C1*sin(2*dB) + C2*sin(4*dB) - C3*sin(6*dB) + C4*sin(8*dB) );

	return dS;
}

double CRTALG_ProjectionGauss::getLatitudeFromArcLen(double arcLength) const
{
	double dB;
	double C0, C1, C2, C3, C4, fPowFlat ,  fSemiMajor;

	fPowFlat = _ellipsoid._dPowFlattening ; //第一偏心率的平方
	fSemiMajor =  _ellipsoid._semiMajorAxis.getStandardValue();//长半轴

	C0 = 1 + 3*fPowFlat/4 + 45*pow(fPowFlat,2)/64+175*pow(fPowFlat,3)/256+11025*pow(fPowFlat,4)/16384 ;
	C1 = 3*fPowFlat/8+15*pow(fPowFlat,2)/32+525*pow(fPowFlat,3)/1024+2205*pow(fPowFlat,4)/4096;
	C2 = 15*pow(fPowFlat,2)/256+105*pow(fPowFlat,3)/1024+2205*pow(fPowFlat,4)/16384 ;
	C3 = 35*pow(fPowFlat,3)/3072+105*pow(fPowFlat, 4)/4096 ;
	C4 = 315*pow(fPowFlat, 4)/131072;

	int iCount = 0;
	double FB0, FB1, dB0;
	dB = arcLength/(fSemiMajor*(1-fPowFlat)*C0);
	do{
		iCount++;
		dB0 = dB;

		FB0 = fSemiMajor*(1-fPowFlat)*(C0*dB - C1*sin(2*dB) + C2*sin(4*dB) - C3*sin(6*dB) + C4*sin(8*dB) );
		FB1 = fSemiMajor*(1-fPowFlat)*(C0 - 2*C1*cos(2*dB) + 4*C2*cos(4*dB) - 6*C3*cos(6*dB) + 8*C4*cos(8*dB) );
		dB = dB0 + (arcLength - FB0)/FB1;
	}while(fabs(dB - dB0) >= 1e-9 && iCount <= 1000);

	return dB;
}

void CRTALG_ProjectionGauss::getProjectXY( double dB , double dL , double& dX , double& dY ) const  
{//传入纬度和经度(弧度表示的)，返回对应的平面坐标
	//计算中用来简化式子的几个变量
	double dN, fSemiMajor, fPowFlat, Pow2dR, dS, dFirst, dSecond, dThird;

	fPowFlat = _ellipsoid._dPowFlattening ; //第一偏心率的平方
	fSemiMajor = _ellipsoid._semiMajorAxis.getStandardValue();//长半轴

	dN = fSemiMajor/sqrt(1 - fPowFlat*pow(sin(dB), 2) );
	Pow2dR = fPowFlat*pow(cos(dB),2)/(1-fPowFlat);
	dS = getPrimeArcLength(dB);		//计算子午线弧长
	//先计算X坐标
	dFirst = dN*pow(dL, 2)*sin(dB)*cos(dB);
	dSecond = pow(dL*cos(dB), 2)*(5-pow(tan(dB), 2) + 9*Pow2dR + 4*Pow2dR*Pow2dR );
	dThird = pow(dL*cos(dB), 4)*(61-58*pow(tan(dB), 2)+pow(tan(dB), 4) + 270*Pow2dR*Pow2dR-330*Pow2dR*pow(tan(dB),2) );

	dX = dS + dFirst/2 + dFirst*dSecond/24 + dFirst*dThird/720;

	//计算Y坐标
	dFirst = dN*dL*cos(dB);
	dSecond = pow(dL*cos(dB), 2)*(1-pow(tan(dB), 2)+Pow2dR);
	dThird = pow(dL*cos(dB), 4)*(5-18*pow(tan(dB), 2)+pow(tan(dB),4)+14*Pow2dR-58*Pow2dR*pow(tan(dB),2) );

	dY = dFirst + dFirst*dSecond/6 + dFirst*dThird/120;

	return;
}

void CRTALG_ProjectionGauss::inverseFromPrjXY(double dX, double dY, double&dB, double& dL) const
{
	double fPowFlat, fMajor, dBf, tBf, dWf, dRf, dFirst, dSecond, dThird;

	fPowFlat = _ellipsoid._dPowFlattening ; //第一偏心率的平方
	fMajor = _ellipsoid._semiMajorAxis.getStandardValue();//长半轴

	dBf = getLatitudeFromArcLen(dX);	//得到底点纬度
	tBf = tan(dBf);
	dWf = sqrt( 1-fPowFlat*sin(dBf)*sin(dBf) );
	dRf = fPowFlat*cos(dBf)*cos(dBf)/(1-fPowFlat);

	dFirst = tBf*pow(dWf, 4)*pow(dY,2)/(2*pow(fMajor,2)*(1-fPowFlat) ) ;
	dSecond = pow(dY*dWf/fMajor,2)*(5+3*tBf*tBf+dRf*dRf-9*pow(tBf*dRf,2)-4*pow(dRf,4) ) ;
	dThird = pow(dY*dWf/fMajor,4)*(61+90*tBf*tBf+45*pow(tBf,4)+46*dRf*dRf-252*pow(tBf*dRf,2)-90*pow(dRf*tBf*tBf,2));

	dB = dBf - dFirst + dFirst*dSecond/12 - dFirst*dThird/360;

	dFirst = dY*dWf/(cos(dBf)*fMajor);
	dSecond = (1+2*tBf*tBf+dRf*dRf)*pow(dY*dWf/fMajor,2);
	dThird = (5+28*tBf*tBf+24*pow(tBf,4)+6*dRf*dRf+8*pow(dRf*tBf,2))*pow(dY*dWf/fMajor, 4);

	dL = dFirst - dFirst*dSecond/6 + dFirst*dThird/120;
}

GIS_Embedded::Pos3D CRTALG_ProjectionGauss::projectFromGeographic( const GIS_Embedded::Pos3D& posSource) const
{//利用公式计算投影的平面坐标  经度和纬度posSource.d0 经度； posSource.d1纬度； posSource.d2高度
	if(posSource.d0 > 180 || posSource.d0 < -180 
		|| posSource.d1 > 90 || posSource.d1 < -90)
		return posSource;

	GIS_Embedded::Pos3D pos;

	double dLatitude,dLongitude,dAltitude;
	double dProjectX,dProjectY;
	double dDiffLong,dDiffAngle;   //距中央经线的经差,fDiffAngle为角度,fDiffLong为弧度

	dLongitude = posSource.d0; //经度	
	dLatitude = posSource.d1*M_PI/180; //纬度转成弧度表示
	dAltitude = posSource.d2; //高度

	//GIS_Embedded::Pos3D originPos;//得到原点位置投影坐标
	//originPos.d0 = m_Central_Meridian;	
	//originPos.d1 = m_OriginLatitude;
	//originPos.d2 = 0;
	//originPos = project6DuZone(originPos);

	 if(dLongitude >= m_Central_Meridian-180)//西半球
	{
		dDiffAngle = dLongitude - m_Central_Meridian;
	}
	else 
	{
		dDiffAngle = 360 - m_Central_Meridian + dLongitude;
	}
	dDiffLong = (dDiffAngle*M_PI)/180;  //转成弧度表示

	//利用公式求出对应的投影平面坐标X  Y
	getProjectXY(dLatitude,dDiffLong,dProjectX,dProjectY);

	pos.d0 = dProjectX - getPrimeArcLength(m_OriginLatitude*M_PI/180); //原点位置的平移
	pos.d1 = dProjectY ;
	pos.d2 = dAltitude ;
	
	pos.d0 = pos.d0 * m_Scale_Factor + m_False_North; //尺度变换和平移变换
	pos.d1 = pos.d1 * m_Scale_Factor + m_False_East;
	pos.d2 *= m_Scale_Factor;

	return pos;
}


//// dx dy 为新系统下的总体平差改正量
//GIS_Embedded::Pos3D CRTALG_ProjectionGauss::project80FromGeographic54(double dx, double dy,const GIS_Embedded::Pos3D& shift_val,const GIS_Embedded::Pos3D& posSource) const
//{
//	//posSource.d0: B54, posSource.d1: L54, posSource.d2: H
//
//	double X80, Y80, X54, Y54;
//
//	getProjectXY(posSource.d0,posSource.d1,X54,Y54); // 54下的高斯投影坐标
//	
//
//	X80 = X54 + dx; // X54 + DX
//	Y80 = Y54 + dy; // Y54 + DY
//
//	GIS_Embedded::Pos3D result(X80,Y80,0);
//		
//	return result;
//
//}

int_32 CRTALG_ProjectionGauss::projectFromProject(double DX,double DY,double X00,double Y00,double& X11, double& Y11,bool is_54_to_80) const
{
	if(is_54_to_80) //54 -> 80 转换
	{
		X11 = X00 + DX;
		Y11 = Y00 + DY;
	}
	else // 80 -> 54
	{
		X11 = X00 - DX;
		Y11 = Y00 - DY;
	}

	return 0;
}



// x80,y80 为新系统下整体平差后的高斯平面坐标值,一般直接从地图上读取
GIS_Embedded::Pos3D CRTALG_ProjectionGauss::getDXYFromGeography(double x80, double y80, const GIS_Embedded::Pos3D& shift_val, const GIS_Embedded::Pos3D& posSource) const
{
	double B80, L80;
	double X80, Y80, X54, Y54;

	CRTALG_GeographyToGeography crt_alg;
	crt_alg._ellipsoid = this->_ellipsoid;

	crt_alg.geographyFromGeography(posSource.d0,posSource.d1,shift_val.d0,shift_val.d1,shift_val.d2,B80,L80);
	getProjectXY(B80,L80,X80,Y80);


	getProjectXY(posSource.d0,posSource.d1,X54,Y54);

	double dx1 = X80 - X54;
	double dy1 = Y80 - Y54;

	double dx2 = x80 - X80;
	double dy2 = y80 - Y80;

	double DX = dx1 + dx2;
	double DY = dy1 + dy2;

	GIS_Embedded::Pos3D temp(DX,DY,0);

	return temp;
}


GIS_Embedded::Pos3D CRTALG_ProjectionGauss::inverseToGeographic( const GIS_Embedded::Pos3D& posSource ) const
{
	GIS_Embedded::Pos3D pos;
	double dProjectX,dProjectY,dAltitude;
	double dLatitue,dLongitude;

	dProjectX = (posSource.d0 - m_False_North) / m_Scale_Factor; //恢复到原来的尺度和位置
	dProjectY = (posSource.d1 - m_False_East) / m_Scale_Factor;
	dAltitude = posSource.d2 ;

	dProjectX += getPrimeArcLength(m_OriginLatitude*M_PI/180) ;//从原点位置恢复到赤道位置

	inverseFromPrjXY(dProjectX, dProjectY, dLatitue, dLongitude);
	double dX, dY, dB, dL;
	int count = 0;
	getProjectXY(dLatitue, dLongitude, dX, dY);
	
	do{ //减少因为反算带来的误差
		count++;
		inverseFromPrjXY(dX-dProjectX, dY-dProjectY, dB, dL);
		dLatitue -= dB;
		dLongitude -= dL;
		getProjectXY(dLatitue, dLongitude, dX, dY);
	}while( (fabs(dProjectX-dX)>1e-3 ||fabs(dProjectY-dY)>1e-3 )&& count <= 1000);

	dLatitue *= 180/M_PI;
	dLongitude *= 180/M_PI;

	if(dLongitude < 180 - m_Central_Meridian ) //西半球
	{
		dLongitude += m_Central_Meridian;
	}
	else
	{
		dLongitude = dL = m_Central_Meridian + dL - 360;;
	}	

	pos.d0 = dLongitude;
	pos.d1 = dLatitue;
	pos.d2 = dAltitude;

	return pos;
}

GIS_Embedded::Pos3D CRTALG_ProjectionGauss::project6DuZone( GIS_Embedded::Pos3D& posSource )
{//6度分带投影
	if(posSource.d0 > 180 || posSource.d0 < -180 
		|| posSource.d1 > 90 || posSource.d1 < -90)
		return posSource;

	GIS_Embedded::Pos3D pos;

	double dLatitude,dLongitude,dAltitude;
	double dProjectX,dProjectY;
	int iZone;          //分度带带区
	double dDiffLong,dDiffAngle;   //距中央经线的经差,fDiffAngle为角度,fDiffLong为弧度

	dLongitude = posSource.d0; //经度	
	dLatitude = posSource.d1*M_PI/180; //纬度转成弧度表示
	dAltitude = posSource.d2; //高度
	//求分度带带区和距中央经线的经差
	if(dLongitude >= 0) //东半球
	{
		iZone = (int)ceil(dLongitude/6);
		dDiffAngle = dLongitude - 6*iZone + 3;
	}
	else //西半球
	{
        dLongitude=dLongitude+360;
		iZone = (int)ceil(dLongitude/6);
		dDiffAngle = dLongitude - 6*iZone + 3;
	}
	dDiffLong = (dDiffAngle*M_PI)/180;  //转成弧度表示

	//利用公式求出对应的投影平面坐标X  Y
	getProjectXY(dLatitude,dDiffLong,dProjectX,dProjectY);

	//将坐标y转化为国家规定的格式 y+500000+1000000*iZone
	dProjectY += 500000 + 1000000*iZone;

	pos.d0 = dProjectX;
	pos.d1 = dProjectY;
	pos.d2 = dAltitude;
	
	return pos;
}

GIS_Embedded::Pos3D CRTALG_ProjectionGauss::inverse6DuZoneToGeographic(GIS_Embedded::Pos3D& posSource)
{//通过公式方式反解平面坐标对应的经纬度坐标 6度
	GIS_Embedded::Pos3D pos;
	double dProjectX,dProjectY,dAltitude;
	double dLatitue,dLongitude;
	int iZone = (int)posSource.d1/1000000;
	bool   bLeft = false;  //在分度带中央经线左边为true，在右边为false
	bool   bNorth = true;  //在北半球为true,在南北球为false

	dProjectX = posSource.d0;
	dProjectY = posSource.d1;
	dAltitude = posSource.d2;

	//求分带区
	dProjectY -= 1000000*iZone;
	dProjectY -= 500000;
	if(dProjectY >= 0)
	{
		bLeft = false;
	}
	else
	{
		bLeft = true;
		dProjectY = - dProjectY;
	}
	if(dProjectX >= 0)
	{
		bNorth = true;
	}
	else
	{
		bNorth = false;
		dProjectX = -dProjectX;
	}

	//迭代计算中用到的中间变量
	double dProjectX0;
	double dProjectY0;
	double dDiffB0,dDiffB1;
	double dDiffL0,dDiffL1;
	double dDeltaX,dDeltaY;
	int    iBRow,iLCol; //iBRow 表示纬度查表号（行号） iLCol表示经度查表号（列号）

	//求与中央经线的经差(角度)
	dDiffB0 = 45; dDiffL0 = 1.5;   //设迭代初始值
	dDeltaX = 187.7; dDeltaY = 128.0;    //用初始的迭代值

	dProjectX0 = 4986041.5;   dProjectY0 = 118270.3;  //相应纬经度的初始平直坐标值
	while(fabs(dProjectX - dProjectX0) > 1e-5 || fabs(dProjectY - dProjectY0) > 1e-5)
	{
		dDiffB1 = dDiffB0 + (dProjectX - dProjectX0)/(600*dDeltaX);  //以角度表示
		dDiffL1 = dDiffL0 + (dProjectY - dProjectY0)/(600*dDeltaY);

		//求新的fProjectX0,fProjectY0,fDeltaX,fDeltaY
		//求查表的行号和列号
		iBRow = int(floor(dDiffB1))/5;     //纬度以每5度递增
		iLCol = int(floor(dDiffL1*10))/5;  //经度以每30分(0.5度)递增

		dDeltaX = _projectGauss[iBRow][iLCol]._fDeltaX;
		dDeltaY = _projectGauss[iBRow][iLCol]._fDeltaY;
		getProjectXY(dDiffB1*M_PI/180, dDiffL1*M_PI/180, dProjectX0, dProjectY0);
		dDiffB0 = dDiffB1;
		dDiffL0 = dDiffL1;
	}

	//求经度
	if(bLeft == true)
	{
		dLongitude = -dDiffL0;
	}
	else
	{
		dLongitude = dDiffL0;
	}
	if(iZone <= 30)  //东经
	{
		dLongitude += 6*iZone - 3;
	}
	else   //西经
	{   
		dLongitude = dLongitude - 360;
		dLongitude += 6*iZone  -3;
	}
	//求纬度
	if(bNorth == true)  //北纬
	{
		dLatitue = dDiffB0;
	}
	else   //南纬
	{
		dLatitue = -dDiffB0;
	}

	pos.d0 = dLongitude;
	pos.d1 = dLatitue;
	pos.d2 = dAltitude;

	return pos;
}
GIS_Embedded::Pos3D CRTALG_ProjectionGauss::project3DuZone(GIS_Embedded::Pos3D& posSource )
{//3度分带投影
	if(posSource.d0 > 180 || posSource.d0 < -180 
		|| posSource.d1 > 90 || posSource.d1 < -90)
		return posSource;

	GIS_Embedded::Pos3D pos;

	double dLatitude,dLongitude,dAltitude;
	double dProjectX,dProjectY;
	int iZone;          //分度带带区
	double dDiffLong,dDiffAngle;   //距中央经线的经差,fDiffAngle为角度,fDiffLong为弧度

	dLongitude = posSource.d0-1.5; //经度	
	dLatitude = posSource.d1*M_PI/180; //纬度转成弧度表示
	dAltitude = posSource.d2; //高度
	//求分度带带区和距中央经线的经差
	if(dLongitude >= 0) 
	{
		iZone = (int)ceil(dLongitude/3);
		dDiffAngle = dLongitude - 3*iZone + 1.5;
	}
	else 
	{
        dLongitude=dLongitude+360;
		iZone = (int)ceil(dLongitude/3);
		dDiffAngle = dLongitude - 3*iZone + 1.5;
	}
	dDiffLong = (dDiffAngle*M_PI)/180;  //转成弧度表示

	//利用公式求出对应的投影平面坐标X  Y
	getProjectXY(dLatitude,dDiffLong,dProjectX,dProjectY);

	//将坐标y转化为国家规定的格式 y+500000+1000000*iZone
	dProjectY += 500000 + 1000000*iZone;

	pos.d0 = dProjectX;
	pos.d1 = dProjectY;
	pos.d2 = dAltitude;
	
	return pos;
}

GIS_Embedded::Pos3D CRTALG_ProjectionGauss::inverse3DuZoneToGeographic(GIS_Embedded::Pos3D& posSource)
{//通过公式方式反解平面坐标对应的经纬度坐标 3度
	GIS_Embedded::Pos3D pos;
	double dProjectX,dProjectY,dAltitude;
	double dLatitue,dLongitude;
	int iZone = (int)posSource.d1/1000000;
	bool   bLeft = false;  //在分度带中央经线左边为true，在右边为false
	bool   bNorth = true;  //在北半球为true,在南北球为false

	dProjectX = posSource.d0;
	dProjectY = posSource.d1;
	dAltitude = posSource.d2;

	//求分带区
	dProjectY -= 1000000*iZone;
	dProjectY -= 500000;
	if(dProjectY >= 0)
	{
		bLeft = false;
	}
	else
	{
		bLeft = true;
		dProjectY = - dProjectY;
	}
	if(dProjectX >= 0)
	{
		bNorth = true;
	}
	else
	{
		bNorth = false;
		dProjectX = -dProjectX;
	}

	//迭代计算中用到的中间变量
	double dProjectX0;
	double dProjectY0;
	double dDiffB0,dDiffB1;
	double dDiffL0,dDiffL1;
	double dDeltaX,dDeltaY;
	int    iBRow,iLCol; //iBRow 表示纬度查表号（行号） iLCol表示经度查表号（列号）

	//求与中央经线的经差(角度)
	dDiffB0 = 45; dDiffL0 = 1.5;   //设迭代初始值
	dDeltaX = 187.7; dDeltaY = 128.0;    //用初始的迭代值

	dProjectX0 = 4986041.5;   dProjectY0 = 118270.3;  //相应纬经度的初始平直坐标值
	while(fabs(dProjectX - dProjectX0) > 1e-5 || fabs(dProjectY - dProjectY0) > 1e-5)
	{
		dDiffB1 = dDiffB0 + (dProjectX - dProjectX0)/(600*dDeltaX);  //以角度表示
		dDiffL1 = dDiffL0 + (dProjectY - dProjectY0)/(600*dDeltaY);

		//求新的fProjectX0,fProjectY0,fDeltaX,fDeltaY
		//求查表的行号和列号
		iBRow = int(floor(dDiffB1))/5;     //纬度以每5度递增
		iLCol = int(floor(dDiffL1*10))/5;  //经度以每30分(0.5度)递增

		dDeltaX = _projectGauss[iBRow][iLCol]._fDeltaX;
		dDeltaY = _projectGauss[iBRow][iLCol]._fDeltaY;
		getProjectXY(dDiffB1*M_PI/180, dDiffL1*M_PI/180, dProjectX0, dProjectY0);
		dDiffB0 = dDiffB1;
		dDiffL0 = dDiffL1;
	}

	//求经度
	if(bLeft == true)
	{
		dLongitude = -dDiffL0;
	}
	else
	{
		dLongitude = dDiffL0;
	}
	if(iZone <= 60)  //东经
	{
		dLongitude += 3*iZone ;
	}
	else   //西经
	{   
		dLongitude= dLongitude- 360;
		dLongitude += 3*iZone ;
	}
	//求纬度
	if(bNorth == true)  //北纬
	{
		dLatitue = dDiffB0;
	}
	else   //南纬
	{
		dLatitue = -dDiffB0;
	}

	pos.d0 = dLongitude;
	pos.d1 = dLatitue;
	pos.d2 = dAltitude;

	return pos;
}


/*GIS_Embedded::Pos3D CRT_ProjectionGuass::selZoneProject(int zone, Pos3D& posSource)
{
	Pos3D pos = posSource;
	int iZone = (int)posSource.d1/1000000;
	if(iZone == zone)
		return pos;

	pos = inverseToGeographic(posSource);
	double dB, dL, dX, dY;
	dB = pos.d1*PI/180;
	dL = (pos.d0-zone*6+3)*PI/180; //相对于zone带的经度差
	getProjectXY(dB, dL, dX, dY);

	pos.d0 = dX;
	pos.d1 = zone*1000000+500000+dY;

	return pos;
}

double CRT_ProjectionGuass::countDistance(Pos3D &posBegin, Pos3D &posEnd)
{
	double d, s, fSemiMajor, fSemiminor, powDy;

	//需要把点转到同一个带才能测量平面上的距离！！
	Pos3D end = selZoneProject((int)posBegin.d1/1000000, posEnd);
	powDy = pow(end.d1 - posBegin.d1, 2);
	double Ym = (posBegin.d1+posEnd.d1)/2;
	d = sqrt(pow(end.d2-posBegin.d2, 2)+pow(end.d1-posBegin.d1, 2)+pow(end.d0-posBegin.d0, 2) );

	posBegin = inverseToGeographic(posBegin);
	posEnd = inverseToGeographic(posEnd);

	fSemiMajor = _ellipsoid._semiMajorAxis.getStandardValue();//长半轴
	fSemiminor = _ellipsoid._semiMinorAxis.getStandardValue();//短半轴

	double pow2Major =  pow(fSemiMajor, 2);
	double pow2Minor =  pow(fSemiminor, 2);
	double Bm = (posEnd.d1+posBegin.d1)*PI/(2*180);//中间点纬度
	//中间点平均曲率半径
	double Rm = pow2Major*fSemiminor/(pow2Major*cos(Bm)*cos(Bm)+pow2Minor*sin(Bm)*sin(Bm) ); 
	
	double m = 1+ Ym*Ym/(2*Rm*Rm)+powDy/(24*Rm*Rm)+pow(Ym,4)/(24*pow(Rm, 4) );

	return s = d/m;
}*/

}