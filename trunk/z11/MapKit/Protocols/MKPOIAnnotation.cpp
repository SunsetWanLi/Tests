#include "MKPOIAnnotation.h"
namespace z11
{

	MKPOIAnnotation::MKPOIAnnotation(void)
	{
		m_maximum_scale=30;
		m_minimum_scale=0;
	}


	MKPOIAnnotation::~MKPOIAnnotation(void)
	{
	}

	z11::MKLocationCoordinate2D MKPOIAnnotation::coordinate()
	{
		return MKLocationShiftProvider::applyChinaLocationShift(m_coord);
	}

	void MKPOIAnnotation::setCoordinate( const MKLocationCoordinate2D&coord )
	{
		m_coord = coord;
	}

	std::string MKPOIAnnotation::title()
	{
		return m_name;
	}

}
