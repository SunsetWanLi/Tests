#pragma  once
#include "MapKit/DataTypes/Base/MKGeometry.h"
#include "MapKit/DataTypes/Base/MKTypes.h"
#include <string>
namespace z11
{

	class MGImage;
	class MKLocalDataFetcher
	{
	public:
		static MKLocalDataFetcher *getInstance();
		MGImage *getImage(const MKGlobalTileIndex &index,MKMapType map_type);
		std::string getLocalURL(const MKGlobalTileIndex &index,MKMapType map_type);
		std::string getAvailableCachePath(const MKGlobalTileIndex &index,MKMapType map_type);
        static void setImagePrefix(const std::string &suffix);
        static void setCustomLocalPath(const std::string& path);
	private:
		std::string getPath(const MKGlobalTileIndex &index,MKMapType map_type,const std::string &origin_path);
		MGImage *getImage(int x,int y,int z,MKMapType map_type);
		std::string getAvailableCachePath(int x,int y,int z,MKMapType map_type);
    private:
        std::string m_custom_local_path;
	};
}
