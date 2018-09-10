
#include "GPS_FromRoute.h"



//#include "../GPS/GPS_If.h" 

namespace IS_GIS
{
int GPS_FromRoute::open(const PathResult &route, const SimParam &sim_param)
{
	SimuNaviTool::transPath2GPSData(route, sim_param, &m_gps_infos);
	m_cursor = 0; 
	return 0;
}
}
