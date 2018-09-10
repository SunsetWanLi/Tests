#ifndef Z_APPLICATION_GPS_STANDARD_H_
#define Z_APPLICATION_GPS_STANDARD_H_

#include "GPS_Wrapper.h"
namespace IS_GIS
{
//class GPS_If;

typedef int (*DLL_OPEN_AUTO)(Port_Info *info);
typedef int (*DLL_OPEN)(unsigned int port, unsigned int baud);
typedef int (*DLL_READ_POS)( GPS_Info* info);
typedef int (*DLL_READ_SA)(Satellites_Info *info);
typedef int (*DLL_CLOSE)();

class GPS_Standard : public GPS_Wrapper
{
	//GPS_If *m_gps_device;

public:
	GPS_Standard();
	virtual ~GPS_Standard();
	virtual int open(const wchar_t *com_port, unsigned int baud);
	virtual int read(GPS_Info *result);
	virtual int read( Satellites_Info *result);
	virtual int close();

private:
//  	static HINSTANCE  s_gpsInst;
	static DLL_OPEN_AUTO s_dll_open_auto;
	static DLL_OPEN s_dll_open;
	static DLL_READ_POS s_read_pos;
	static DLL_READ_SA s_read_sa;
	static DLL_CLOSE s_dll_close;

};
}
#endif //Z_APPLICATION_GPS_STANDARD_H_