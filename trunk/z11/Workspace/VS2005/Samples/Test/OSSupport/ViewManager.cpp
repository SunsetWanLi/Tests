#include "AppMain.h"
#include "ViewManager.h"
#include "../../../../../System/Device/MGDevice.h"
#include "../../../../../System/IO/MGBundle.h"
#include "../../../../../System/Shared/MGPath.h"
#include "../../../../../System/Shared/MGLog.h"
namespace z11
{

	ViewManager *ViewManager::_instance=NULL;
	ViewManager::ViewManager(void)
	{
		m_app_main=NULL;
	}

	ViewManager::~ViewManager(void)
	{
	}

	void ViewManager::init(void *wnd_handler)
	{
		if (m_app_main==NULL)
		{
			z11::MGBundle::main_bundle_path=z11::MGPath::getInstance()->getRootDriverPath() + "MobileApp";//"´æ´¢¿¨/Mobile.App";//z11::MGPath::getInstance()->getCurrentWorkingDir();
			MGLogD("%s",z11::MGBundle::main_bundle_path.c_str());
			m_app_main=new AppMain();
			m_app_main->initialise(wnd_handler);
			m_app_main->runLoop();
		}
	}

}
