#pragma  once
#ifdef WINCE
#include <vector>
#include <Windows.h>
#include <string>
namespace z11
{
#ifndef DEFAULT_BAUD
#define DEFAULT_BAUD 4800
#endif
;
class FindPort
{
public:	
	FindPort();
	~FindPort();
	bool findPortWithBaudrate();
	void writeFile(const std::wstring& conf_path);
	wchar_t m_comm[20];
	int m_baudrate;
private:
	bool writeCOMM(HANDLE in_handle);
	int openPort(const WCHAR* com_port,unsigned int baud,HANDLE & out_handle);
	bool readComm(HANDLE & in_handle);
	HANDLE m_comm_handle;
	std::vector<int> m_enum_boudrate;

};

}
#endif