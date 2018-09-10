#ifndef Z_APPLICATION_GPS_STATE_H_
#define Z_APPLICATION_GPS_STATE_H_

#include <time.h>
namespace IS_GIS
{
struct GPS_State
{
	bool is_valid;
	bool is_device_connected;
public:
	GPS_State() : is_device_connected(false), is_valid(false){}
};
}
#endif //Z_APPLICATION_GPS_STATE_H_