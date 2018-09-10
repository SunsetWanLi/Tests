
#include "MIKit.h"
#include "MGFoundation.h"
#include "AppMain.h"
#include "ViewManager.h"

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
			m_app_main=new AppMain();
			m_app_main->initialise(wnd_handler);
			m_app_main->runLoop();
		}
	}
}
