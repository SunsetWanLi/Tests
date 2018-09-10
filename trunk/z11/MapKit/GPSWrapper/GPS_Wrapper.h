#ifndef Z_APPLICATION_GPS_WRAPPER_H_
#define Z_APPLICATION_GPS_WRAPPER_H_

#include "GPS_Info.h"
#include <string>
namespace IS_GIS
{
class GPS_Wrapper
{
public:
	virtual ~GPS_Wrapper(){}
	virtual int open(const char *com_port, unsigned int baud) = 0;
	virtual int read(GPS_Info *result) = 0;
	virtual int read( Satellites_Info *result){return 0;}/*=0;*/
	virtual int update(GPS_Info result) {return 0;}
	virtual int close() = 0;
	virtual bool isOpen(){return false;}//lw  2010/10/17
};
}
#endif //Z_APPLICATION_GPS_WRAPPER_H_