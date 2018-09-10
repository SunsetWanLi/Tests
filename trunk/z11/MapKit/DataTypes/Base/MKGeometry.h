/*
 * =====================================================================================
 *
 *       Filename:  MKDataTypes.h
 *
 *    Description:  define Map Kit Data Types
 *
 *        Version:  1.0
 *        Created:  04/11/2012 03:15:59 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#pragma  once

#include "../../../CommonBase/CommonDef.h"

namespace z11
{
	typedef double MKLocationAccuracy;
	typedef double MKLocationDegrees;
	typedef double MKLocationDirection;
	typedef double MKLocationDistance;
	struct  MKLocationCoordinate2D
	{
		MKLocationCoordinate2D():latitude(0.0),longitude(0.0)
		{
		}
		MKLocationCoordinate2D(MKLocationDegrees _latitude,MKLocationDegrees _longitude):latitude(_latitude),longitude(_longitude)
		{
		}
		MKLocationDegrees latitude;
		MKLocationDegrees longitude;
	};
    struct  MKLocationCoordinate3D
	{
		MKLocationCoordinate3D():latitude(0.0),longitude(0.0),altitude(0.0)
		{
		}
		MKLocationCoordinate3D(MKLocationDegrees _latitude,MKLocationDegrees _longitude ,MKLocationDegrees _altitude = 0):latitude(_latitude),longitude(_longitude),altitude(_altitude)
		{
		}
		MKLocationDegrees latitude;
		MKLocationDegrees longitude;
        MKLocationDegrees altitude;
	};
	typedef int  MKInternalDegrees;
	struct MKInternalCoordinate2D
	{
		MKInternalCoordinate2D():latitude(0),longitude(0)
		{
		}
		MKInternalCoordinate2D(MKInternalDegrees _latitude,MKInternalDegrees _longitude):latitude(_latitude),longitude(_longitude)
		{
		}
		MKInternalDegrees latitude;
		MKInternalDegrees longitude;
	};

	typedef struct 
	{
		MKLocationDegrees latitudeDelta;
		MKLocationDegrees longitudeDelta;
	} MKCoordinateSpan;

	typedef struct {
		MKLocationCoordinate2D center;
		MKCoordinateSpan span;
	} MKCoordinateRegion;

	class MKMapPoint
	{
	public:
		double x;
		double y;
		MKMapPoint():x(0.0),y(0.0)
		{

		}

		MKMapPoint(double _x,double _y)
			:x(_x),y(_y)
		{

		}
	};

	class MKMapSize
	{
	public:
		double width;
		double height;
		MKMapSize():width(0.0),height(0.0)
		{

		}

		MKMapSize(double _width,double _height)
			:width(_width),height(_height)
		{

		}
	};

	class MKMapRegion
	{
	public:
		MKMapPoint center;
		MKMapSize span;
		MKMapRegion(double x,double y,double width,double height)
			:center(x,y),span(width,height)
		{

		}
		MKMapRegion():center(0,0),span(0,0)
		{

		}
	};

	class MKMapRect
	{
	public:
		//MKMapRect's origin based on left bottom point;
		MKMapPoint origin;
		MKMapSize size;
		double getMinX() const
		{
			return origin.x;
		}
		double getMinY() const
		{
			return origin.y;
		}
		double getMaxX() const
		{
			return origin.x+size.width;
		}
		double getMaxY() const
		{
			return origin.y+size.height;
		}
		bool intersect(const MKMapRect &rect) const
		{
			return (rect.getMaxX()>getMinX() && rect.getMinX()<getMaxX() &&
				rect.getMaxY()>getMinY() && rect.getMinY()<getMaxY());

		}
		//MKMapRect Intersection()
		//{

		//}
		MKMapRect(double x,double y,double width,double height)
			:origin(x,y),size(width,height)
		{

		}
		MKMapRect():origin(0,0),size(0,0)
		{

		}
	};

	typedef float MKZoomScale;

	MKMapPoint MKMapPointForCoordinate(const MKLocationCoordinate2D& coordinate);
	MKLocationCoordinate2D MKCoordinateForMapPoint(const MKMapPoint& mapPoint);
	MKCoordinateRegion MKCoordinateRegionForMapRect(const MKMapRect& rect);


	class MKGlobalTileIndex
	{
	public:
		MKGlobalTileIndex();
		MKGlobalTileIndex(int_32 _x,int_32 _y ,int _z);
		int_32 x;
		int_32 y;
		int z;
		bool operator <(const MKGlobalTileIndex&rhs) const;
        bool operator ==(const MKGlobalTileIndex&rhs) const;
	};
}
