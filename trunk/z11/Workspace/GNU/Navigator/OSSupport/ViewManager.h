#pragma once
namespace z11
{
	class AppMain;
	class ViewManager
	{
	public:
		~ViewManager(void);
		static ViewManager *getInstance()
		{
			if (_instance==NULL)
			{
				_instance=new ViewManager();
			}
			return _instance;
		}
		void init(void *wnd_handler);
		AppMain *m_app_main;
	private:
		ViewManager(void);
		static ViewManager *_instance;
	};
}