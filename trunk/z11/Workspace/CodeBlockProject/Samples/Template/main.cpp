#include "../../../../System/Device/MGDevice.h"
#include "../../../../System/IO/MGBundle.h"
#include "../../../../Application/Samples/WenZhouTong/AppDelegate.h"
using namespace z11;

int main(int argc, char *argv[])
{

    z11::MGDevice::setScreenWidth(480);
    z11::MGDevice::setScreenHeight(640);
    z11::MGBundle::main_bundle_path="/home/lucifer/Projects/z11/BuildResult/Application/Mobile.App";
    AppDelegate delegate;
    MGApplication::sharedApplication().run();
    return 0;
}
