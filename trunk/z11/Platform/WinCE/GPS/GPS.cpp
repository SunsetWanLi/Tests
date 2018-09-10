//#include "stdafx.h"
#ifdef WINCE
#include "gps.h"
#include "CommReadThread.h"
#include "uploadGPSThread.h"
#include <fstream>
#include <cassert>
using namespace std;
namespace z11
{
	GPS* GPS::m_instance;
GPS::GPS()
{
	m_is_start = false;
	m_hdl = NULL;
	m_read_thread = NULL;
	sa_info=Satellites_Info();
}
GPS* GPS::getInstance()
{
	if(m_instance ==NULL)
	{
		return m_instance = new GPS();
	}
	return m_instance;
}

void GPS::releaseInstance()
{
	if (m_instance)
	{
		delete m_instance;
		m_instance = NULL;
	}
}

GPS::~GPS()
{
	//close();
	stopGPS();
	if(m_instance)
	{
		delete m_instance;
		m_instance = NULL;
	}
	if(m_upload_thread)
	{
		delete m_upload_thread;
		m_upload_thread = NULL;
	}
}
bool GPS::isStart()
{
	return m_is_start;
}
int GPS::stopGPS()
{
// 	if(m_read_thread)
// 	{
// 		//m_read_thread->markStop();
// 	}

	if(m_hdl)
		CloseHandle(m_hdl);
	m_hdl = NULL;

	if(m_read_thread)
	{
		m_read_thread->requestExit();
		Sleep(3000);
		delete m_read_thread;
		m_read_thread = NULL;
	}
	if(m_upload_thread)
	{
		m_upload_thread->requestExit();
		Sleep(3000);
		delete m_upload_thread;
		m_upload_thread = NULL;
	}

	return 0;
}
int GPS::startGPS(const std::wstring& conf_path)
{
	if(m_is_start)
	{
		return 0;
	}
	wstring comm_port = L"";
	wstring baundrate = L"";
	parseFile(conf_path,comm_port,baundrate);
	int int_baundrate = _wtoi(baundrate.c_str());
	wchar_t comm[20];
	memset(comm,0,20*sizeof(wchar_t));
	wsprintf(comm,L"%s",comm_port.c_str());

	if(open(comm,int_baundrate) !=0)
	{
		m_find_gps.findPortWithBaudrate();
		m_find_gps.writeFile(conf_path);
		open(m_find_gps.m_comm,m_find_gps.m_baudrate);
//		if(open(m_find_gps.m_comm,m_find_gps.m_baudrate)==0)
// 		{
// 			m_is_start = true;
// 		}
// 		else
// 		{
// 			return -1;
// 		}
	}
	else
	{
		m_is_start = true;
	}
	return 0;

}

int GPS::open(const wchar_t* com_port,unsigned int baud)
{
	if( m_hdl != NULL )
		::CloseHandle( m_hdl );
	//com_port = L"COM6:";

	// 打开COM端口
	m_hdl = ::CreateFile(com_port, GENERIC_READ|GENERIC_WRITE, 
			0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL , 
			NULL );
	if( m_hdl == INVALID_HANDLE_VALUE )
	{
		m_hdl = NULL;
		DWORD err_code = GetLastError();
		return err_code;
	}

	// 设置COM数值
	::SetCommMask(m_hdl, EV_RXCHAR );//| EV_ERR | EV_RXFLAG); 
	::SetupComm(m_hdl, 4096, 4096); 
	::PurgeComm(m_hdl, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR); 

	COMMTIMEOUTS CommTimeOuts; 
	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF; 
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0; 
	CommTimeOuts.ReadTotalTimeoutConstant = 1000; 
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0; 
	CommTimeOuts.WriteTotalTimeoutConstant = 1000; 
	::SetCommTimeouts(m_hdl, &CommTimeOuts); 

	DCB dcb; 
	dcb.DCBlength = sizeof(DCB); 
	
	::GetCommState(m_hdl, &dcb); 
	dcb.BaudRate = baud;
	dcb.fBinary = TRUE; 
	dcb.fParity = FALSE; 	
	dcb.fOutxDsrFlow = FALSE; 
	dcb.fDtrControl = DTR_CONTROL_ENABLE; 
	dcb.fOutxCtsFlow = FALSE; 
	dcb.fRtsControl = RTS_CONTROL_ENABLE; 
	dcb.fInX = dcb.fOutX = FALSE; 
	dcb.StopBits = ONESTOPBIT; 
	dcb.ByteSize = 8;//bitsum; // 8
	dcb.Parity = NOPARITY; 
	BOOL fRetVal = SetCommState(m_hdl, &dcb); 
	if( fRetVal == FALSE )
	{
		DWORD err_code = GetLastError();
		::CloseHandle( m_hdl );
		m_hdl = NULL;
		return (int)err_code;
	}

	if(m_read_thread)
	{
		assert(0);
	}
	m_read_thread = new ComReadThread();
	m_read_thread->setCOMPort(m_hdl);
	m_read_thread->setWriteTarget(this);
	m_read_thread->StartWAAS_EGNOS(1);
	m_is_start = true;
	m_read_thread->run();

	m_upload_thread = new uploadGPSThread();
	m_upload_thread->run();
	return 0;
}

int GPS::read(GPS_Info *result)
{
	lock();
	*result = m_info;
	unlock();
	return 0;
}
int GPS::read( Satellites_Info *result)
{
	lock();
	*result=sa_info;
	unlock();
	return 0;
}

void GPS::write(const GPS_Info &val)
{
	lock();
	m_info = val;
	unlock();
}

void GPS::write(const Satellites_Info& val)
{
	lock();
	sa_info=val;
	unlock();
}
bool GPS::parseFile(const std::wstring& conf_path,wstring & comm_port,wstring & baundrate)
{
	wifstream conf_if;
	conf_if.open(conf_path.c_str(),ios_base::in|ios_base::_Nocreate);
	if(!conf_if.is_open())
	{
		return false;
	}
	unsigned int size = 1024;
	wchar_t buff[1024];
	while(!conf_if.eof())
	{
		memset(buff,0,size);
		conf_if.getline(buff,size);
		buff[size -1] = 0;
		wstring old_buf = clearSpaceCharacter(buff);

		wstring::size_type equal_pos = old_buf.find_first_of(L"=");
		if(equal_pos == wstring::npos)
		{
			continue;//return NULL;
		}
		else
		{
			wstring first_part = clearSpaceCharacter(old_buf.substr(0,equal_pos));
			//wstring last_part = clearSpaceCharacter(old_buf.substr(equal_pos + 1));
			if(first_part.compare(L"COMM PORT") == 0)
			{
				comm_port = clearSpaceCharacter(old_buf.substr(equal_pos + 1));
			}
			else if(first_part.compare(L"COMM BAUDRATE") == 0)
			{
				baundrate = clearSpaceCharacter(old_buf.substr(equal_pos + 1));
			}
			else
			{
				continue;
			}
		}
		
	}
	return true;
}
// wstring GPS::readcont(const wstring& old_buf)
// {
// 	wstring::size_type equal_pos = old_buf.find_first_of(L"=");
// 	if(equal_pos == wstring::npos)
// 	{
// 		return NULL;
// 	}
// 	else
// 	{
// 		wstring first_part = clearSpaceCharacter(old_buf.substr(0,equal_pos));
// 		wstring last_part = clearSpaceCharacter(old_buf.substr(equal_pos + 1));
// 	}
// }
wstring GPS::clearSpaceCharacter(const wstring &old)
{
	//assert(buf);
	//buf[size-1] = 0;
	uint_32 size = old.size() + 1;
	wchar_t *temp = new wchar_t[size];
	memset(temp,0,size * sizeof(wchar_t));
	size_t start = 0, end = old.size();

	while(start < end) //清除行前
	{
		if(!iswspace(old[start]))//|| buf[i] == L'\0')
		{
			break;
		}
		start ++;
	}

	while(start < end) //清除行尾
	{
		if(!iswspace(old[end-1]))// || buf[i] == L'\0')
		{
			break;
		}
		end --;
	}

	if(end > start)
		memcpy(temp, &old[0] + start, (end - start) * sizeof(wchar_t));

	wstring ret = temp;
	//memcpy(buf, temp,size * sizeof(wchar_t));
	delete[] temp;
	return ret;
}

}
#endif