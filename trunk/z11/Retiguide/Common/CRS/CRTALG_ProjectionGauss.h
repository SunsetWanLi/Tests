#ifndef GIS_Embeded_CRTALG_ProjectionGauss_H_
#define GIS_Embeded_CRTALG_ProjectionGauss_H_

//#include"CRT_Projection.h"
#include "extern.h"
#include "Datum.h"

namespace GIS_Embedded 
{
;

struct Pos3D;

struct ProjectXY
{
	//double fLatitude,fLogitude;
	double _fOriginatedX,_fOriginatedY;
	double _fDeltaX,_fDeltaY;
};

class CRTALG_ProjectionGauss 
{
private:
	static ProjectXY _projectGauss[18][7];  //高斯-克吕格投影坐标表

public:
	//CRTALG_ProjectionGauss( )
	//{
	//	this->m_projectType = PRJ_GAUSS;
	//}

	~CRTALG_ProjectionGauss( )
	{
	}

	void SetOriginLatitude( double originLat ); //设置中央子午线

	double GetOriginLatitude( ); //返回中央子午线

	void SetScaleFactor( double scale_factor); //设置尺度变换比例

	double GetScaleFactor();		//返回尺度变换比例

	CRTALG_ProjectionGauss* copy() const;

	GIS_Embedded::Pos3D project6DuZone(GIS_Embedded::Pos3D& posSource );	//6度分带投影

	GIS_Embedded::Pos3D inverse6DuZoneToGeographic(GIS_Embedded::Pos3D& posSource );//反算6度分带地理坐标

	GIS_Embedded::Pos3D project3DuZone(GIS_Embedded::Pos3D& posSource );	//3度分带投影

	GIS_Embedded::Pos3D inverse3DuZoneToGeographic(GIS_Embedded::Pos3D& posSource );//反算3度分带地理坐标

	GIS_Embedded::Pos3D projectFromGeographic( const GIS_Embedded::Pos3D& posSource) const; //投影到同一个坐标

	GIS_Embedded::Pos3D inverseToGeographic( const GIS_Embedded::Pos3D& posSource) const;   //反算回地理坐标


	// 54投影坐标到80投影坐标的转换,DX, DY为总体偏移量.
	int_32 projectFromProject(double DX, double DY,double X00, double Y00,double& X11,double& Y11,bool is_54_to_80 = true) const;

	// 求总体平差改正量DX，DY, posSource(B54，L54，0)
	GIS_Embedded::Pos3D getDXYFromGeography(double x80, double y80, const GIS_Embedded::Pos3D& shift_val, const GIS_Embedded::Pos3D& posSource) const; // 返回结果（DX，DY，0）

	//GIS_Embedded::Pos3D selZoneProject(int zone, GIS_Embedded::Pos3D& posSource); //换到第zone带投影

	//double countDistance(GIS_Embedded::Pos3D& posBegin, GIS_Embedded::Pos3D& posEnd); //测量高斯投影上两点的距离 
	//virtual bool operator == (const CRT_Projection &right)
	//{
	//	const CRTALG_ProjectionGauss *_right = dynamic_cast<const CRTALG_ProjectionGauss*>(&right);
	//	if(!_right)
	//		return false;
	//	return CRT_Projection::operator ==(right) && m_OriginLatitude == _right->m_OriginLatitude
	//		&& m_Scale_Factor == _right->m_Scale_Factor;
	//}

	//virtual bool isInversable() const {	return true;	}
	//virtual CRT* createInverseCRT() const;
	//virtual bool operator == (const CRT &right) const;

	//virtual PrjType prjType() const {	return PRJ_GAUSS;	}
private:
	 void getProjectXY( double dB , double dL , double& dX , double& dY ) const;	//传入纬度和经度，返回对应的平面坐标
	 void inverseFromPrjXY(double dX, double dY, double& dB, double& dL) const;	//传入投影x,y值，求经纬度
	 double getPrimeArcLength(double dB) const;	//根据纬度计算子午线弧长
	 double getLatitudeFromArcLen(double arcLength) const;		//根据子午线弧长计算纬度
		 
public:
	double m_OriginLatitude;	//原点纬度
	double m_Scale_Factor;		//尺度变换比例
	double m_False_East;
	double m_False_North;
	double m_Central_Meridian;	//中央子午线
	CD_Ellipsoid _ellipsoid;
};



}

#endif //GIS_Embeded_CRTALG_ProjectionGauss_H_