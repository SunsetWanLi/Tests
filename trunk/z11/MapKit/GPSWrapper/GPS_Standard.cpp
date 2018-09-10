#include "GPS_Standard.h"
#include <string>
#include "../../System/IO/MGBundle.h"
#include <assert.h>

namespace IS_GIS
{

//  HINSTANCE GPS_Standard::s_gpsInst=NULL;

DLL_OPEN GPS_Standard::s_dll_open = NULL;
DLL_READ_POS GPS_Standard::s_read_pos = NULL;
DLL_READ_SA GPS_Standard::s_read_sa = NULL;
DLL_CLOSE GPS_Standard::s_dll_close = NULL;

GPS_Standard::GPS_Standard()
{
//  	if(s_gpsInst==NULL)
//  	{
//  		string dllRoot = z11::MGBundle::mainBundle()->pathWithFileName("GIS_Embedded\\Include\\GPS_DEVICE.dll");
//  		s_gpsInst = LoadLibraryA(dllRoot.c_str());
//
//  		assert(s_gpsInst != NULL);
//
//  		s_dll_open = (DLL_OPEN)GetProcAddress(s_gpsInst,L"spec_open");
//  		s_read_pos = (DLL_READ_POS)GetProcAddress(s_gpsInst,L"read_pos");
//  		s_read_sa = (DLL_READ_SA)GetProcAddress(s_gpsInst,L"read_sa");
//  		s_dll_close = (DLL_CLOSE)GetProcAddress(s_gpsInst,L"close_gps");
//  	}
}

GPS_Standard::~GPS_Standard()
{
}

int GPS_Standard::open(const wchar_t *com_port, unsigned int baud)
{
	if(s_dll_open == NULL)
		return -1;
	unsigned int port = com_port[wcslen(com_port) - 2] - L'0';
	if(port > 12 || port ==0)
		port = 1; //waiting for lww to modify the open para
	s_dll_open(port,baud);
	return 0;
}

int GPS_Standard::read(GPS_Info *result)
{
	if(s_read_pos == NULL)
		return -1;
	return s_read_pos(result);
}
int GPS_Standard::read( Satellites_Info *result)
{
	if(s_read_sa == NULL)
		return -1;
	return s_read_sa(result);//m_gps_device->read(result);
}
int GPS_Standard::close()
{
	if(s_dll_close == NULL)
		return -1;

	return s_dll_close();//m_gps_device->close();
}
}
