#pragma once
#include "../../../System/Thread/MGThread.h"
#include "GPS_Info.h"
#include "../../../System/LocationService/MGLocation.h"
namespace z11
{

class uploadGPSThread:public MGThread
{
public:
	uploadGPSThread();
private:
	bool is_pre_leagle;
	MGLocation m_previous;
	virtual bool threadLoop();
	bool isNeedUpload(const GPS_Info& read_info);

};
}