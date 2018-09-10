//#ifndef GIS_Embeded_CRT_PROJECTIONLAMBERT_H_
//#define GIS_Embeded_CRT_PROJECTIONLAMBERT_H_
//
//#include"CRT_Projection.h"
//
//namespace GIS_Embedded 
//{
//	
////兰勃特等面积投影
//class CRT_ProjectionLambert : public CRT_Projection
//{
//public:
//	CRT_ProjectionLambert()
//	{
////		this->m_projectType = PRJ_LAMBERT;
//	};
//
//	~CRT_ProjectionLambert()
//	{
//	};
//
//	void SetOriginLatitude(double originLatitude);
//
//	double GetOriginLatitude();
//
//	void SetStandardParallel1(double standard_p1);
//
//	double GetStandardParallel1();
//
//	CRT_Projection* copy() const;
//
//	GIS_Embedded::Pos3D projectFromGeographic( const GIS_Embedded::Pos3D& posSource ) const;
//
//	GIS_Embedded::Pos3D inverseToGeographic( const GIS_Embedded::Pos3D& posSource ) const;
//
//	virtual bool isInversable() const {	return true;	}
//	virtual CRT* createInverseCRT() const;
//	virtual bool operator == (const CRT &right) const;
//
//	virtual PrjType prjType() const {	return PRJ_LAMBERT;	}
//protected:
//	double m_Standard_Parallel1;	//投影相切纬度
//	double m_OriginLatitude;		//原点纬度
//
//	double m_Central_Meridian;
//};
//
////兰勃特正形投影
//class CRT_ProjectionLambertConformal : public CRT_Projection
//{
//public:
//	CRT_ProjectionLambertConformal()
//	{
////		this->m_projectType = PRJ_LAMBERT_CONFORMAL;
//	};
//
//	~CRT_ProjectionLambertConformal()
//	{
//	};
//
//	void SetOriginLatitude(double originLatitude);
//
//	double GetOriginLatitude();
//
//	void SetStandardParallel1(double standard_p1);
//
//	double GetStandardParallel1();
//
//	void SetStandardParallel2(double standard_p2);
//
//	double GetStandardParallel2();
//
//	CRT_Projection* copy() const;
//
//	GIS_Embedded::Pos3D projectFromGeographic( const GIS_Embedded::Pos3D& posSource) const;
//
//	GIS_Embedded::Pos3D inverseToGeographic( const GIS_Embedded::Pos3D& posSource ) const;
//	virtual bool isInversable() const {	return true;	}
//	virtual CRT* createInverseCRT() const;
//	virtual bool operator == (const CRT &right) const;
//
//	virtual PrjType prjType() const {	return PRJ_LAMBERT_CONFORMAL;	}
//protected:
//	double m_Standard_Parallel1;//这两个成员如果相等表明相切投影否则相割投影
//	double m_Standard_Parallel2;
//	double m_OriginLatitude; //原点纬度
//
//	double m_Central_Meridian;//中央经线	
//};
//	
//}
//
//#endif//GIS_Embeded_CRT_PROJECTIONLAMBERT_H_