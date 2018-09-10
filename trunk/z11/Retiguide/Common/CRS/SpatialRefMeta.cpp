

#include "SpatialRefMeta.h"

namespace GIS_Embedded{

inline bool MC_UnitOfMeasure::isStandarnUnit()
{
	if(_c == 0)
	{
		return false;
	}
	else if(_b/_c == 1 && _a == 0 && _d == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

double MC_UnitOfMeasure::convertToStdValue( double dValue )
{
	double dRet ;
	dRet = ( _a + _b*dValue )/( _c + _d*dValue );
	return dRet ;
}

double MC_Measure::getStandardValue( ) const
{
	double dStd;
	dStd = ( _uom._a + _uom._b*_dValue)/(_uom._c + _uom._d*_dValue);
	return dStd;
}

}//namespace GIS_Embedded
