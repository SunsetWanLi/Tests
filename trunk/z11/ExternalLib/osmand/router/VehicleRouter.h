#pragma once
#include "../native/binaryRead.h"
class VehicleRouter
{
public:
	VehicleRouter(void);
	~VehicleRouter(void);
	bool isFollowSpeedLimitations() const;
	float defineSpeed(const RouteDataObject& road)  const;
};

