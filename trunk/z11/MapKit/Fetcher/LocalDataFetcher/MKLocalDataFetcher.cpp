#include "MKLocalDataFetcher.h"
#include <string>
#include <stdio.h>
#include "GraphicCore/MGImage.h"
#include "System/Shared/MGLog.h"
#include "System/IO/MGBundle.h"
#include "System/Shared/Utility.h"
#include "System/Network/URLFetcher/MGURLFetcher.h"
#include "MapKit/Fetcher/Pyramid/MKGlobalMapTiles.h"
#include "Platform/MGPlatformConfig.h"
#include "GraphicCore/MGCanvas.h"
#include "GraphicCore/MGPNGCodec.h"
using namespace std;
namespace z11
{
    static string image_suffix = "jpg";
	string MKLocalDataFetcher::getAvailableCachePath(int x,int y,int z,MKMapType map_type)
	{
		std::string path;
		path=getLocalURL(MKGlobalTileIndex(x,y,z),map_type);
		if (Utility::isFileORDirExist(path))
			return path;

		MGLogD("MKLocalDataFetcher can't find file!!!! %s.", path.c_str());
		return path;
	}
    
    void MKLocalDataFetcher::setImagePrefix(const std::string &suffix)
    {
        image_suffix = suffix;
    }

	std::string MKLocalDataFetcher::getAvailableCachePath( const MKGlobalTileIndex &index,MKMapType map_type )
	{
		return getAvailableCachePath(index.x,index.y,index.z,map_type);
	}

	static MKLocalDataFetcher *_instance=NULL;
	MKLocalDataFetcher * MKLocalDataFetcher::getInstance()
	{
		if (_instance==NULL)
		{
			_instance = new MKLocalDataFetcher();
            _instance->m_custom_local_path = "";
		}
		//static MKLocalDataFetcher _instance;
		return _instance;
	}
    
    void MKLocalDataFetcher::setCustomLocalPath(const std::string &path)
    {
        MKLocalDataFetcher* fetcher = MKLocalDataFetcher::getInstance();
        fetcher->m_custom_local_path = path;
    }

	MGImage * MKLocalDataFetcher::getImage( int x,int y,int z ,MKMapType map_type)
	{
        MGImage* image = NULL;
        string file_full_path = getAvailableCachePath(x,y,z, map_type);
        if(Utility::isFileORDirExist(file_full_path))
        {
            image = MGImage::imageWithContentsOfFile(file_full_path, true);
            if (image == NULL)
            {
                //the image file must be broken
                Utility::delAFile(getAvailableCachePath(x, y, z, map_type));
            }
        }
		return image;
	}

	MGImage * MKLocalDataFetcher::getImage( const MKGlobalTileIndex &index,MKMapType map_type )
	{
		return MKLocalDataFetcher::getImage(index.x,index.y,index.z,map_type);
	}

	std::string MKLocalDataFetcher::getLocalURL( const MKGlobalTileIndex &index,MKMapType map_type )
	{
		char filename[256];
		std::string tmp_path = m_custom_local_path;
        if (m_custom_local_path.empty()) {
            tmp_path=MGBundle::mainBundle()->drawablePathWithFileName("draw_tile_caches/");
        }
        sprintf(filename,"%d/%d/",index.z,index.x);
		tmp_path+=filename;
		if (!Utility::isFileORDirExist(tmp_path))
			Utility::createDirOnPath(tmp_path);
		sprintf(filename, "%d.%s", index.y, image_suffix.c_str());
		tmp_path+=filename;
		return tmp_path;
	}
}
