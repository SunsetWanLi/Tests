#include "MGFoundation.h"
#include "MIKit.h"
#include "Platform/MGApplication_platform.h"
#include "GUIRootView/GUIRootViewController.h"
using namespace z11;

class AppDelegate : public z11::MGApplication
{
public :
    virtual bool applicationDidFinishLaunching()
    {
        MGApplication::applicationDidFinishLaunching();
        gui_root=GUIRootViewController::getInstance();

        return true;
    }
};

int main(int argc, char *argv[])
{
    z11::MGDevice::setScreenWidthAndHeight(480,740);
    z11::MGBundle::main_bundle_path="/home/lucifer/z11/BuildResult/Samples/Demo";
    AppDelegate delegate;
    MGApplication::sharedApplication().run();
    return 0;
}
