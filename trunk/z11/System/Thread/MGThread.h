#pragma once

#if defined(_WIN32) || defined(_WIN64)
	
#else
	#include <pthread.h>
#endif

#include "../../CommonBase/CommonDef.h"
#include "../../CommonBase/MGObject.h"
namespace z11
{
#if defined(_WIN32) || defined(_WIN64)
	typedef void * native_handle_type ;
#else
	typedef pthread_t native_handle_type;
#endif

	class MGMutex;
	class MGSemaphore;
	class MGThread:public MGObject
	{
	public:
		MGThread(void);
		virtual ~MGThread(void);
		//开始运行  
		virtual int    run(    const char* name = 0,
			int priority = 0,
			size_t stack = 0);
		//请求退出线程  
		virtual void        requestExit();
		//退出并等待其结束
		int requestExitAndWait();
		//初试化线程所需的资源
		virtual int    readyToRun();
		//暂停  
		void pause();
        // 请求暂停并等待暂停成功 //
        void pauseAndWait();
		//恢复  
		void resume();

		//deteminate thread state
		bool isExecuted();
		bool isPaused();
		bool isFinished();
		// exitPending() returns true if requestExit() has been called.
		bool        exitPending() const;

		virtual int    readyToStop();
		void join();
	protected:
		MGSemaphore *m_hMutex;
        MGMutex *thread_lock;
	private:
		std::string m_name;
		enum MGThreadState
		{
            MGThreadStateReadyToRun,
			MGThreadStateRunning,
			MGThreadStateReadyToSleep,
			MGThreadStateSleeping,
			MGThreadStateFinished
		};
		// 子类必须实现这个方法,这个表示线程的循环  
		virtual bool        threadLoop() = 0;
		native_handle_type native_handle;
		bool           mExitPending;
		static void*  _threadLoop(void * lpParameter);
		MGThreadState m_state;
	public:
		int priority;
	};

	extern void MGSleep(unsigned long dwMilliseconds);
}


