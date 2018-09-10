#include "../MGApplication_platform.h"
#include "../MGPlatformMacros.h"
#include "MGEGLView_win32.h"
#include "../../GUI/GuiContainer/GuiContainer.h"
#include "../../GUI/LightGraphic/LightGraphic.h"
#include "../../GUI/Base/MIView.h"
#include "../../GUI/Events/MIEvent.h"
#include "../../System/Shared/MGLog.h"
//#include "../../System/Device/MGDevice.h"
#include "../../GUI/Animations/MIAnimationsManager.h"
#include "../../System/Device/MGDevice.h"
#include "../../System/IO/MGBundle.h"
#include "../../System/Shared/MGPath.h"
#include "../../System/Shared/Utility.h"
//#include "../../System/Timer/RealTimeLoop.h"
#include "../../System/Media/Audio/MGAudioPlayer.h"
#include "../../System/WebService/MGWebServiceThread.h"
#include "Media/MciPlayer/MciPlayerManager.h"
#include <map>
#include <windowsx.h>
#include <vfw.h>
#pragma comment(lib,"vfw32.lib")

namespace z11
{

	// sharedApplication pointer
	MGApplication * MGApplication::sm_pSharedApplication = 0;

	MGApplication::MGApplication()
		: m_hInstance(NULL)
	{
		m_hCapWnd = NULL;
		wnd_type = WINDOWS_NORMAL;
		isSimulateMultiTouch = true;
		title = TEXT("");
		m_hInstance	= GetModuleHandle(NULL);
		m_nAnimationInterval.QuadPart = 0;
		MG_ASSERT(! sm_pSharedApplication);
		sm_pSharedApplication = this;

		string current_working_path ;

		wchar_t szFull[_MAX_PATH];
		::GetModuleFileNameW(NULL, szFull, sizeof(szFull)/sizeof(wchar_t));
		wstring strPath(szFull);
		current_working_path= Utility::wstr2astr(strPath);
		string project_name;
		{
			size_t name_offset=current_working_path.find_last_of('\\');
			project_name=current_working_path.substr(name_offset+1);
			current_working_path = current_working_path.substr(0, name_offset);
			project_name=project_name.substr(0,project_name.find_last_of('.'));
		}
		for (size_t pos=current_working_path.find_first_of('\\');pos!=string::npos;pos=current_working_path.find_first_of('\\'))
		{
			current_working_path[pos]='/';
		}


		std::string _drawable_path = current_working_path + "/drawable";
		if (!z11::Utility::isFileORDirExist(_drawable_path)) 
		{
			string find_string[]={
				"buildresult/win32/bin",
				"BuildResult/win32/bin",
				"win32/bin",
				"Win32/bin",
				//For Apps/
			};

			int size=sizeof(find_string)/sizeof(std::string);
			int index=0;
			bool found_str = false;
			for (index=0;index<size;index++)
			{
				size_t bin_offset=current_working_path.find(find_string[index]);
				if (bin_offset!=string::npos && (bin_offset + find_string[index].size()) == current_working_path.size())
				{
					//using outside folder
					current_working_path.erase(bin_offset);
					current_working_path.append("Resources");
					_drawable_path=current_working_path + "/drawable";
					found_str = true;
					break;
				}
			}
			if (!found_str)
			{
				current_working_path.append("/Resources");
				_drawable_path=current_working_path + "/drawable";
			}
		}
		if (z11::MGBundle::main_bundle_path.size() == 0)
			z11::MGBundle::main_bundle_path = current_working_path;
		if (z11::MGBundle::storage_card_bundle_path.size() == 0)
			z11::MGBundle::storage_card_bundle_path=current_working_path+"/../storage/";
		if (z11::MGBundle::shared_bundle_path.size() == 0)
			z11::MGBundle::shared_bundle_path = "C:/Shared/z11/";

		if (!z11::Utility::isFileORDirExist(z11::MGBundle::mainBundle()->storageCardPathWithFileName("")))
		{
			z11::Utility::createDirOnPath(z11::MGBundle::mainBundle()->storageCardPathWithFileName(""));
		}
        if (!z11::Utility::isFileORDirExist(z11::MGBundle::mainBundle()->documentsPathWithFileName("")))
		{
			z11::Utility::createDirOnPath(z11::MGBundle::mainBundle()->documentsPathWithFileName(""));
		}
        if (!z11::Utility::isFileORDirExist(z11::MGBundle::mainBundle()->tmpPathWithFileName("")))
		{
			z11::Utility::createDirOnPath(z11::MGBundle::mainBundle()->tmpPathWithFileName(""));
		}
        if (!z11::Utility::isFileORDirExist(z11::MGBundle::mainBundle()->tmpPathWithFileName("controls/")))
		{
			z11::Utility::createDirOnPath(z11::MGBundle::mainBundle()->tmpPathWithFileName("controls/"));
		}
        if (!z11::Utility::isFileORDirExist(z11::MGBundle::mainBundle()->resourcesPathWithFileName("")))
		{
			z11::Utility::createDirOnPath(z11::MGBundle::mainBundle()->resourcesPathWithFileName(""));
		}

	}

	MGApplication::~MGApplication()
	{
		MG_ASSERT(this == sm_pSharedApplication);
		sm_pSharedApplication = NULL;
	}

	void initSysInfo(byte szSystemInfo[], UINT &uSystemInfoLen)
	{
		BOOL bException = FALSE;
		BYTE szCpu[16]  = { 0 };
		UINT uCpuID1     = 0U;
		UINT uCpuID2	   = 0U;
		UINT uCpuID3     = 0U;
		UINT uCpuID4	   = 0U;
		__try 
		{
			_asm 
			{
	        mov eax, 0
	        cpuid
	        mov dword ptr szCpu[0], ebx
	        mov dword ptr szCpu[4], edx
		    mov dword ptr szCpu[8], ecx
	        mov eax, 1
			xor ebx, ebx
			xor ecx, ecx
			xor edx, edx
	        cpuid
			mov uCpuID1, eax
			mov uCpuID2, ebx
			mov uCpuID3, ecx
			mov uCpuID4, edx
			}
		}
		__except( EXCEPTION_EXECUTE_HANDLER )
		{
			bException = TRUE;
		}
		 
		if( !bException )
		{
			CopyMemory( szSystemInfo + uSystemInfoLen, &uCpuID1, sizeof( UINT ) );
			uSystemInfoLen += sizeof( UINT );
			CopyMemory( szSystemInfo + uSystemInfoLen, &uCpuID2, sizeof( UINT ) );
			uSystemInfoLen += sizeof( UINT );
			CopyMemory( szSystemInfo + uSystemInfoLen, &uCpuID3, sizeof( UINT ) );
			uSystemInfoLen += sizeof( UINT );
			CopyMemory( szSystemInfo + uSystemInfoLen, &uCpuID4, sizeof( UINT ) );
			uSystemInfoLen += sizeof( UINT );
			UINT len = strlen( ( char* )szCpu );
			CopyMemory( szSystemInfo + uSystemInfoLen, szCpu, len );
			uSystemInfoLen += len;
		}
	}

	void initDeviceID()
	{
		BYTE szSystemInfo[4096];
		UINT uSystemInfoLen = 0;
		initSysInfo(szSystemInfo, uSystemInfoLen);
		stringstream ss;
		for(int index = 0; index < uSystemInfoLen; ++index)
		{
			int value = (int)szSystemInfo[index];
			int first = value/16;
			int second = value%16;
			if(first > 9)
				ss << (char)('A'+first-10);
			else
				ss << first;
			if(second > 9)
				ss << (char)('A'+second-10);
			else
				ss << second;
		}
		MGDevice::setDeviceID(ss.str());
	}

	int MGApplication::run()
	{
		// init DeviceID
		initDeviceID();

		// Main message loop:
		MSG msg;
		LARGE_INTEGER nFreq;
		LARGE_INTEGER nLast;
		//LARGE_INTEGER nNow;

		QueryPerformanceFrequency(&nFreq);
		QueryPerformanceCounter(&nLast);

		// Initialize instance.
		if (! initInstance() || ! applicationDidFinishLaunching())
		{
			return 0;
		}

		while (!GuiContainer::container->light_graphic->exitPending())
		{
			if ( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				/*if (WM_QUIT == msg.message || WM_CLOSE==msg.message)
				{
					ExitProcess(0);
					break;
				}*/
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
			else
			{
				MGSleep(30);
			}


			// liuhao: We simply starts another thread to do OpenGL stuffs. Removed here.
			MGTimerWin32::getInstance()->handle();
		}

		return (int) msg.wParam;
	}

	bool MGApplication::preHandleMessage(MSG& msg)
	{
		return true;
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
	bool MGApplication::afterRenderScreen()
	{
		return true;
	}
	void MGApplication::preserveEvent(z11::MIEvent *event)
	{
		if (event->isImmediatellyHandleEvent())
		{
			MGEGLView& pMainWnd = MGEGLView::sharedOpenGLView();
			pMainWnd.handleEventImmediatelly(event);
			//SendMessage((HWND)wnd_handle,WM_USER+2,0,0);
		}else{
			GuiContainer::preserveEvent(event);
			PostMessage((HWND)wnd_handle,WM_USER+1,0,0);
		}
	}
	bool MGApplication::initOpenGLES()
	{
		MGLogD("initOpenGLES ");
		//initGraphics();
		MGEGLView * pMainWnd = new MGEGLView();
		(! pMainWnd || ! pMainWnd->create(title, MGDevice::getScreenWidth(), MGDevice::getScreenHeight(),wnd_type, isSimulateMultiTouch));
		//glEnable(GL_TEXTURE_2D);
		//wnd_handle = pMainWnd->getHWnd();
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
		//MGAssert(event != NULL,"event != NULL");
		//	MGAssert(event != NULL);
		switch (event->_event_type) {
			//case BECOME_FIRST_RESPONDER_EVENT: //弹出键盘  
			//	{
			//		MGLogD("BECOME_FIRST_RESPONDER_EVENT");
			//		MIView *text_view=static_cast<MIView *>(event->_responder);
			//		MGRect rect=text_view->getBounds();
			//		SetDrawableRect(rect.x,rect.y,rect.width,rect.height);
			//		SipShowIM(SIPF_ON);
			//		break;
			//	}
			//case RESIGN_FIRST_RESPONDER_EVENT: //收回键盘     
			//	{
			//		MGLogD("RESIGN_FIRST_RESPONDER_EVENT");
			//		SipShowIM(SIPF_OFF);
			//		SetDrawableRect(0,0,MGDevice::getScreenWidth(),MGDevice::getScreenHeight());
			//		break;
			//	}
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
		case INIT_AUDIO_PLAY_EVENT:
			{
				if(event->_content!=NULL)
				{
					MGAudioPlayer *player=(MGAudioPlayer*)event->_content;
					MciPlayerManager::init(player);
				}
			}
			break;
		case PLAY_AUDIO_PLAY_EVENT:
			{
				if(event->_content!=NULL)
				{
					MGAudioPlayer *player=(MGAudioPlayer*)event->_content;
					MciPlayerManager::play(player,player->number_of_loops);
				}
			}
			break;
		case PAUSE_AUDIO_PLAY_EVENT:
			{
				if(event->_content!=NULL)
				{
					MGAudioPlayer *player=(MGAudioPlayer*)event->_content;
					MciPlayerManager::pause(player);
				}
			}
			break;
		case STOP_AUDIO_PLAY_EVENT:
			{
				if(event->_content!=NULL)
				{
					MGAudioPlayer *player=(MGAudioPlayer*)event->_content;
					MciPlayerManager::stop(player);
				}
			}
			break;
		case UNINIT_AUDIO_PLAY_EVENT:
			{
				if(event->_content!=NULL)
				{
					MGAudioPlayer *player=(MGAudioPlayer*)event->_content;
					MciPlayerManager::uninit(player);
				}
			}
			break;
		case REWIND_AUDIO_PLAY_EVENT:
			{
				if(event->_content!=NULL)
				{
					MGAudioPlayer *player=(MGAudioPlayer*)event->_content;
					MciPlayerManager::rewind(player);
				}
			}
			break;
		case VALIDATE_A_TIMER_EVENT:
			{
				if (event->_content!=NULL)
				{
					MGTimerObj* obj = (MGTimerObj*)event->_content;
					MGTimerWin32::getInstance()->add(obj);
				}
				//MGLogD("VALIDATE_A_TIMER_EVENT");
			}
			break;
		case INVALIDATE_A_TIMER_EVENT:
			{
				if (event->_content!=NULL)
				{
					MGTimerObj* obj = (MGTimerObj*)event->_content;
					MGTimerWin32::getInstance()->remove(obj);
				}
				//MGLogD("INVALIDATE_A_TIMER_EVENT");
			}
			break;
			case START_CAMERA_CAPTURE_SERVICE_EVENT:
        {
            if(event != NULL)
            {
                MGCaptureDevicePosition pos = MGCaptureDevicePositionUnspecified;
                if (event->_content != NULL)
                {
                    MGCaptureDevicePositionContent* content = dynamic_cast<MGCaptureDevicePositionContent*>(event->_content);
                    if (content != NULL) {
                        pos = content->position;
                    }
                }
                startCameraCapture(pos);
            }
            break;
        }
        case STOP_CAMERA_CAPTURE_SERVICE_EVENT:
        {
            if(event != NULL)
            {
                stopCameraCapture();
            }
            break;
        }
        case ADJUST_CAMERA_CAPTURE_FOCUS_EVENT:
        {
            // Do Nothing, Windows Not Support.
            break;
        }
        case FOREIGNER_SDK_API_CALL_EVENT:
        case FOREIGNER_SDK_API_IMMEDIATELY_CALL_EVENT:
            {
                if (event != NULL && event->_content != NULL) {
                    MGTextContent* content = (MGTextContent*)event->_content;
                    handleForeignerSDKApiCall(content->text_content);
                }
            }
            break;
        case EXIT_APPLICATION_EVENT:
			{
				if (wnd_type & WINDOWS_FORCE_FULL_SCREEN)
				{
					HWND task;
					task=FindWindow(L"Shell_TrayWnd",NULL);  
					ShowWindow(task,SW_SHOW);//显示任务栏
				}
				MGEGLView& egl_view = MGEGLView::sharedOpenGLView();
				egl_view.release();
				//::PostQuitMessage(0);
				//ExitProcess(0);
			}
            break;
		default:
			break;
		}

	}

	unsigned char* YUV2RGB(int &BitCount, unsigned int Compression, unsigned char *YUV2buff, unsigned long dwSize)
	{
		unsigned char* ret = NULL;
		if (Compression == BI_RGB)
			return YUV2buff;
		int Y = 0;
		int U = 0;
		int V = 0;
		int R = 0;
		int G = 0;
		int B = 0;
		switch (BitCount)
		{
		case 12:		// YUV 4:1:1  Y0U0Y1 Y2V0Y3 Y4U1Y5 Y6V1Y7
			{
				assert(dwSize*2%3 == 0);
				ret = new unsigned char[dwSize*4/3];		// 12位YUV转成16位RGB //
				unsigned long pix_count = dwSize*2/3;
				for (unsigned long idx = 0; idx < pix_count; ++idx)
				{
					if (idx%2 == 0) {
						Y = YUV2buff[3*idx/2];
					} else {
						Y = YUV2buff[3*(idx-1)/2+2];
					}
					if (idx%4 == 0)
					{
						U = YUV2buff[(idx/4)*6+1];
						V = YUV2buff[(idx/4)*6+4];
					}
					R = Y + 1.4075 *(V - 128);
					G = Y - 0.3455 *(U - 128) - 0.7169 *(V - 128);
					B = Y + 1.779 *(U - 128);

					R = R>255?255:R;
					G = G>255?255:G;
					B = B>255?255:B;
					R = R<0?0:R;
					G = G<0?0:G;
					B = B<0?0:B;
					ret[idx*2+1] = (R & 0xF8) + (G>>5);
					ret[idx*2] = ((G<<3) & 0xE0) + (B>>3);
				}
				BitCount = 16;
			}
			break;
		case 16:		// YUV 4:2:2  Y0U0 Y1V0 Y2U1 Y3V1
			{
				assert(dwSize%2 == 0);
				ret = new unsigned char[dwSize];
				unsigned long pix_count = dwSize / 2;
				for (unsigned long idx = 0; idx < pix_count; ++idx)
				{
					Y = YUV2buff[2*idx];
					if (idx%2 == 0) {
						U = YUV2buff[(idx/2)*4+1];
						V = YUV2buff[(idx/2)*4+3];
					}
					R = Y + 1.4075 *(V - 128);
					G = Y - 0.3455 *(U - 128) - 0.7169 *(V - 128);
					B = Y + 1.779 *(U - 128);

					R = R>255?255:R;
					G = G>255?255:G;
					B = B>255?255:B;
					R = R<0?0:R;
					G = G<0?0:G;
					B = B<0?0:B;
					ret[idx*2+1] = (R & 0xF8) + (G>>5);
					ret[idx*2] = ((G<<3) & 0xE0) + (B>>3);
				}
			}
			break;
		case 24:		// YUV 4:4:4  YUV YUV
			{
				assert(dwSize%3 == 0);
				ret = new unsigned char[dwSize];
				unsigned long pix_count = dwSize / 3;
				for (unsigned long idx = 0; idx < pix_count; ++idx)
				{
					Y = YUV2buff[2*idx];
					U = YUV2buff[2*idx+1];
					V = YUV2buff[2*idx+2];
					R = Y + 1.4075 *(V - 128);
					G = Y - 0.3455 *(U - 128) - 0.7169 *(V - 128);
					B = Y + 1.779 *(U - 128);

					R = R>255?255:R;
					G = G>255?255:G;
					B = B>255?255:B;
					R = R<0?0:R;
					G = G<0?0:G;
					B = B<0?0:B;
					ret[idx*3] = R;
					ret[idx*3+1] = G;
					ret[idx*3+2] = B;
				}
			}
			break;
		default:
			break;
		}
		return ret;
	}

	static int biCompression = 0;
	static int biBitCount = 0;
	static int biWidth = 0;
	static int biHeight = 0;

	LRESULT CALLBACK capVideoStreamCallback(HWND hWnd, LPVIDEOHDR lpVHdr)
	{
		int bit = biBitCount;
		unsigned char* rgb_buff = YUV2RGB(bit, biCompression, (unsigned char*)lpVHdr->lpData, lpVHdr->dwBufferLength);
		if (rgb_buff == NULL)
			return 0;
		MGImage * capture_image = MGImage::fromMem(bit, biWidth, biHeight, rgb_buff);
		MGCameraCaptureManager::didCameraCaptureOutput(capture_image);
		delete capture_image;
		if (rgb_buff != (unsigned char*)lpVHdr->lpData)
			delete[] rgb_buff;
		return 0;
	}

	void MGApplication::startCameraCapture(MGCaptureDevicePosition pos)
	{
		if (m_hCapWnd != NULL)
			return;
		m_hCapWnd=capCreateCaptureWindow((LPTSTR)TEXT("摄像头视频捕捉程序"),WS_CHILD|WS_VISIBLE,0,0,1,1,(HWND)wnd_handle,0);

		deque<int> drivers;
		for (int idx = 0; idx < 10; ++idx)
		{
			wchar_t name[100];
			wchar_t version[50];
			if (capGetDriverDescription(idx,name,sizeof(name),version,sizeof(version)) == TRUE)
				drivers.push_back(idx);
		}

		if (drivers.size() == 0)
		{
			DestroyWindow(m_hCapWnd);
			m_hCapWnd = NULL;
			MessageBox((HWND)wnd_handle,L"没有找到摄像头！", L"设备无效", MB_OK);
			return;
		}

		BOOL ret = capDriverConnect(m_hCapWnd,*drivers.begin());
		if (ret == FALSE)		// Reconnect for Win7 issue
			ret = capDriverConnect(m_hCapWnd,*drivers.begin());
		if (ret == FALSE)
		{
			capDriverDisconnect(m_hCapWnd);
			DestroyWindow(m_hCapWnd);
			m_hCapWnd = NULL;
			MessageBox((HWND)wnd_handle,L"无法打开摄像头！", L"设备无效", MB_OK);
			return;
		}
		capSetCallbackOnFrame(m_hCapWnd, capVideoStreamCallback);
		LPBITMAPINFO lpbi;
		DWORD dwSize;
		dwSize = capGetVideoFormatSize(m_hCapWnd);
		lpbi = (LPBITMAPINFO)GlobalAllocPtr(GHND, dwSize);
		capGetVideoFormat(m_hCapWnd, lpbi, dwSize);
		biCompression = lpbi->bmiHeader.biCompression;
		biBitCount = lpbi->bmiHeader.biBitCount;
		biWidth = lpbi->bmiHeader.biWidth;
		biHeight = lpbi->bmiHeader.biHeight;
		GlobalFreePtr(lpbi);

		capPreviewRate(m_hCapWnd,30); // 设置预示帧频 //
		capPreview(m_hCapWnd,TRUE); // 设置预示方式 //
		capPreviewScale(m_hCapWnd,false);  
		CAPDRIVERCAPS CapDrvCaps; 
		capDriverGetCaps(m_hCapWnd,&CapDrvCaps,sizeof(CapDrvCaps));
		if(CapDrvCaps.fHasOverlay)      //检查驱动器是否有叠加能力  //
			capOverlay(m_hCapWnd,TRUE);    //启动Overlay模式  //
	}

	void MGApplication::stopCameraCapture()
	{
		if (m_hCapWnd == NULL)
			return;
		capPreview(m_hCapWnd, FALSE);
		capSetCallbackOnFrame(m_hCapWnd, NULL);
		capDriverDisconnect(m_hCapWnd);
		DestroyWindow(m_hCapWnd);
		m_hCapWnd = NULL;
	}
    
    void MGApplication::handleForeignerSDKApiCall(const string& cmd)
    {
        
    }

	bool MGApplication::applicationDidFinishLaunching()
	{
		//GuiContainer::disableAnimation();
		//light_graphic->run();
		light_graphic->readyToRun();
		//real_time->run("MGApplication::real_time_thread");
		return true;
	}

	bool MGApplication::initInstance()
	{
		GuiContainer::initialise(NULL);
		return true;
	}

}
