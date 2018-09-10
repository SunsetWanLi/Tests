#include "MGFoundation.h"
#include "MIKit.h"
#include "GUIRootView/GUIRootViewController.h"
//#include "Soap/SoapDelegate.h"
class  AppDelegate : public z11::MGApplication
{
public:
	virtual bool applicationDidFinishLaunching()
	{
		MGApplication::applicationDidFinishLaunching();
		//MGWebService::setSystemDelegate(new SoapDelegate());
		gui_root=GUIRootViewController::getInstance();
		//GuiContainer::disableAnimation();
		return true;
	}
};
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPTSTR szCmd, INT )
{
	AppDelegate delegate;
	MGApplication::sharedApplication().run();
	return 0;
}