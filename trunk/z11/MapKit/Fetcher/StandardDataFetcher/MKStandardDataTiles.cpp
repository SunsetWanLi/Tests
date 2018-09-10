/*
* =====================================================================================
*
*       Filename:  MKStandardDataTiles.cpp
*
*    Description:  for the given scale,MKMapPoint,trans to (0,0) (0,1) (0,2)...
*
*        Version:  1.0
*        Created:  04/24/2012 09:32:00 AM
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  Lucifer (GRF), gurongfang@gmail.com
*        Company:  ZJU
*
* =====================================================================================
*/

#include "MKStandardDataTiles.h"
#include "MapKit/Views/MapViews/MKTileLayerOverlay.h"
namespace z11
{
	MKStandardDataTiles::MKStandardDataTiles( const MKMapRegion& center_region ) 
	{
		m_cell_span = center_region.span;
		m_origion_point=center_region.center;
		m_origion_point.x-=m_cell_span.width/2;
		m_origion_point.y+=m_cell_span.height/2;

		m_based_rect=MGRectMake(0,0,MKMapViewCellWidth,MKMapViewCellHeight);
	}

	MKStandardDataTiles::MKStandardDataTiles( const MKMapRect& origion_rect )
	{
		m_cell_span = origion_rect.size;
		m_origion_point=origion_rect.origin;
		m_origion_point.y+=m_cell_span.height;

		m_based_rect=MGRectMake(0,0,MKMapViewCellWidth,MKMapViewCellHeight);
	}

	z11::MGIntPoint MKStandardDataTiles::query( const MKMapPoint &query_point )
	{
		return MGIntPointMake(-(query_point.y-m_origion_point.y)/m_cell_span.height,
			(query_point.x-m_origion_point.x)/m_cell_span.width
			);
	}

	z11::MKMapPoint MKStandardDataTiles::mapPointForPoint( const MGPoint &point )
	{
		MKMapPoint ret;
		ret.x = m_origion_point.x + point.x/m_based_rect.width * m_cell_span.width;
		ret.y = m_origion_point.y - point.y/m_based_rect.height * m_cell_span.height;
		return ret;
	}

	z11::MGPoint MKStandardDataTiles::pointForMapPoint( const MKMapPoint&point )
	{
		MGPoint ret;
		return ret;
	}

	z11::MKMapRect MKStandardDataTiles::mapRectForRect( const MGRect &rect )
	{
		MKMapRect ret;
		return ret;
	}

	z11::MGRect MKStandardDataTiles::rectForMapRect( const MKMapRect&rect )
	{
		MGRect ret;
		return ret;
	}

}