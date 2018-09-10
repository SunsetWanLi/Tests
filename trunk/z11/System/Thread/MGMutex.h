#pragma once
#include "../../CommonBase/CommonDef.h"
#include "../../CommonBase/MGObject.h"
#if defined(_WIN32) || defined(_WIN64)
#include "stddef.h"
#else
# include <pthread.h>
#endif

namespace z11
{


	class MGMutex:public MGObject
	{
	public:
		enum {
			PRIVATE = 0,
			SHARED = 1
		};

		MGMutex(void);
		MGMutex(const char* name);
		MGMutex(int type, const char* name = NULL);
		virtual ~MGMutex();

		// lock or unlock the mutex
		int    lock();
		void        unlock();

		// lock if possible; returns 0 on success, error otherwise
		int    tryLock();
	private:
#if defined(_WIN32) || defined(_WIN64)
		void *   mState;
#else
		pthread_mutex_t   mMutex;
#endif
	};
	class MGAutoLock
	{
	public:
		MGAutoLock(MGMutex *mutex)
		{
			locker=mutex;
			locker->lock();
		}
		virtual ~MGAutoLock()
		{
			locker->unlock();
		}
	private:
		MGMutex *locker;
	};
	#define AUTO_LOCK(mutex) MGAutoLock ___THE_AUTO_LOCK_OF_THIS___(mutex);
}

