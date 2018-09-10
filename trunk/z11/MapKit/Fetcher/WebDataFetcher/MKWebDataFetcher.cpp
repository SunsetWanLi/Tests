/*
 * =====================================================================================
 *
 *       Filename:  MKWebDataFetcher.cpp
 *
 *    Description:  MKWebDataFetcher
 *
 *        Version:  1.0
 *        Created:  04/13/2012 09:30:08 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */

#include "MKWebDataFetcher.h"
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
	string MKWebDataFetcher::getAvailableCachePath(int x,int y,int z,MKMapType map_type)
	{
		std::string path;
		//std::string path=getBundleCachePath(MKGlobalTileIndex(x,y,z),map_type);
		//if (Utility::isFileORDirExist(path))
		//	return path;

		//path=getLocalDownloadPath(MKGlobalTileIndex(x,y,z),map_type);
		//if (Utility::isFileORDirExist(path))
		//	return path;

		path=getDownloadPath(MKGlobalTileIndex(x,y,z),map_type);
		if (Utility::isFileORDirExist(path))
			return path;


		MGLogD("MKWebDataFetcher can't find file!!!!");
		return path;
	}

	std::string MKWebDataFetcher::getAvailableCachePath( const MKGlobalTileIndex &index,MKMapType map_type )
	{
		return getAvailableCachePath(index.x,index.y,index.z,map_type);
	}

	static MKWebDataFetcher *_instance=NULL;
	MKWebDataFetcher * MKWebDataFetcher::getInstance()
	{
		if (_instance==NULL)
		{
			_instance = new MKWebDataFetcher();
            _instance->m_custom_web_path = "";
            _instance->m_custom_url = "";
			_instance->proxy_host = "";
			_instance->proxy_port = -1;			_instance->setMapProvider(MKMapProviderGoogle);
		}
		//static MKWebDataFetcher _instance;
		return _instance;
	}

	MGImage * MKWebDataFetcher::getImage( int x,int y,int z ,MKMapType map_type)
	{
		MGImage * image= MGImage::imageWithContentsOfFile(getAvailableCachePath(x,y,z, map_type), true);
		if (image==NULL)
		{
			//the image file must be broken
			Utility::delAFile(getAvailableCachePath(x,y,z, map_type));
		}
		return image;
	}

	MGImage * MKWebDataFetcher::getImage( const MKGlobalTileIndex &index,MKMapType map_type )
	{
		return MKWebDataFetcher::getImage(index.x,index.y,index.z,map_type);
	}
    void MKWebDataFetcher::setCustomMapUrl(const std::string &url)
    {
        m_custom_url = url;
    }
    void MKWebDataFetcher::setCustomMapLocalPath(const std::string &url)
    {
        m_custom_web_path = url;
    }
	std::string MKWebDataFetcher::getURL( const MKGlobalTileIndex &index,MKMapType map_type )
	{
		char url[1024]={0};
        string ret;
		GlobalMercator mercator;
		MGIntPoint g= mercator.GoogleTile(index.x, index.y, index.z);
		static int server_id=0;
		switch(m_provider)
		{
		case MKMapProviderGoogle:
			switch (map_type)
			{
                case MKMapTypeStandard:
                    sprintf(url,"http://mt%d.google.cn/vt/lyrs=m@177000000&hl=zh-CN&gl=cn&src=app&x=%d&y=%d&z=%d&s=Galileo",(server_id++%4),g.x,g.y,index.z);
                    break;
				case MKMapTypeSatellite:
					sprintf(url,"http://mt%d.google.cn/vt/lyrs=s@177000000&hl=zh-CN&gl=cn&src=app&x=%d&y=%d&z=%d&s=Ga",(server_id++%4),g.x,g.y,index.z);
					break;
                case MKMapTypeHybrid:
                    sprintf(url,"http://mt%d.google.cn/vt/imgtp=png32&lyrs=h@177000000&hl=zh-CN&gl=cn&src=app&x=%d&y=%d&z=%d&s=Galile",(server_id++%4),g.x,g.y,index.z);
                    break;
                case MKMapTypeTopographic:
                    sprintf(url,"http://mt%d.google.cn/vt/lyrs=t@132,r@255000000&hl=zh-CN&gl=CN&src=app&x=%d&y=%d&z=%d&s=Gal",(server_id++%4),g.x,g.y,index.z);
                    break;
                default:
                    break;
			}
			break;
		case MKMapProviderBing:
			switch (map_type)
			{
                case MKMapTypeStandard:
                    sprintf(url,"http://r%d.tiles.ditu.live.com/tiles/r%s.png?g=94&mkt=zh-cn",(server_id++%4),mercator.QuadTree(index.x,index.y,index.z).c_str());
                    break;
                case MKMapTypeSatellite:
                    sprintf(url,"http://ecn.t%d.tiles.virtualearth.net/tiles/a%s.jpeg?g=587&mkt=en-gb&n=z",(server_id++%4),mercator.QuadTree(index.x,index.y,index.z).c_str());
                    break;
                //case MKMapTypeHybrid:
				//	sprintf(url,"http://mt%d.google.cn/vt/imgtp=png32&lyrs=h@177000000&hl=zh-CN&gl=cn&src=app&x=%d&y=%d&z=%d&s=Galile",(server_id++%4),g.x,g.y,index.z);
                    //break;
                    //sprintf(url,"http://ecn.t%d.tiles.virtualearth.net/tiles/a%s.jpeg?g=587&mkt=en-gb&n=z",(server_id++%4),mercator.QuadTree(index.x,index.y,index.z).c_str());
                default:
                    break;
			}
			break;
		case MKMapProviderTiandi:
			switch (map_type)
			{
			case z11::MKMapTypeTiandiVec:
				sprintf(url, "http://t0.tianditu.com/DataServer?T=vec_w&x=%d&y=%d&l=%d",g.x,g.y,index.z);
				//sprintf(url, "http://t0.tianditu.com/vec_w/wmts&x=%d&y=%d&l=%d",g.x,g.y,index.z);
				break;
			case z11::MKMapTypeTiandiImg:
				sprintf(url, "http://t0.tianditu.com/DataServer?T=img_w&x=%d&y=%d&l=%d",g.x,g.y,index.z);
				break;
			case z11::MKMapTypeTiandiTer:
				sprintf(url, "http://t0.tianditu.com/DataServer?T=ter_w&x=%d&y=%d&l=%d",g.x,g.y,index.z);
				break;
			default:
				break;
			}
			break;
        case MKMapProviderCustom:
            switch (map_type)
            {
			case MKMapTypeCustomTile:
				sprintf(url,"%d/%d/%d.jpg",index.z,index.x,index.y);
				break;
			case MKMapTypeCustomWebTile:
				sprintf(url,"%d/%d/%d.jpg",index.z,index.x,g.y);
				break;
			case MKMapTypeYDJWTile:
					sprintf(url,"%d/%d/%d.png",index.z,g.x,g.y);
					break;
			default:
				break;
            }
            break;
        default:
            break;
		}
        if(m_provider==MKMapProviderCustom){
            ret = m_custom_url+url;
            return ret;
        }
		return url;
	}

	std::string MKWebDataFetcher::getDownloadPath( const MKGlobalTileIndex &index,MKMapType map_type )
	{
		return getPath(index,map_type,MGBundle::mainBundle()->storageCardPathWithFileName(""));
	}

	std::string MKWebDataFetcher::getBundleCachePath( const MKGlobalTileIndex &index,MKMapType map_type )
	{
		return getPath(index,map_type,MGBundle::mainBundle()->pathWithFileName(""));
	}

	std::string MKWebDataFetcher::getPath( const MKGlobalTileIndex &index,MKMapType map_type,const std::string &_origin_path )
	{

		string path;
		char filename[256];
		GlobalMercator mercator;
		MGIntPoint g = mercator.GoogleTile(index);
		//g.x = index.x;
		//g.y = index.y;
		std::string origin_path=_origin_path;
		if (m_provider==MKMapProviderBing)
		{
			origin_path+="bing/";
		}else if(m_provider==MKMapProviderTiandi)
		{
			origin_path+="tiandi/";
		}
		switch (map_type)
		{
		case MKMapTypeTiandiVec:
			path=origin_path+("vec/");
			break;
		case MKMapTypeTiandiImg:
			path=origin_path+("img/");
			break;
		case MKMapTypeTiandiTer:
			path=origin_path+("ter/");
			break;
		case MKMapTypeStandard:
			path=origin_path+("standard/");
			break;		
		case MKMapTypeSatellite:
			path=origin_path+("satellite/");
			break;
		case MKMapTypeHybrid:
			path=origin_path+("hybrid/");
			break;		
        case MKMapTypeTopographic:
            path=origin_path+("topographic/");
            break;
        case MKMapTypeCustomTile:
            path=origin_path+("customTile/");
            if(!m_custom_web_path.empty()){
                 path += m_custom_web_path+"/";
            }
            break;
        case MKMapTypeCustomWebTile:
            path=origin_path+("customWebTile/");
            if(!m_custom_web_path.empty()){
                path += m_custom_web_path+"/";
            }
            break;
		case MKMapTypeYDJWTile:
			path=origin_path+("YdjwTile/");
			if(!m_custom_web_path.empty()){
				path += m_custom_web_path+"/";
			}
			break;
		//case MKMapTypeTiandiImg:
		//	path=origin_path+("img/");
		//	break;
		//case MKMapTypeTiandiTer:
		//	path=origin_path+("ter/");
		//	break;
		//case MKMapTypeTiandiVec:
		//	path=origin_path+("vec/");
		//	break;
        default:
            break;
		}
		sprintf(filename,"%d/",index.z);
		path+=filename;
		sprintf(filename,"%d/",index.x);
		path+=filename;
		if (!Utility::isFileORDirExist(path))
			Utility::createDirOnPath(path);
		//因为资源文件搞错了y的坐标，只能使用g.y了 //
		switch (map_type)
		{
		case MKMapTypeStandard:
			sprintf(filename,"%d.png",index.y);
			break;
		case MKMapTypeSatellite:
			sprintf(filename,"%d.jpg",index.y);
			break;
		case MKMapTypeHybrid:
			sprintf(filename,"%d.png",index.y);
			break;
        case MKMapTypeTopographic:
            sprintf(filename,"%d.jpg",index.y);
            break;
        case MKMapTypeCustomTile:
            sprintf(filename,"%d.jpg",index.y);
            break;
        case MKMapTypeCustomWebTile:
            sprintf(filename,"%d.jpg",g.y);
            break;
		case MKMapTypeTiandiVec:
			sprintf(filename,"%d.jpg",g.y);
			break;
		case MKMapTypeTiandiImg:
			sprintf(filename,"%d.jpg",g.y);
			break;
		case MKMapTypeTiandiTer:
			sprintf(filename,"%d.jpg",g.y);
			break;
		case MKMapTypeYDJWTile:
			sprintf(filename,"%d.png",g.y);
			break;
        default:
            break;
		}
		path+=filename;
        //MGLogD("MKWebDataFetcher::getPath ==== %s",path.c_str());
		return path;
	}

	std::string MKWebDataFetcher::getLocalURL( const MKGlobalTileIndex &index,MKMapType map_type )
	{
		char url[1024];
		GlobalMercator mercator;
		MGIntPoint g= mercator.GoogleTile(index.x, index.y, index.z);
		switch (map_type)
		{
		case MKMapTypeStandard:
			sprintf(url,"ftp://172.18.1.118/upload/standard/%d/%d/%d.png",index.z,g.x,g.y);
			break;
		case MKMapTypeSatellite:
			sprintf(url,"ftp://172.18.1.118/upload/satellite/%d/%d/%d.jpg",index.z,g.x,g.y);
			break;
		case MKMapTypeHybrid:
			sprintf(url,"ftp://172.18.1.118/upload/hybrid/%d/%d/%d.png",index.z,g.x,g.y);
			break;
        default:
                
            break;
		}
		return url;
	}

	std::string MKWebDataFetcher::getLocalDownloadPath( const MKGlobalTileIndex &index,MKMapType map_type )
	{
		string path=MGBundle::mainBundle()->storageCardPathWithFileName("");
		//string path=MGBundle::mainBundle()->storageCardPathWithFileName("local_caches/");
		if (!Utility::isFileORDirExist(path))
			Utility::createDirOnPath(path);
		return getPath(index,map_type,path);

	}

	void MKWebDataFetcher::setMapProvider( MKMapProvider provider )
	{
		m_provider=provider;
	}

	z11::MKMapProvider MKWebDataFetcher::mapProvider()
	{
		return m_provider;
	}

	void MKWebDataFetcher::setProxy(const std::string &host,const int &port)
	{
		proxy_host = host;
		proxy_port = port;
	}

	std::string MKWebDataFetcher::getProxyHost()
	{
		return proxy_host;
	}

	int MKWebDataFetcher::getProxyPort()
	{
		return proxy_port;
	}}
