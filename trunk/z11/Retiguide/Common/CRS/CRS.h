#ifndef GIS_Embeded_CRS_H_
#define GIS_Embeded_CRS_H_


#include "Datum.h"
#include "CoordSystem.h"
//-------------------------------------------------------------------------------------------
namespace GIS_Embedded{

typedef int_64 GM_ID;
typedef GM_ID CRS_ID ;

class CRS
{
	//DECLARE_INSTANCE_TRACE("CRS")
public:
	virtual ~CRS(){}

	virtual CRS_Type getCrsType() const = 0 ;
	//virtual void setCoordinateSystem( const CoordinateSystem & refCs) = 0;
	//virtual CoordinateSystem getCoordinateSystem() const = 0 ;
	virtual CoordinateSystem &coordinateSystem() 
	{	return m_coord_system;	}
	virtual const CoordinateSystem &coordinateSystem() const
	{	return m_coord_system;	}

	virtual CRS * copy() const = 0;
	virtual bool operator == ( const CRS& crs) const = 0;

	virtual bool isCompound() const = 0;
protected:
	CoordinateSystem m_coord_system;
};


//class CRS_RealWorld : public CRS_Single
//{
//public:
//	virtual void setGeoCrsType(GCSType geoCrsType) ;//
//	virtual GCSType getGeoCrsType() const ;//
//	virtual void setDatum(const CD_GeodesicDatum & datum ) ;//
//	virtual CD_GeodesicDatum getDatum( ) const ;//
//	//virtual bool operator==( const CRS& crs) const;
//protected:
//	int_32 packPart(byte_8 *buffer, int_32 buffer_size, int_32 pack_type_code, const void *how_to) const;
//	int_32 packPartSize(const void *how_to) const;
//	int_32 unpackPart(const byte_8 *buffer, int_32 pack_type_code);//, int_32 buf_size = PACK_IGNORE_BUF_SIZE);
//protected:
//	GCSType _geoCrsType;	//记录坐标系统类型：WGS84 BEIJING54 XIAN80...
//	CD_GeodesicDatum _datum;         //the geodetic datum that specifies the origin,orientation,and scale.
//};



}//namespace GIS_Embedded
#endif //GIS_Embeded_CRS_H_