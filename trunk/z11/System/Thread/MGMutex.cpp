#include "MGMutex.h"
using namespace z11;
#if defined(_WIN32) || defined(_WIN64)
#include <winsock2.h>
#include <windows.h>
#include "stdio.h"
MGMutex::MGMutex()
{
	mState = CreateMutex(NULL, FALSE, NULL);
}

MGMutex::MGMutex(const char* name)
{
	mState = CreateMutex(NULL, FALSE, NULL);
}

MGMutex::MGMutex(int type, const char* name)
{
	mState = CreateMutex(NULL, FALSE, NULL);
}

MGMutex::~MGMutex()
{
	CloseHandle(mState);
}

int MGMutex::lock()
{
	z11::uint_32 dwWaitResult;
	dwWaitResult = WaitForSingleObject(mState, INFINITE);
	return dwWaitResult != WAIT_OBJECT_0 ? -1 : NO_ERROR;
}

void MGMutex::unlock()
{
	if (!ReleaseMutex(mState))
		printf( "WARNING: bad result from unlocking mutex\n");
}

int MGMutex::tryLock()
{
	z11::uint_32 dwWaitResult;
    
	dwWaitResult = WaitForSingleObject( mState, 0);
	if (dwWaitResult != WAIT_OBJECT_0 && dwWaitResult != WAIT_TIMEOUT)
		printf( "WARNING: bad result from try-locking mutex\n");
	return (dwWaitResult == WAIT_OBJECT_0) ? 0 : -1;
}


#else
MGMutex::MGMutex()
{
	pthread_mutexattr_t attr ;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE );
	pthread_mutex_init(&mMutex, &attr);
	pthread_mutexattr_destroy(&attr);

}

MGMutex::MGMutex(const char* name)
{
	pthread_mutexattr_t attr ;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE );
	pthread_mutex_init(&mMutex, &attr);
	pthread_mutexattr_destroy(&attr);
}

MGMutex::MGMutex(int type, const char* name)
{
	if (type == SHARED) {
		pthread_mutexattr_t attr;
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE );
		pthread_mutex_init(&mMutex, &attr);
		pthread_mutexattr_destroy(&attr);
	} else {
		pthread_mutex_init(&mMutex, NULL);
	}
}

MGMutex::~MGMutex() {
	pthread_mutex_destroy(&mMutex);
}
 int MGMutex::lock() {
	return -pthread_mutex_lock(&mMutex);
}
void MGMutex::unlock() {
	pthread_mutex_unlock(&mMutex);
}
int MGMutex::tryLock() {
	return -pthread_mutex_trylock(&mMutex);
}
#endif
