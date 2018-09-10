#include "../MGApplication_platform.h"
#include "../MGPlatformMacros.h"
#include "MGEGLView_wince.h"
#include "../../GUI/GuiContainer/GuiContainer.h"
#include "../../GUI/LightGraphic/LightGraphic.h"
#include "../../GUI/Base/MIView.h"
#include "../../GUI/Events/MIEvent.h"
#include "../..//System/Shared/MGLog.h"
#include "../../GUI/Animations/MIAnimationsManager.h"
#include "../../System/Device/MGDevice.h"
#include "../../System/IO/MGBundle.h"
#include "../../System/Shared/MGPath.h"
#include "../../System/Timer/RealTimeLoop.h"
#include "../../System/Shared/Utility.h"
#include "../../System/MediaPicker/MGMediaPicker.h"
#include "GPS/GPS.h"
#include "Media/CameraTool.h"
#include <sipapi.h>
namespace z11
{

// sharedApplication pointer
MGApplication * MGApplication::sm_pSharedApplication = 0;

MGApplication::MGApplication()
: m_hInstance(NULL)
, m_hAccelTable(NULL)
{
	m_hInstance	= GetModuleHandle(NULL);
	m_nAnimationInterval.QuadPart = 0;
	MG_ASSERT(! sm_pSharedApplication);
	sm_pSharedApplication = this;
	z11::MGDevice::setScreenHeight(GetSystemMetrics(SM_CYSCREEN));
	z11::MGDevice::setScreenWidth(GetSystemMetrics(SM_CXSCREEN));
	z11::MGBundle::main_bundle_path=MGPath::getInstance()->getCurrentWorkingDir();
	z11::MGBundle::storage_card_bundle_path="/Storage Card/";
}

MGApplication::~MGApplication()
{
	MG_ASSERT(this == sm_pSharedApplication);
	sm_pSharedApplication = NULL;
}

int MGApplication::run()
{

	// Main message loop:
	MSG msg;
	LARGE_INTEGER nFreq;
	LARGE_INTEGER nLast;
	LARGE_INTEGER nNow;

	QueryPerformanceFrequency(&nFreq);
	QueryPerformanceCounter(&nLast);

	// Initialize instance and cocos2d.
	if (! initInstance() || ! applicationDidFinishLaunching())
	{
		return 0;
	}

	MGEGLView& mainWnd = MGEGLView::sharedOpenGLView();
	mainWnd.centerWindow();
	ShowWindow(mainWnd.getHWnd(), SW_SHOW);

	while (1)
	{
		if ( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			z11::GuiContainer::container->handleEvent();
			if (WM_QUIT == msg.message)
			{
				// Quit message loop.
				break;
			}
			// Deal with windows message.
			else if (! m_hAccelTable || ! TranslateAccelerator(msg.hwnd, m_hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			GuiContainer::container->light_graphic->threadLoop();
		}

		
	}

	HWND hFindWnd=FindWindow(L"HHTaskBar",0);
	if (hFindWnd)
	{
		ShowWindow(hFindWnd,SW_SHOW);
	}

	return (int) msg.wParam;
}

void MGApplication::setAnimationInterval(double interval)
{
	LARGE_INTEGER nFreq;
	QueryPerformanceFrequency(&nFreq);
	m_nAnimationInterval.QuadPart = (LONGLONG)(interval * nFreq.QuadPart);
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
	PostMessage((HWND)wnd_handle,WM_USER+1,0,0);
}

bool MGApplication::initOpenGLES()
{
	MGLogD("initOpenGLES ");
	//initGraphics();
	MGEGLView * pMainWnd = new MGEGLView();
	(! pMainWnd || ! pMainWnd->Create(TEXT("z11"), 
		MGDevice::getScreenWidth(), MGDevice::getScreenHeight()));
	//glEnable(GL_TEXTURE_2D);
	return true;
}

void MGApplication::renderScreen()
{
	MGEGLView& mainWnd = MGEGLView::sharedOpenGLView();
	mainWnd.swapBuffers();
}
void MGApplication::handleSysCallbackEvent(z11::MIEvent *event)
{
	//MGAssert(event != NULL,"event != NULL");
	MGAssert(event != NULL);
	switch (event->_event_type) {
		case BECOME_FIRST_RESPONDER_EVENT: //µ¯³ö¼üÅÌ  
			{
				MGLogD("BECOME_FIRST_RESPONDER_EVENT");
				MIView *text_view=static_cast<MIView *>(event->_responder);
				MGRect rect=text_view->getBounds();
				//SetDrawableRect(rect.x,rect.y,rect.width,rect.height);
				SipShowIM(SIPF_ON);
				break;
			}
		case RESIGN_FIRST_RESPONDER_EVENT: //ÊÕ»Ø¼üÅÌ     
			{
				MGLogD("RESIGN_FIRST_RESPONDER_EVENT");
				SipShowIM(SIPF_OFF);
				//SetDrawableRect(0,0,MGDevice::getScreenWidth(),MGDevice::getScreenHeight());
				break;
			}
		case START_LOCATION_SERVICE_EVENT:
			{
				MGLogD("try to start GPS");
				wstring config_path = Utility::getCurrentWorkingDirW();
				config_path += L"\\GPS_Config.txt";
				GPS* instance = GPS::getInstance();
				if(instance->startGPS(config_path) != 0)
				{
					MGLogD("fail to start GPS");
					throw exception("fail to start GPS");
				}			
			}
			break;
		case STOP_LOCATION_SERVICE_EVENT:
			{
				GPS* instance  = GPS::getInstance();
				instance->stopGPS();

			}
			break;
		case START_HEADING_SERVICE_EVENT:
			{
				//TODO:
			}
		case STOP_HEADING_SERVICE_EVENT:
			{
				//TODO:
			}
		case MEDIA_PICK_EVENT:
			{
				//TODO:
				MGMediaPickerType* pickerType = static_cast<MGMediaPickerType*>(event->_content);
				CameraTool* camera_open = CameraTool::getInstance();
				camera_open->openMedia(pickerType);
			}
		case MEDIA_DROP_EVENT:
			{
				//TODO:
				CameraTool* camera_close = CameraTool::getInstance();
				camera_close->closeMedia();
			}
        case WEB_SERVICE_BEGIN_EVENT:
        {
            if(event->_content!=NULL)
			{
				MGWebServiceThread *thread=new MGWebServiceThread(static_cast<MGRequestSoapObject *>(event->_content));
				event->_content=NULL; //it's important;
				thread->run();
			}
            break;
        }
        case WEB_SERVICE_END_EVENT:
        {
            if (event->_content != NULL)
            {
                MGWebServiceThread *thread = dynamic_cast<MGWebServiceThread *>(event->_content);
                if (thread != NULL)
                {
                    thread->requestExitAndWait();
                    delete_and_set_null(thread);
                }
                event->_content=NULL;
            }
            break;
        }
		default:
			break;
	}
}

bool MGApplication::applicationDidFinishLaunching()
{
	//light_graphic->run();
	light_graphic->readyToRun();
	real_time->run();
	return true;
}

void MGApplication::applicationDidEnterBackground()
{

}

void MGApplication::applicationWillEnterForeground()
{

}

bool MGApplication::initInstance()
{
	GuiContainer::initialise(NULL);
	GuiContainer::disableAnimation();
	return true;
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
