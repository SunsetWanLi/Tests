#pragma once
#include "../../System/Timer/MGTimer.h"
/* printf ��ͷ�ļ� */
#include <stdio.h>
/* pause ��ͷ�ļ� */
#include <unistd.h>
/* signal ��ͷ�ļ� */
#include <signal.h>
/* memset ��ͷ�ļ� */
#include <string.h>
/* struct itimeral �� setitimer ��ͷ�ļ� */
#include <sys/time.h>
#include "../../System/Shared/MGLog.h"
#define HANDLE_A_TIMER_EVENT INVALIDATE_A_TIMER_EVENT+100 //just test
namespace z11
{
	void MGTimerAndroidProc(int sig);
//����ȷ��Android�����к�����ͬһ���߳���ִ�У�(Ŀǰ�������߳�) //
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