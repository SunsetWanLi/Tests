#ifndef TEST_GPS_H_
#define TEST_GPS_H_
#ifdef WINCE
//#include "GPS_If.h"
//#include "Syncable.h"
//#include "../Common/Types/Syncable.h"
#include "../../../System/Shared/Syncable.h"
#include <windows.h>
#include "findPort.h"
#include "GPS_Info.h"
#include <string>

//using namespace GIS_Embedded;
namespace z11
{
class ComReadThread;
class uploadGPSThread;
class GPS :public Syncable/* public GPS_If, */
{
	static GPS* m_instance;
	HANDLE m_hdl;
	ComReadThread *m_read_thread;
	uploadGPSThread* m_upload_thread;
	GPS_Info m_info;
	Satellites_Info sa_info;
public:
	bool isStart();
	static GPS* getInstance();
	static void releaseInstance();
	virtual int read(GPS_Info *result);
	virtual int read( Satellites_Info *result);

	//0表示成功
	virtual int startGPS(const std::wstring& conf_path);

	virtual int stopGPS();
	virtual int open(const wchar_t*comm,unsigned int baundrate);
	void write(const GPS_Info &val);
	void write(const Satellites_Info &val);
private:
	bool m_is_start;
	FindPort m_find_gps;
	bool parseFile(const std::wstring& conf_path,std::wstring & comm,std::wstring& baundrate);
	std::wstring clearSpaceCharacter(const std::wstring &old);

	GPS();
	~GPS();
	GPS(const GPS&);
	GPS& operator =(const GPS&);

};
}
#endif //WINCE
#endif // TEST_GPS_H_