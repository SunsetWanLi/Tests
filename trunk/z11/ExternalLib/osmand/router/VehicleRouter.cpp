#include "VehicleRouter.h"

static bool followSpeedLimitations=true;

VehicleRouter::VehicleRouter(void)
{
}


VehicleRouter::~VehicleRouter(void)
{
}

float VehicleRouter::defineSpeed( const RouteDataObject& road ) const
{
	if (isFollowSpeedLimitations())
	{
		float m =road.getMaximumSpeed();
		if (m>0)
		{
			return m;
		}
	}
}
bool VehicleRouter::isFollowSpeedLimitations() const
{
	return followSpeedLimitations;
}
