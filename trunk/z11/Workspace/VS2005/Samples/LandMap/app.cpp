#include "MGFoundation.h"
#include "MIKit.h"
#include "Platform/MGApplication_platform.h"
#include "GUIRootView/GUIRootViewController.h"

using namespace z11;
class  AppDelegate : public z11::MGApplication
{
public:
	virtual bool applicationDidFinishLaunching()
	{
		MGApplication::applicationDidFinishLaunching();
		gui_root=GUIRootViewController::getInstance();
		return true;
	}
};

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPTSTR szCmd, INT )
{
	AppDelegate delegate;
	MGApplication::sharedApplication().run();
	return 0;
}