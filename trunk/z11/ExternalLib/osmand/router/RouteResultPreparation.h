#pragma once
#include "../native/binaryRoutePlanner.h"
class RouteResultPreparation
{
public:
	RouteResultPreparation(void);
	~RouteResultPreparation(void);
	void validateAllPointsConnected(const std::vector<RouteSegmentResult>& result)
	{
		//TODO: to validate
	}
	void prepareResult(RoutingContext *ctx,bool leftside,std::vector<RouteSegmentResult>&result);
private:
	void checkAndInitRouteRegion(RoutingContext *ctx, RouteDataObject& road);
	 void attachRoadSegments(RoutingContext *ctx, vector<RouteSegmentResult>& result, int routeInd, int pointInd, bool plus);
	void calculateTimeSpeedAndAttachRoadSegments(RoutingContext *ctx, std::vector<RouteSegmentResult>& result);
	long getPoint(RouteDataObject& road, int pointInd);
	void addTurnInfo(bool leftside, std::vector<RouteSegmentResult>& result);
	TurnType getTurnInfo(std::vector<RouteSegmentResult>& result, int i, bool leftSide);
	TurnType attachKeepLeftInfoAndLanes(bool leftSide, RouteSegmentResult& prev, RouteSegmentResult& rr, TurnType& t);
};

