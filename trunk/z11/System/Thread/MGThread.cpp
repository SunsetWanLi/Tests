#include "MGThread.h"
#include "MGSemaphore.h"
#include "MGMutex.h"
using namespace z11;
#if defined(_WIN32) || defined(_WIN64)
#include <winsock2.h>
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif
#include "../Shared/MGLog.h"
namespace z11
{

	void*  MGThread::_threadLoop(void * lpParameter)
	{
		MGThread *self=static_cast<MGThread*> (lpParameter);
		if (self->m_name!="")
		{
			MGLogD("MGThread %s (%p) start",self->m_name.c_str(),self);
		}
		else
		{
			MGLogD("MGThread %p start",self);
		}
		bool result=true;
        
        self->readyToRun();
        
		do 
		{
            self->thread_lock->lock();
			if (self->m_state == MGThreadStateReadyToSleep && !self->mExitPending)
			{
                self->m_state = MGThreadStateSleeping;
                self->thread_lock->unlock();
				self->m_hMutex->Wait();
                self->thread_lock->lock();
			}
            if (!self->mExitPending) {
                result=self->threadLoop();
            }
            self->thread_lock->unlock();
		} while (result&&!self->mExitPending);
		self->m_state = MGThreadStateFinished;
        self->readyToStop();
		if (self->m_name!="")
		{
			MGLogD("MGThread %s (%p) stop",self->m_name.c_str(),self);
		}
		else
		{
			MGLogD("MGThread %p stop",self);
		}
		return 0;
	}
	MGThread::MGThread(void)
	{
		native_handle=NULL;
		mExitPending=false;
		m_hMutex=new MGSemaphore(0);
        thread_lock = new MGMutex();
        m_state=MGThreadStateReadyToRun;
		priority = 0;
	}

	MGThread::~MGThread(void)
	{
		requestExitAndWait();
		delete_and_set_null(m_hMutex);
		delete_and_set_null(thread_lock);

#if defined(_WIN32) || defined(_WIN64)
		if (native_handle)
		{
			CloseHandle(native_handle);
			native_handle=NULL;
		}
#endif
	}

	void MGThread::requestExit()
	{
        mExitPending=true;
        m_hMutex->Signal();
	}

	int MGThread::readyToRun()
	{
		//do nothing
		return 0;
	}

	int MGThread::run( const char* name /*= 0*/, int priority /*= 0*/, size_t stack /*= 0*/ )
	{
		if (native_handle!=NULL)
		{
			return -1;
		}
		if (name!=NULL)
		{
			m_name=name;
		}
#if defined(_WIN32) || defined(_WIN64)
		native_handle=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)MGThread::_threadLoop,this,CREATE_SUSPENDED,0);
		SetThreadPriority(native_handle, priority);
		ResumeThread(native_handle);
#else
		int err = pthread_create(&native_handle,NULL,MGThread::_threadLoop,this);
        if(err != 0){
            MGLogD("can't create thread: %s\n",strerror(err));
            return -1;
        }
#endif

		m_state = MGThreadStateRunning;
		return 0;
	}

	int MGThread::requestExitAndWait()
	{
		requestExit();

		join();
		return 0;
	}

	bool MGThread::exitPending() const
	{
		return mExitPending;
	}

	void MGThread::pauseAndWait()
	{
        thread_lock->lock();
		if (MGThreadStateRunning == m_state && !mExitPending)
		{
			m_state = MGThreadStateReadyToSleep;
		}
        thread_lock->unlock();
	}
    
    void MGThread::pause()
	{
		if (MGThreadStateRunning == m_state && !mExitPending)
		{
			m_state = MGThreadStateReadyToSleep;
		}
	}

	void MGThread::resume()
	{
		if (MGThreadStateSleeping == m_state)
		{
			m_hMutex->Signal();
		}
        m_state = MGThreadStateRunning;
	}

	bool MGThread::isExecuted()
	{
		return m_state == MGThreadStateRunning || m_state == MGThreadStateReadyToSleep;
	}

	bool MGThread::isPaused()
	{
		return m_state == MGThreadStateSleeping;
	}

	bool MGThread::isFinished()
	{
		return m_state == MGThreadStateFinished;
	}

	int MGThread::readyToStop()
	{
		return 0;
	}

	void MGThread::join()
	{
#if defined(_WIN32) || defined(_WIN64)
		if (native_handle!=NULL)
		{
			WaitForSingleObject(native_handle , INFINITE);
		}
#else
		void* status=NULL;
		pthread_join(native_handle,&status);
#endif
	
	}

	void MGSleep(unsigned long dwMilliseconds)
	{
#if defined(_WIN32) || defined(_WIN64)
		Sleep(dwMilliseconds);
#else
		usleep(dwMilliseconds*1000);
#endif
	}


}


