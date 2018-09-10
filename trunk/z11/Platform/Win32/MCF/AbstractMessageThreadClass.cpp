#include <winsock2.h>
#include <windows.h>
#include <stdlib.h>
#include <tchar.h>
#include "AbstractMessageThreadClass.h"

// ���캯��������������
AbstractMessageThreadClass::AbstractMessageThreadClass() :
	_m_ReadyEvent(false)
{
}
AbstractMessageThreadClass::~AbstractMessageThreadClass(){
	PostQuitMsg(0);
	WaitAndGetExitCode();
}

// �����Ǿ�̬��Ա������
void AbstractMessageThreadClass::_PostCreate(){
}
int AbstractMessageThreadClass::_ThreadProc(){
	MSG msgDummy;
	PeekMessage(&msgDummy, NULL, WM_USER, WM_USER, PM_NOREMOVE);

	_PostCreate();
	_m_ReadyEvent.Set();
	const int nExitCode = _DoMessageLoop();
	_PreTerminate(nExitCode);

	return nExitCode;
}
int AbstractMessageThreadClass::_DoMessageLoop(){
	MSG Msg;

	for(;;){
		while(!PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE)){
			// ��û����Ϣ��ִ�п���ѭ����
			if(!_DoOnIdle()){
				// ����ѭ�����ط� false������� GetMessage() ����˯��״̬��
				if(!GetMessage(&Msg, NULL, 0, 0)){
					return (int)Msg.wParam;
				}
				break;
			}
		}
		// ��ʱ Msg ��Ч�������Ǵ���Ϣ�������Ƴ��ġ�
		do {
			if(Msg.message == WM_QUIT){
				return (int)Msg.wParam;
			}
			_DoOnMessage(&Msg);
		} while(PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE));
	}
}
void AbstractMessageThreadClass::_PreTerminate(int nExitCode){
	UNREF_PARAMS(nExitCode);
}

void AbstractMessageThreadClass::Create(bool bSuspended){
	_m_ReadyEvent.Clear();
	AbstractThreadClass::Create(false);
	_m_ReadyEvent.Wait();
	if(bSuspended){
		Suspend();
	}
}

void AbstractMessageThreadClass::PostMsg(UINT uMsg, WPARAM wParam, LPARAM lParam) const {
	_s_csLock.Enter();
		const DWORD dwThreadID = GetThreadID();
		if(dwThreadID != 0){
			PostThreadMessage(dwThreadID, uMsg, wParam, lParam);
		}
	_s_csLock.Leave();
}
void AbstractMessageThreadClass::PostQuitMsg(int nExitCode) const {
	PostMsg(WM_QUIT, (WPARAM)nExitCode, 0);
}
