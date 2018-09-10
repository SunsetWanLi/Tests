#include "MINavigationController.h"
#include "../MIControls/MINavigationBar.h"
#include "../../System/Thread/MGThread.h"
#include "../../System/Shared/MGLog.h"
#include "../../System/Shared/MGTime.h"
#include "../GuiContainer/GuiContainer.h"
#include <sstream>
#include <queue>
#include <cstdio>
using namespace std;
namespace z11
{
	static MINavigationController *g_navigation_controller=NULL;
	enum ScheduleType
	{
		ScheduleTypePush,
		ScheduleTypePop,
		ScheduleTypeBlank
	};
	class ControllerSchedulableElement: public MIViewController
	{
	public:
		ControllerSchedulableElement()
			:m_semaphore(0),
			viewController(NULL),
			type(ScheduleTypeBlank)
		{

		}
		MGSemaphore m_semaphore;
		ScheduleType type;
		virtual void schedule()
		{
			switch (type)
			{
			case ScheduleTypePush:
				push();
				break;
			case ScheduleTypePop:
				pop();
				break;
            default:
                break;
			}
			//MGLogD("m_semaphore.Wait Start");
			m_semaphore.Wait();
			//MGLogD("m_semaphore.Wait Over");
		}

		void afterPopViewController( z11::MGObject* _sender )
		{
			if (g_navigation_controller->m_delegate == NULL)
			{
				delete_and_set_null(g_navigation_controller->preController);
			}
			else
			{
				g_navigation_controller->m_delegate->didShowViewController(g_navigation_controller,g_navigation_controller->preController);
			}
			//MGLogD("m_semaphore.Signal");
			m_semaphore.Signal();
			g_navigation_controller->lockRootViewHit(false);
		}

		void afterPushViewController( z11::MGObject* _sender )
		{
			//ControllerSchedulableElement*_this=dynamic_cast<ControllerSchedulableElement*>(_sender);
			m_semaphore.Signal();
			g_navigation_controller->lockRootViewHit(false);

			//MGLogD("m_semaphore.Signal");
		}
		void pop()
		{
			g_navigation_controller->preController=g_navigation_controller->m_view_controllers.back();

			MIViewController *controller=NULL;
			if (g_navigation_controller->m_view_controllers.size()>1)
			{
				controller=g_navigation_controller->m_view_controllers.back();
				g_navigation_controller->m_view_controllers.pop_back();
			}
			else
			{
				g_navigation_controller->preController=NULL;
				afterPopViewController(NULL);
				return ;
			}
			MIView *from_view=g_navigation_controller->preController->view;
			MIView *to_view=g_navigation_controller->m_view_controllers.back()->view;

			if (true)
			{
				to_view->setAlpha(0.0f);
				to_view->show();
				//stringstream ss;
				char animation_name[512];
				sprintf(animation_name,"%ld,%ld,%lld",(long)(this),(long)(controller),MGTimeTicks::Now().ToInternalValue());
				//ss<<"MINavigationController::popViewController."<<(long)(this)<<(long)(controller)<<MGTime::Now().ToDoubleT();
				MIView::beginAnimation(animation_name);
				MIView::setAnimationCurve(MIViewAnimationOptionShowHideTransitionViews);
				MIView::setAnimationDuration(0.24f);
				from_view->setAlpha(0.0f);
				to_view->setAlpha(1.0f);
				to_view->setRectTransition(MGRectTransitionMake(0, 0, 0));

				SELHandler selector;
				selector.sender = this;
				selector.sel_func = (SELFunc)&ControllerSchedulableElement::afterPopViewController;
				MIView::setAnimationDidStopSelector(this, selector);

				MIView::commitAnimation();
			}

			//return controller;
		}
		void push()
		{

			MINavigationControllerDelegate *m_delegate=g_navigation_controller->getDelegate();
			if (m_delegate!=NULL)
			{
				m_delegate->willShowViewController(g_navigation_controller,viewController);
			}

			MIView *from_view=NULL;
			if (g_navigation_controller->m_view_controllers.size()>1)
			{
				from_view=g_navigation_controller->m_view_controllers.back()->view;
			}
			MIView *to_view=viewController->view;

			g_navigation_controller->m_view_controllers.push_back(viewController);
			if (true)
			{
				g_navigation_controller->m_view_controllers.front()->view->addSubview(to_view);
				if (from_view!=NULL)
				{
					from_view->hide();
				}
				
				to_view->setAlpha(0.0f);
				stringstream ss;
				ss<<"MINavigationController::pushViewController."<<(long)(this)<<(long)(viewController)<<MGTimeTicks::Now().ToInternalValue();
				MIView::beginAnimation(ss.str());
				MIView::setAnimationCurve(MIViewAnimationOptionShowHideTransitionViews);
				MIView::setAnimationDuration(0.24f);
				if (from_view!=NULL)
				{
					from_view->setAlpha(0.0f);
				}
				
				to_view->setAlpha(1.0f);
				to_view->setRectTransition(MGRectTransitionMake(0, 0, 0));

				SELHandler selector;
				selector.sender = this;
				selector.sel_func = (SELFunc)&ControllerSchedulableElement::afterPushViewController;
				MIView::setAnimationDidStopSelector(this, selector);

				MIView::commitAnimation();
			}
		}
		MIViewController *viewController;
	};
	class ControllerSchedulableManager:public MGThread
	{
	public:
		static ControllerSchedulableManager *getInstance()
		{
			if (m_instance == NULL)
			{
				m_instance=new ControllerSchedulableManager();
			}
			return m_instance;
		}
		//retain
		void push(ControllerSchedulableElement *element)
		{
			m_elements.push(element);
			m_semaphore.Signal();
		}
		void schedule(MGObject* sender)
		{
			if (!m_elements.empty())
			{
				m_elements.front()->schedule();
				delete_and_set_null(m_elements.front());
				m_elements.pop();
			}
		}
	private:
		virtual bool threadLoop() 
		{
			m_semaphore.Wait();
			if (!m_elements.empty())
			{
				m_elements.front()->schedule();
				delete_and_set_null(m_elements.front());
				m_elements.pop();
			}
			return true;
		}

		static ControllerSchedulableManager *m_instance;
		ControllerSchedulableManager()
			:m_semaphore(0)
		{
			run("MINavigationController::ControllerSchedulableManager");
		}
		queue<ControllerSchedulableElement *> m_elements;
		MGSemaphore m_semaphore;
	};
	ControllerSchedulableManager *ControllerSchedulableManager::m_instance=NULL;


	MINavigationController::MINavigationController(void)
		:semaphore(1)
	{
		m_navigationBar=new MINavigationBar();
		m_delegate=NULL;
		preController=NULL;
		g_navigation_controller=this;
	}

	MINavigationController::~MINavigationController(void)
	{
	}

	MINavigationController * MINavigationController::initWithRootViewController( MIViewController *rootViewController )
	{
		MINavigationController *navigation_controller=new MINavigationController();
		navigation_controller->m_view_controllers.push_back(rootViewController);
		return navigation_controller;
	}

	const MIViewController * MINavigationController::topViewController()
	{
		AUTO_LOCK_THIS;
		return m_view_controllers.back();
	}

	const MIViewController * MINavigationController::visibleViewController()
	{
		AUTO_LOCK_THIS;
		return m_view_controllers.back();
	}

	const vector<MIViewController *>& MINavigationController::viewControllers()
	{
		return m_view_controllers;
	}

	void MINavigationController::setViewControllers( const vector<MIViewController *> &_viewControllers )
	{
		m_view_controllers=_viewControllers;
	}

	void MINavigationController::pushViewController( MIViewController *viewController,bool animated )
	{
		AUTO_LOCK_THIS;
		ControllerSchedulableElement *element = new ControllerSchedulableElement();
		element->type=ScheduleTypePush;
		element->viewController=viewController;
		lockRootViewHit(true);
		ControllerSchedulableManager::getInstance()->push(element);
	}

	MIViewController * MINavigationController::popViewController( bool animated )
	{
		AUTO_LOCK_THIS;
		ControllerSchedulableElement *element = new ControllerSchedulableElement();
		element->type=ScheduleTypePop;
		element->viewController=NULL;
		//const MIViewController *viewController=topViewController();
		lockRootViewHit(true);
		ControllerSchedulableManager::getInstance()->push(element);
		return NULL;
	}

	MIViewController * MINavigationController::popViewController( MIViewController *viewController,bool animated/*=true*/ )
	{
		AUTO_LOCK_THIS;
		ControllerSchedulableElement *element = new ControllerSchedulableElement();
		element->type=ScheduleTypePop;
		element->viewController=NULL;
		lockRootViewHit(true);
		ControllerSchedulableManager::getInstance()->push(element);
		return NULL;
	}

	std::vector<MIViewController *> MINavigationController::popToRootViewController()
	{
		//TODO: 
		return m_view_controllers;
	}

	std::vector<MIViewController *> MINavigationController::popToViewController( MIViewController *viewController )
	{
		//TODO:
		return m_view_controllers;
	}

	MINavigationBar * MINavigationController::navigationBar()
	{
		return m_navigationBar;
	}

	bool MINavigationController::isNavigationBarHidden()
	{
		return m_navigationBar->isHide();
	}

	void MINavigationController::setNavigationBarHidden( bool hidden )
	{
		if (hidden)
			m_navigationBar->hide();
		else
			m_navigationBar->show();
	}

	void MINavigationController::releasePreController()
	{

	}

	void MINavigationController::lockViewHit(MIViewController *viewController, bool lock )
	{

		if(viewController&&viewController->view)
		{
			viewController->view->_view_flags.user_interaction_enabled=(!lock);
		}
	}

	void MINavigationController::lockRootViewHit( bool lock )
	{
		if (!m_view_controllers.empty())
		{
			lockViewHit(m_view_controllers[0],lock);
		}
	}
}
