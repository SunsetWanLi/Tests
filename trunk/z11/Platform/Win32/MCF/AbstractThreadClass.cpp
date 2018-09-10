#include <winsock2.h>
#include <windows.h>
#include <stdlib.h>
#include <tchar.h>
#include <process.h>
#include <cassert>
#include "AbstractThreadClass.h"

// *** 请仔细阅读下面的约定，这些约定对于维护线程安全是至关重要的。 ***
// 1. 线程控制块总是由线程创建者申请、线程自己释放。
//    因此在线程函数内部线程控制块总是可以访问的，
//    而如果在线程创建者中访问则需要进入临界区。
// 2. 如果线程函数不涉及对 this 指针的访问，可以在创建线程后
//    直接调用 Release()；这需要用户自己遵守这种约定。
//    如果调用 Release() 后，线程函数中又访问了 this 指针，
//    结果是未定义的。

// 静态变量定义。
CriticalSection AbstractThreadClass::_s_csLock;

// 静态成员函数。
unsigned int __stdcall AbstractThreadClass::_ThreadStartRoutine(void *lpParam){
	THREADCONTROLBLOCK *const lpThreadControlBlock = (THREADCONTROLBLOCK *)lpParam;

	assert(lpThreadControlBlock != NULL);

	const int nExitCode = lpThreadControlBlock->lpOwner->_ThreadProc();

	_s_csLock.Enter();
		if(lpThreadControlBlock->lpOwner != NULL){
			lpThreadControlBlock->lpOwner->_m_nExitCode = nExitCode;
			lpThreadControlBlock->lpOwner->Release();
		}
	_s_csLock.Leave();

	if(lpThreadControlBlock->bAutoCloseHandle){
		CloseHandle(lpThreadControlBlock->hThread);
	}
	delete lpThreadControlBlock;

	return 0;
}

// 构造函数和析构函数。
AbstractThreadClass::AbstractThreadClass() :
	_m_lpThreadControlBlock(NULL),
	_m_nExitCode(0)
{
}
AbstractThreadClass::~AbstractThreadClass(){
	Release();
}

// 其他非静态成员函数。
HANDLE AbstractThreadClass::GetHThread() const {
	HANDLE hThread = NULL;

	_s_csLock.Enter();
		if(_m_lpThreadControlBlock != NULL){
			hThread = _m_lpThreadControlBlock->hThread;
		}
	_s_csLock.Leave();

	return hThread;
}
DWORD AbstractThreadClass::GetThreadID() const {
	DWORD dwThreadID = 0;

	_s_csLock.Enter();
		if(_m_lpThreadControlBlock != NULL){
			dwThreadID = _m_lpThreadControlBlock->dwThreadID;
		}
	_s_csLock.Leave();

	return dwThreadID;
}

void AbstractThreadClass::Create(bool bSuspended){
	Release();

	_s_csLock.Enter();
		_m_lpThreadControlBlock						= new THREADCONTROLBLOCK;
		unsigned int uThreadID;
		const HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, &_ThreadStartRoutine, _m_lpThreadControlBlock, CREATE_SUSPENDED, &uThreadID);
		DuplicateHandle(GetCurrentProcess(), hThread, GetCurrentProcess(), &_m_lpThreadControlBlock->hThread, THREAD_ALL_ACCESS & ~THREAD_TERMINATE, FALSE, DUPLICATE_CLOSE_SOURCE);
		_m_lpThreadControlBlock->dwThreadID			= uThreadID;
		_m_lpThreadControlBlock->bIsSuspended		= true;
		_m_lpThreadControlBlock->bAutoCloseHandle	= true;
		_m_lpThreadControlBlock->lpOwner			= this;

		if(!bSuspended){
			ResumeThread(_m_lpThreadControlBlock->hThread);
			_m_lpThreadControlBlock->bIsSuspended = false;
		}
	_s_csLock.Leave();
}
void AbstractThreadClass::Release(){
	_s_csLock.Enter();
		if(_m_lpThreadControlBlock != NULL){
			_m_lpThreadControlBlock->lpOwner = NULL;
			_m_lpThreadControlBlock = NULL;
		}
	_s_csLock.Leave();
}
int AbstractThreadClass::WaitAndGetExitCode() const {
	HANDLE hThread = NULL;

	_s_csLock.Enter();
		if(_m_lpThreadControlBlock != NULL){
			hThread = _m_lpThreadControlBlock->hThread;
			_m_lpThreadControlBlock->bAutoCloseHandle = false;
		}
	_s_csLock.Leave();

	if(hThread != NULL){
		WaitForSingleObject(hThread, INFINITE);
		CloseHandle(hThread);
	}
	return _m_nExitCode;
}
int AbstractThreadClass::Parasitize(HANDLE hThread, bool bSuspended){
	Release();

	_s_csLock.Enter();
		_m_lpThreadControlBlock						= new THREADCONTROLBLOCK;
		DuplicateHandle(GetCurrentProcess(), hThread, GetCurrentProcess(), &_m_lpThreadControlBlock->hThread, THREAD_ALL_ACCESS & ~THREAD_TERMINATE, FALSE, 0);
		_m_lpThreadControlBlock->dwThreadID			= GetCurrentThreadId();
		_m_lpThreadControlBlock->bIsSuspended		= true;
		_m_lpThreadControlBlock->bAutoCloseHandle	= true;
		_m_lpThreadControlBlock->lpOwner			= this;

		if(!bSuspended){
			while (ResumeThread(_m_lpThreadControlBlock->hThread) > 1);
			_m_lpThreadControlBlock->bIsSuspended = false;
		}
	_s_csLock.Leave();

	_ThreadStartRoutine(_m_lpThreadControlBlock);

	return _m_nExitCode;
}

void AbstractThreadClass::Suspend(){
	_s_csLock.Enter();
		if(_m_lpThreadControlBlock != NULL){
			SuspendThread(_m_lpThreadControlBlock->hThread);
			_m_lpThreadControlBlock->bIsSuspended = true;
		}
	_s_csLock.Leave();
}
void AbstractThreadClass::Resume(){
	_s_csLock.Enter();
		if(_m_lpThreadControlBlock != NULL){
			while (ResumeThread(_m_lpThreadControlBlock->hThread) > 1);
			_m_lpThreadControlBlock->bIsSuspended = false;
		}
	_s_csLock.Leave();
}
void AbstractThreadClass::SetPriority(int priority){
	_s_csLock.Enter();
		if(_m_lpThreadControlBlock != NULL){
			SetThreadPriority(_m_lpThreadControlBlock->hThread, priority);
		}
	_s_csLock.Leave();
}

bool AbstractThreadClass::IsSuspended() const {
	bool bIsSuspended = false;

	_s_csLock.Enter();
		if(_m_lpThreadControlBlock != NULL){
			bIsSuspended = _m_lpThreadControlBlock->bIsSuspended;
		}
	_s_csLock.Leave();

	return bIsSuspended;
}
