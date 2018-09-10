#ifndef Z_APPLICATION_GPS_FROM_LOG_H_	
#define Z_APPLICATION_GPS_FROM_LOG_H_	

#include "GPS_Wrapper.h"
#include <fstream>
namespace IS_GIS
{
class GPS_FromLog : public GPS_Wrapper
{
	std::fstream m_ifs;
public:
	GPS_FromLog();
	virtual ~GPS_FromLog();
	// com_port: file name with full path, baud: not used
	virtual int open(const char *com_port, unsigned int baud);
	virtual int read(GPS_Info *result);
	virtual int update(GPS_Info result);

	virtual int close();
	virtual bool isOpen() //lw  2010/10/17
	{
		if(m_ifs.is_open())
			return true;
		return false;
	}
};
}
#endif //Z_APPLICATION_GPS_FROM_LOG_H_	