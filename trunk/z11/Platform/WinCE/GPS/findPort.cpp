#ifdef WINCE
#include <iostream>
#include <fstream>
using namespace std;
#include "findPort.h"
#include <assert.h>

using namespace z11;
FindPort::FindPort()
{
	m_baudrate = 0;
	m_comm_handle = 0;
	/*m_comm = 0;*/
	memset(m_comm,0,20*sizeof(wchar_t));
	m_enum_boudrate.push_back(4800);
	m_enum_boudrate.push_back(9600);
	m_enum_boudrate.push_back(14400);
	m_enum_boudrate.push_back(19200);
	m_enum_boudrate.push_back(38400);
	m_enum_boudrate.push_back(57600);
	m_enum_boudrate.push_back(115200);
}
FindPort::~FindPort()
{
	if(m_comm_handle)
	{
		CloseHandle(m_comm_handle);
	}
	m_enum_boudrate.clear();
}
void FindPort::writeFile(const wstring & config_path)
{
	//wstring config_path = L"GPS_config.txt";
	wofstream out;
	out.open(config_path.c_str(),ios_base::out|ios_base::trunc);
	if(!out.is_open())
	{
		assert(0);
	}
	out<<L"COMM PORT="<<m_comm<<endl;
	out<<L"COMM BAUDRATE="<<m_baudrate<<endl;
	out.close();
}
bool FindPort::findPortWithBaudrate()
{
	//baud = DEFAULT_BAUD;
	vector<int>::const_iterator it_boudrate;
	for(it_boudrate = m_enum_boudrate.begin();it_boudrate != m_enum_boudrate.end();it_boudrate++ )
	{
		int boud = *(it_boudrate);
		for(int i = 1;i<11;i++ )
		{	
			wchar_t wCom[20] = L"COM";
			wchar_t wPort[20];
			memset(wPort, 0, 20 * sizeof(wchar_t));
			_itow(i, wPort, 10);
			wcscat(wCom, wPort);
			wcscat(wCom, L":");
			//HANDLE temp_handle;
			if(openPort(wCom,boud,m_comm_handle)!=0)
			{
				cout<<"open com"<<i<<" failed"<<endl;
				CloseHandle(m_comm_handle);
				continue;
			}
			if(readComm(m_comm_handle) ==0)
			{
				cout<<"do not find in COM"<<i<<" ."<<endl;
				CloseHandle(m_comm_handle);
				continue;
			}
			//m_comm = wCom;
			wsprintf(m_comm,L"%s",wCom);
			m_baudrate = boud;
			CloseHandle(m_comm_handle);
			return true;
		}
	}
	return false;
}
bool FindPort::writeCOMM(HANDLE m_com_hdl)
{
	char write_data[20] = "$PSRF151,01*0F\r\n";
	DWORD dwBytesWritten,
		dwSize = 16,
		dwErrorFlags;
	COMSTAT ComStat;
	OVERLAPPED OverLappedWrite;
	memset(&OverLappedWrite, 0, sizeof(OverLappedWrite));
	OverLappedWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	DWORD dwEvtMask = 0;

	int res = ClearCommError(m_com_hdl, &dwErrorFlags, &ComStat);
	// 	if(!ComStat.cbInQue)
	// 	{
	// 		return 0;
	// 	}
	int total_num = 0;

	do
	{
		BOOL bWriteState = WriteFile( m_com_hdl, 
			write_data, 
			dwSize, 
			&dwBytesWritten,
			&OverLappedWrite
			);
		if(!bWriteState)
		{
			int err_no = GetLastError();
			if(GetLastError() == ERROR_IO_PENDING)
			{
				DWORD e = WaitForSingleObject(OverLappedWrite.hEvent, INFINITE);
				if(e != WAIT_OBJECT_0) dwBytesWritten = 0;
			}
			else
			{
				dwBytesWritten = 0;
			}
		}
		total_num += dwBytesWritten;
	}while(!::WaitCommEvent(m_com_hdl, &dwEvtMask, NULL));
	bool bool_result = (EV_TXEMPTY == dwEvtMask|EV_TXEMPTY);
	CloseHandle(OverLappedWrite.hEvent);
	return dwBytesWritten;

}
bool FindPort::readComm(HANDLE & in_handle)
{
	char buffer[5100];//
	memset(buffer, 0, sizeof(buffer));

	DWORD dwEvtMask = 0;
	bool result = true;
	DWORD dwErrorFlags;
	COMSTAT ComStat;
	OVERLAPPED o;
	o.Offset = o.OffsetHigh = 0;
	o.hEvent = CreateEvent(
		NULL,   // no security attributes 
		TRUE,  // auto reset event 
		FALSE,  // not signaled 
		NULL    // no name 
		);

	assert(o.hEvent);
//	DWORD errornum;
// 	result = ::WaitCommEvent(in_handle, &dwEvtMask, &o);
// 	errornum = ::GetLastError();
// 	//没有等到消息
// 	if(!result)
// 	{
// 		if(::GetLastError() == ERROR_IO_PENDING)
// 		{
// 			DWORD e = WaitForSingleObject(o.hEvent, 20000);
// 			if(e != WAIT_OBJECT_0) return result;
// 		}
// 		else
// 		{
// 			return result;
// 		}
// 	}
	Sleep(3000);
	::ClearCommError(in_handle, &dwErrorFlags, &ComStat); 

	DWORD readsum;
// 	if( errornum == ERROR_IO_PENDING )
// 	{
		//把Camstat.cbInQue注释掉是因为在某些机器上clearCommerror不能正确返回COMSTAT
// 		if((dwEvtMask & EV_RXCHAR) == EV_RXCHAR/* && ComStat.cbInQue > 0*/ )
// 		{
			readsum = 4096;

			 result = ::ReadFile(
				in_handle,
				buffer,
				readsum,
				&readsum, 
				&o//NULL 
				);
			 if(!result)
				{
					if(::GetLastError() == ERROR_IO_PENDING)
					{
						//GetOverlappedResult(m_com_hdl, &o, &readsum, TRUE);
						DWORD e = WaitForSingleObject(o.hEvent, 20000);
						if(e != WAIT_OBJECT_0) return result;
					}
					else
					{
						return result;
					}
				}

			if( result == TRUE && readsum > 0)
			{
				char *p = buffer;
				while(p - buffer < (int)readsum)
				{
					if(*p == '$')	// a sentence start here
					{
						//是GPS端口
						return true;
					}
					else
					{
						p ++;
					}
				}
			//}
		//}
		//}
	}
	return false;
}
int FindPort::openPort(const WCHAR* com_port,unsigned int baud,HANDLE& out_handle)
{
 	out_handle = ::CreateFile(com_port, 
 		GENERIC_READ|GENERIC_WRITE, 
 		0, 
 		NULL, 
 		OPEN_EXISTING, 
 		FILE_FLAG_OVERLAPPED, 
 		NULL 
	 		);

	if( out_handle == INVALID_HANDLE_VALUE )
	{
		out_handle = NULL;
		DWORD err_code = GetLastError();
		return err_code;
	}

	// 设置COM数值
	int temp;
	temp = ::SetCommMask(out_handle, EV_TXEMPTY );//| EV_ERR | EV_RXFLAG); 
	temp = ::SetupComm(out_handle, 4096, 4096); 
	temp = ::PurgeComm(out_handle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR); 

	COMMTIMEOUTS CommTimeOuts; 
	CommTimeOuts.ReadIntervalTimeout = 0; 
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0; 
	CommTimeOuts.ReadTotalTimeoutConstant = 1000; 
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0; 
	CommTimeOuts.WriteTotalTimeoutConstant = 1000; 
	temp = ::SetCommTimeouts(out_handle, &CommTimeOuts); 

	DCB dcb; 
	dcb.DCBlength = sizeof(DCB); 

	temp = ::GetCommState(out_handle, &dcb); 
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

	BOOL fRetVal = SetCommState(out_handle, &dcb); 

	if( fRetVal == FALSE )
	{
		DWORD err_code = GetLastError();
		::CloseHandle( out_handle );
		out_handle = NULL;
		return (int)err_code;
	}
	return 0;
}
#endif