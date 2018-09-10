#include "GuiderGeoPoint.h"
namespace GIS_Embedded
{
	unsigned int GuiderGeoPoint::total_languages =0;
	unsigned int GuiderGeoPoint::index_languages =0;
	void GIS_Embedded::GuiderGeoPoint::setTotalLanguages( unsigned int total )
	{
		total_languages = total;
	}

	void GIS_Embedded::GuiderGeoPoint::setIndexLanguages( unsigned int index )
	{
		index_languages = index;
	}

	GIS_Embedded::GuiderGeoPoint::GeoTypeT GIS_Embedded::GuiderGeoPoint::getMyGeoType() const
	{
		return m_GType;
	}

	void GIS_Embedded::GuiderGeoPoint::setGeoType( GeoTypeT type )
	{
		m_GType = type;
	}

	const Pos2DEx GIS_Embedded::GuiderGeoPoint::getMyPosition() const
	{
		return m_pos;
	}

	void GIS_Embedded::GuiderGeoPoint::setMyPosition( int x,int y )
	{
		m_pos.d0 = x;
		m_pos.d1 = y;
	}
}
