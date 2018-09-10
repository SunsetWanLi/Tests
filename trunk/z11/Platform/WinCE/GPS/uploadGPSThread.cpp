#include "uploadGPSThread.h"
#include <cassert>
#include "../../../System/LocationService/MGLocationManager.h"
#include "GPS.h"
using namespace std;
namespace z11
{
	//暂定3米以内不上传；
	static const  double distance_geo = 0.0000269796;
	uploadGPSThread::uploadGPSThread()
	{
		is_pre_leagle = false;
	}
	bool uploadGPSThread::threadLoop()
	{
		GPS * gps = GPS::getInstance();
		if(!gps->isStart())
		{
			assert(0);
			return true;
		}
		GPS_Info read_info ;
		gps->read(&read_info);
		if(!is_pre_leagle)
		{
			is_pre_leagle = true;
			MGLocation location;
			location.longitude = read_info.longitude;
			location.latitude = read_info.latitude;
			location.altitude = read_info.altitude_sea_level;
			location.speed = read_info.speed;
			//location.course = read_info.
			MGLocationManager::didUpdateLocation(location,m_previous);
			m_previous = location;
		}
		else
		{
			bool is_need_upload = isNeedUpload(read_info);
			if(is_need_upload)
			{
				MGLocation location;
				location.longitude = read_info.longitude;
				location.latitude = read_info.latitude;
				location.altitude = read_info.altitude_sea_level;
				location.speed = read_info.speed;
				MGLocationManager::didUpdateLocation(location,m_previous);
				m_previous = location;
			}
		}
		return true;
	}
	bool uploadGPSThread::isNeedUpload(const GPS_Info& read_info)
	{
		if((read_info.latitude - m_previous.latitude > distance_geo)||(read_info.longitude -m_previous.longitude >distance_geo))
		{
			return true;
		}
		return false;
	}
}