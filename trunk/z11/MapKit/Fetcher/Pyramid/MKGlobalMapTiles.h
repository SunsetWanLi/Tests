/*
* =====================================================================================
*
*       Filename:  MKGlobalMapTiles.h
*
*    Description:  Global Map Tiles convertor
*
*        Version:  1.0
*        Created:  04/13/2012 09:32:00 AM
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  Lucifer (GRF), gurongfang@gmail.com
*        Company:  ZJU
*
* =====================================================================================
*/
#pragma  once
#include <math.h>
#include <string>
#include "../../../CommonBase/MGStructure.h"
#include "MapKit/DataTypes/Base/MKGeometry.h"
#include "MapKit/DataTypes/Base/MKTypes.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288 
#endif
#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923132169163975144
#endif
namespace z11
{
	/*
	TMS Global Mercator Profile
	---------------------------

	Functions necessary for generation of tiles in Spherical Mercator projection,
	EPSG:900913 (EPSG:gOOglE, Google Maps Global Mercator), EPSG:3785, OSGEO:41001.

	Such tiles are compatible with Google Maps, Microsoft Virtual Earth, Yahoo Maps,
	UK Ordnance Survey OpenSpace API, ...
	and you can overlay them on top of base maps of those web mapping applications.

	Pixel and tile coordinates are in TMS notation (origin [0,0] in bottom-left).

	What coordinate conversions do we need for TMS Global Mercator tiles::

	LatLon      <->       Meters      <->     Pixels    <->       Tile     

	WGS84 coordinates   Spherical Mercator  Pixels in pyramid  Tiles in pyramid
	lat/lon            XY in metres     XY pixels Z zoom      XYZ from TMS 
	EPSG:4326           EPSG:900913                                         
	.----.              ---------               --                TMS      
	/      \     <->     |       |     <->     /----/    <->      Google    
	\      /             |       |           /--------/          QuadTree   
	-----               ---------         /------------/                   
	KML, public         WebMapService         Web Clients      TileMapService

	What is the coordinate extent of Earth in EPSG:900913?

	[-20037508.342789244, -20037508.342789244, 20037508.342789244, 20037508.342789244]
	Constant 20037508.342789244 comes from the circumference of the Earth in meters,
	which is 40 thousand kilometers, the coordinate origin is in the middle of extent.
	In fact you can calculate the constant as: 2 * PI * 6378137 / 2.0
	$ echo 180 85 | gdaltransform -s_srs EPSG:4326 -t_srs EPSG:900913
	Polar areas with abs(latitude) bigger then 85.05112878 are clipped off.

	What are zoom level constants (pixels/meter) for pyramid with EPSG:900913?

	whole region is on top of pyramid (zoom=0) covered by 256x256 pixels tile,
	every lower zoom level resolution is always divided by two
	initialResolution = 20037508.342789244 * 2 / 256 = 156543.03392804062

	What is the difference between TMS and Google Maps/QuadTree tile name convention?

	The tile raster itself is the same (equal extent, projection, pixel size),
	there is just different identification of the same raster tile.
	Tiles in TMS are counted from [0,0] in the bottom-left corner, id is XYZ.
	Google placed the origin [0,0] to the top-left corner, reference is XYZ.
	Microsoft is referencing tiles by a QuadTree name, defined on the website:
	http://msdn2.microsoft.com/en-us/library/bb259689.aspx

	The lat/lon coordinates are using WGS84 datum, yeh?

	Yes, all lat/lon we are mentioning should use WGS84 Geodetic Datum.
	Well, the web clients like Google Maps are projecting those coordinates by
	Spherical Mercator, so in fact lat/lon coordinates on sphere are treated as if
	the were on the WGS84 ellipsoid.

	From MSDN documentation:
	To simplify the calculations, we use the spherical form of projection, not
	the ellipsoidal form. Since the projection is used only for map display,
	and not for displaying numeric coordinates, we don't need the extra precision
	of an ellipsoidal projection. The spherical projection causes approximately
	0.33 percent scale distortion in the Y direction, which is not visually noticable.

	How do I create a raster in EPSG:900913 and convert coordinates with PROJ.4?

	You can use standard GIS tools like gdalwarp, cs2cs or gdaltransform.
	All of the tools supports -t_srs 'epsg:900913'.

	For other GIS programs check the exact definition of the projection:
	More info at http://spatialreference.org/ref/user/google-projection/
	The same projection is degined as EPSG:3785. WKT definition is in the official
	EPSG database.

	Proj4 Text:
	+proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0
	+k=1.0 +units=m +nadgrids=@null +no_defs

	Human readable WKT format of EPGS:900913:
	PROJCS["Google Maps Global Mercator",
	GEOGCS["WGS 84",
	DATUM["WGS_1984",
	SPHEROID["WGS 84",6378137,298.2572235630016,
	AUTHORITY["EPSG","7030"]],
	AUTHORITY["EPSG","6326"]],
	PRIMEM["Greenwich",0],
	UNIT["degree",0.0174532925199433],
	AUTHORITY["EPSG","4326"]],
	PROJECTION["Mercator_1SP"],
	PARAMETER["central_meridian",0],
	PARAMETER["scale_factor",1],
	PARAMETER["false_easting",0],
	PARAMETER["false_northing",0],
	UNIT["metre",1,
	AUTHORITY["EPSG","9001"]]]
	*/
	class GlobalMercator
	{

	public:
		//Initialize the TMS Global Mercator pyramid
		GlobalMercator(int_32 tileSize=MAP_CELL_SIZE)
		{
			this->tileSize = tileSize;
			this->initialResolution = 2 * M_PI * 6378137 / this->tileSize;
			// 156543.03392804062 for tileSize 256 pixels
			this->originShift = 2 *  M_PI_2 * 6378137;
			// 20037508.342789244

		}
	private:
		int_32 tileSize;
		double initialResolution;
		double originShift;

	public:
		static double get31LongitudeX(int_32 tileX);
		static double get31LatitudeY(int_32 tileY);
		static int_32 get31TileNumberX(double longitude);
		static int_32 get31TileNumberY( double latitude);
		static MKInternalCoordinate2D get31TileNumber(const MKLocationCoordinate2D&coord);

		static MKLocationCoordinate2D get31Coordinate2D(const MKInternalCoordinate2D &pt);
		
		// Converts given lat/lon in WGS84 Datum to XY in Spherical Mercator EPSG:900913
		MGDoublePoint LatLonToMeters(double lat,double lon );

		//Converts XY point from Spherical Mercator EPSG:900913 to lat/lon in WGS84 Datum
		MGDoublePoint MetersToLatLon(double mx,double my );

		//Converts pixel coordinates in given zoom level of pyramid to EPSG:900913
		MGDoublePoint PixelsToMeters(int_64 px,int_64 py,int zoom);

		//Converts EPSG:900913 to pyramid pixel coordinates in given zoom level
		MGLongPoint MetersToPixels(double mx,double my,int zoom);

		//Returns a tile covering region in given pixel coordinates
		MGIntPoint PixelsToTile(int_64 px,int_64 py)
		{
			int_32 tx = int_32( ceil( px / double(this->tileSize) ) - 1 );
			int_32 ty = int_32( ceil( py / double(this->tileSize) ) - 1 );
			return MGIntPointMake(tx, ty);
		}


		//Move the origin of pixel coordinates to top-left corner"
		MGLongPoint PixelsToRaster(int_64 px,int_64 py,int zoom)
		{
			int_64 mapSize = (int_64)(this->tileSize << zoom);
			return MGLongPointMake(px, mapSize - py);
		}


		//Returns tile for given mercator coordinates
		MGIntPoint MetersToTile(double mx,double my,int zoom)
		{
			MGLongPoint pt = this->MetersToPixels( mx, my, zoom);
			return this->PixelsToTile( pt.x, pt.y);
		}


		//Returns bounds of the given tile in EPSG:900913 coordinates
		MGDoubleRect TileBounds(int_32 tx,int_32 ty,int zoom);

		//Returns bounds of the given tile in latutude/longitude using WGS84 datum
		MGDoubleRect TileLatLonBounds(int_32 tx,int_32 ty,int zoom )
		{
			MGDoubleRect bounds = this->TileBounds( tx, ty, zoom);
			MGDoublePoint min_pt = this->MetersToLatLon(bounds.x, bounds.y);
			MGDoublePoint max_pt = this->MetersToLatLon(bounds.x+bounds.width, bounds.y+bounds.height);
			return MGDoubleRectMake( min_pt.x, min_pt.y, max_pt.x-min_pt.x, max_pt.y-min_pt.y );

		}



		//Resolution (meters/pixel) for given zoom level (measured at Equator)
		double Resolution(int zoom )
		{
			// return (2 * PI * 6378137) / (this->tileSize * 2**zoom)
			return this->initialResolution / (1<<zoom);

		}


		//Maximal scaledown zoom of the pyramid closest to the pixelSize.
		int ZoomForPixelSize(int_64 pixelSize )
		{
            int i = 0;
			for(i = 0; i < 31; ++i)
				if(pixelSize < this->tileSize << i)
				{
					if (i!=0)
					{
						return i-1;
					}
					else
					{
						return 0;
					}
				}
				//return i-1 if i!=0 else 0 #We don't want to scale up
            return i;
		}


		//Converts TMS tile coordinates to Google Tile coordinates"
		MGIntPoint GoogleTile(int_32 tx,int_32 ty,int zoom)
		{
			// coordinate origin is moved from bottom-left to top-left corner of the extent
			return MGIntPointMake(tx, ((1<<zoom) - 1) - ty);

		}
		//Converts TMS tile coordinates to Google Tile coordinates"
		MGIntPoint GoogleTile(const MKGlobalTileIndex& index)
		{
			// coordinate origin is moved from bottom-left to top-left corner of the extent
			return MGIntPointMake(index.x, ((1<<index.z) - 1) - index.y);

		}


		//Converts TMS tile coordinates to Microsoft QuadTree
		static std::string QuadTree(int_32 tx,int_32 ty,int_32 zoom );
		//Converts TMS tile coordinates to Microsoft QuadTree
		static std::string QuadTree(const MKGlobalTileIndex  &index);

		static long long QuadTreeInt64(int_32 tx,int_32 ty,int_32 zoom );
		static long long QuadTreeInt64(const MKGlobalTileIndex  &index);
		static MKGlobalTileIndex QuadKeyToTileIndexInt64(const long long&quadKey );
	};



	//#---------------------
	//
	//	class GlobalGeodetic(object):
	//		"""
	//		TMS Global Geodetic Profile
	//		---------------------------
	//
	//		Functions necessary for generation of global tiles in Plate Carre projection,
	//EPSG:4326, "unprojected profile".
	//
	//	 Such tiles are compatible with Google Earth (as any other EPSG:4326 rasters)
	//	 and you can overlay the tiles on top of OpenLayers base map.
	//
	//	 Pixel and tile coordinates are in TMS notation (origin [0,0] in bottom-left).
	//
	//	 What coordinate conversions do we need for TMS Global Geodetic tiles?
	//
	//	 Global Geodetic tiles are using geodetic coordinates (latitude,longitude)
	//	 directly as planar coordinates XY (it is also called Unprojected or Plate
	//	 Carre). We need only scaling to pixel pyramid and cutting to tiles.
	//	 Pyramid has on top level two tiles, so it is not square but rectangle.
	//	 Area [-180,-90,180,90] is scaled to 512x256 pixels.
	//	 TMS has coordinate origin (for pixels and tiles) in bottom-left corner.
	//	 Rasters are in EPSG:4326 and therefore are compatible with Google Earth.
	//
	//	 LatLon      <->      Pixels      <->     Tiles     
	//
	//	 WGS84 coordinates   Pixels in pyramid  Tiles in pyramid
	//	 lat/lon         XY pixels Z zoom      XYZ from TMS 
	//EPSG:4326                                           
	//	 .----.                ----                         
	//	 /      \     <->    /--------/    <->      TMS      
	//	 \      /         /--------------/                   
	//	 -----        /--------------------/                
	//	 WMS, KML    Web Clients, Google Earth  TileMapService
	//	 """
	//
	//	 def __init__(this, tileSize = 256):
	//	this->tileSize = tileSize
	//
	//		def LatLonToPixels(this, lat, lon, zoom):
	//	"Converts lat/lon to pixel coordinates in given zoom of the EPSG:4326 pyramid"
	//
	//		res = 180 / 256.0 / 2**zoom
	//		px = (180 + lat) / res
	//		py = (90 + lon) / res
	//		return px, py
	//
	//		def PixelsToTile(this, px, py):
	//	"Returns coordinates of the tile covering region in pixel coordinates"
	//
	//		tx = int( math.ceil( px / float(this->tileSize) ) - 1 )
	//		ty = int( math.ceil( py / float(this->tileSize) ) - 1 )
	//		return tx, ty
	//
	//		def Resolution(this, zoom ):
	//	"Resolution (arc/pixel) for given zoom level (measured at Equator)"
	//
	//		return 180 / 256.0 / 2**zoom
	//#return 180 / float( 1 << (8+zoom) )
	//
	//		def TileBounds(tx, ty, zoom):
	//	"Returns bounds of the given tile"
	//		res = 180 / 256.0 / 2**zoom
	//		return (
	//		tx*256*res - 180,
	//		ty*256*res - 90,
	//		(tx+1)*256*res - 180,
	//		(ty+1)*256*res - 90
	//		)

}
