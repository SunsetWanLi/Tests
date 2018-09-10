#include "binaryRead.h"
#include "renderRules.h"
#include "rendering.h"
#include "ResultMatcher.h"
#include "Amenity.h"
#include <SkImageEncoder.h>
#include <SkGraphics.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include "osmand_main.h"
#include "SearchRequest.h"
#include "../Utils/MathUtils.h"
void println(const char * msg) {
	printf("%s\n", msg);
}

void printUsage(std::string info) {
	if(info.size() > 0) {
		println(info.c_str());
	}
	println("Inspector is console utility for working with binary indexes of OsmAnd.");
	println("It allows print info about file, extract parts and merge indexes.");
	println("\nUsage for print info : inspector [-renderingOutputFile=..] [-vaddress] [-vmap] [-vpoi] [-vtransport] [-zoom=Zoom] [-bbox=LeftLon,TopLat,RightLon,BottomLan] [file]");
	println("  Prints information about [file] binary index of OsmAnd.");
	println("  -v.. more verbouse output (like all cities and their streets or all map objects with tags/values and coordinates)");
	println("  -renderingOutputFile= renders for specified zoom, bbox into a file");
}


class VerboseInfo {
public:
	bool vaddress;
	bool vtransport;
	bool vpoi;
	bool vmap;
	double lattop, latbottom, lonleft, lonright;
	int zoom;

	VerboseInfo(int argc, char params[][256]) {
		lattop = 85;
		latbottom = -85;
		lonleft = -180;
		lonright = 180;
		zoom = 15;
		int z;
		double l1, l2, l3, l4;
		for (int i = 1; i != argc; ++i) {
			if (strcmp(params[i], "-vaddress") == 0) {
				vaddress = true;
			} else if (strcmp(params[i], "-vmap") == 0) {
				vmap = true;
			} else if (strcmp(params[i], "-vpoi") == 0) {
				vpoi = true;
			} else if (strcmp(params[i], "-vtransport") == 0) {
				vtransport = true;
			} else if (sscanf(params[i], "-zoom=%d", &z)) {
				zoom = z;
			} else if (sscanf(params[i], "-bbox=%le,%le,%le,%le", &l1, &l2, &l3, &l4)) {
				lonleft = l1;
				lattop = l2;
				lonright = l3;
				latbottom = l4;

			}
		}
	}
};

//		public boolean contains(MapObject o){
//			return lattop >= o.getLocation().getLatitude() && latbottom <= o.getLocation().getLatitude()
//					&& lonleft <= o.getLocation().getLongitude() && lonright >= o.getLocation().getLongitude();
//
//		}


const char* formatBounds(int left, int right, int top, int bottom){
	float l = get31LongitudeX(left);
	float r = get31LongitudeX(right);
	float t = get31LatitudeY(top);
	float b = get31LatitudeY(bottom);
	char* ch = new char[150];
	sprintf(ch, "(left top - right bottom) : %g, %g NE - %g, %g NE", l, t,r, b);
	return ch;
}


void printFileInformation(const char* fileName, VerboseInfo* verbose) {
	BinaryMapFile* file = initBinaryMapFile(fileName);
	std::vector<BinaryPartIndex*>::iterator its = file->indexes.begin();
	time_t date = file->dateCreated/1000;
	printf("Obf file.\n Version %d, basemap %d, date %s \n", file->version,
			file->basemap, ctime(&date));

	int i = 1;
	for (; its != file->indexes.end(); its++, i++) {
		BinaryPartIndex* it = *its;
		std::string partname = "";
		if (it->type == MAP_INDEX) {
			partname = "Map";
		} else if (it->type == TRANSPORT_INDEX) {
			partname = "Transport";
		} else if (it->type == ROUTING_INDEX) {
			partname = "Routing";
		} else if (it->type == POI_INDEX) {
			partname = "Poi";
		} else if (it->type == ADDRESS_INDEX) {
			partname = "Address";
		}
		printf("%d. %s data %s - %d bytes\n", i, partname.c_str(), it->name.c_str(), it->length);
		if (it->type == MAP_INDEX) {
			MapIndex* m = ((MapIndex*) it);
			int j = 1;
			std::vector<MapRoot>::iterator rt = m->levels.begin();
			for (; rt != m->levels.end(); rt++) {
				const char* ch = formatBounds(rt->left, rt->right, rt->top, rt->bottom);
				printf("\t%d.%d Map level minZoom = %d, maxZoom = %d, size = %d bytes \n\t\t Bounds %s \n",
						i, j++, rt->minZoom, rt->maxZoom, rt->length, ch);
			}
			if ((verbose != NULL && verbose->vmap)) {
				//printMapDetailInfo(verbose, index);
			}
		} else if (it->type == TRANSPORT_INDEX) {
//			TransportIndex ti = ((TransportIndex) p);
//			int sh = (31 - BinaryMapIndexReader.TRANSPORT_STOP_ZOOM);
//			println(
//					"\t Bounds "
//							+ formatBounds(ti.getLeft() << sh, ti.getRight() << sh, ti.getTop() << sh,
//									ti.getBottom() << sh));
		} else if (it->type == POI_INDEX && (verbose != NULL && verbose->vpoi)) {
			//printPOIDetailInfo(verbose, index, (PoiRegion) p);
		} else if (it->type == ADDRESS_INDEX && (verbose != NULL && verbose->vaddress)) {
//			printAddressDetailedInfo(verbose, index);
		}

	}
}


void runSimpleRendering( string renderingFileName, string resourceDir, RenderingInfo* info) {
	SkColor defaultMapColor = SK_ColorLTGRAY;

	if (info->width > 10000 || info->height > 10000) {
		osmand_log_print(LOG_ERROR, "We don't rendering images more than 10000x10000 ");
		return;
	}

	osmand_log_print(LOG_INFO, "Rendering info bounds(%d, %d, %d, %d) zoom(%d), width/height(%d/%d) tilewidth/tileheight(%d/%d) fileName(%s)",
			info->left, info->top, info->right, info->bottom, info->zoom, info->width, info->height, info->tileWX, info->tileHY, info->tileFileName.c_str());
	RenderingRulesStorage* st = new RenderingRulesStorage(renderingFileName.c_str());
	st->parseRulesFromXmlInputStream(renderingFileName.c_str(), NULL);
	RenderingRuleSearchRequest* searchRequest = new RenderingRuleSearchRequest(st);
	ResultPublisher* publisher = new ResultPublisher();
	SearchQuery q(floor((float)info->left), floor((float)info->right), ceil((float)info->top), ceil((float)info->bottom), searchRequest, publisher);
	q.zoom = info->zoom;

	ResultPublisher* res = searchObjectsForRendering(&q, true,0, "Nothing found");
	osmand_log_print(LOG_INFO, "Found %d objects", res->result.size());

	SkBitmap* bitmap = new SkBitmap();
	bitmap->setConfig(SkBitmap::kRGB_565_Config, info->width, info->height);

	size_t bitmapDataSize = bitmap->getSize();
	void* bitmapData = malloc(bitmapDataSize);
	bitmap->setPixels(bitmapData);

	osmand_log_print(LOG_INFO, "Initializing rendering style and rendering context");
	ElapsedTimer initObjects;
	initObjects.start();

	RenderingContext rc;
	rc.setDefaultIconsDir(resourceDir);
	searchRequest->clearState();
	searchRequest->setIntFilter(st->PROPS.R_MINZOOM, info->zoom);
	if (searchRequest->searchRenderingAttribute(A_DEFAULT_COLOR)) {
		defaultMapColor = searchRequest->getIntPropertyValue(searchRequest->props()->R_ATTR_COLOR_VALUE);
	}
	searchRequest->clearState();
	searchRequest->setIntFilter(st->PROPS.R_MINZOOM, info->zoom);
	if (searchRequest->searchRenderingAttribute(A_SHADOW_RENDERING)) {
		rc.setShadowRenderingMode(searchRequest->getIntPropertyValue(searchRequest->props()->R_ATTR_INT_VALUE));
		//rc.setShadowRenderingColor(searchRequest->getIntPropertyValue(searchRequest->props()->R_SHADOW_COLOR));
	}
	rc.setLocation(
			((double)info->left)/getPowZoom(31-info->zoom),
			((double)info->top)/getPowZoom(31-info->zoom)
			);
	rc.setDimension(info->width, info->height);
	rc.setZoom(info->zoom);
	rc.setRotate(0);
	rc.setDensityScale(1);
	osmand_log_print(LOG_INFO, "Rendering image");
	initObjects.pause();
	SkCanvas* canvas = new SkCanvas(*bitmap);
	canvas->drawColor(defaultMapColor);
	doRendering(res->result, canvas, searchRequest, &rc);
	osmand_log_print(LOG_INFO, "End Rendering image");
	osmand_log_print(LOG_INFO, "Native ok (init %d, rendering %d) ", initObjects.getElapsedTime(),
			rc.nativeOperations.getElapsedTime());
	SkImageEncoder* enc = SkImageEncoder::Create(SkImageEncoder::kPNG_Type);
	if (enc != NULL && !enc->encodeFile(info->tileFileName.c_str(), *bitmap, 100)) {
		osmand_log_print(LOG_ERROR, "FAIL to save tile to %s", info->tileFileName.c_str());
	} else {
		osmand_log_print(LOG_INFO, "Tile successfully saved to %s", info->tileFileName.c_str());
	}
	delete enc;
	delete publisher;
	delete searchRequest;
	delete st;
	delete canvas;
	delete bitmap;
	free(bitmapData);
	return;
}



void testRenderingRuleStorage(const char* basePath, const char* name) {
	string filePath = string(basePath) + string(name);
	RenderingRulesStorage* st = new RenderingRulesStorage(filePath.c_str());
	st->parseRulesFromXmlInputStream(filePath.c_str(),
			new BasePathRenderingRulesStorageResolver(string(basePath)));
	st->printDebug(RenderingRulesStorage::TEXT_RULES);
	RenderingRuleSearchRequest* searchRequest = new RenderingRuleSearchRequest(st);
	searchRequest->setStringFilter(st->PROPS.R_TAG, "highway");
	searchRequest->setStringFilter(st->PROPS.R_VALUE, "motorway");
	searchRequest->setIntFilter(st->PROPS.R_LAYER, 1);
	searchRequest->setIntFilter(st->PROPS.R_MINZOOM, 15);
	searchRequest->setIntFilter(st->PROPS.R_MAXZOOM, 15);
	//	searchRequest.setBooleanFilter(storage.PROPS.R_NIGHT_MODE, true);
	// searchRequest.setBooleanFilter(storage.PROPS.get("hmRendered"), true);

	bool res = searchRequest->search(RenderingRulesStorage::LINE_RULES, true);
	printf("Result %d\n", res);
	searchRequest->printDebugResult();
}
extern vector<Amenity> searchAmenities(double lat,double lon,ResultMatcher<Amenity> *matcher);
#include "binaryRoutePlanner.h"
#include "../router/RouteResultPreparation.h"
vector<RouteSegmentResult>  runRouteTest(int startx,int starty,int endx,int endy)
{
	searchAmenities(0,0,NULL);
	int coordinates[12]=
	{
		startx,//get31TileNumberX( 120.03926771640778 ),
		starty,//get31TileNumberY( 30.276131954368033 ),
		endx,//get31TileNumberX( 120.18414991855622 ),
		endy,//get31TileNumberY( 30.0968854964861 ),
		1790552105,
		884122446,
		1790034466,
		883796243,
		1790565076,
		884190209,
		1791788793,
		883071940
	};
	int data[]={0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4};
	string kl[]={"highway$motorway", "avoid_motorway:highway$motorway", "highway$motorway_link", "avoid_motorway:highway$motorway_link", "highway$trunk", "highway$trunk_link", "highway$primary", "highway$primary_link", "highway$secondary", "highway$secondary_link", "highway$tertiary", "highway$tertiary_link", "highway$unclassified", "short_way:highway$unclassified", "highway$road", "short_way:highway$road", "highway$residential", "short_way:highway$residential", "highway$service", "short_way:highway$service", "highway$track", "short_way:highway$track", "highway$living_street", "short_way:highway$living_street", "route$ferry", "avoid_ferries:route$ferry", "highway$motorway", "highway$motorway_link", "highway$trunk", "highway$trunk_link", "highway$primary", "highway$primary_link", "highway$secondary", "highway$secondary_link", "highway$tertiary", "highway$tertiary_link", "highway$unclassified", "short_way:highway$unclassified", "highway$road", "short_way:highway$road", "highway$residential", "short_way:highway$residential", "highway$service", "short_way:highway$service", "highway$track", "short_way:highway$track", "avoid_unpaved:highway$track", "highway$living_street", "short_way:highway$living_street", "route$ferry", "tracktype$grade5", "access$private", "motor_vehicle$private", "toll$yes", "access$no", "motorcycle$no", "motorcar$no", "motor_vehicle$no", "vehicle$no", "traffic_calming$", "highway$traffic_signals", "railway$crossing", "railway$level_crossing", "motorcar$no", "barrier$lift_gate", "barrier$gate", "barrier$bollard", "traffic_calming$", "highway$traffic_signals", "railway$crossing", "railway$level_crossing", "motorcar$no", "barrier$lift_gate", "barrier$gate", "barrier$bollard"};
	string vl[]={"110.0", "0.0", "80.0", "0.0", "100.0", "75.0", "65.0", "50.0", "60.0", "50.0", "45.0", "40.0", "45.0", "45.0", "45.0", "45.0", "45.0", "45.0", "45.0", "45.0", "45.0", "45.0", "35.0", "35.0", "15.0", "0.0", "1.2", "1.2", "1.2", "1.2", "1.05", "1.05", "1.0", "1.0", "0.95", "0.95", "1.0", "1.0", "1.0", "1.0", "1.0", "1.0", "1.0", "1.0", "0.7", "0.7", "0.1", "1.0", "1.0", "1.0", "0.6", "0.3", "0.3", "1.0", "0.0", "0.0", "0.0", "0.0", "0.0", "8.0", "25.0", "25.0", "25.0", "-1.0", "25.0", "25.0", "25.0", "10.0", "5.0", "25.0", "25.0", "-1.0", "1000.0", "1000.0", "1000.0"};

	float initDirection=-360;
	vector<ROUTE_TRIPLE> cfg;
	size_t size=sizeof(data)/sizeof(data[0]);
	for (int i=0;i<size;i++)
	{
		cfg.push_back(ROUTE_TRIPLE(data[i],std::pair<string, string>(kl[i],vl[i])));
	}
	
	RoutingConfiguration config(cfg, initDirection);
	RoutingContext c(config);
	c.startX = coordinates[0];
	c.startY = coordinates[1];
	c.endX = coordinates[2];
	c.endY = coordinates[3];
	//c.progress= SHARED_PTR<RouteCalculationProgress>(new RouteCalculationProgress());
	ElapsedTimer prepareResultTimer;
	prepareResultTimer.start();
	vector<RouteSegmentResult> r = searchRouteInternal(&c, false);
	RouteResultPreparation().prepareResult(&c,false,r);
	prepareResultTimer.pause();
	osmand_log_print(LOG_WARN, "[Native] search route total %d",prepareResultTimer.getElapsedTime());
	return r;

	c.startX = coordinates[4];
	c.startY = coordinates[5];
	c.endX = coordinates[6];
	c.endY = coordinates[7];
	//vector<RouteSegmentResult> r2 = searchRouteInternal(&c, false);
	c.startX = coordinates[8];
	c.startY = coordinates[9];
	c.endX = coordinates[10];
	c.endY = coordinates[11];

	//vector<RouteSegmentResult> r3 = searchRouteInternal(&c, false);
}
int osmand_main(int argc,char argv[][256]) {
	if (argc <= 1) {
		// 1. Test Rendering rule storage
//		testRenderingRuleStorage("/home/victor/projects/OsmAnd/git/DataExtractionOSM/src/net/osmand/render/",
//				"test_depends.render.xml"
//				"default.render.xml"
//				);
		// 2. Test simple rendering
		printUsage("");
		return 1;
	}
	const char* f = argv[1];
	if (f[0] == '-') {
		// command
		if (f[1]=='v') {
			if (argc < 2) {
				printUsage("Missing file parameter");
			} else {
				VerboseInfo* vinfo = new VerboseInfo(argc, argv);
				printFileInformation(argv[argc -1], vinfo);
			}
		} else if (f[1]=='r') {
			if (argc < 2) {
				printUsage("Missing file parameter");
			} else {
				RenderingInfo* info = new RenderingInfo(argc, argv);
				char s[100];
				for (int i = 1; i != argc; ++i) {
					if (sscanf(argv[i], "-renderingInputFile=%s", s)) {
						BinaryMapFile* mf = initBinaryMapFile(s);
						osmand_log_print(LOG_INFO, "Init %d (success) binary map file %s.", mf->version,
								mf->inputName.c_str());
					}
				}
				//runRouteTest();
				runSimpleRendering(info->renderingFileName, info->imagesFileName, info);
				for (int i = 1; i != argc; ++i) {
					if (sscanf(argv[i], "-renderingInputFile=%s", s)) {
						closeBinaryMapFile(s);
					}
				}
				delete info;
			}
		} 
		else if(f[1]=='s')
		{
			//runRouteTest();
		}else {
			printUsage("Unknown command");
		}
	} 
	else {
		printFileInformation(f, NULL);
	}

	SkGraphics::PurgeFontCache();
	purgeCachedBitmaps();
}

RenderingInfo::RenderingInfo( int argc, char params[][256] )
{
	double l1, l2;
	char s[100];
	int z, z1, z2 ;
	lattop = 85;
	tileHY = 2;
	lonleft = -180;
	tileWX = 2;
	zoom = 15;
	for (int i = 1; i != argc; ++i) {
		if (sscanf(params[i], "-renderingOutputFile=%s", s)) {
			tileFileName = s;
		} else if (sscanf(params[i], "-imagesBasePathFile=%s", s)) {
			imagesFileName = s;
		} else if (sscanf(params[i], "-renderingStyleFile=%s", s)) {
			renderingFileName = s;
		} else if (sscanf(params[i], "-zoom=%d", &z)) {
			zoom = z;
		} else if (sscanf(params[i], "-lbox=%lg,%lg", &l1, &l2)) {
			lonleft = l1;
			lattop = l2;
		} else if (sscanf(params[i], "-lt=%d,%d", &z1, &z2)) {
			tileWX = z1;
			tileHY = z2;
		}
	}

	left = get31TileNumberX(lonleft);
	top = get31TileNumberY(lattop);
	right = left + tileWX * (1 << (31 - zoom));
	bottom = top + tileHY * (1 << (31 - zoom));
	width = tileWX * TILE_SIZE;
	height = tileHY * TILE_SIZE;
}

extern vector<Amenity> searchPoi(SearchRequest<Amenity> &req);

vector<Amenity> searchAmenities(double lat, double lon, double topLatitude,
	double bottomLatitude, double leftLongitude, double rightLongitude, ResultMatcher<Amenity> * matcher)
{
	SearchRequest<Amenity> req ;
	req.left=leftLongitude;
	req.right=rightLongitude;
	req.top=topLatitude;
	req.bottom=bottomLatitude;
	return searchPoi(req);
}
vector<Amenity> searchAmenities(double lat,double lon,ResultMatcher<Amenity> *matcher)
{
	double baseDistY = getDistance(lat,lon,lat-1,lon);
	double baseDistX = getDistance(lat,lon,lat,lon-1);
	double distance = 1 * 1000;//1 kilometers

	double topLatitude = MathUtils::min(lat + (distance/ baseDistY ), 84.);
	double bottomLatitude = MathUtils::max(lat - (distance/ baseDistY ), -84.);
	double leftLongitude = MathUtils::max(lon - (distance / baseDistX), -180);
	double rightLongitude = MathUtils::min(lon + (distance/ baseDistX), 180);

	return searchAmenities(lat,lon,topLatitude,bottomLatitude,leftLongitude,rightLongitude,matcher);
}
vector<Amenity> initializeNewSearch(double lat, double lon ,int firstTimeLimit,ResultMatcher<Amenity> *matcher)
{
	return searchAmenities(lat,lon,matcher);
}
