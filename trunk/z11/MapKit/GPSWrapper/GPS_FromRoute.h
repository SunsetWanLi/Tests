#ifndef Z_APPLICATION_GPS_FROM_ROUTE_H_	
#define Z_APPLICATION_GPS_FROM_ROUTE_H_	

#include "GPS_Wrapper.h"
#include <fstream>
#include "../Embedded/Application/PathResult.h"
#include "../Embedded/Application/SimuNaviTool.h"

using namespace Embedded;
namespace IS_GIS
{
class GPS_FromRoute : public GPS_Wrapper
{
	vector<GPS_Info> m_gps_infos;
	uint_32 m_cursor;
public:
	GPS_FromRoute()
	{}
	virtual ~GPS_FromRoute()
	{}
	virtual int open(const char *com_port, unsigned int baud)
	{
		return 0;
	}
	virtual int open(const PathResult &route, const SimParam &sim_param);
	virtual int restart()
	{
		m_cursor = 0;
		return 0;
	}
	virtual int read(GPS_Info *result)
	{
		if(m_gps_infos.empty())
			return -1;
		*result = m_gps_infos[m_cursor];
		if(m_cursor < m_gps_infos.size() - 1)
			++ m_cursor;
		return 0;
	}
	virtual int close()
	{
		m_gps_infos.clear();
		m_cursor = 0;
		return 0;
	}
};
}
#endif //Z_APPLICATION_GPS_FROM_LOG_H_	