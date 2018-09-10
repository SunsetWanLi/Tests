//#include "stdafx.h"
#include <iostream>
#include "GPS_FromLog.h"
//#include "GPS_If.h"
//#include "../Common/RootDriverPath.h"
//#include "../Common/String_Tools.h"
#include "../../System/IO/MGBundle.h"


using namespace std;
//using namespace GIS_Embedded;
namespace IS_GIS
{
GPS_FromLog::GPS_FromLog()
{
	//if(!m_ifs.is_open())
	//{
	//	string file_name = ROOT_DRIVER_PATH;
	//	file_name += "gps_data.txt";
	//	m_ifs.open(file_name);
	//}
}

GPS_FromLog::~GPS_FromLog()
{
	if(m_ifs.is_open())
	{
		m_ifs.close();
	}
}

int GPS_FromLog::open(const char *com_port, unsigned int baud)
{
	//wstring file_name = Utility::ustr2wstr(ROOT_DRIVER_PATH);
	//file_name += Utility::ustr2wstr(GEO_DATA_PATH);
	string file_name  = z11::MGBundle::mainBundle()->pathWithFileName("GIS_Embedded/");
	//wstring file_name = Utility::ustr2wstr(GEO_DATA_PATH);
	file_name += com_port;
	m_ifs.open(file_name.c_str(), ios_base::in|ios_base::out);
	if(m_ifs.is_open())
	{
		return 0;
	}
	else
		return -1;
}

int GPS_FromLog::read(GPS_Info *result)
{
	// to be continued...
	*result = GPS_Info();
	if(m_ifs.is_open())
	{
		if(m_ifs.eof())
		{
			m_ifs.clear();
			m_ifs.seekg(0);
		}
		//m_ifs >> result->latitude >> result->longitude 
		//	>> result->speed >> result->heading >> result->altitude_84 >> result->altitude_sea_level;
		m_ifs >> result->utc_date.yy >> result->utc_date.mm >>  result->utc_date.dd
					>> result->utc_time.hh >>  result->utc_time.mm >>  result->utc_time.ss
					>> result->latitude >>  result->longitude 
					>> result->speed >>  result->heading 
					>> result->altitude_sea_level >> result->altitude_84;
		return 0;
	}
	return -1;
}
int GPS_FromLog::update(GPS_Info result)
{
	if(m_ifs.is_open())
	{
		m_ifs.clear();
		m_ifs.seekp(0); 

		m_ifs <<result.utc_date.yy<<" " <<result.utc_date.mm <<" "<< result.utc_date.dd
			<<" "<< result.utc_time.hh <<" "<<result.utc_time.mm <<" "<< result.utc_time.ss
			<<" "<< result.latitude <<" "<< result.longitude 
			<<" "<< result.speed <<" "<< result.heading 
			<<" "<< result.altitude_sea_level<<" "<< result.altitude_84;//<<endl;
		return 0;
	}
	return -1;
}
int GPS_FromLog::close()
{
	if(m_ifs.is_open())
	{
		m_ifs.close();
	}
	return 0;
}

}