#ifndef GIS_Embeded_CRT_PROJECTIONGAUSS_H_
#define GIS_Embeded_CRT_PROJECTIONGAUSS_H_

#include "CRT_Projection.h"
#include "Datum.h"
 

namespace GIS_Embedded 
{
;

class CRT_ProjectionGauss : public CRT_Projection/*, public Packable*/
{
public:
	CRT_ProjectionGauss();
	CRT_ProjectionGauss(const CRT_ProjectionGauss &right);
	virtual ~CRT_ProjectionGauss();

	virtual CRT *copy() const;
	virtual bool operator == (const CRT &right) const;
	virtual bool isInversable() const {	return true;	}
	virtual CRT *createInverseCRT() const;
	virtual int_32 trans(const Pos3D &src, Pos3D *dest) const;

	virtual PrjType prjType() const	{	return PRJ_GAUSS;	}

	//virtual int_32 packSize(const void *how_to = NULL) const;
	//virtual int_32 pack(byte_8 *buffer, int_32 buffer_size, byte_8 **end_pos, const void *how_to = NULL) const;
	//virtual int_32 pack(byte_8 **result, const void *how_to = NULL)const;
	//virtual int_32 unpack(const byte_8 *buffer, int_32 buf_size = PACK_IGNORE_BUF_SIZE);
public:
	CD_Ellipsoid m_ellipsoid;
	double m_OriginLatitude;	//原点纬度
	double m_Scale_Factor;		//尺度变换比例
	double m_false_east;
	double m_false_north;
	double m_central_meridian;	//中央子午线
};

class CRT_ProjectionGauss_Inverse : public CRT
{
public:
	CRT_ProjectionGauss_Inverse();
	CRT_ProjectionGauss_Inverse(const CRT_ProjectionGauss_Inverse &right);
	CRT_ProjectionGauss_Inverse(const CRT_ProjectionGauss &prj_gauss);
	virtual ~CRT_ProjectionGauss_Inverse();

	virtual CRT *copy() const;
	virtual bool operator == (const CRT &right) const;
	virtual bool isInversable() const {	return true;	}
	virtual CRT *createInverseCRT() const;
	virtual int_32 trans(const Pos3D &src, Pos3D *dest) const;

public:
	CRT_ProjectionGauss m_prj_gauss;
};

} // GIS_Embedded

#endif //GIS_Embeded_CRT_PROJECTIONGAUSS_H_