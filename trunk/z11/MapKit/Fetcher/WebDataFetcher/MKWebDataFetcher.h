/*
 * =====================================================================================
 *
 *       Filename:  MKWebDataFetcher.h
 *
 *    Description:  MKWebDataFetcher
 *
 *        Version:  1.0
 *        Created:  04/13/2012 09:32:00 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#pragma  once
#include "MapKit/DataTypes/Base/MKGeometry.h"
#include "MapKit/DataTypes/Base/MKTypes.h"
#include <string>
namespace z11
{

	class MGImage;
	class MKWebDataFetcher
	{
	public:
		static MKWebDataFetcher *getInstance();
		MGImage *getImage(const MKGlobalTileIndex &index,MKMapType map_type);
		std::string getURL(const MKGlobalTileIndex &index,MKMapType map_type);
		std::string getLocalURL(const MKGlobalTileIndex &index,MKMapType map_type);
		std::string getDownloadPath(const MKGlobalTileIndex &index,MKMapType map_type);
		std::string getLocalDownloadPath(const MKGlobalTileIndex &index,MKMapType map_type);
		std::string getBundleCachePath(const MKGlobalTileIndex &index,MKMapType map_type);
		std::string getAvailableCachePath(const MKGlobalTileIndex &index,MKMapType map_type);
		std::string getProxyHost();
		int getProxyPort();
		void setMapProvider(MKMapProvider provider);
        void setCustomMapUrl(const std::string &url);
        void setCustomMapLocalPath(const std::string &url);
		void setProxy(const std::string &host,const int &port);
		MKMapProvider mapProvider();
	private:
        std::string m_custom_web_path;
        std::string m_custom_url;
		MKMapProvider m_provider;
		std::string getPath(const MKGlobalTileIndex &index,MKMapType map_type,const std::string &origin_path);
		MGImage *getImage(int x,int y,int z,MKMapType map_type);
		std::string getAvailableCachePath(int x,int y,int z,MKMapType map_type);
		std::string proxy_host;
		int proxy_port;
	};
}
