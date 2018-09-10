#pragma once
#include "MIViewController.h"
#include "../../System/Thread/MGSemaphore.h"
#include <vector>
namespace z11
{
	class MINavigationBar;
	class MINavigationControllerDelegate ;
	class MINavigationController:public MIViewController ,public Syncable
	{
	public:

		//Creating Navigation Controllers
		static MINavigationController *initWithRootViewController(MIViewController *rootViewController);

		//Accessing Items on the Navigation Stack
		const MIViewController *topViewController();
		const MIViewController *visibleViewController();
		const std::vector<MIViewController *>& viewControllers();
		void setViewControllers(const std::vector<MIViewController *> &_viewControllers);

		//Pushing and Popping Stack Items
		void pushViewController(MIViewController *viewController,bool animated=true);
		MIViewController *popViewController(bool animated=true);
		std::vector<MIViewController *> popToRootViewController();
		std::vector<MIViewController *> popToViewController(MIViewController *viewController );

		//Configuring Navigation Bars
		MINavigationBar *navigationBar();
		bool isNavigationBarHidden();
		void setNavigationBarHidden(bool hidden);

		//Accessing the Delegate
		MGPROPERTY(MINavigationControllerDelegate *,m_delegate,Delegate);

		//Configuring Custom Toolbars
		//toolbar  property
		//¨C setToolbarHidden:animated:
		//toolbarHidden  property
	private:
		MIViewController *popViewController(MIViewController *viewController,bool animated=true);
		MINavigationController(void);
		~MINavigationController(void);
		std::vector<MIViewController *> m_view_controllers;
		MINavigationBar *m_navigationBar;
		void releasePreController();
		void afterPopViewController(z11::MGObject* _sender);
		void afterPushViewController(z11::MGObject* _sender);
		MIViewController *preController;
		MGSemaphore semaphore;
		friend class ControllerSchedulableElement;

		void lockRootViewHit(bool lock);
		void lockViewHit(MIViewController *viewController,bool lock);
	};
	
	class MINavigationControllerDelegate
	{
	public:
		virtual void willShowViewController(MINavigationController *navigationController,MIViewController *viewController )=0;
		virtual void didShowViewController(MINavigationController *navigationController,MIViewController *viewController )=0;
	};
}
