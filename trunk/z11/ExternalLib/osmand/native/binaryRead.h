#ifndef _OSMAND_BINARY_READ_H
#define _OSMAND_BINARY_READ_H

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <fstream>
#include <map>
#include <string>
#include <stdint.h>
#include "mapObjects.h"
#include "multipolygons.h"
#include "common.h"


#include "mapObjects.h"
#include "renderRules.h"

static const int MAP_VERSION = 2;
static const int BASEMAP_ZOOM = 11;


struct MapTreeBounds {
	uint32_t length;
	uint32_t filePointer;
	uint32_t mapDataBlock;
	uint32_t left ;
	uint32_t right ;
	uint32_t top ;
	uint32_t bottom;
	bool ocean;

	MapTreeBounds() {
		ocean = -1;
	}
};
struct RoutingIndex;
struct RouteSubregion {
	uint32_t length;
	uint32_t filePointer;
	uint32_t mapDataBlock;
	uint32_t left;
	uint32_t right;
	uint32_t top;
	uint32_t bottom;
	std::vector<RouteSubregion> subregions;
	RoutingIndex* routingIndex;

	RouteSubregion(RoutingIndex* ind) : length(0), filePointer(0), mapDataBlock(0), routingIndex(ind){
	}
};


struct MapRoot: MapTreeBounds {
	int minZoom ;
	int maxZoom ;
	std::vector<MapTreeBounds> bounds;
};

enum PART_INDEXES {
	MAP_INDEX = 1,
	POI_INDEX,
	ADDRESS_INDEX,
	TRANSPORT_INDEX,
	ROUTING_INDEX,
};

struct BinaryPartIndex {
	uint32_t length;
	int filePointer;
	PART_INDEXES type;
	std::string name;

	BinaryPartIndex(PART_INDEXES tp) : type(tp) {}
};

struct RouteTypeRule
{
	enum
	{
		NONE=0,
		ACCESS = 1,
		ONEWAY = 2,
		HIGHWAY_TYPE =3,
		MAXSPEED =4,
		ROUNDABOUT = 5,
		TRAFFIC_SIGNALS = 6,
		RAILWAY_CROSSING = 7,
		LANES =8
	};
	std::string t;
	std::string v;
	int intValue;
	float floatValue;
	int type;
	bool roundabout() const{
		return type == ROUNDABOUT;
	}

	int getType() const {
		return type;
	}
	int lanes() const{
		if(type == LANES){
			return intValue;
		}
		return -1;
	}
	int onewayDirection() const{
		if(type == ONEWAY){
			return intValue;
		}
		return 0;
	}
	float maxSpeed() const; 
	RouteTypeRule(const std::string & t, const std::string & v);
	RouteTypeRule()
	{
		type=NONE;
	}
	bool isNull()
	{
		return type == NONE;
	}
	void analyze();
};
struct RoutingIndex : BinaryPartIndex {
//	UNORDERED(map)< uint32_t, tag_value > decodingRules;
	vector< tag_value > decodingRules;
	vector<SHARED_PTR< RouteTypeRule> > decodingRulesCache;
	std::vector<RouteSubregion> subregions;
	std::vector<RouteSubregion> basesubregions;
	RoutingIndex() : BinaryPartIndex(ROUTING_INDEX) {
	}

	void initRouteEncodingRule(uint32_t id, std::string tag, std::string val) {
		tag_value pair = tag_value(tag, val);
		// DEFINE hash
		//encodingRules[pair] = id;
		SHARED_PTR< RouteTypeRule> type=SHARED_PTR< RouteTypeRule>(new RouteTypeRule(pair.first,pair.second));
		decodingRulesMap[pair.first+"$"+pair.second] = type;
		while(decodingRules.size() < id + 1){
			decodingRules.push_back(pair);
			decodingRulesCache.push_back(type);
		}
		decodingRules[id] = pair;
		decodingRulesCache[id]=type;
	}
	RouteTypeRule& quickGetEncodingRule(int id)
	{
		return *decodingRulesCache[id];
		//return RouteTypeRule(pair.first,pair.second);
	}

	RouteTypeRule& quickGetEncodingRule(const std::string& key)
	{
		std::map<std::string,SHARED_PTR< RouteTypeRule>>::iterator it=decodingRulesMap.find(key);
		if (it!=decodingRulesMap.end())
		{
			return *it->second;
		}
		static RouteTypeRule empty_rule;
		return empty_rule;
	}
	std::map<std::string,SHARED_PTR< RouteTypeRule>> decodingRulesMap;
};

struct RouteDataObject {
	RoutingIndex* region;
	std::vector<uint32_t> types ;
	std::vector<uint32_t> pointsX ;
	std::vector<uint32_t> pointsY ;
	std::vector<uint64_t> restrictions ;
	std::vector<std::vector<uint32_t> > pointTypes;
	int64_t id;

	UNORDERED(map)<int, std::string > names;
	vector<pair<uint32_t, uint32_t> > namesIds;

	string getName() {
		if(names.size() > 0) {
			return names.begin()->second;
		}
		return "";
	}

	int getSize() {
		int s = sizeof(this);
		s += pointsX.capacity()*sizeof(uint32_t);
		s += pointsY.capacity()*sizeof(uint32_t);
		s += types.capacity()*sizeof(uint32_t);
		s += restrictions.capacity()*sizeof(uint64_t);
		std::vector<std::vector<uint32_t> >::iterator t = pointTypes.begin();
		for(;t!=pointTypes.end(); t++) {
			s+= (*t).capacity() * sizeof(uint32_t);
		}
		s += namesIds.capacity()*sizeof(pair<uint32_t, uint32_t>);
		s += names.size()*sizeof(pair<int, string>)*10;
		return s;
	}

	double directionRoute(int startPoint, bool plus){
		// look at comment JAVA
		return directionRoute(startPoint, plus, 5);
	}

	// Gives route direction of EAST degrees from NORTH ]-PI, PI]
	double directionRoute(int startPoint, bool plus, float dist) {
		int x = pointsX[startPoint];
		int y = pointsY[startPoint];
		int nx = startPoint;
		int px = x;
		int py = y;
		double total = 0;
		do {
			if (plus) {
				nx++;
				if (nx >= pointsX.size()) {
					break;
				}
			} else {
				nx--;
				if (nx < 0) {
					break;
				}
			}
			px = pointsX[nx];
			py = pointsY[nx];
			// translate into meters
			total += abs(px - x) * 0.011 + abs(py - y) * 0.01863;
		} while (total < dist);
		return -atan2( (float)x - px, (float) y - py );
	}

	int getPoint31XTile(int i)  const{
		return pointsX[i];
	}

	int getPoint31YTile(int i)  const{
		return pointsY[i];
	}

	bool roundabout() const;
	int getPointsLength() const;
	int64_t getId() const;
	int getLanes() const;
	int getOneway() const;
	float getMaximumSpeed() const;
};



struct MapIndex : BinaryPartIndex {

	std::vector<MapRoot> levels;

	UNORDERED(map)<int, tag_value > decodingRules;
	// DEFINE hash
	//UNORDERED(map)<tag_value, int> encodingRules;

	int nameEncodingType;
	int refEncodingType;
	int coastlineEncodingType;
	int coastlineBrokenEncodingType;
	int landEncodingType;
	int onewayAttribute ;
	int onewayReverseAttribute ;
	UNORDERED(set)< int > positiveLayers;
	UNORDERED(set)< int > negativeLayers;

	MapIndex() : BinaryPartIndex(MAP_INDEX) {
		nameEncodingType = refEncodingType = coastlineBrokenEncodingType = coastlineEncodingType = -1;
		landEncodingType = onewayAttribute = onewayReverseAttribute = -1;
	}

	void finishInitializingTags() {
		int free = decodingRules.size() * 2 + 1;
		coastlineBrokenEncodingType = free++;
		initMapEncodingRule(0, coastlineBrokenEncodingType, "natural", "coastline_broken");
		if (landEncodingType == -1) {
			landEncodingType = free++;
			initMapEncodingRule(0, landEncodingType, "natural", "land");
		}
	}

	void initMapEncodingRule(uint32_t type, uint32_t id, std::string tag, std::string val) {
		tag_value pair = tag_value(tag, val);
		// DEFINE hash
		//encodingRules[pair] = id;
		decodingRules[id] = pair;

		if ("name" == tag) {
			nameEncodingType = id;
		} else if ("natural" == tag && "coastline" == val) {
			coastlineEncodingType = id;
		} else if ("natural" == tag && "land" == val) {
			landEncodingType = id;
		} else if ("oneway" == tag && "yes" == val) {
			onewayAttribute = id;
		} else if ("oneway" == tag && "-1" == val) {
			onewayReverseAttribute = id;
		} else if ("ref" == tag) {
			refEncodingType = id;
		} else if ("layer" == tag) {
			if (val != "" && val != "0") {
				if (val[0] == '-') {
					negativeLayers.insert(id);
				} else {
					positiveLayers.insert(id);
				}
			}
		}
	}
};

typedef std::string AmenityType;
class PoiRegion :public BinaryPartIndex
{
public:
	PoiRegion() : BinaryPartIndex(POI_INDEX) {
	}
	std::vector<std::string> categories ;
	std::vector<AmenityType> categoriesType ;
	std::vector< std::vector<std::string> >subcategories ;
	double getLeftLongitude() {
		return leftLongitude;
	}

	double getRightLongitude() {
		return rightLongitude;
	}

	double getTopLatitude() {
		return topLatitude;
	}

	double getBottomLatitude() {
		return bottomLatitude;
	}
public :
	double leftLongitude;
	double rightLongitude;
	double topLatitude;
	double bottomLatitude;
};

struct BinaryMapFile {
	std::string inputName;
	uint32_t version;
	uint64_t dateCreated;
	std::vector<MapIndex> mapIndexes;
	std::vector<RoutingIndex*> routingIndexes;
	std::vector<PoiRegion *>poiIndexes;
	std::vector<BinaryPartIndex*> indexes;
	int fd;
	int routefd;
	bool basemap;

	bool isBasemap(){
		return basemap;
	}

	~BinaryMapFile() {
		close(fd);
		close(routefd);
	}
};

struct ResultPublisher {
	std::vector< MapDataObject*> result;

	bool publish(MapDataObject* r) {
		result.push_back(r);
		return true;
	}
	bool publish(std::vector<MapDataObject*> r) {
		result.insert(result.begin(), r.begin(), r.end());
		return true;
	}
	bool isCancelled() {
		return false;
	}
	virtual ~ResultPublisher() {
		deleteObjects(result);
	}
};

struct SearchQuery {
	RenderingRuleSearchRequest* req;
	int left;
	int right;
	int top;
	int bottom;
	int zoom;
	ResultPublisher* publisher;

	coordinates cacheCoordinates;
	bool ocean;
	bool mixed;

	int numberOfVisitedObjects;
	int numberOfAcceptedObjects;
	int numberOfReadSubtrees;
	int numberOfAcceptedSubtrees;

	SearchQuery(int l, int r, int t, int b, RenderingRuleSearchRequest* req, ResultPublisher* publisher) :
			req(req), left(l), right(r), top(t), bottom(b),publisher(publisher) {
		numberOfAcceptedObjects = numberOfVisitedObjects = 0;
		numberOfAcceptedSubtrees = numberOfReadSubtrees = 0;
		ocean = mixed = false;
	}
	SearchQuery(int l, int r, int t, int b) :
				left(l), right(r), top(t), bottom(b) {
	}

	SearchQuery(){

	}

	bool publish(MapDataObject* obj) {
		return publisher->publish(obj);
	}
};

void searchRouteSubregions(SearchQuery* q, std::vector<RouteSubregion>& tempResult);

void searchRouteDataForSubRegion(SearchQuery* q, std::vector<RouteDataObject*>& list, RouteSubregion* sub);

ResultPublisher* searchObjectsForRendering(SearchQuery* q, bool skipDuplicates, int renderRouteDataFile, std::string msgNothingFound);

BinaryMapFile* initBinaryMapFile(std::string inputName);

bool initMapFilesFromCache(std::string inputName) ;

bool closeBinaryMapFile(std::string inputName);

#endif
