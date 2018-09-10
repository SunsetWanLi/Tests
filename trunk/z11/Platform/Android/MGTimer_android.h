#pragma once
#include "../../System/Timer/MGTimer.h"
/* printf 用头文件 */
#include <stdio.h>
/* pause 用头文件 */
#include <unistd.h>
/* signal 用头文件 */
#include <signal.h>
/* memset 用头文件 */
#include <string.h>
/* struct itimeral 和 setitimer 用头文件 */
#include <sys/time.h>
#include "../../System/Shared/MGLog.h"
#define HANDLE_A_TIMER_EVENT INVALIDATE_A_TIMER_EVENT+100 //just test
namespace z11
{
	void MGTimerAndroidProc(int sig);
//必须确保Android的所有函数在同一个线程中执行，(目前都是主线程) //
	class MGTimerAndroid:public Syncable
	{
	public:
		MGTimerAndroid();
		static MGTimerAndroid *getInstance();
	public:
		void handleEvent(MGTimerTag id);
		void stopTimer(MGTimerTag id);
		void startTimer(MGTimerTag id,int delay,int period);
		
		void add(MGTimerObj *timer);
		void remove(MGTimerObj *timer);
	};
	
}