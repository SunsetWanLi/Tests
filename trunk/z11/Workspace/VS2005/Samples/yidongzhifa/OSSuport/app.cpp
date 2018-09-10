#pragma comment(linker, "/nodefaultlib:oldnames.lib")
#include <windows.h>
#include <aygshell.h>
#pragma comment(lib, "aygshell.lib")
//#include "GraphicCore/DeviceFactory.h"
#include "app.h"
#include "ViewManager.h"
#include "appMain.h"
#include "MGFoundation.h"

HWND	g_mWnd = NULL;				// 主窗体句柄

extern "C"
{

	extern void OnMouseDown(unsigned long wParam, long lParam);
	extern void OnMouseUp(unsigned long wParam, long lParam);
	extern void OnMouseMove(unsigned long wParam, long lParam);
	extern void OnChar(unsigned long wParam, long lParam);
	extern void OnKeyDown(unsigned long code);
}
extern void OnBlit(unsigned long wParam, long lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT ret = 0;
	//DWORD dwNum;
	switch (message) 
	{
	case WM_LBUTTONUP:
		OnMouseUp(wParam,lParam);
		break;
	case WM_LBUTTONDOWN:
		OnMouseDown(wParam,lParam);
		break;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam,lParam);
		break;
	case WM_DESTROY:		
		PostQuitMessage(1);
		break;
	case WM_CHAR:
		//接受输入法输入字符,插入当前输入控件
		//注意两个消息体不一样，虽然可能值一样，但一个是WinCE系统消息，一个是GISEngine控件层消息
		OnChar(wParam,lParam);
		break;
	case WM_BLIT:
		OnBlit(wParam,lParam);
		break;
	case WM_KEYDOWN:
		if (wParam==VK_ESCAPE )
		{
			PostQuitMessage(1);
		}
		else
		{
			OnKeyDown(wParam);
		}
		break;
	case WM_USER+1:
		GuiContainer::container->handleEvent();
		break;
		
	}
	ret = DefWindowProc(hWnd, message, wParam, lParam);
	return ret;
}

//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
extern bool initEGL(void* wnd);
extern void deinitEGL();
extern void appInit();
extern void appDeinit();

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPTSTR szCmd, INT )
{
	WNDCLASS wc = { CS_HREDRAW | CS_VREDRAW, WndProc, 0L, 0L, 
		hInst, NULL, NULL, (HBRUSH) GetStockObject(BLACK_BRUSH), NULL,
		TEXT("Loader class!") };
	RegisterClass( &wc );
	int iScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int iScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	z11::MGDevice::setScreenHeight(iScreenHeight);
	z11::MGDevice::setScreenWidth(iScreenWidth);
	// Create the application's window
	HWND hWnd = CreateWindow(TEXT("Loader class!"), 
		TEXT("Loader"), 
		WS_VISIBLE, 
		0, 0, iScreenWidth, iScreenHeight,
		NULL, NULL, wc.hInstance, NULL );
	g_mWnd = hWnd;
	SHFullScreen(hWnd, SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON | SHFS_HIDESTARTICON);
	HWND hFindWnd=FindWindow(L"HHTaskBar",0);
	if (hFindWnd)
	{
		ShowWindow(hFindWnd,SW_HIDE);
	}

	SetFocus(hWnd);

	z11::ViewManager *view_manager=z11::ViewManager::getInstance();
	if (view_manager!=NULL)
	{
		view_manager->init(hWnd);
	}

	MSG msg; 
	while( GetMessage( &msg, NULL, 0, 0 ) )
	{
		//z11::ViewManager::getInstance()->m_app_main->handleEvent();
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	//appDeinit();
	//deinitEGL();

	hFindWnd=FindWindow(L"HHTaskBar",0);
	if (hFindWnd)
	{
		ShowWindow(hFindWnd,SW_SHOW);
	}

	return 0;
}