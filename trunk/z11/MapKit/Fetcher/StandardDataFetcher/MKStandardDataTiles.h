/*
 * =====================================================================================
 *
 *       Filename:  MKStandardDataTiles.h
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
#pragma once
#include "CommonBase/MGStructure.h"
#include "MapKit/DataTypes/Base/MKGeometry.h"
namespace z11
{
	class MKTileIndex
	{
	public:
		int x;
		int y;
		MKTileIndex(int _x,int _y)
			:x(_x),y(_y)
		{

		}
		bool operator< (const MKTileIndex& index) const
		{
			if (x  != index.x)
			{
				return x<index.x;
			}
			return y<index.y;
		}
	};
	class MKStandardDataTiles
	{
	public:
		MKStandardDataTiles(const MKMapRegion& center_region);
		MKStandardDataTiles(const MKMapRect& origion_rect);
		//based on m_origion_point
		MGIntPoint query(const MKMapPoint &query_point);
		//based on (m_based_rect.x,m_based_rect.y) ,initially based on(0,0)
		MKMapPoint mapPointForPoint(const MGPoint &point);
		MGPoint pointForMapPoint(const MKMapPoint&point);
		MKMapRect mapRectForRect(const MGRect &rect);
		MGRect rectForMapRect(const MKMapRect&rect);
	private:
		MKMapPoint m_origion_point;
		MKMapSize m_cell_span;
		MGRect m_based_rect;
	};
};
