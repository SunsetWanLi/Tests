#ifndef GIS_Embeded_CRT_PROJECTIONMERCATOR_TRANSVERSE_H_
#define GIS_Embeded_CRT_PROJECTIONMERCATOR_TRANSVERSE_H_

#include"CRT_Projection.h"
#include "Datum.h"
 
namespace GIS_Embedded 
{
;	
	
class CRT_ProjectionMercatorTransverse : public CRT_Projection/*, public Packable*/
{
public:
	CRT_ProjectionMercatorTransverse();
	CRT_ProjectionMercatorTransverse(const CRT_ProjectionMercatorTransverse &right);
	virtual ~CRT_ProjectionMercatorTransverse();
	
	virtual CRT *copy() const;
	virtual bool operator == (const CRT &right) const;
	virtual bool isInversable() const {	return true;	}
	virtual CRT *createInverseCRT() const;
	virtual int_32 trans(const Pos3D &src, Pos3D *dest) const;

	virtual PrjType prjType() const	{	return PRJ_MERCATOR_TRANVERSE;	}

	//virtual int_32 packSize(const void *how_to = NULL) const;
	//virtual int_32 pack(byte_8 *buffer, int_32 buffer_size, byte_8 **end_pos, const void *how_to = NULL) const;
	//virtual int_32 pack(byte_8 **result, const void *how_to = NULL)const;
	//virtual int_32 unpack(const byte_8 *buffer, int_32 buf_size = PACK_IGNORE_BUF_SIZE);
public:
	CD_Ellipsoid m_ellipsoid;
	double m_false_east;
	double m_false_north;
	double m_central_meridian;	//中央子午线
	double m_OriginLatitude;	//原点纬度
	double m_Scale_Factor;		//尺度变换比例
};

class CRT_ProjectionMercatorTransverse_Inverse : public CRT
{
public:
	CRT_ProjectionMercatorTransverse_Inverse();
	CRT_ProjectionMercatorTransverse_Inverse(const CRT_ProjectionMercatorTransverse_Inverse &right);
	CRT_ProjectionMercatorTransverse_Inverse(const CRT_ProjectionMercatorTransverse &prj_mercator);
	virtual ~CRT_ProjectionMercatorTransverse_Inverse();

	virtual CRT *copy() const;
	virtual bool operator == (const CRT &right) const;
	virtual bool isInversable() const {	return true;	}
	virtual CRT *createInverseCRT() const;
	virtual int_32 trans(const Pos3D &src, Pos3D *dest) const;

public:
	CRT_ProjectionMercatorTransverse m_prj_mercator;
};

}//namespace GIS_Embedded

#endif //GIS_Embeded_CRT_PROJECTIONMERCATOR_H_