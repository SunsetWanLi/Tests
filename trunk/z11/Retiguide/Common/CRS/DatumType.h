#ifndef GIS_Embeded_CRS_DATUMTYPE_H_
#define GIS_Embeded_CRS_DATUMTYPE_H_

#include "extern.h"
#include "../Types/basic_types.h"

#include "CRS_TypeEnum.h"

namespace GIS_Embedded 
{
// 坐标系统的参照面、投影方式、坐标类型、单位等的定义，参考mapinfo关于MIF文件的格式
// 对于nonearch的一律按照CRSS_GEOCENTRIC, ETC_WGS_1984来做
// Ellipsoid type c
//;
//enum EllipsoidType
//{
//	ETC_UNKNOWN,
//	ETC_KRASSOVSKY_1940 = 1,
//	ETC_CLARK_1856 = 2,
//	ETC_CLARK_1863 = 3,
//	ETC_CLARK_1866 = 4,
//	ETC_CLARK_1880 = 5,
//	ETC_BESSEL_1841 = 6,
//	ETC_WGS_1984 = 7,
//	ETC_EVEREST_1830 = 8,
//	ETC_IAG_1975 = 9,
//	ETC_DEFAULT = ETC_WGS_1984,
//	ETC_LAST
//};
//坐标类型  CoordinateSystemType
enum CoordinateSystemType
{
	CS_CARTESIAN = 1,
	CS_ELLIPSOIDAL = 2,
	CS_SPHERICAL = 3,
	CS_TEMPORSL = 4
};

//坐标系统类型 Crs_Type
enum CRS_Type
{
	CRS_SINGLE = 0x0100,
	CRS_COMPOUND = 0x0200,
	CRS_GEOCENTRIC = 0x01 | CRS_SINGLE, 
	CRS_GEOGRAPHY = 0x02 | CRS_SINGLE,
	CRS_PROJECT = 0x01 | CRS_COMPOUND,
	CRS_GENERIC = 0x02 | CRS_COMPOUND
};

//简单坐标系统类型 Crs_SingleType
enum CRS_SingleType
{
	CRSS_GEOCENTRIC = 1,
	CRSS_GEOGRAPHIC = 2,
	CRSS_PROJECTED = 3,
	CRSS_ENGINEERING = 4,
	CRSS_TEMPORAL = 5
};

//坐标转换系统类型 CRT_TYPE
enum CRT_Type
{
	CRT_UNKNOWN = 0,
	CRT_CONVERSION = 1,
	CRT_TRANSFORMATION = 2,
	CRT_PROJECTION = 3,
	CRT_PRJ_GAUSS,
	CRT_PRJ_GAUSS_INVERSE,
	CRT_PRJ_GAUSS_ZONED,
	CRT_PRJ_MERCATOR,
	CRT_PRJ_MERCATOR_TRANVERSE,
	CRT_PRJ_LAMBERT,
	CRT_PRJ_LAMBERT_CONFORMAL
};

// unintName types 坐标单位
//标准单位
enum UnitName
{
	UTC_STD = 1,
	UTC_METER = 1,//UTC_STD,
	// 非标准单位
	UTC_CENTIMETER = 2,
	UTC_CHAIN = 3,
	UTC_FEET = 4,
	UTC_INCHE = 5,
	UTC_KILOMETER = 6,
	UTC_MILE = 7,
	UTC_LINK = 8,
	UTC_MILlIMETER = 9,
	UTC_NAUTICALMILE = 10,
	UTC_ROD = 11,
	UTC_USSURVEYFEET = 12,
	UTC_YARD = 13
};

//投影类型projectType
//enum PrjType
//{
//	PRJ_GAUSS,
//	PRJ_GAUSS_INVERSE,
//	PRJ_GAUSS_ZONED,
//	PRJ_MERCATOR,
//	PRJ_MERCATOR_TRANVERSE,
//	PRJ_LAMBERT,
//	PRJ_LAMBERT_CONFORMAL
//};
typedef ProjectionType PrjType;

// projection type for other use
const uint_32 PRO_LONGITUDE_LATITUDE = 1 ; 
const uint_32 PRO_GAUSS_KRUGER = 2 ;
const uint_32 PRO_ALBERS_EUQAL_AREA_CONIC = 3 ;
// todo

// 本初子午线PrimeMeridian
enum PrimeMeridianType
{
	PMC_GREENWICH = 1
};

//地理坐标类型
//enum GCSType
//{
//	GCS_UNKNOWN = 0,
//	GCS_WGS_1984 = 1,
//	GCS_BEIJING_1954 = 2,
//	GCS_XIAN_1980 = 3,
//	GCS_CUSTOM = 4,
//	GCS_DEFAULT = GCS_WGS_1984//GCS_BEIJING_1954
//};

typedef GeographicCRSType GCSType;

//都按照WGS84为目标坐标的转换参数设置
#pragma pack(push, 4)
struct CRTParam
{
	uint_32 geoCrsType;
	double dShiftX, dShiftY, dShiftZ;		//three shift parameters
	double dRotateX, dRotateY, dRotateZ;	//three rotation parameters
	double dScale;
};
#pragma pack(pop)

const CRTParam CRTParamList[5] =
{
	//GCS_ID,ShiftX,ShiftY,ShiftZ, RotateX, RotateY,RotateZ,Scale
	{GCS_WGS_1984,0, 0, 0, 0, 0, 0, 0},
	{GCS_BEIJING_1954, 0, 0, 0, 0, 0, 0, 0},
	{GCS_XIAN_1980, -95.936120992863465, -46.746264260546646, -19.730149872914069, -2.6977374377042869e-006, 
		-3.4533647240167044e-007, -1.4999415977450498e-006, -3.7421893858978458e-007},
	{GCS_DEFAULT, 0, 0, 0, 0, 0, 0, 0},
	{GCS_UNKNOWN}
};

}
#endif