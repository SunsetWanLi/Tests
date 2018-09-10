#include "MapUtils.h"
#include "../utils/MathUtils.h"

MapUtils::MapUtils(void)
{
}


MapUtils::~MapUtils(void)
{
}

double MapUtils::alignAngleDifference( double diff )
{
	while(diff > MathUtils::PI) {
		diff -= 2 * MathUtils::PI;
	}
	while(diff <=-MathUtils::PI) {
		diff += 2 * MathUtils::PI;
	}
	return diff;
}

double MapUtils::degreesDiff( double a1, double a2 )
{
	double diff = a1 - a2;
	while(diff > 180) {
		diff -= 360;
	}
	while(diff <=-180) {
		diff += 360;
	}
	return diff;
}