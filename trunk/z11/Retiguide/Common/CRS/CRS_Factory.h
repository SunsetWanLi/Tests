#ifndef GIS_Embeded_CRS_FACTORY_H_
#define GIS_Embeded_CRS_FACTORY_H_

#include "Datum.h"
#include "CRS.h"

namespace GIS_Embedded 
{
;
class CRS_Single;
class CRS_Geography;

class CRS_Factory
{
public:
	static CD_Ellipsoid createEllipse( const EllipsoidType & type )	; 
	static MC_UnitOfMeasure createUnit( const UnitName & unitType ) ;
	static CD_PrimeMeridian createMeridian( const PrimeMeridianType & meridianType ) ;
	static CoordinateSystem createCoordinateSystem( const UnitName & unitType, const uint_32 & dimension, const CoordinateSystemType & csType ) ;
//	static CRS_Single * createDefaultSingleCrs() ;
//	static CRS_Single *createSingleCrs(CRS_SingleType type = CRSS_PROJECTED); // mem allocated
	static CD_GeodesicDatum createDatum(GCSType type = GCS_WGS_1984);
	static CRS* createGenericCRS();
	static CRS* createCRS_Geography(GCSType type = GCS_WGS_1984);
	static CRS* createCRS_Geography(EllipsoidType type = ETC_WGS_1984);
	static CRS* createCRS_Project(const CRS_Geography &crs_g, PrjType type = PRJ_MERCATOR);
	static CRS* createDefaultCRS();
	
};

const static double DEFAULT_CENTRAL_MERIDIAN = 0;

}
#endif