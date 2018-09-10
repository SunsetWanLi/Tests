#include "GPSDataRecord.h"
#include "../../Retiguide/Common/Types/basic_types.h"
#include "../../System/IO/MGBundle.h"
#include "GPS_Info.h"
#include "../../System/Shared/MGTime.h"
#include "../../System/Shared/Utility.h"

#include <sstream>

using namespace GIS_Embedded;
using namespace z11;

namespace IS_GIS
{
// 	ofstream GPSDataRecord::data_out = ofstream();
	void GPSDataRecord::record(const GPS_Info & gps_info)
	{
// 		if(gps_info.latitude != 0
// 			|| gps_info.longitude != 0
// 			|| gps_info.speed != 0
// 			|| gps_info.altitude_sea_level != 0
// 			|| gps_info.altitude_84 != 0)
// 		{
// 			if(!data_out.is_open())
// 			{
// 				//by lw
// 				z11::MGTime cur_time = z11::MGTime::Now();
// 				z11::MGTime::Exploded tmp_exploded;
// 				cur_time.UTCExplode(&tmp_exploded);
//
// 				string flog_path = z11::MGBundle::mainBundle()->pathWithFileName("GIS_Embedded/");
// 				flog_path += "gps_data_";
// 				flog_path += z11::Utility::BuiltInTypeToString(tmp_exploded.year);
// 				flog_path += "_";
// 				flog_path += z11::Utility::BuiltInTypeToString(tmp_exploded.month);
// 				flog_path += "_";
// 				flog_path += z11::Utility::BuiltInTypeToString(tmp_exploded.day_of_month);
// 				flog_path += "_";
// 				flog_path += z11::Utility::BuiltInTypeToString(tmp_exploded.hour);
// 				flog_path += "_";
// 				flog_path += z11::Utility::BuiltInTypeToString(tmp_exploded.minute);
// 				flog_path += "_";
// 				flog_path += z11::Utility::BuiltInTypeToString(tmp_exploded.second);
// 				flog_path += ".txt";
// 				data_out.open(flog_path.c_str(), std::ios_base::app | std::ios_base::out);
// 			}
//
// 			if(data_out.is_open())
// 			{
// 					ostringstream oss;
// 					oss.precision(12);
// 					oss << gps_info.utc_date.yy << " " << gps_info.utc_date.mm << " " << gps_info.utc_date.dd << " "
// 						<< gps_info.utc_time.hh << " " << gps_info.utc_time.mm << " " << gps_info.utc_time.ss << " "
// 						<< gps_info.latitude << " " << gps_info.longitude << " "
// 						<< gps_info.speed << " " << gps_info.heading << " "
// 						<< gps_info.altitude_sea_level <<" " << gps_info.altitude_84;
//
// 					data_out << oss.str() << endl;
// 					data_out.flush();
// 			}
// 		}
	}
}
