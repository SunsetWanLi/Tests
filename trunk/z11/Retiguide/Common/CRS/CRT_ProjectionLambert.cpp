
//#include "CRT_ProjectionLambert.h"
//#include "CRT_Math.h"
//
//namespace GIS_Embedded{
//	
//void CRT_ProjectionLambert::SetOriginLatitude(double originLat)
//{
//	m_OriginLatitude = originLat;
//}
//
//double CRT_ProjectionLambert::GetOriginLatitude()
//{
//	return m_OriginLatitude;
//}
//
//void CRT_ProjectionLambert::SetStandardParallel1(double standard_p1)
//{
//	m_Standard_Parallel1 = standard_p1;
//}
//
//double CRT_ProjectionLambert::GetStandardParallel1()
//{
//	return m_Standard_Parallel1;
//}
//
//CRT_Projection* CRT_ProjectionLambert::copy() const
//{
//	CRT_ProjectionLambert* crt = new CRT_ProjectionLambert;
//
//	crt->_ellipsoid = this->_ellipsoid;//in common
////	crt->m_projectType = this->m_projectType;
//	crt->m_Central_Meridian = this->m_Central_Meridian;
////	crt->m_False_East = this->m_False_East;
////	crt->m_False_North = this->m_False_North;
//
//	crt->m_OriginLatitude = this->m_OriginLatitude;
//	crt->m_Standard_Parallel1 = this->m_Standard_Parallel1;
//
//	return crt;
//}
//
//GIS_Embedded::Pos3D CRT_ProjectionLambert::projectFromGeographic( const GIS_Embedded::Pos3D& posSource) const
//{
//	GIS_Embedded::Pos3D pos;
//	double dX, dY;
//	double dB, dL;
//
//	dL = posSource.d0;
//	dB = posSource.d1*PI/180;
//	
//	if(dL >= m_Central_Meridian-180)//西半球
//	{
//		dL = (dL - m_Central_Meridian)*PI/180;
//	}
//	else 
//	{
//		dL = (360 - m_Central_Meridian + dL)*PI/180;
//	}
//	
//	double fMajor, fPowFlat, dN0, dR0, dR;
//	double langentLat = m_Standard_Parallel1*PI/180;
//	double sinB = sin(dB), sinB0 = sin(langentLat), tanB0 = tan(langentLat);
//	
//	fMajor = _ellipsoid._semiMajorAxis.getStandardValue();//长半轴
//	fPowFlat = _ellipsoid._dPowFlattening ; //第一偏心率的平方
//	
//	dN0 = fMajor/sqrt(1 - fPowFlat*pow(sinB0, 2) );
//	dR0 = dN0/tanB0;
//	dR = dN0*sqrt(1/pow(tanB0,2) + 2*(1-sinB/sinB0) );
//
//	dX = dR0 - dR*cos(dL*sinB0);
//	dY = dR*sin(dL*sinB0);
//	pos.d0 = dX;
//	pos.d1 = dY;
//	pos.d2 = posSource.d2;
//	
//	return pos;
//}
//
//GIS_Embedded::Pos3D CRT_ProjectionLambert::inverseToGeographic( const GIS_Embedded::Pos3D& posSource ) const
//{
//	GIS_Embedded::Pos3D pos = posSource;
//	double dX, dY, dL, dB;
//	dX = posSource.d0;
//	dY = posSource.d1;
//
//	double fMajor, fPowFlat, dN0, dR0, dR;
//	double langentLat = m_Standard_Parallel1*PI/180;
//	double sinB, sinB0 = sin(langentLat), tanB0 = tan(langentLat);
//	
//	fMajor = _ellipsoid._semiMajorAxis.getStandardValue();//长半轴
//	fPowFlat = _ellipsoid._dPowFlattening ; //第一偏心率的平方
//	
//	dN0 = fMajor/sqrt(1 - fPowFlat*pow(sinB0, 2) );
//	dR0 = dN0/tanB0;
//
//	dR = sqrt(pow(dY,2) + pow(dR0-dX,2) );
//	//dL = asin(dY/dR)/sinB0;
//	double arcsin = asin(dY/dR);
//
//	if(dX > dR0 )	//此时方位角A超过+ -90度
//	{
//		if(dY > 0)	//过90
//			dL = (PI-arcsin)/sinB0;
//		else		//过-90
//			dL = (-PI-arcsin)/sinB0;
//	}
//	else
//		dL = arcsin/sinB0;
//
//	sinB = sinB0*(1 - (pow(dR/dN0, 2)- 1/pow(tanB0,2) )/2);
//	if(sinB>1)//不能超过值域
//		sinB = 1;
//	else if(sinB < -1)
//		sinB = -1;
//
//	dB = asin(sinB);
//
//	dL *= 180/PI;
//	dB *= 180/PI;
//
//	if(dL < 180 - m_Central_Meridian )
//		dL += m_Central_Meridian ;
//	else
//		dL = m_Central_Meridian + dL - 360;
//		//dL = 360 - m_Central_Meridian - dL;	
//
//	pos.d0 = dL ;
//	pos.d1 = dB ;
//
//	return pos;
//}
//
//
////兰勃特正形投影CRT_ProjectionLambertConformal
//	void CRT_ProjectionLambertConformal::SetOriginLatitude(double originLat)
//{
//	m_OriginLatitude = originLat;
//}
//
//double CRT_ProjectionLambertConformal::GetOriginLatitude()
//{
//	return m_OriginLatitude;
//}
//
//void CRT_ProjectionLambertConformal::SetStandardParallel1(double standard_p1)
//{
//	m_Standard_Parallel1 = standard_p1;
//}
//
//double CRT_ProjectionLambertConformal::GetStandardParallel1()
//{
//	return m_Standard_Parallel1;
//}
//
//void CRT_ProjectionLambertConformal::SetStandardParallel2(double standard_p2)
//{
//	m_Standard_Parallel2 = standard_p2;
//}
//
//double CRT_ProjectionLambertConformal::GetStandardParallel2()
//{
//	return m_Standard_Parallel2;
//}
//
//CRT_Projection* CRT_ProjectionLambertConformal::copy() const
//{
//	CRT_ProjectionLambertConformal* crt = new CRT_ProjectionLambertConformal;
//	
//	crt->_ellipsoid = this->_ellipsoid;//in common
////	crt->m_projectType = this->m_projectType;
//	crt->m_Central_Meridian = this->m_Central_Meridian;
////	crt->m_False_East = this->m_False_East;
////	crt->m_False_North = this->m_False_North;
//
//	crt->m_OriginLatitude = this->m_OriginLatitude;
//	crt->m_Standard_Parallel1 = this->m_Standard_Parallel1;
//	crt->m_Standard_Parallel2 = this->m_Standard_Parallel2;
//
//	return crt;
//}
//
//GIS_Embedded::Pos3D CRT_ProjectionLambertConformal::projectFromGeographic( const GIS_Embedded::Pos3D& posSource) const
//{
//	GIS_Embedded::Pos3D pos;
//	double dX, dY;
//	double dB, dL;
//
//	dL = posSource.d0;
//	dB = posSource.d1*PI/180;
//	
//	if(dL >= m_Central_Meridian-180)//对地球重新按照中央子午线分经度
//	{
//		dL = (dL - m_Central_Meridian)*PI/180;
//	}
//	else 
//	{
//		dL = (360 - m_Central_Meridian + dL)*PI/180;
//	}
//	
//	double fMajor, fPowFlat, dN0, dR0, dR;
//	double langentLat = m_Standard_Parallel1*PI/180;
//
//	//对于这种投影原点纬度不能在极点，不做太靠极点的投影
//	if(langentLat > PI/2-0.175 )
//		langentLat = PI/2-0.175;
//	else if(langentLat < 0.175-PI/2 )
//		langentLat = 0.175-PI/2;
//
//	double sinB = sin(dB), sinB0 = sin(langentLat);
//	fMajor = _ellipsoid._semiMajorAxis.getStandardValue();//长半轴
//	fPowFlat = _ellipsoid._dPowFlattening ; //第一偏心率的平方
//	double e = sqrt(fPowFlat);
//	
//	dN0 = fMajor/sqrt(1 - fPowFlat*pow(sinB0, 2) );
//	dR0 = dN0/tan(langentLat);
//	dR = dR0*pow( (1+e*sinB)*(1-e*sinB0)/( (1-e*sinB)*(1+e*sinB0) ), e*sinB0/2);
//	dR /= pow( (1+sinB)*(1-sinB0)/( (1-sinB)*(1+sinB0) ) , sinB0/2);
//
//	dX = dR0 - dR*cos(dL*sinB0);
//	dY = dR*sin(dL*sinB0);
//	pos.d0 = dX;
//	pos.d1 = dY;
//	pos.d2 = posSource.d2;
//	
//	return pos;
//}
//
//GIS_Embedded::Pos3D CRT_ProjectionLambertConformal::inverseToGeographic( const GIS_Embedded::Pos3D& posSource ) const
//{
//	GIS_Embedded::Pos3D pos = posSource;
//	double dX, dY, dL, dB;
//	dX = posSource.d0;
//	dY = posSource.d1;
//
//	double fMajor, fPowFlat, e, dN0, dR0, dR;
//	double langentLat = m_Standard_Parallel1*PI/180;
//	double sinB, sinB0 = sin(langentLat);
//	
//	fMajor = _ellipsoid._semiMajorAxis.getStandardValue();//长半轴
//	fPowFlat = _ellipsoid._dPowFlattening ; //第一偏心率的平方
//	e = sqrt(fPowFlat);
//	
//	dN0 = fMajor/sqrt(1 - fPowFlat*pow(sinB0, 2) );
//	dR0 = dN0/tan(langentLat);
//
//	dR = sqrt(pow(dY,2) + pow(dR0-dX,2) );
//	//dL = asin(dY/dR)/sinB0;
//	double arcsin = asin(dY/dR);
//
//	if(dX > dR0 )	//此时方位角A超过+ -90度
//	{
//		if(dY > 0)	//过90
//			dL = (PI-arcsin)/sinB0;
//		else		//过-90
//			dL = (-PI-arcsin)/sinB0;
//	}
//	else
//		dL = arcsin/sinB0;
//
//	//求纬度B
//	double mediac = dR*pow( (1-sinB0)/(1+sinB0), sinB0/2)/(dR0*pow((1-e*sinB0)/(1+e*sinB0), e*sinB0/2) );
//	double y = exp(2*log(mediac)/sinB0);
//	double minSinB = -1.0 + 1e-4, maxSinB = 1.0 - 1e-4;
//
//	CCRTMath::newtonCalRoot(e, y, minSinB, minSinB, maxSinB, &sinB);//牛顿下山法计算 
//
//	if(sinB>1)//不能超过值域
//		sinB = 1;
//	else if(sinB < -1)
//		sinB = -1;
//
//	dB = asin(sinB);
//
//	dL *= 180/PI;
//	dB *= 180/PI;
//
//	if(dL < 180 - m_Central_Meridian )
//		dL += m_Central_Meridian ;
//	else
//		dL = m_Central_Meridian + dL - 360;
//		//dL = 360 - m_Central_Meridian - dL;	
//
//	pos.d0 = dL ;
//	pos.d1 = dB ;
//
//	return pos;
//}
//
//}