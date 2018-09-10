
#include "CRS_Factory.h"
#include "CRS.h"
#include "CRS_Generic.h"
#include "CRS_Geography.h"
#include "CRS_Project.h"
#include "CRT_Factory.h"
#include "CRT_ProjectionGauss.h"
#include "CRT_ProjectionMercator.h"
#include "CRT_ProjectionMercatorTransverse.h"

namespace GIS_Embedded
{
	//const uint_32 ETC_KRASSOVSKY_1940 = 1 ;
	//const uint_32 ETC_CLARK_1856 = 2 ;
	//const uint_32 ETC_CLARK_1863 = 3 ;
	//const uint_32 ETC_CLARK_1866 = 4 ;
	//const uint_32 ETC_CLARK_1880 = 5 ;
	//const uint_32 ETC_BESSEL_1841 = 6 ;
	//const uint_32 ETC_WGS_1984 = 7 ;
	//const uint_32 ETC_EVEREST_1830 = 8 ;
;
struct DatumType_EllipseType
{
	GCSType d_type;
	EllipsoidType e_type;
};

const static DatumType_EllipseType DatumEllipseTypeList[] =
{
	{	GCS_UNKNOWN, ETC_WGS_1984	},
	{	GCS_WGS_1984, ETC_WGS_1984	},
	{	GCS_BEIJING_1954, ETC_KRASSOVSKY_1940	},
	{	GCS_XIAN_1980, ETC_IAG_1975	}
};

struct EllipseType_DatumType
{
	EllipsoidType e_type;
	GCSType d_type;
};

const static EllipseType_DatumType EllipseDatumTypeList[] =
{
	{	ETC_KRASSOVSKY_1940, GCS_BEIJING_1954	},
	{	ETC_CLARK_1856, GCS_CUSTOM	},
	{	ETC_CLARK_1863, GCS_CUSTOM	},
	{	ETC_CLARK_1866, GCS_CUSTOM	},
	{	ETC_CLARK_1880, GCS_CUSTOM	},
	{	ETC_BESSEL_1841, GCS_CUSTOM	},
	{	ETC_WGS_1984, GCS_WGS_1984	},
	{	ETC_EVEREST_1830, GCS_CUSTOM	},
	{	ETC_IAG_1975, GCS_XIAN_1980	}
};

static EllipsoidType getEllipsoidType(GCSType datum_type)
{
	for(uint_32 i = 0; i < sizeof(DatumEllipseTypeList)/sizeof(DatumType_EllipseType); i ++)
	{
		if(DatumEllipseTypeList[i].d_type == datum_type)
		{
			return DatumEllipseTypeList[i].e_type;
		}
	}

	return ETC_UNKNOWN;
}

static GCSType getDatumType(EllipsoidType e_type)
{
	for(uint_32 i = 0; i < sizeof(EllipseDatumTypeList)/sizeof(EllipseType_DatumType); i ++)
	{
		if(EllipseDatumTypeList[i].e_type == e_type)
		{
			return EllipseDatumTypeList[i].d_type;
		}
	}

	return GCS_CUSTOM;
}

CD_Ellipsoid CRS_Factory::createEllipse( const EllipsoidType & type )
{
	//原来是name, a,  b,  r = (a-b)/a 错， 应该是pow(e, 2) = (pow(a,2)-pow(b,2))/pow(a,2)
	//这里把扁率和第一偏心率搞错了应该是name, a ,b , pow(e,2)
	static double ellipse[][4] = {
	{ETC_KRASSOVSKY_1940,	6378245, 6356863.018773,	0.00669342162297},	//北京54采用克拉索夫斯基椭球参数
	{ETC_IAG_1975,			6378140, 6356755.288158,	0.00669438499959},	//北京80坐标采用的国际椭球参数 
	{ETC_CLARK_1856,		6377862, 6356466.958068,	0.00669790480033},
	{ETC_CLARK_1863,		6378288, 6356622.619565,	0.00678194042422},
	{ETC_CLARK_1866,		6378206, 6356583.350176,	0.00676867399129},
	{ETC_CLARK_1880,		6378249, 6356514.280670,	0.00680365014843},
	{ETC_BESSEL_1841,		6377397, 6356078.750415,	0.00667439033565},
	{ETC_WGS_1984,			6378137, 6356752.3142451,	0.00669437999013},
	{ETC_EVEREST_1830,		6377276, 6356075.019950,	0.00663786205157},
	{0.0,					6378245, 6356863.018773,	0.00669342162298} // default ellipse ETC_KRASSOVSKY_1940
	} ;

	int_32 i = 0 ;

	while( ellipse[i][0] != 0.0 && ellipse[i][0] != type )
		i ++ ;

	double * data_ellips = ellipse[i] ;
	CD_Ellipsoid rt_ellipse ;
	rt_ellipse._semiMajorAxis._dValue = data_ellips[1] ;
	rt_ellipse._semiMinorAxis._dValue = data_ellips[2] ;
	rt_ellipse._dPowFlattening = data_ellips[3] ;

	return rt_ellipse ;
}
//
//const uint_32 UTC_METER = 1 ;
//const uint_32 UTC_CENTIMETER = 2;
//const uint_32 UTC_CHAIN = 3 ;
//const uint_32 UTC_FEET = 4 ;
//const uint_32 UTC_INCHE = 5 ;
//const uint_32 UTC_KILOMETER = 6 ;
//const uint_32 UTC_MILE = 7 ;
//const uint_32 UTC_LINK = 8 ;
//const uint_32 UTC_MILlIMETER = 9 ;
//const uint_32 UTC_NAUTICALMILE = 10 ;
//const uint_32 UTC_ROD = 11 ;
//const uint_32 UTC_USSURVEYFEET = 12 ;
//const uint_32 UTC_YARD = 13 ;
MC_UnitOfMeasure CRS_Factory::createUnit( const UnitName & unitType )
{
	// TODO fullfil the table
	static int unit_transform_table[][5] = 
	{
		{UTC_STD, 0, 1, 1, 0 },
		{UTC_KILOMETER, 0, 1000, 1, 0},
		{UTC_CENTIMETER, 0,1,10,0},
		{UTC_MILlIMETER, 0,1,100,0},
		{0,0,1,1,0}
	};

	int_32 i = 0 ;

	while( unit_transform_table[i][0] != 0 && unit_transform_table[i][0] != unitType )
		i ++ ;

	int * data_unit = unit_transform_table[i] ;
	MC_UnitOfMeasure rt ;

	rt._a = data_unit[1] ;
	rt._b = data_unit[2] ;
	rt._c = data_unit[3] ;
	rt._d = data_unit[4] ;

	return rt ;

}

CD_PrimeMeridian CRS_Factory::createMeridian( const PrimeMeridianType & meridianType )
{
	// TODO ,目前仅仅用green wich ，以后扩展
	//assert( meridianType == PMC_GREENWICH ) ;

	CD_PrimeMeridian prime ;
	MC_Angle greenWich ;
	greenWich._uom = createUnit( UTC_STD ) ;
	greenWich._dValue = 0 ;
	prime._greenwichLongitude = greenWich ; //GreenWich子午线

	return prime ;

}

CoordinateSystem CRS_Factory::createCoordinateSystem( const UnitName & unitType, 
													 const uint_32 & dimension, const CoordinateSystemType & csType )
{
	CoordinateSystem refCoord ;
	CoordinateSystemAxis axisX , axisY , axisZ ;
	
	refCoord.setCSType( csType ) ; 
	refCoord.setDimension( dimension ) ;  
	axisX._uom = createUnit( unitType ) ;
	axisY._uom = createUnit( unitType ) ;
	axisZ._uom = createUnit( unitType ) ;
	refCoord.appenCoordinateSystemAxis(0, axisX );
	refCoord.appenCoordinateSystemAxis(1, axisY );
	if( dimension == 3 )
		refCoord.appenCoordinateSystemAxis(2, axisZ );
	
	return refCoord ;
}

//CRS_Single * CRS_Factory::createDefaultSingleCrs()
//{
//	return createSingleCrs(CRSS_PROJECTED);
	//CRS_Single * rt_crs = NULL ;

	//rt_crs = new CRS_Single ;
	////1 定义坐标
	//CoordinateSystem refCoord ;
	//refCoord = createCoordinateSystem( UTC_STD, 2, CS_CARTESIAN);
	////2 定义地球的椭球结构,可以设为全局函数
	//CD_Ellipsoid ellip ;  //下面设定的参数为埃维尔斯特(Everest)地球椭球模型
	//ellip = createEllipse( /*ETC_EVEREST_1830*/ETC_WGS_1984 ) ;//WGS84为默认椭球

	////3 定义本初子午线
	//CD_PrimeMeridian prime ;
	//prime = createMeridian(PMC_GREENWICH) ;

	////4 先定义一个Datum类,初始化Datum 类
	//CD_GeodesicDatum geoDatum ;
	//geoDatum._ellipsoid = ellip ;
	//geoDatum._primeMeridian = prime ;

	////rt_crs->setCrsType(CRS_SINGLE);
	//rt_crs->setSingleCrsType( CRSS_PROJECTED );  //类型设为空间坐标
	//rt_crs->setGeoCrsType(GCS_WGS_1984);	//坐标类型
	//rt_crs->setDatum( geoDatum );  //设置参照系
	//rt_crs->setCoordinateSystem( refCoord );

	//return rt_crs ;

//}

//CRS_Single * CRS_Factory::createSingleCrs(CRS_SingleType single_type)
//{
//	CRS_RealWorld * rt_crs = NULL ;
//
//	switch(single_type)
//	{
//	case CRSS_GEOCENTRIC:
//		assert(0);
//		break;
//	case CRSS_GEOGRAPHIC:
//		rt_crs = new CRS_Geography;
//		break;
//	case CRSS_PROJECTED:
//		rt_crs = new CRS_Project;
//		dynamic_cast<CRS_Project*>(rt_crs)->setProjectType(PRJ_MERCATOR); 
//		break;
//	case CRSS_ENGINEERING:
//		assert(0);
//		break;
//	case CRSS_TEMPORAL:
//		assert(0);
//		break;
//	default:
//		break;
//	}
//	//rt_crs = new CRS_Single ;
//	//1 定义坐标
//	CoordinateSystem refCoord ;
//	refCoord = createCoordinateSystem( UTC_STD, 2, CS_CARTESIAN);
//	//2 定义地球的椭球结构,可以设为全局函数
//	CD_Ellipsoid ellip ;  //下面设定的参数为埃维尔斯特(Everest)地球椭球模型
//	ellip = createEllipse( /*ETC_EVEREST_1830*/ETC_WGS_1984 ) ;//WGS84为默认椭球
//
//	//3 定义本初子午线
//	CD_PrimeMeridian prime ;
//	prime = createMeridian(PMC_GREENWICH) ;
//
//	//4 先定义一个Datum类,初始化Datum 类
//	CD_GeodesicDatum geoDatum ;
//	geoDatum._ellipsoid = ellip ;
//	geoDatum._primeMeridian = prime ;
//
//	//rt_crs->setCrsType(CRS_SINGLE);
//	//rt_crs->setSingleCrsType( CRSS_PROJECTED );  //类型设为空间坐标
//	rt_crs->setGeoCrsType(GCS_WGS_1984);	//坐标类型
//	rt_crs->setDatum( geoDatum );  //设置参照系
//	rt_crs->setCoordinateSystem( refCoord );
//
//	return rt_crs ;
//
//}

CD_GeodesicDatum CRS_Factory::createDatum(GCSType type)
{
	CD_GeodesicDatum ret;

	ret.m_geo_crs_type = type;
	ret._ellipsoid = createEllipse(getEllipsoidType(type));
	ret._primeMeridian = createMeridian(PMC_GREENWICH);

	return ret;
}

CRS *CRS_Factory::createGenericCRS()
{
	return new CRS_Generic();
}

CRS *CRS_Factory::createCRS_Geography(GCSType type)
{
	CRS_Geography *ret = new CRS_Geography();

	ret->coordinateSystem() = createCoordinateSystem(UTC_STD, 2, CS_CARTESIAN);
	ret->m_datum = createDatum(type);

	return ret;
}

CRS *CRS_Factory::createCRS_Geography(EllipsoidType type )
{
	CRS_Geography *ret = new CRS_Geography();

	ret->coordinateSystem() = createCoordinateSystem(UTC_STD, 2, CS_CARTESIAN);
	ret->m_datum._ellipsoid = createEllipse(type);
	ret->m_datum._primeMeridian = createMeridian(PMC_GREENWICH);
	ret->m_datum.m_geo_crs_type = getDatumType(type);

	return ret;
}

CRS *CRS_Factory::createCRS_Project(const CRS_Geography &crs_g, PrjType type)
{
	CRS_Project *ret = new CRS_Project();
	ret->coordinateSystem() = createCoordinateSystem(UTC_STD, 2, CS_CARTESIAN);
	ret->setRefCRS(dynamic_cast<const CRS_Geography*>(&crs_g)); // mem alloc
	// CRT
	if(type == PRJ_GAUSS)
	{
		CRT_ProjectionGauss *crt_gauss = new CRT_ProjectionGauss();
		crt_gauss->m_ellipsoid = crs_g.m_datum._ellipsoid;
		crt_gauss->m_central_meridian = DEFAULT_CENTRAL_MERIDIAN;

		ret->setRefCRT(crt_gauss); // mem alloc
		delete crt_gauss;
	}
	else if(type == PRJ_MERCATOR)
	{
		CRT_ProjectionMercator *crt_mercator = new CRT_ProjectionMercator();
		crt_mercator->m_ellipsoid = crs_g.m_datum._ellipsoid;
		crt_mercator->m_central_meridian = DEFAULT_CENTRAL_MERIDIAN;

		ret->setRefCRT(crt_mercator); // mem alloc
		delete crt_mercator;

	}
	else if(type == PRJ_MERCATOR_TRANVERSE)
	{
		CRT_ProjectionMercatorTransverse *crt_mercator_transverse = new CRT_ProjectionMercatorTransverse();
		crt_mercator_transverse->m_ellipsoid = crs_g.m_datum._ellipsoid;
		crt_mercator_transverse->m_central_meridian = DEFAULT_CENTRAL_MERIDIAN;

		ret->setRefCRT(crt_mercator_transverse); // mem alloc
		delete crt_mercator_transverse;
	}
	return ret;
}

CRS *CRS_Factory::createDefaultCRS() 
{
	return createGenericCRS();
}

}
