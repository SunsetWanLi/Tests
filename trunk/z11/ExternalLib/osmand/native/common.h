#ifndef _OSMAND_COMMON_H
#define _OSMAND_COMMON_H

#include <string>
#include <vector>
#if defined(ANDROID)
#	include <unordered_map>
#	include <unordered_set>
#elif defined(__APPLE__)
#	include <tr1/unordered_map>
#	include <tr1/unordered_set>
#elif defined(_WIN32) || defined(_WIN64)
#include <sparsehash/internal/sparseconfig.h>
#include <sparsehash_config.h>
#include <sparsehash/dense_hash_set>
#include <sparsehash/dense_hash_map>
namespace google
{
	template<typename K>
	class google_hash_set : public dense_hash_set<K> {
	public:
		google_hash_set() {
			this->set_empty_key(-1);
		}
	};
	template<typename K, typename V>
	class google_hash_map : public dense_hash_map<K,V> {
	public:
		google_hash_map() {
			this->set_empty_key(-1);
			this->set_deleted_key(-2);
		}
	};
	struct stringKeyHash
	{
		size_t operator()(const std::string& str) const
		{
			unsigned int seed = 131; // the magic number, 31, 131, 1313, 13131, etc.. orz..

			unsigned int hash = 0;
			for (size_t i=0;i<str.size();i++)
			{
				hash = hash*seed + (str[i]);
			}
			return hash;
			//return hash % NHASH;
		}
	};
	struct stringKeyEq
	{
		size_t operator()(const std::string& p1, const std::string& p2) const
		{
			return p1 == p2;
		}
	};
	struct int64KeyHash
	{
		size_t operator()(const long long& i) const
		{
			unsigned int seed = 131; // the magic number, 31, 131, 1313, 13131, etc.. orz..

			
			unsigned int hash = (unsigned int)(i>>32)*seed + ((unsigned int)i);
			return hash;
			//return hash % NHASH;
		}
	};
	struct int64KeyEq
	{
		size_t operator()(const long long& i1, const long long& i2) const
		{
			return i1==i2;
		}
	};
	template<>
	class google_hash_set< long long> : public dense_hash_set< long long,int64KeyHash,int64KeyEq> {
	public:
		google_hash_set() {
			this->set_empty_key(-1);
		}
	};
	template< typename V>
	class google_hash_map<long long,V> : public dense_hash_map<long long,V,int64KeyHash,int64KeyEq> {
	public:
		google_hash_map() {
			this->set_empty_key(-1);
		}
	};
	template< typename V>
	class google_hash_map<std::string,V> : public dense_hash_map<std::string,V,stringKeyHash,stringKeyEq> {
	public:
		google_hash_map() {
			this->set_empty_key("!@#$%");//random
		}
	};
	template<typename K, typename V>
	class google_hash_map<K*, V> : public dense_hash_map<K*,V> {
	public:
		google_hash_map() {
			this->set_empty_key((K*)(~0));
		}
	};
}
#else
#	include <unordered_map>
#	include <unordered_set>
#endif
#include <stdint.h>

#if defined(__APPLE__)
#	include <mach/mach_time.h>|
#endif

#include <SkPath.h>
#include <SkBitmap.h>

#include "osmand_log.h"

// M_PI is no longer part of math.h/cmath by standart, but some GCC's define them
#define _USE_MATH_DEFINES
#include <math.h>
#if !defined(M_PI)
	const double M_PI = 3.14159265358979323846;
#endif
#if !defined(M_PI_2)
	const double M_PI_2 = M_PI / 2.0;
#endif

// Wrapper for unordered classes
#if defined(ANDROID)
#	define UNORDERED_NAMESPACE std::tr1
#	define UNORDERED_map unordered_map
#	define UNORDERED_set unordered_set
#elif defined(__APPLE__)
#	define UNORDERED_NAMESPACE std::tr1
#	define UNORDERED_map unordered_map
#	define UNORDERED_set unordered_set
#elif defined(_WIN32) || defined(_WIN64)
#	define UNORDERED_NAMESPACE google
#	define UNORDERED_map google_hash_map
#	define UNORDERED_set google_hash_set
#else
#	define UNORDERED_NAMESPACE std
#	define UNORDERED_map unordered_map
#	define UNORDERED_set unordered_set
#endif
#define UNORDERED(cls) UNORDERED_NAMESPACE::UNORDERED_##cls

#if defined(ANDROID)
#include "shared_ptr.h"
#define SHARED_PTR my_shared_ptr
//#   include <tr1/shared_ptr.h>
//#   define SHARED_PTR std::tr1::shared_ptr
#elif defined(WINDOWS)
#else
#	include "shared_ptr.h"
#   define SHARED_PTR my_shared_ptr
//#   include <tr1/memory>
//#   define SHARED_PTR std::tr1::shared_ptr
#endif


// Better don't do this
using namespace std;

#ifdef PROFILE_NATIVE_OPERATIONS
	#define PROFILE_NATIVE_OPERATION(rc, op) rc->nativeOperations.pause(); op; rc->nativeOperations.start()
#else
	#define PROFILE_NATIVE_OPERATION(rc, op) op;
#endif

struct RenderingContext;

inline double toRadians(double angdeg) {
	return angdeg / 180 * M_PI;
}

class ElapsedTimer
{
private:
	long elapsedTime;
	bool enableFlag;
	bool run;

#if defined(_WIN32)
	DWORD startInit;
	DWORD endInit;
#elif defined(__APPLE__)
	mach_timebase_info_data_t machTimeInfo;
	uint64_t startInit;
	uint64_t endInit;
#else
	timespec startInit;
	timespec endInit;
#endif
	
public:
	ElapsedTimer();

	void enable();
	void disable();

	void start();
	void pause();

	int getElapsedTime();
};

struct TextDrawInfo {
	TextDrawInfo(std::string);
	~TextDrawInfo();

	std::string text;

	SkRect bounds;
	float centerX;
	float centerY;

	float textSize;
	float minDistance;
	int textColor;
	int textShadow;
	uint32_t textWrap;
	bool bold;
	std::string shieldRes;
	int textOrder;

	bool drawOnPath;
	SkPath* path;
	float pathRotate;
	float vOffset;
	float hOffset;
};

struct IconDrawInfo
{
	IconDrawInfo();

	SkBitmap* bmp;
	float x;
	float y;
};

static const int TILE_SIZE = 256;
struct RenderingContext
{
private :
	// parameters
	bool useEnglishNames;
	float density;

	float leftX;
	float topY;
	int width;
	int height;
	int defaultColor;

	int zoom;
	float rotate;
	// int shadowRenderingMode = 0; // no shadow (minumum CPU)
	// int shadowRenderingMode = 1; // classic shadow (the implementaton in master)
	// int shadowRenderingMode = 2; // blur shadow (most CPU, but still reasonable)
	// int shadowRenderingMode = 3; solid border (CPU use like classic version or even smaller)
	int shadowRenderingMode;
	int shadowRenderingColor;
	string defaultIconsDir;

public:
	// debug purpose
	int pointCount;
	int pointInsideCount;
	int visible;
	int allObjects;
	int lastRenderedKey;
	ElapsedTimer textRendering;
	ElapsedTimer nativeOperations;

// because they used in 3rd party functions
public :

	// calculated
	float tileDivisor;
	float cosRotateTileSize;
	float sinRotateTileSize;

	std::vector<TextDrawInfo*> textToDraw;
	std::vector<IconDrawInfo> iconsToDraw;
	// use to calculate points
	float calcX;
	float calcY;

	// not expect any shadow
	int shadowLevelMin;
	int shadowLevelMax;
	int polygonMinSizeToDisplay;
	int roadDensityZoomTile;
	int roadsDensityLimitPerTile;

public:
	RenderingContext() : shadowLevelMax(0), shadowLevelMin(256), density(true), useEnglishNames(false), pointCount(0),
		pointInsideCount(0), visible(0), allObjects(0), shadowRenderingColor(0xff969696) {
		setRotate(0);
		setZoom(15);
		setDefaultColor(0xfff1eee8);
		roadsDensityLimitPerTile = 0;
		roadDensityZoomTile = 0;
		polygonMinSizeToDisplay = 0;
	}
	virtual ~RenderingContext();

	virtual bool interrupted();
	virtual SkBitmap* getCachedBitmap(const std::string& bitmapResource);
	virtual std::string getTranslatedString(const std::string& src);
	virtual std::string getReshapedString(const std::string& src);

	void setDefaultIconsDir(string path) {
		defaultIconsDir = path;
	}

	void setZoom(int z) {
		this->zoom = z;
		this->tileDivisor = (1 << (31 - z));
	}

	void setTileDivisor(float tileDivisor) {
		this->tileDivisor = tileDivisor;
	}

	void setDefaultColor(int z) {
		this->defaultColor = z;
	}

	void setRotate(float rot) {
		this->rotate = rot;
		this->cosRotateTileSize = cos(toRadians(rot)) * TILE_SIZE;
		this->sinRotateTileSize = sin(toRadians(rot)) * TILE_SIZE;
	}

	void setLocation(double leftX, double topY) {
		this->leftX = leftX;
		this->topY = topY;
	}

	void setDimension(int width, int height) {
		this->width = width;
		this->height = height;
	}

	inline int getShadowRenderingMode(){
		return shadowRenderingMode;
	}

	int getShadowRenderingColor(){
		return shadowRenderingColor;
	}

	void setShadowRenderingColor(int color) {
		shadowRenderingColor = color;
	}

	inline int getWidth(){
		return width;
	}

	inline int getDefaultColor(){
		return defaultColor;
	}

	inline int getHeight(){
		return height;
	}

	inline int getZoom() {
		return zoom;
	}

	inline float getLeft() {
		return leftX;
	}

	inline float getTop() {
		return topY;
	}

	void setShadowRenderingMode(int mode){
		this->shadowRenderingMode = mode;
	}

	void setDensityScale(float val) {
		density = val;
	}

	float getDensityValue(float val) {
		return val * density;
	}

	void setUseEnglishNames(bool b){
		this->useEnglishNames = b;
	}

	bool isUsingEnglishNames(){
		return this->useEnglishNames;
	}

};

SkBitmap* getCachedBitmap(RenderingContext* rc, const std::string& bitmapResource);
void purgeCachedBitmaps();

int get31TileNumberX(double longitude);
int get31TileNumberY( double latitude);

double getPowZoom(float zoom);

double getLongitudeFromTile(float zoom, double x) ;
double getLatitudeFromTile(float zoom, double y);

double get31LongitudeX(int tileX);
double get31LatitudeY(int tileY);
double getTileNumberX(float zoom, double longitude);
double getTileNumberY(float zoom, double latitude);
double getDistance(double lat1, double lon1, double lat2, double lon2);
double getPowZoom(float zoom);

double calculateProjection31TileMetric(int xA, int yA, int xB, int yB, int xC, int yC);
double squareDist31TileMetric(int x1, int y1, int x2, int y2) ;
double convert31YToMeters(int y1, int y2);
double convert31XToMeters(int y1, int y2);
double alignAngleDifference(double diff);


#endif /*_OSMAND_COMMON_H*/
