#include "AppDelegate.h"
#include "MGFoundation.h"
#include "MIKit.h"

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPTSTR szCmd, INT )
{
	AppDelegate delegate;
	MGApplication::sharedApplication().run();
	return 0;
}