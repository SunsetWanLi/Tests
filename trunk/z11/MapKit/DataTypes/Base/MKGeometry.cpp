/*
 * =====================================================================================
 *
 *       Filename:  MKGeometry.cpp
 *
 *    Description:  define Map Kit Data Types
 *
 *        Version:  1.0
 *        Created:  2012/04/14  03:15:59 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */

#include "MKGeometry.h"
namespace z11
{
	MKMapRect MKMapRectMake(double x,double y,double width,double height)
	{
		MKMapRect ret;
		ret.origin.x=x;
		ret.origin.y=y;
		ret.size.width=width;
		ret.size.height=height;
		return ret;
	}


	MKGlobalTileIndex::MKGlobalTileIndex() :x(0),y(0),z(0)
	{

	}

	MKGlobalTileIndex::MKGlobalTileIndex( int_32 _x,int_32 _y ,int _z ) :x(_x),y(_y),z(_z)
	{

	}

	bool MKGlobalTileIndex::operator<( const MKGlobalTileIndex&rhs ) const
	{
		if (this->z!=rhs.z)
		{
			return this->z<rhs.z;
		}

		if (this->y!=rhs.y)
		{
			return this->y<rhs.y;
		}

		return this->x<rhs.x;
	}
    
    bool MKGlobalTileIndex::operator==( const MKGlobalTileIndex&rhs ) const
	{
		if (this->z!=rhs.z)
		{
			return false;
		}
        
		if (this->y!=rhs.y)
		{
			return false;
		}
        
		return this->x == rhs.x;
	}
}