#include "MGMemoryInfo.h"
#include <winsock2.h>
#include <windows.h>
#include <Tlhelp32.h>
#include <vector>
#include "../../System/Shared/MGLog.h"
using namespace std;
int iStartTime=0;
wchar_t filename[128];
//#pragma comment( lib, "Toolhelp.lib" )
static bool bInit=false;
void InitTools()
{
	iStartTime=GetTickCount();
	GetModuleFileName(NULL,filename,128);
	int len=wcslen(filename);
	int start;
	for (start=len-1;start>=0&&filename[start]!='\\';start--);
	for (int j=0;filename[j]=filename[start+1];start++,j++);
	bInit=true;
	
}
void OutputDebugStringEx(LPCTSTR lpOutputString,int num=-999999999)
{
	if (!bInit)
	{
		InitTools();
	}
	//OutputDebugString(lpOutputString);
	wchar_t temp[256];
	if (num!=-999999999)
	{
		wsprintf(temp,lpOutputString,num);
	}
	else
		wsprintf(temp,L"%s",lpOutputString);
	swprintf(temp,L"%s Time = %.3lf\n",temp,(GetTickCount()-iStartTime)/1000.0);
	OutputDebugString(temp);
}


typedef BOOL (*PROC_EMUN_PROC)(PROCESSENTRY32,vector<PROCESSENTRY32> &); 
//const unsigned TH32CS_SNAPNOHEAPS = 0x40000000;
BOOL EnumProcInfo(PROC_EMUN_PROC pProcFunc,vector<PROCESSENTRY32> &pParam)
{
	BOOL bRes = FALSE;
	HANDLE hSnapshot = INVALID_HANDLE_VALUE;


	__try
	{    


		hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hSnapshot == INVALID_HANDLE_VALUE)
		{
			__leave;
		}

		PROCESSENTRY32 processEntry;
		processEntry.dwSize = sizeof(PROCESSENTRY32);

		if(Process32First(hSnapshot, &processEntry) == FALSE)
		{
			__leave;
		}
		if (!wcscmp(processEntry.szExeFile,filename))
		{
			pParam.push_back(processEntry);
		}

		while (Process32Next(hSnapshot, &processEntry) != FALSE)
		{

			if (!wcscmp(processEntry.szExeFile,filename))
			{
				pParam.push_back(processEntry);
			}

		}

		bRes = TRUE;
	}
	__finally 
	{
		if (hSnapshot != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hSnapshot);
			hSnapshot = NULL;
		}
	}

	return bRes;
}

BOOL GetAllProcInfo(vector<PROCESSENTRY32> &vtProcInfo)
{  
	vtProcInfo.clear();
	return EnumProcInfo(NULL,vtProcInfo);
	//return EnumProcInfo(EnumAllProcessProc,&vtProcInfo);
}

BOOL GetHeapList(DWORD dwProcID,std::vector<HEAPLIST32> &vtHeapList)
{
	BOOL bRes = FALSE;
	HANDLE hSnapShot = INVALID_HANDLE_VALUE;

	__try
	{
		vtHeapList.clear();

		hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPHEAPLIST, dwProcID);
		if(hSnapShot == INVALID_HANDLE_VALUE)
		{
			__leave;
		}

		HEAPLIST32 heapList;
		heapList.dwSize = sizeof(HEAPLIST32);

		//Begin to find the heap list
		if(Heap32ListFirst(hSnapShot,&heapList) == FALSE)
		{
			__leave;
		}
		vtHeapList.push_back(heapList);

		while(Heap32ListNext(hSnapShot, &heapList) != FALSE)
		{
			vtHeapList.push_back(heapList);
		}

		bRes = TRUE;
	}
	__finally
	{
		if(hSnapShot != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hSnapShot);
		}
	}

	return bRes;
}

BOOL GetHeapEntry(DWORD dwProcID,const HEAPLIST32 &heapList,std::vector<HEAPENTRY32> &vtHeapEntry)
{
	BOOL bRes = FALSE;
	HANDLE hSnapShot = INVALID_HANDLE_VALUE;

	__try
	{
		vtHeapEntry.clear();

		hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPHEAPLIST, dwProcID);
		if(hSnapShot == INVALID_HANDLE_VALUE)
		{
			__leave;
		}

		HEAPENTRY32 heapEntry = {0};
		heapEntry.dwSize = sizeof(HEAPENTRY32);

		//Begin to find the heap entry  
		if(Heap32First( &heapEntry, heapList.th32ProcessID, heapList.th32HeapID) == FALSE)
		{
			__leave;
		}
		vtHeapEntry.push_back(heapEntry);

		while(Heap32Next( &heapEntry) != FALSE)
		{
			vtHeapEntry.push_back(heapEntry);
		}

		bRes = TRUE;
	}
	__finally
	{  
		if(hSnapShot != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hSnapShot);
		}
	}


	return bRes;
}

DWORD GetHeapSize(const std::vector<HEAPENTRY32> &vtHeapEntry)
{
	DWORD dwSize = 0;
	for(std::vector<HEAPENTRY32>::const_iterator iter = vtHeapEntry.begin(); iter != vtHeapEntry.end(); ++ iter)
	{
		dwSize += iter->dwBlockSize;
	}

	return dwSize;
}
DWORD GetFixHeapSize(const std::vector<HEAPENTRY32> &vtHeapEntry)
{
	DWORD dwSize = 0;
	//for(std::vector<HEAPENTRY32>::const_iterator iter = vtHeapEntry.begin(); iter != vtHeapEntry.end(); ++ iter)
	//{
	//	if (iter->dwLockCount)
	//	{
	//		dwSize += iter->dwBlockSize;
	//	}
	//}
	for(std::vector<HEAPENTRY32>::const_iterator iter = vtHeapEntry.begin(); iter != vtHeapEntry.end(); ++ iter)
	{
		dwSize += iter->dwSize;
	}

	return dwSize;
}
int AvailVirtualMem()
{
	MEMORYSTATUS mst;			
	mst.dwLength = sizeof(MEMORYSTATUS);			
	GlobalMemoryStatus(&mst);
	OutputDebugStringEx(L"avail size=%d\n",mst.dwAvailVirtual);
	return mst.dwAvailVirtual;
}
int TestMem()
{
	if (!bInit)
	{
		InitTools();
	}
	MEMORYSTATUS mst;			
	mst.dwLength = sizeof(MEMORYSTATUS);			
	GlobalMemoryStatus(&mst);
	vector<PROCESSENTRY32> vtProcInfo;
	GetAllProcInfo(vtProcInfo);
	size_t size=0;
	if (!vtProcInfo.empty())
	{
		vector<HEAPLIST32> vtHeapList;
		GetHeapList(vtProcInfo[0].th32ProcessID,vtHeapList);
		for (size_t i=0;i<vtHeapList.size();i++)
		{
			vector<HEAPENTRY32> vtHeapEntry;
			GetHeapEntry(vtProcInfo[0].th32ProcessID,vtHeapList.at(i),vtHeapEntry);
			size+=GetFixHeapSize(vtHeapEntry);
		}
		OutputDebugStringEx(L"Heap size=%d\n",size);
	}
	return size;
}
void TestMem(wchar_t *str)
{
	if (str!=NULL)
	{
		OutputDebugString(str);
	}
	TestMem();
}
struct RegionSize 
{
	DWORD dwFree;
	DWORD dwReserve;
	DWORD dwCommit;
};

BOOL GetRegionSizeFromMemoryBase(DWORD dwAddressBase,RegionSize &RegionSize)
{
	DWORD dwStartAddress = dwAddressBase;
	DWORD dwEndAddress = dwStartAddress + 0x40000000;
	DWORD dwAddress = dwStartAddress;

	if(dwAddress >= dwEndAddress)
	{
		return FALSE;
	}

	memset(&RegionSize,0,sizeof(RegionSize));

	while (dwAddress < dwEndAddress)
	{
		MEMORY_BASIC_INFORMATION memoryInfo;
		SIZE_T rv = ::VirtualQuery((LPVOID)dwAddress, &memoryInfo, sizeof(memoryInfo));
		if (rv != 0)
		{
			if (memoryInfo.State == MEM_COMMIT)
			{
				RegionSize.dwCommit += memoryInfo.RegionSize;
			}
			if (memoryInfo.State == MEM_RESERVE)
			{
				RegionSize.dwReserve += memoryInfo.RegionSize;
			}
			if (memoryInfo.State == MEM_FREE)
			{
				RegionSize.dwFree += memoryInfo.RegionSize;
			}
			dwAddress += memoryInfo.RegionSize;
		}
		else
		{
			break;
		}
	}

	return TRUE;
}
#include <Psapi.h>
#pragma comment(lib,"Psapi.lib")
int z11::MGMemoryInfo::getCurrentHeapSize()
{
	HANDLE handle = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(handle, & pmc, sizeof (pmc));
	MGLogD("heap size:%d", pmc.WorkingSetSize);
	return pmc.WorkingSetSize;
	//return TestMem();
}