#pragma once
#include "System/Thread/MGMutex.h"
#include "MapKit/DataTypes/Base/MKGeometry.h"
#include "MapKit/DataTypes/Base/MKTypes.h"
#include <string>
namespace z11
{
	class MGImage;
	class MKOsmMapFetcher
	{
	public:
		static MKOsmMapFetcher* getInstance();
		bool loadMapData();
		MGImage *getImage(const MKGlobalTileIndex &index,MKMapType map_type=MKMapTypeOsm);
		std::string getCacheFolderPath( const MKGlobalTileIndex &index,MKMapType type=MKMapTypeOsm );
		bool noPolygons;
	private: 
		bool initMapData(bool noPolygons=false);
		MGMutex fetch_locker;
		MKOsmMapFetcher();
		~MKOsmMapFetcher();
	};



}