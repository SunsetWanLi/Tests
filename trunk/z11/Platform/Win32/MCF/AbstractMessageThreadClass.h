#pragma once

#include <winsock2.h>
#include <windows.h>
#include "GenericHelpers.h"
#include "AbstractThreadClass.h"

class AbstractMessageThreadClass : public AbstractThreadClass {
private:
	Event _m_ReadyEvent;
public:
	AbstractMessageThreadClass();
	virtual ~AbstractMessageThreadClass();
private:
	virtual int _ThreadProc();
private:
	int _DoMessageLoop();
protected:
	virtual void _PostCreate();
	virtual bool _DoOnIdle() = 0;
	virtual void _DoOnMessage(MSG *lpMsg) = 0;
	virtual void _PreTerminate(int nExitCode);
public:
	void Create(bool bSuspended);

	void PostMsg(UINT uMsg, WPARAM wParam, LPARAM lParam) const;
	void PostQuitMsg(int nExitCode) const;
};
