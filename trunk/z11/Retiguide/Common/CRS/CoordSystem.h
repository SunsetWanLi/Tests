#ifndef GIS_Embeded_COORD_SYSTEM_H_
#define GIS_Embeded_COORD_SYSTEM_H_

#include "Datum.h"

namespace GIS_Embedded
{
;

struct CoordinateSystemAxis
{
	//std::string _strName;
	//std::string _strAxisDirection;	//the currently defined axis direction are:
	//								//North,NothEast,East,SouthEast,South,SouthWest,West,NorthWest,
	//								//prime meridian,future,past,up and down,out,and other
	MC_UnitOfMeasure _uom; 

public:
	CoordinateSystemAxis & operator = (const CoordinateSystemAxis &right);
	bool operator == ( const CoordinateSystemAxis& right ) const;
};


class CoordinateSystem
{
public:
	//CoordinateSystem( )
	//{}


	CoordinateSystem(/* CRS_ID id , */int dimension = 2 , CoordinateSystemType cs_type = CS_CARTESIAN );

	virtual ~CoordinateSystem( );

	void setCSType( CoordinateSystemType csType) ;
	CoordinateSystemType getCSType() const ;
	void setDimension(int dimension) ;
	int getDimension() const ;
	void appenCoordinateSystemAxis( const int & iPos, const CoordinateSystemAxis axis );
	bool removeCoordinateSystemAxis( int iPos );
	bool getCrsAxisAt( int iPos , CoordinateSystemAxis& csAxis ) const ;
	
	bool operator==(const CoordinateSystem & coord) const; 
private:
	CoordinateSystemType _csType;
	int _dimension;
	CoordinateSystemAxis _x, _y, _z ;
};

} //
#endif //GIS_Embeded_COORD_SYSTEM_H_