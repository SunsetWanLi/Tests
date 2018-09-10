#include "MGSemaphore.h"
using namespace z11;
#if MG_TARGET_PLATFORM == MG_PLATFORM_WIN32
#include <winsock2.h>
#include <windows.h>
MGSemaphore::MGSemaphore(unsigned int val)
{
	hSemaphore=CreateSemaphore(NULL,val,1000,0);
}

MGSemaphore::~MGSemaphore(void)
{
	CloseHandle(hSemaphore);
	hSemaphore=NULL;
}

void MGSemaphore::Wait()
{
	WaitForSingleObject(hSemaphore,INFINITE);
}

void MGSemaphore::Signal()
{
	ReleaseSemaphore(hSemaphore,1,NULL);
}


#elif MG_TARGET_PLATFORM == MG_PLATFORM_ANDROID

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include "../Shared/MGLog.h"
#include "../Shared/MGTime.h"
#include <string>
using namespace z11;
MGSemaphore::MGSemaphore(unsigned int val)
{
	sem_init(&hSemaphore,0,val);
}

MGSemaphore::~MGSemaphore(void)
{
	sem_destroy(&hSemaphore);
}

void MGSemaphore::Wait()
{
	sem_wait(&hSemaphore);
}

void MGSemaphore::Signal()
{
	sem_post(&hSemaphore);
}
#else
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include "../Shared/MGLog.h"
#include "../Shared/MGTime.h"
#include <string>
using namespace z11;
MGSemaphore::MGSemaphore(unsigned int val)
{
    sprintf(inName,"SP %p %d",this,(short int)MGTimeTicks::Now().ToInternalValue());
    sem_t * semaphore = sem_open( inName, O_CREAT, 0644, val );  
    
    if( semaphore == SEM_FAILED )  
    {  
        MGLogD("sem_open error:%s",strerror(errno));
        assert(false);  
    }  
    
    hSemaphore= semaphore;  
}

MGSemaphore::~MGSemaphore(void)
{
    //iOS only support named-Semaphore
    int retErr = sem_close( hSemaphore );  
    
    //  
    if( retErr == -1 )  
    {  
        MGLogD("sem_close error:%s",strerror(errno));
        assert(false);   
    }  
    
    //clear
    retErr = sem_unlink(inName);  
    if( retErr == -1 )  
    {  
        MGLogD("sem_unlink error:%s",strerror(errno));
        assert(false);   
    }  
}

void MGSemaphore::Wait()
{
	sem_wait(hSemaphore);
}

void MGSemaphore::Signal()
{
	sem_post(hSemaphore);
}
#endif

