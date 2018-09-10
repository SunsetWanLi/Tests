#pragma once
#include "../../Platform/MGPlatformConfig.h"
#if MG_TARGET_PLATFORM != MG_PLATFORM_WIN32
#include <semaphore.h>
#endif
#include "../../CommonBase/MGObject.h"
namespace z11
{
	class MGSemaphore:public MGObject
	{
	public:
		MGSemaphore(unsigned int initval);
		~MGSemaphore(void);
		void Wait();
		void Signal();
	private:
#if MG_TARGET_PLATFORM == MG_PLATFORM_WIN32
		void * hSemaphore;
#elif  MG_TARGET_PLATFORM == MG_PLATFORM_ANDROID
		sem_t hSemaphore;
#else
        char inName[256];
		sem_t *hSemaphore;
#endif
	};

}
