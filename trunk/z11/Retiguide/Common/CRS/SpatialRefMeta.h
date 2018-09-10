
#ifndef GIS_Embeded_SPATIALREFMETA_H_
#define GIS_Embeded_SPATIALREFMETA_H_
//#pragma once


#include "extern.h"
//#include "DatumType.h"

namespace GIS_Embedded{

#pragma pack(push, 4)
struct MC_UnitOfMeasure
{
	int _a,_b,_c,_d; //y = (a+b*x)/(c+d*x),for the units of measure of length,angle,and scale,
					 //the addition constants 'a' and 'd' are zero.

	MC_UnitOfMeasure( )
	{
		_a = 0 ; _b = 1 ; _c = 1 ; _d = 0 ;
	}

	MC_UnitOfMeasure( int a , int b , int c , int d )
	{
		_a = a ; _b = b ; _c = c ; _d = d ;
	}
	
	void setPara( int a , int b , int c , int d )
	{
		_a = a ; _b = b ; _c = c ; _d = d ;
	}

	MC_UnitOfMeasure & operator = (const MC_UnitOfMeasure &right)
	{
		_a = right._a;
		_b = right._b;
		_c = right._c;
		_d = right._d;

		return *this;
	}

	bool operator == ( const MC_UnitOfMeasure& right ) const
	{
		return ( _a == right._a && _b == right._b && _c == right._c && _d == right._d ); 
	}

	bool isStandarnUnit( );
	double convertToStdValue( double dValue );
};

struct MC_Measure
{
	MC_UnitOfMeasure _uom;
	double _dValue;

	MC_Measure & operator = (const MC_Measure &right)
	{
		_uom = right._uom;
		_dValue = right._dValue;

		return *this;
	}
	bool operator == ( const MC_Measure& right ) const
	{
		return ( _uom == right._uom && _dValue == right._dValue ); 
	}

	double getStandardValue( ) const;

};
#pragma pack(pop)

#define MC_Angle	MC_Measure
#define MC_Length	MC_Measure
#define MC_Velocity MC_Measure //, PersistentGM
#define MC_Scale	MC_Measure
#define MC_Time		MC_Measure
#define MC_Area		MC_Measure
#define MC_Volume	MC_Measure
#define MC_Distance MC_Length

}//namespace GIS_Embedded

#endif
