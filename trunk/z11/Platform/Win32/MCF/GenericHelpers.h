#pragma once

#include <vector>
#include <winsock2.h>
#include <windows.h>

// 禁止拷贝构造和赋值。
class _NonCopyableAndAssignableBase {
protected:
	_NonCopyableAndAssignableBase(){ }
	~_NonCopyableAndAssignableBase(){ }
private:
	_NonCopyableAndAssignableBase(const _NonCopyableAndAssignableBase &);
	void operator=(const _NonCopyableAndAssignableBase &);
};
#define NO_COPY_OR_ASSIGN	private ::_NonCopyableAndAssignableBase

// 防止编译器产生“未引用的形参”的警告。
#define UNREF_PARAMS(...)	(void)(__VA_ARGS__, 0)

// 以下四个类用于线程同步。
class CriticalSection sealed : NO_COPY_OR_ASSIGN {
private:
	mutable CRITICAL_SECTION _m_CriticalSection;
public:
	CriticalSection(){
//		InitializeCriticalSectionAndSpinCount(&_m_CriticalSection, 0x400);
		InitializeCriticalSection(&_m_CriticalSection);
	}
	~CriticalSection(){
		DeleteCriticalSection(&_m_CriticalSection);
	}
public:
	// 可以用于 const 对象。
	//bool Try() const {
	//	return TryEnterCriticalSection(&_m_CriticalSection) != FALSE;
	//}
	void Enter() const {
		EnterCriticalSection(&_m_CriticalSection);
	}
	void Leave() const {
		LeaveCriticalSection(&_m_CriticalSection);
	}
};
class Event sealed : NO_COPY_OR_ASSIGN {
private:
	HANDLE _m_hEvent;
public:
	Event(bool bIsInitiallySet){
		_m_hEvent = CreateEvent(NULL, TRUE, bIsInitiallySet ? TRUE : FALSE, NULL);
	}
	~Event(){
		CloseHandle(_m_hEvent);
	}
public:
	bool Query() const {
		return WaitTimeout(0);
	}
	void Wait() const {
		WaitTimeout(INFINITE);
	}
	bool WaitTimeout(DWORD dwMilliSeconds) const {
		return WaitForSingleObject(_m_hEvent, dwMilliSeconds) != WAIT_TIMEOUT;
	}

	void Set(){
		SetEvent(_m_hEvent);
	}
	void Clear(){
		ResetEvent(_m_hEvent);
	}
};
class Semaphore sealed : NO_COPY_OR_ASSIGN {
private:
	HANDLE _m_hSemaphore;
public:
	Semaphore(long lInitCount, long lMaxCount){
		_m_hSemaphore = CreateSemaphore(NULL, lInitCount, lMaxCount, NULL);
	}
	~Semaphore(){
		CloseHandle(_m_hSemaphore);
	}
public:
	void Wait(){
		WaitForSingleObject(_m_hSemaphore, INFINITE);
	}
	// 计数器自减，但是不等待。
	void Alloc(long lCountToAlloc){
		for(long i = 0; i < lCountToAlloc; ++i){
			WaitForSingleObject(_m_hSemaphore, 0);
		}
	}
	// 每一个 Wait() 和 Alloc() 必须对应一个 Signal()。
	void Signal(long lCountToDealloc = 1){
		ReleaseSemaphore(_m_hSemaphore, lCountToDealloc, NULL);
	}
};
class ReadWriteLock sealed : NO_COPY_OR_ASSIGN {
private:
	mutable Semaphore _m_semWriteLock;
	mutable unsigned int _m_uReadCount;
	mutable CriticalSection _m_csReadCountLock;
public:
	ReadWriteLock() :
		_m_semWriteLock(1, 1),
		_m_uReadCount(0)
	{
	}
	~ReadWriteLock(){
	}
public:
	void AcquireReadLock() const {
		_m_csReadCountLock.Enter();
			if(++_m_uReadCount == 1){
				_m_semWriteLock.Wait();
			}
		_m_csReadCountLock.Leave();
	}
	void ReleaseReadLock() const {
		_m_csReadCountLock.Enter();
			if(--_m_uReadCount == 0){
				_m_semWriteLock.Signal();
			}
		_m_csReadCountLock.Leave();
	}
	void AcquireWriteLock(){
		_m_semWriteLock.Wait();
	}
	void ReleaseWriteLock(){
		_m_semWriteLock.Signal();
	}
};

// 判断一个键是否被按下。
inline bool IsKeyDown(int nVirtualCode){
	return (GetKeyState(nVirtualCode) & 0x8000) != 0;
}

inline char *StrTokByNewLine(char *lpszSrc, char **lppContext){
	LPSTR lpszCurrentToken;
	if(lpszSrc != NULL){
		lpszCurrentToken = lpszSrc;
	} else {
		lpszCurrentToken = *lppContext;
	}
	// 如果字符串以换行符结尾，就会产生 lpszCurrentToken[0] = 0 这种情况。
	if((lpszCurrentToken == NULL) || (lpszCurrentToken[0] == 0)){
		return NULL;
	}

	LPSTR lpszNextToken = lpszCurrentToken;
	for(;;){
		switch(*lpszNextToken){
			case 0:
				*lppContext = NULL;
				return lpszCurrentToken;
			case '\r':
				*lpszNextToken = 0;
				++lpszNextToken;
				// 这里没有 break。
			case '\n':
				*lpszNextToken = 0;
				*lppContext = lpszNextToken + 1;
				return lpszCurrentToken;
		}
		++lpszNextToken;
	}
}
inline wchar_t *WcsTokByNewLine(wchar_t *lpwszSrc, wchar_t **lppContext){
	LPWSTR lpwszCurrentToken;
	if(lpwszSrc != NULL){
		lpwszCurrentToken = lpwszSrc;
	} else {
		lpwszCurrentToken = *lppContext;
	}
	// 如果字符串以换行符结尾，就会产生 lpwszCurrentToken[0] = 0 这种情况。
	if((lpwszCurrentToken == NULL) || (lpwszCurrentToken[0] == 0)){
		return NULL;
	}

	LPWSTR lpwszNextToken = lpwszCurrentToken;
	for(;;){
		switch(*lpwszNextToken){
			case 0:
				*lppContext = NULL;
				return lpwszCurrentToken;
			case L'\r':
				*lpwszNextToken = 0;
				++lpwszNextToken;
				// 这里没有 break。
			case L'\n':
				*lpwszNextToken = 0;
				*lppContext = lpwszNextToken + 1;
				return lpwszCurrentToken;
		}
		++lpwszNextToken;
	}
}
#ifdef UNICODE
#	define TcsTokByNewLine WcsTokByNewLine
#else
#	define TcsTokByNewLine StrTokByNewLine
#endif
