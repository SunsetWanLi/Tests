#include "AppMain.h"
#include "ViewManager.h"
#include "Application/Samples/WenZhouTong/GUIRootView/GUIRootView.h"
#include "Application/Samples/WenZhouTong/IndexView/IndexView.h"
#include "System/Device/MGDevice.h"
//#include "../../../../../System/IO/MGBundle.h"
#include "System/Shared/MGPath.h"
#include "GUI/LightGraphic/LightGraphic.h"
#include "System/Shared/MGLog.h"
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
			z11::MGBundle::main_bundle_path="/data/data/com.z11.mobile.framework/files/Android.App";
			m_app_main=new AppMain();
			m_app_main->initialise(wnd_handler);
			//z11::MGBundle::main_bundle_path="/sdcard/com.z11.mobile.framework/files/Android.App";
			m_app_main->light_graphic->readyToRun();
			MGLogD("m_app_main->light_graphic->readyToRun");
		}
	}

}
