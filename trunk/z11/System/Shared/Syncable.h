#pragma once
#if defined(_WIN32) || defined(_WIN64)
#ifdef WINCE
#include <winsock2.h>
#include <windows.h>
#else
#include <winsock2.h>
#include <windows.h>
#endif
#else// linux
	#include <pthread.h>
#endif // #if defined(_WIN32) || defined(_WIN64)

class Syncable
{
public:
	Syncable();
	virtual ~Syncable();
public:
	void lock() const;	
	void unlock() const;
	
	class AutoLock
	{
	public:		
		AutoLock(const Syncable *sync);	
		~AutoLock();		
	private:
		const Syncable *m_sync;
	};
    
    #define AUTO_LOCK_THIS AutoLock ___THE_AUTO_LOCK_OF_THIS___(this);

private:
#if defined(_WIN32) || defined(_WIN64)
	mutable CRITICAL_SECTION m_mutex;
#else
	mutable pthread_mutex_t m_mutex;
#endif // #if defined(_WIN32) || defined(_WIN64)

};



