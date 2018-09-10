/*
 * =====================================================================================
 *
 *       Filename:  MITabBarController.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/06/2012 03:24:22 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#include "MIViewController.h"
#include "../MIControls/MITabBar.h"
namespace z11
{
	typedef std::vector<MIViewController *> MIViewControllerArray;
	class MITabBarControllerDelegate;
	class MINavigationController;
	class MITabBarController:public MIViewController
	{
		public :
			MITabBarController():m_tab_bar(NULL),m_more_navigation_controller(NULL)
			{}
			MITabBarControllerDelegate *delegate;

			/**
			 * @brief tabBar :The tab bar view associated with this controller. (read-only)
			 *
			 * @return 
			 */
			const MITabBar *tabBar()
			{
				return m_tab_bar;
			}
			
			/**
			 * @brief &viewControllers :An array of the root view controllers displayed 
			 * by the tab bar interface.
			 *
			 * @return 
			 */
			const MIViewControllerArray &viewControllers()
			{
				return m_view_controllers;
			}

			/**
			 * @brief setViewControllers :Sets the root view controllers of the tab bar controller.
			 *
			 * @param view_controllers : The array of customviewcontrollers to display in the 
			 * tab bar interface
			 */
			void setViewControllers(const MIViewControllerArray &view_controllers)
			{
				m_view_controllers=view_controllers;
			}

			/**
			 * @brief customizableViewControllers :The subset of view controllers managed by this 
			 * tab bar controller that can be customized.
			 *
			 * @return 
			 */
			const MIViewControllerArray & customizableViewControllers()
			{
				return m_customizable_view_controllers;
			}

			/**
			 * @brief moreNavigationController :The view controller that manages the More 
			 * navigation interface. (read-only)
			 *
			 * @return 
			 */
			const MINavigationController *moreNavigationController()
			{
				return m_more_navigation_controller;
			}

			MGPROPERTY(MIViewController * ,m_selected_view_controller,SelectedViewController);
			MGPROPERTY(uint_32, m_selected_index,SelectedIndex);
		private:
			MINavigationController * m_more_navigation_controller;
			MIViewControllerArray m_customizable_view_controllers;
			MIViewControllerArray m_view_controllers;
			MITabBar *m_tab_bar;
	};

	class MITabBarControllerDelegate 
	{
		/**
		 * @brief tabBarControllerShouldSelectViewController :Asks the delegate whether the specified 
		 * view controller should be made active.
		 *
		 * @param tabBarController : The tab bar controller containing viewController.
		 * @param viewController : The view controller belonging to the tab that was tapped by the user.
		 *
		 * @return YES if the view controller’s tab should be selected or NO if the current tab 
		 * should remain active.
		 */
		virtual bool tabBarControllerShouldSelectViewController(MITabBarController *tabBarController,
				MIViewController *viewController)=0;
		
		/**
		 * @brief tabBarControllerDidSelectViewController :Tells the delegate that the user selected 
		 * an item in the tab bar.
		 *
		 * @param tabBarController : The tab bar controller containing viewController. 
		 * @param viewController : The view controller that the user selected
		 */
		virtual void tabBarControllerDidSelectViewController(MITabBarController *tabBarController,
				MIViewController *viewController)=0;

		/**
		 * @brief tabBarControllerWillBeginCustomizingViewControllers :Tells the delegate that the 
		 * tab bar customization sheet is about to be displayed.
		 *
		 * @param tabBarController : The tab bar controller that is being customized.
		 * @param viewControllers : The view controllers to be displayed in the customization sheet.
		 */
		virtual void tabBarControllerWillBeginCustomizingViewControllers(
				MITabBarController *tabBarController,
				const MIViewControllerArray &viewControllers)=0;

		
		/**
		 * @brief tabBarControllerWillEndCustomizingViewControllers :Tells the delegate that 
		 * the tab bar customization sheet is about to be dismissed.
		 *
		 * @param tabBarController : The tab bar controller that is being customized.
		 * @param viewControllers : The viewcontrollers of the tab bar controller.
		 */
		virtual void tabBarControllerWillEndCustomizingViewControllers(
				MITabBarController *tabBarController,
				const MIViewControllerArray & viewControllers)=0;

		/**
		 * @brief tabBarControllerDidCUstomizingViewControllers :Tells the delegate that 
		 * the tab bar customization sheet was dismissed.
		 *
		 * @param tabBarController : The tab bar controller that is being customized.
		 * @param viewControllers : The viewcontrollers of the tab bar controller.
		 */
		virtual void tabBarControllerDidEndCustomizingViewControllers(
				MITabBarController *tabBarController,
				const MIViewControllerArray & viewControllers)=0;
	};
}
