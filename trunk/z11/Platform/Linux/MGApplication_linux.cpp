#include "../MGApplication_platform.h"
#include "../MGPlatformMacros.h"
#include "MGEGLView_linux.h"
#include "../../GUI/GuiContainer/GuiContainer.h"
#include "../../GUI/LightGraphic/LightGraphic.h"
#include "../../GUI/Base/MIView.h"
#include "../../GUI/Events/MIEvent.h"
#include "../../System/Shared/MGLog.h"
#include "../../System/Device/MGDevice.h"
#include "../../GUI/Animations/MIAnimationsManager.h"
#include "../../System/Device/MGDevice.h"
#include "../../System/IO/MGBundle.h"
#include "../../System/Shared/MGPath.h"
#include "../../System/Timer/RealTimeLoop.h"
#include <stdio.h>
namespace z11
{

// sharedApplication pointer
MGApplication * MGApplication::sm_pSharedApplication = 0;

bool MGApplication::initInstance()
{
    MGLogD("MGApplication::initInstance");
    GuiContainer::initialise(NULL);
    return true;
}

bool MGApplication::applicationDidFinishLaunching()
{
    MGLogD("MGApplication::applicationDidFinishLaunching");

	light_graphic->readyToRun();
	if (real_time)
	{
		real_time->run();
	}
    LightGraphic::setFps(60);
    return true;
}

MGApplication::MGApplication()
{
    MGLogD("MGApplication start");
	MG_ASSERT(! sm_pSharedApplication);
	sm_pSharedApplication = this;
	//z11::MGBundle::main_bundle_path=MGPath::getInstance()->getCurrentWorkingDir();
}

MGApplication::~MGApplication()
{
	MG_ASSERT(this == sm_pSharedApplication);
	sm_pSharedApplication = NULL;
}

int MGApplication::run()
{

	// Main message loop:
    MGLogD("MGApplication run start");

	// Initialize instance
	if(!initInstance())
	{
		return 0;
	}
	if (!applicationDidFinishLaunching())
	{
		return 0;
	}

	MGEGLView& mainWnd = MGEGLView::sharedOpenGLView();
	//mainWnd.centerWindow();
	//ShowWindow(mainWnd.getHWnd(), SW_SHOW);

    Display *sDisplay=mainWnd.getDisplay();
    while(sDisplay==NULL)
        sleep(1);
	//GuiContainer::disableAnimation();
    while(1)
    {
		z11::GuiContainer::container->handleEvent();
        if (XPending(sDisplay))
        {
            XEvent ev;
            XNextEvent(sDisplay, &ev);
            MGLogD("ev.type=%d\n",ev.type);
            mainWnd.WindowProc(ev);
        }
		else
			z11::GuiContainer::container->light_graphic->threadLoop();
    }
    return 0;
}

void MGApplication::setAnimationInterval(double interval)
{
}

MGApplication::Orientation MGApplication::setOrientation(Orientation orientation)
{
	//do nothing
	return orientation;

	//// swap width and height
	//CCEGLView * pView = CCDirector::sharedDirector()->getOpenGLView();
	//if (pView)
	//{
	//	return (Orientation)pView->setDeviceOrientation(orientation);
	//}
	//return (Orientation)CCDirector::sharedDirector()->getDeviceOrientation();
}

void MGApplication::statusBarFrame(MGRect * rect)
{
	//do nothing
	if (rect)
	{
		// Windows doesn't have status bar.
		*rect = MGRectMake(0, 0, 0, 0);
	}
}

//////////////////////////////////////////////////////////////////////////
// static member function
//////////////////////////////////////////////////////////////////////////
MGApplication& MGApplication::sharedApplication()
{
	MG_ASSERT(sm_pSharedApplication);
	return *sm_pSharedApplication;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void MGApplication::beforeRenderScreen()
{

}
void MGApplication::afterRenderScreen()
{

}
void MGApplication::preserveEvent(z11::MIEvent *event)
{
	GuiContainer::preserveEvent(event);
}
bool MGApplication::initOpenGLES()
{
	MGLogD("initOpenGLES ");
	//initGraphics();
	MGEGLView * pMainWnd = new MGEGLView();
	(! pMainWnd || ! pMainWnd->create(("z11 on Linux "),
		MGDevice::getScreenWidth(), MGDevice::getScreenHeight()));
	//glEnable(GL_TEXTURE_2D);
	return true;
}
//void MGApplication::initialise(WndHandle wnd)
//{
//	GuiContainer::initialise(wnd);
//	//gui_root=GUIRootViewController::getInstance();
//
//	//TODO: add your own initialise code here!
//}
void MGApplication::renderScreen()
{
	MGEGLView& mainWnd = MGEGLView::sharedOpenGLView();
	mainWnd.swapBuffers();
}
void MGApplication::handleSysCallbackEvent(z11::MIEvent *event)
{
    	MGAssert(event != NULL);
	switch (event->_event_type) {
		case BECOME_FIRST_RESPONDER_EVENT: //弹出键盘
			{
				MGLogD("BECOME_FIRST_RESPONDER_EVENT");
				MIView *text_view=static_cast<MIView *>(event->_responder);
				MGRect rect=text_view->getBounds();
				//SipShowIM(SIPF_ON);
				break;
			}
		case RESIGN_FIRST_RESPONDER_EVENT: //收回键盘
			{
				MGLogD("RESIGN_FIRST_RESPONDER_EVENT");
				//SipShowIM(SIPF_OFF);
				break;
			}
		default:
			break;
	}

	//MGAssert(event != NULL,"event != NULL");
//	MGAssert(event != NULL);
	switch (event->_event_type) {
		//case BECOME_FIRST_RESPONDER_EVENT: //µ¯³ö¼üÅÌ
		//	{
		//		MGLogD("BECOME_FIRST_RESPONDER_EVENT");
		//		MIView *text_view=static_cast<MIView *>(event->_responder);
		//		MGRect rect=text_view->getBounds();
		//		SetDrawableRect(rect.x,rect.y,rect.width,rect.height);
		//		SipShowIM(SIPF_ON);
		//		break;
		//	}
		//case RESIGN_FIRST_RESPONDER_EVENT: //ÊÕ»Ø¼üÅÌ
		//	{
		//		MGLogD("RESIGN_FIRST_RESPONDER_EVENT");
		//		SipShowIM(SIPF_OFF);
		//		SetDrawableRect(0,0,MGDevice::getScreenWidth(),MGDevice::getScreenHeight());
		//		break;
		//	}
		default:
			break;
	}

}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//ccLanguageType MGApplication::getCurrentLanguage()
//{
//	int arrayChineseCode[] = {
//		2052, 4100, 1028, 3076, 5124
//	};
//
//	LCID LanguageID = GetUserDefaultLCID();
//	ccLanguageType ret = kLanguageEnglish;
//
//	int nCount = sizeof(arrayChineseCode) / sizeof(int);
//	for (int i = 0; i < nCount; ++i)
//	{
//		if (arrayChineseCode[i] == LanguageID)
//		{
//			ret = kLanguageChinese;
//			break;
//		}
//	}
//
//	return ret;
//}

}
