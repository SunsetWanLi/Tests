
#include "Syncable.h"

Syncable::Syncable()
{
	// recursive
#if defined(_WIN32) || defined(_WIN64)
	InitializeCriticalSection(&m_mutex);
#else
	pthread_mutexattr_t attr ;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE );
	pthread_mutex_init(&m_mutex,&attr); 
	pthread_mutexattr_destroy(&attr);
#endif

}

Syncable::~Syncable()
{
#if defined(_WIN32) || defined(_WIN64)
	DeleteCriticalSection(&m_mutex);
#else
	pthread_mutex_destroy(&m_mutex); 
#endif
}


void Syncable::lock() const
{
#if defined(_WIN32) || defined(_WIN64)
	EnterCriticalSection(&m_mutex);
#else
	pthread_mutex_lock(&m_mutex); 
#endif
	
}

void Syncable::unlock() const
{
#if defined(_WIN32) || defined(_WIN64)
	LeaveCriticalSection(&m_mutex);
#else
	pthread_mutex_unlock(&m_mutex); 
#endif
	
}

// lock in constructor
Syncable::AutoLock::AutoLock(const Syncable *sync)
{
	m_sync = sync;
	m_sync->lock();
}


// unlock in destructor
Syncable::AutoLock::~AutoLock()
{	
	m_sync->unlock();
}

