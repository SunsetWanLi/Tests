#include "MGFoundation.h"
#include "MIKit.h"
#include "Platform/MGApplication_platform.h"
#include "Application/Samples/AIT/AppDelegate/AppDelegate.h"
using namespace z11;

int main(int argc, char *argv[])
{
    z11::MGDevice::setScreenWidthAndHeight(480,740);
    z11::MGBundle::main_bundle_path="/home/lucifer/z11/BuildResult/Samples/AIT";
    AppDelegate delegate;
    MGApplication::sharedApplication().run();
    return 0;
}
