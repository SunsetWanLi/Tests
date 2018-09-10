#include "RouteResultPreparation.h"
#include "../utils/MathUtils.h"
#include "../osm/MapUtils.h"
#include "../utils/StringUtils.h"
#include <assert.h>
#include "../../../System/Shared/MGLog.h"
#include <sstream>
static const float TURN_DEGREE_MIN = 45;
static const int MAX_SPEAK_PRIORITY = 5;
RouteResultPreparation::RouteResultPreparation(void)
{
}


RouteResultPreparation::~RouteResultPreparation(void)
{
}

void RouteResultPreparation::calculateTimeSpeedAndAttachRoadSegments( RoutingContext *ctx, std::vector<RouteSegmentResult>& result )
{
	for (int i = 0; i < result.size(); i++) {
		//if(ctx.checkIfMemoryLimitCritical(ctx.config.memoryLimitation)) {
		//	ctx.unloadUnusedTiles(ctx.config.memoryLimitation);
		//}
		RouteSegmentResult* rr = &result.at(i);
		RouteDataObject& road = *rr->getObject();
		checkAndInitRouteRegion(ctx, road);
		double distOnRoadToPass = 0;
		double speed = ctx->config.defineSpeed(rr->getObject());
		if (speed == 0) {
			speed = ctx->config.getMinDefaultSpeed();
		}
		bool plus = rr->getStartPointIndex() < rr->getEndPointIndex();
		int next;
		double distance = 0;
		for (int j = rr->getStartPointIndex(); j != rr->getEndPointIndex(); j = next) {
			next = plus ? j + 1 : j - 1;
			if (j == rr->getStartPointIndex()) {
				attachRoadSegments(ctx, result, i, j, plus);
			}
			if (next != rr->getEndPointIndex()) {
				attachRoadSegments(ctx, result, i, next, plus);
			}

			double d = measuredDist(road.getPoint31XTile(j), road.getPoint31YTile(j), road.getPoint31XTile(next),
				road.getPoint31YTile(next));
			distance += d;
			
			double obstacle=ctx->config.defineRoutingObstacle(rr->getObject(),j);
			if(obstacle < 0) {
				obstacle = 0;
			}
			distOnRoadToPass += d / speed + obstacle;

			vector<RouteSegmentResult>& attachedRoutes = rr->getAttachedRoutes(next);
			if (next != rr->getEndPointIndex() && !rr->getObject()->roundabout() && !attachedRoutes.empty()) {
				float before = rr->getBearing(next, !plus);
				float after = rr->getBearing(next, plus);
				bool straight = MathUtils::abs(MapUtils::degreesDiff(before + 180, after)) < TURN_DEGREE_MIN;
				bool isSplit = false;
				// split if needed
				for (size_t ii=0;ii<attachedRoutes.size();ii++) {
					RouteSegmentResult& rs=attachedRoutes[ii];
					double diff = MapUtils::degreesDiff(before + 180, rs.getBearingBegin());
					if (MathUtils::abs(diff) <= TURN_DEGREE_MIN) {
						isSplit = true;
					} else if (!straight && MathUtils::abs(diff) < 100) {
						isSplit = true;
					}
				}
				if (isSplit) {
					int endPointIndex = rr->getEndPointIndex();
					RouteSegmentResult split = RouteSegmentResult(rr->getObject(), next, endPointIndex);
					split.copyPreattachedRoutes(*rr, MathUtils::abs(next - rr->getStartPointIndex()));
					rr->setSegmentTime((float) distOnRoadToPass);
					rr->setSegmentSpeed((float) speed);
					rr->setDistance((float) distance);
					rr->setEndPointIndex(next);

					if (i+1>=result.size())
					{
						assert(i+1==result.size());
						result.insert(result.begin()+i+1,split);
					}
					else
					{
						result[i+1]=split;
					}
					//result.add(i + 1, split);
					i++;
					// switch current segment to the splitted
					rr = &result[i];
					distOnRoadToPass = 0;
					distance = 0;
				}
			}
		}
		// last point turn time can be added
		// if(i + 1 < result.size()) { distOnRoadToPass += ctx.getRouter().calculateTurnTime(); }
		rr->setSegmentTime((float) distOnRoadToPass);
		rr->setSegmentSpeed((float) speed);
		rr->setDistance((float) distance);


	}
}

void RouteResultPreparation::attachRoadSegments( RoutingContext *ctx, vector<RouteSegmentResult>& result, int routeInd, int pointInd, bool plus )
{
	//TODO:
	RouteSegmentResult& rr = result.at(routeInd);
	RouteDataObject& road = *rr.getObject();
	long nextL = pointInd < road.getPointsLength() - 1 ? getPoint(road, pointInd + 1) : 0;
	long prevL = pointInd > 0 ? getPoint(road, pointInd - 1) : 0;

	

	// by default make same as this road id
	long previousRoadId = road.getId();
	if (pointInd == rr.getStartPointIndex() && routeInd > 0) {
		// attach additional roads to represent more information about the route
		RouteSegmentResult& previousResult = result.at(routeInd - 1);
		previousRoadId = previousResult.getObject()->getId();
		if (previousRoadId != road.getId()) {
			if (previousResult.getStartPointIndex() < previousResult.getEndPointIndex()
				&& previousResult.getEndPointIndex() < previousResult.getObject()->getPointsLength() - 1) {
					rr.attachRoute(pointInd, RouteSegmentResult(previousResult.getObject(), previousResult.getEndPointIndex(),
						previousResult.getObject()->getPointsLength() - 1));
			} else if (previousResult.getStartPointIndex() > previousResult.getEndPointIndex() 
				&& previousResult.getEndPointIndex() > 0) {
					rr.attachRoute(pointInd, RouteSegmentResult(previousResult.getObject(), previousResult.getEndPointIndex(), 0));
			}
		}
	}
	//Iterator<RouteSegment> it;
	//if(!rr.getPreAttachedRoutes(pointInd).empty()) {
	//	vector<RouteSegmentResult>& list = rr.getPreAttachedRoutes(pointInd);
	//	//TODO
	//	it = new Iterator<BinaryRoutePlanner.RouteSegment>() {
	//		int i = 0;
	//		@Override
	//			public bool hasNext() {
	//				return i < list.length;
	//		}

	//		@Override
	//			public RouteSegment next() {
	//				RouteSegmentResult r = list[i++];
	//				return new RouteSegment(r.getObject(), r.getStartPointIndex());
	//		}

	//		@Override
	//			public void remove() {
	//		}
	//	};	
	//} else {
	//	SHARED_PTR<RouteSegment> rt = ctx->loadRouteSegment(road.getPoint31XTile(pointInd), road.getPoint31YTile(pointInd), ctx.config.memoryLimitation);
	//	it = rt.getIterator();
	//}

	SHARED_PTR<RouteSegment> it = ctx->loadRouteSegment(road.getPoint31XTile(pointInd), road.getPoint31YTile(pointInd)/*, ctx.config.memoryLimitation*/);
	if (it.get()==NULL)
	{
		return ;
	}
	// try to attach all segments except with current id
	while (it->next.get()!=NULL) {
		SHARED_PTR<RouteSegment> routeSegment = it->next;
		if (routeSegment->road->getId() != road.getId() && routeSegment->road->getId() != previousRoadId) {
			SHARED_PTR<RouteDataObject> addRoad = (routeSegment->road);
			RouteDataObject& addRoadObj=*addRoad;
			//checkAndInitRouteRegion(ctx, addRoad);
			// TODO restrictions can be considered as well
			int oneWay=1;
			//int oneWay = ctx.getRouter().isOneWay(addRoad);
			if (oneWay >= 0 && routeSegment->getSegmentStart() < addRoad->getPointsLength() - 1) {
				long pointL = getPoint(addRoadObj, routeSegment->getSegmentStart() + 1);
				if(pointL != nextL && pointL != prevL) {
					// if way contains same segment (nodes) as different way (do not attach it)
					rr.attachRoute(pointInd, RouteSegmentResult(addRoad, routeSegment->getSegmentStart(), addRoad->getPointsLength() - 1));
				}
			}
			if (oneWay <= 0 && routeSegment->getSegmentStart() > 0) {
				long pointL = getPoint(addRoadObj, routeSegment->getSegmentStart() - 1);
				// if way contains same segment (nodes) as different way (do not attach it)
				if(pointL != nextL && pointL != prevL) {
					rr.attachRoute(pointInd, RouteSegmentResult(addRoad, routeSegment->getSegmentStart(), 0));
				}
			}
		}
		it=routeSegment;
	}
}

void RouteResultPreparation::prepareResult( RoutingContext *ctx,bool leftside,std::vector<RouteSegmentResult>&result )
{
	//validateAllPointsConnected(result);
	//calculate time
	calculateTimeSpeedAndAttachRoadSegments(ctx, result);
	addTurnInfo(leftside, result);
	//for (size_t i=0;i<result.size();i++)
	//{
	//	std::stringstream ss;
	//	RouteSegmentResult &r=result[i];
	//	ss<<"<segment id=\""<<r.getObject()->getId()<<"\" start=\""<<r.getStartPointIndex()<<"\" end=\""<<r.getEndPointIndex()<<"\"";
	//	ss<<"time = \""<<r.segmentTime<<"\" ";
	//	ss<<"name = \""<<r.getObject()->getName()<<"\" ";
	//	ss<<"maxspeed = \""<<r.getSegmentSpeed()*3.6f<<"\" ";
	//	ss<<"distance = \""<<r.getDistance()<<"\" ";
	//	ss<<"turn = \""<<r.getTurnType().toString()<<"\" ";
	//	ss<<"start_bearing = \""<<r.getBearingBegin()<<"\" ";
	//	ss<<"end_bearing = \""<<r.getBearingEnd()<<"\" ";
	//	ss<<"description = \""<<r.getDescription()<<"\" ";
	//	ss<<"/>";
	//	z11::MGLogD(ss.str().c_str());
	//	for (int ii=r.getStartPointIndex();ii<=r.getEndPointIndex();ii++)
	//	{
	//		z11::MGLogD("x[%d]=%d,y[%d]=%d",ii,r.getObject()->getPoint31XTile(ii),ii,
	//			r.getObject()->getPoint31YTile(ii));
	//	}
	//}
}


long RouteResultPreparation::getPoint( RouteDataObject& road, int pointInd )
{
	return (((long) road.getPoint31XTile(pointInd)) << 31) + (long) road.getPoint31YTile(pointInd);
}

void RouteResultPreparation::checkAndInitRouteRegion( RoutingContext *ctx, RouteDataObject& road )
{
	//BinaryMapIndexReader reader = ctx.reverseMap.get(road.region);
	//if(reader != null) {
	//	reader.initRouteRegion(road.region);
	//}
}

void RouteResultPreparation::addTurnInfo( bool leftside, std::vector<RouteSegmentResult>& result )
{
	int prevSegment = -1;
	float dist = 0;
	int next = 1;
	for (int i = 0; i <= result.size(); i = next) {
		TurnType t ;
		next = i + 1;
		if (i < result.size()) {
			t = getTurnInfo(result, i, leftside);
			// justify turn
			if(!t.isNull() && i < result.size() - 1) {
				bool tl = TurnType::TL == (t.getValue());
				bool tr = TurnType::TR == (t.getValue());
				if(tl || tr) {
					TurnType tnext = getTurnInfo(result, i + 1, leftside);
					if(!tnext.isNull() && result.at(i).getDistance() < 35) {
						if(tl && TurnType::TL == (tnext.getValue()) ) {
							next = i + 2;
							t = TurnType::valueOf(TurnType::TU, false);
						} else if(tr && TurnType::TR == (tnext.getValue()) ) {
							next = i + 2;
							t = TurnType::valueOf(TurnType::TU, true);
						}
					}
				}
			}
			result.at(i).setTurnType(t);
		}
		if (!t.isNull() || i == result.size()) {
			if (prevSegment >= 0) {
				string turn = result.at(prevSegment).getTurnType().toString();
				if (!result.at(prevSegment).getTurnType().getLanes().empty()) {

					//turn += Arrays.toString(result.at(prevSegment).getTurnType().getLanes());
				}
				result.at(prevSegment).setDescription(turn + StringUtils::format(" and go %.2f meters", dist));
				if(result.at(prevSegment).getTurnType().isSkipToSpeak()) {
					result.at(prevSegment).setDescription("-*"+result.at(prevSegment).getDescription());
				}
			}
			prevSegment = i;
			dist = 0;
		}
		if (i < result.size()) {
			dist += result.at(i).getDistance();
		}
	}
}

TurnType RouteResultPreparation::getTurnInfo( std::vector<RouteSegmentResult>& result, int i, bool leftSide )
{
	if (i == 0) {
		return TurnType::valueOf(TurnType::C, false);
	}
	RouteSegmentResult& prev = result.at(i - 1) ;
	if(prev.getObject()->roundabout()) {
		// already analyzed!
		return TurnType();
	}
	RouteSegmentResult& rr = result.at(i);
	if (rr.getObject()->roundabout()) {
		assert(0);
		//return processRoundaboutTurn(result, i, leftSide, prev, rr);
	}
	TurnType t ;
	if (prev.getObject().get()!=NULL) {
		bool noAttachedRoads = rr.getAttachedRoutes(rr.getStartPointIndex()).size() == 0;
		// add description about turn
		double mpi = MapUtils::degreesDiff(prev.getBearingEnd(), rr.getBearingBegin());
		if(noAttachedRoads){
			// TODO VICTOR : look at the comment inside direction route
			//				double begin = rr.getObject()->directionRoute(rr.getStartPointIndex(), rr.getStartPointIndex() < 
			//						rr.getEndPointIndex(), 25);
			//				mpi = MapUtils::degreesDiff(prev.getBearingEnd(), begin);
		}
		if (mpi >= TURN_DEGREE_MIN) {
			if (mpi < 60) {
				t = TurnType::valueOf(TurnType::TSLL, leftSide);
			} else if (mpi < 120) {
				t = TurnType::valueOf(TurnType::TL, leftSide);
			} else if (mpi < 135 || leftSide) {
				t = TurnType::valueOf(TurnType::TSHL, leftSide);
			} else {
				t = TurnType::valueOf(TurnType::TU, leftSide);
			}
		} else if (mpi < -TURN_DEGREE_MIN) {
			if (mpi > -60) {
				t = TurnType::valueOf(TurnType::TSLR, leftSide);
			} else if (mpi > -120) {
				t = TurnType::valueOf(TurnType::TR, leftSide);
			} else if (mpi > -135 || !leftSide) {
				t = TurnType::valueOf(TurnType::TSHR, leftSide);
			} else {
				t = TurnType::valueOf(TurnType::TU, leftSide);
			}
		} else {
			//assert(0);
			t = attachKeepLeftInfoAndLanes(leftSide, prev, rr, t);
		}
		if (!t.isNull()) {
			t.setTurnAngle((float) -mpi);
		}
	}
	return t;
}

TurnType RouteResultPreparation::attachKeepLeftInfoAndLanes( bool leftSide, RouteSegmentResult& prev, RouteSegmentResult& rr, TurnType& t )
{
	// keep left/right
	vector<int> lanes;
	bool kl = false;
	bool kr = false;
	vector<RouteSegmentResult>& attachedRoutes = rr.getAttachedRoutes(rr.getStartPointIndex());
	int ls = prev.getObject()->getLanes();
	if(ls >= 0 && prev.getObject()->getOneway() == 0) {
		ls = (ls + 1) / 2;
	}
	int left = 0;
	int right = 0;
	bool speak = false;
	int speakPriority = MAX_SPEAK_PRIORITY;
	//int speakPriority = MathUtils::max(highwaySpeakPriority(prev.getObject()->getHighway()), highwaySpeakPriority(rr.getObject()->getHighway()));
	if (!attachedRoutes.empty()) {
		for (size_t ii=0;ii< attachedRoutes.size();ii++) {
			RouteSegmentResult& rs = attachedRoutes[ii];
			double ex = MapUtils::degreesDiff(rs.getBearingBegin(), rr.getBearingBegin());
			double mpi = MathUtils::abs(MapUtils::degreesDiff(prev.getBearingEnd(), rs.getBearingBegin()));
			int rsSpeakPriority = MAX_SPEAK_PRIORITY;
			//int rsSpeakPriority = highwaySpeakPriority(rs.getObject()->getHighway());
			if (rsSpeakPriority != MAX_SPEAK_PRIORITY || speakPriority == MAX_SPEAK_PRIORITY) {
				if ((ex < TURN_DEGREE_MIN || mpi < TURN_DEGREE_MIN) && ex >= 0) {
					kl = true;
					int lns = rs.getObject()->getLanes();
					if(rs.getObject()->getOneway() == 0) {
						lns = (lns + 1) / 2;
					}
					if (lns > 0) {
						right += lns;
					}
					speak = speak || rsSpeakPriority <= speakPriority;
				} else if ((ex > -TURN_DEGREE_MIN || mpi < TURN_DEGREE_MIN) && ex <= 0) {
					kr = true;
					int lns = rs.getObject()->getLanes();
					if(rs.getObject()->getOneway() == 0) {
						lns = (lns + 1) / 2;
					}
					if (lns > 0) {
						left += lns;
					}
					speak = speak || rsSpeakPriority <= speakPriority;
				}
			}
		}
	}
	if(kr && left == 0) {
		left = 1;
	} else if(kl && right == 0) {
		right = 1;
	}
	int current = rr.getObject()->getLanes();
	if(rr.getObject()->getOneway() == 0) {
		current = (current + 1) / 2;
	}
	if (current <= 0) {
		current = 1;
	}
	if(ls >= 0 /*&& current + left + right >= ls*/){
		lanes.resize(current + left + right);
		ls = current + left + right;
		for(int it=0; it< ls; it++) {
			if(it < left || it >= left + current) {
				lanes[it] = 0;
			} else {
				lanes[it] = 1;
			}
		}
		// sometimes links are 
		if ((current <= left + right) && (left > 1 || right > 1)) {
			speak = true;
		}
	}

	if (kl) {
		t = TurnType::valueOf(TurnType::KL, leftSide);
		t.setSkipToSpeak(!speak);
	} else if(kr){
		t = TurnType::valueOf(TurnType::KR, leftSide);
		t.setSkipToSpeak(!speak);
	}
	if (!t.isNull() && !lanes.empty()) {
		t.setLanes(lanes);
	}
	return t;
}