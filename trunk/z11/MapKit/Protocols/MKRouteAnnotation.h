#pragma once
#include "CommonBase/CommonDef.h"
#include "MapKit/DataTypes/Shapes/MKPointAnnotation.h"
#include <vector>
namespace z11
{
	class MKRouteAnnotation :
		public MKPointAnnotation
	{
	public:
		MKRouteAnnotation(void);
		~MKRouteAnnotation(void);
		MGPROPERTY(float,m_BearingBegin,BearingBegin);
		MGPROPERTY(float,m_BearingEnd,BearingEnd);
		MGPROPERTY(std::vector<MKLocationCoordinate2D>,m_points,Points);
	private:
	};
}
