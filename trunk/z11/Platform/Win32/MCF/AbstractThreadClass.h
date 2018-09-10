#pragma once

#include <winsock2.h>
#include <windows.h>
#include "GenericHelpers.h"

class AbstractThreadClass : NO_COPY_OR_ASSIGN {
private:
	typedef struct _tagThreadControlBlock {
		HANDLE				hThread;
		DWORD				dwThreadID;
		bool				bIsSuspended;
		bool				bAutoCloseHandle;
		AbstractThreadClass	*lpOwner;
	} THREADCONTROLBLOCK;
protected:
	static CriticalSection _s_csLock;
private:
	static unsigned int __stdcall AbstractThreadClass::_ThreadStartRoutine(void *lpParam);
private:
	THREADCONTROLBLOCK *_m_lpThreadControlBlock;
	int _m_nExitCode;
public:
	AbstractThreadClass();
	virtual ~AbstractThreadClass();
protected:
	virtual int _ThreadProc() = 0;
public:
	HANDLE GetHThread() const;
	DWORD GetThreadID() const;

	void Create(bool bSuspended);
	void Release();
	int WaitAndGetExitCode() const;
	int Parasitize(HANDLE hThread, bool bSuspended);

	void Suspend();
	void Resume();
	void SetPriority(int priority);

	bool IsSuspended() const;
};
