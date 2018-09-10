#pragma once

#include <winsock2.h>
#include <windows.h>
#include <wingdi.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "MCF/GenericHelpers.h"
#include "MCF/AbstractMessageThreadClass.h"
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "opengl32.lib")

namespace z11 {
	class OpenGLThreadClass sealed : public AbstractMessageThreadClass {
	private:
		const HWND _m_hOwner;
		const long _m_lScreenWidth;
		const long _m_lScreenHeight;

		HGLRC _m_hglrcContext;
	public:
		OpenGLThreadClass(HWND hOwner, long lScreenWidth, long lScreenHeight);
		~OpenGLThreadClass();
	private:
		virtual void _PostCreate();
		virtual bool _DoOnIdle();
		virtual void _DoOnMessage(MSG *lpMsg);
		virtual void _PreTerminate(int nExitCode);
	public:
	};
}
