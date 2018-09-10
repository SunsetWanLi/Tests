/*
* =====================================================================================
*
*       Filename:  MKGlobalMapTiles.cpp
*
*    Description:  Global Map Tiles convertor
*
*        Version:  1.0
*        Created:  04/13/2012 09:30:08 AM
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  Lucifer (GRF), gurongfang@gmail.com
*        Company:  ZJU
*
* =====================================================================================
*/
#include "MKGlobalMapTiles.h"
#ifndef M_PI
#define M_PI   3.14159265358979323846
#endif
namespace z11
{
	
	MGDoublePoint GlobalMercator::LatLonToMeters(double lat,double lon )
	{
		double mx = (double)(lon * this->originShift / 180.0);
		double my = log( tan((90 + lat) * M_PI / 360.0 )) / (M_PI / 180.0);
		my = my * this->originShift / 180.0;
		return MGDoublePointMake(mx, my);
	}
	
	MGDoublePoint GlobalMercator::MetersToLatLon(double mx,double my )
	{
		double lon = (mx / this->originShift) * 180.0;
		double	lat = (my / this->originShift) * 180.0;

		lat = 180 / M_PI * (2 * atan( exp( lat * M_PI / 180.0)) - M_PI / 2.0);
		return MGDoublePointMake(lat, lon);
	}
	
	MGDoublePoint GlobalMercator::PixelsToMeters(int_64 px,int_64 py,int zoom)
	{
		double res = Resolution( zoom );
		double	mx = px * res - this->originShift;
		double	my = py * res - this->originShift;
		return MGDoublePointMake(mx, my);
	}
	
	MGLongPoint GlobalMercator::MetersToPixels(double mx,double my,int zoom)
	{
		double res = Resolution( zoom );
		int_64 px = (mx + this->originShift) / res;
		int_64 py = (my + this->originShift) / res;
		return MGLongPointMake(px, py);
	}
	
	MGDoubleRect GlobalMercator::TileBounds(int_32 tx,int_32 ty,int zoom)
	{
		MGDoublePoint min_pt = this->PixelsToMeters( tx*this->tileSize, ty*this->tileSize, zoom );
		MGDoublePoint max_pt= this->PixelsToMeters( (tx+1)*this->tileSize, (ty+1)*this->tileSize, zoom );
		return MGDoubleRectMake( min_pt.x, min_pt.y, max_pt.x-min_pt.x, max_pt.y-min_pt.y );
	}

	z11::MKInternalCoordinate2D GlobalMercator::get31TileNumber( const MKLocationCoordinate2D&coord )
	{
		return MKInternalCoordinate2D(get31TileNumberY(coord.latitude),get31TileNumberX(coord.longitude));
	}

	z11::MKLocationCoordinate2D GlobalMercator::get31Coordinate2D( const MKInternalCoordinate2D &pt )
	{
		return MKLocationCoordinate2D(get31LatitudeY(pt.latitude),get31LongitudeX(pt.longitude));
	}

	static double checkLongitude(double longitude) {
		while (longitude < -180 || longitude > 180) {
			if (longitude < 0) {
				longitude += 360;
			} else {
				longitude -= 360;
			}
		}
		return longitude;
	}

	static double checkLatitude(double latitude) {
		while (latitude < -90 || latitude > 90) {
			if (latitude < 0) {
				latitude += 180;
			} else {
				latitude -= 180;
			}
		}
		if (latitude < -85.0511) {
			return -85.0511;
		} else if (latitude > 85.0511) {
			return 85.0511;
		}
		return latitude;
	}

	double getPowZoom(float zoom){
		if(zoom >= 0 && zoom - floor(zoom) < 0.05f){
			return 1 << ((int)zoom);
		} else {
			return pow(2, zoom);
		}
	}

	double getLongitudeFromTile(float zoom, double x) {
		return x / getPowZoom(zoom) * 360.0 - 180.0;
	}


	double getLatitudeFromTile(float zoom, double y){
		int sign = y < 0 ? -1 : 1;
		double result = atan(sign*sinh(M_PI * (1 - 2 * y / getPowZoom(zoom)))) * 180. / M_PI;
		return result;
	}

	double GlobalMercator::get31LongitudeX( int_32 tileX )
	{
		return getLongitudeFromTile(21, tileX /1024.);

	}

	double GlobalMercator::get31LatitudeY( int_32 tileY )
	{
		return getLatitudeFromTile(21, tileY /1024.);
	}

	int_32 GlobalMercator::get31TileNumberX( double longitude )
	{
		longitude = checkLongitude(longitude);
		long long int l =  1;
		l <<= 31;
		return (int_32)((longitude + 180)/360 * l);

	}
	inline double toRadians(double angdeg) {
		return angdeg / 180 * M_PI;
	}

	int_32 GlobalMercator::get31TileNumberY( double latitude )
	{
		latitude = checkLatitude(latitude);
		double eval = log(tan(toRadians(latitude)) + 1 / cos(toRadians(latitude)));
		long long int l =  1;
		l <<= 31;
		if (eval > M_PI) {
			eval = M_PI;
		}
		return (int_32) ((1 - eval / M_PI) / 2 * l);
	}

	long long GlobalMercator::QuadTreeInt64( int_32 tx,int_32 ty,int_32 zoom )
	{
		std::string quadKey2;
		long long quadKey =0;
		ty = ((1<<zoom) - 1) - ty;
		for (int i=zoom;i>0;i--)
		{
			quadKey<<=2;
			int digit = 0;
			int mask = 1 << (i-1);
			if ((tx & mask) != 0)
				digit += 1;
			if ((ty & mask) != 0)
				digit += 2;
			quadKey|=digit;
		}
		return quadKey;
	}

	long long GlobalMercator::QuadTreeInt64( const MKGlobalTileIndex &index )
	{
		return QuadTreeInt64(index.x,index.y,index.z);
	}

	z11::MKGlobalTileIndex GlobalMercator::QuadKeyToTileIndexInt64( const long long&quadKey )
	{
		int_32 tileX =0;
		int_32 tileY = 0;
		int_32 z= 0;
		long long key=quadKey;
		while (key)
		{
			tileX|=((key&1)<<z);
			key>>=1;
			tileY|=((key&1)<<z);
			key>>=1;
			z++;
		}
		return MKGlobalTileIndex(tileX,tileY,z);
	}

	std::string GlobalMercator::QuadTree( int_32 tx, int_32 ty, int_32 zoom )
	{
		std::string quadKey = "";
		ty = ((1<<zoom) - 1) - ty;
		for (int i=zoom;i>0;i--)
		{
			int digit = 0;
			int mask = 1 << (i-1);
			if ((tx & mask) != 0)
				digit += 1;
			if ((ty & mask) != 0)
				digit += 2;
			quadKey.push_back('0'+digit);
		}
		return quadKey;
	}

	std::string GlobalMercator::QuadTree( const MKGlobalTileIndex &index )
	{
		return QuadTree(index.x,index.y,index.z);
	}

}