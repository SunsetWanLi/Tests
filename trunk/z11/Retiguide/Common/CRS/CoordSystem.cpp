
#include "CoordSystem.h"

namespace GIS_Embedded
{
CoordinateSystemAxis & CoordinateSystemAxis::operator = (const CoordinateSystemAxis &right)
{
	//_strName = right._strName;
	//_strAxisDirection = right._strAxisDirection;
	if(this != &right)
	{
		_uom = right._uom;
	}

	return *this;
}

bool CoordinateSystemAxis::operator == ( const CoordinateSystemAxis& right ) const
{
	return ( _uom == right._uom );
}

//////////////////////////////////////////////////////////////////////////
////	CoordinateSystem
CoordinateSystem::CoordinateSystem( int dimension, CoordinateSystemType csType)
:  _dimension(dimension) , _csType(csType)
{ 
}

void CoordinateSystem::setCSType( CoordinateSystemType csType)
{
	_csType = csType;
}

CoordinateSystemType CoordinateSystem::getCSType() const 
{
	return _csType;
}

void CoordinateSystem::setDimension(int dimension)
{
	_dimension = dimension;
}

int CoordinateSystem::getDimension() const
{
	return _dimension;
}


CoordinateSystem::~CoordinateSystem( )
{
}

void CoordinateSystem::appenCoordinateSystemAxis( const int & iPos,const CoordinateSystemAxis axis )
{
	if( iPos == 0 )
		_x = axis ;
	if( iPos == 1 )
		_y = axis ;
	if( iPos == 2 && _dimension == 3 )
		_z = axis ;

}

bool CoordinateSystem::removeCoordinateSystemAxis( int iPos )
{
	return false ;
}

bool CoordinateSystem::getCrsAxisAt( int iPos , CoordinateSystemAxis& csAxis ) const
{
	if( iPos == 0 )
		csAxis = _x ;
	if( iPos == 1 )
		csAxis = _y ;
	if( iPos == 2 && _dimension == 3 )
		csAxis = _z ;

	return true ;
}

bool CoordinateSystem::operator==(const CoordinateSystem& coord) const
{
	// warning----------------------------
	return true;
	//------------------------------------
	//------------------------------------
	// WRONG switch-case---->>>
	bool rt = this->_csType == coord._csType;
	switch(_dimension)
	{
	case 1:
		rt = rt && this->_x == coord._x;
	case 2:
		rt = rt && this->_y == coord._y;
	case 3:
		rt = rt && this->_z == coord._z;
		break;
	default:
		break;
	}
	
	return rt;
}

}