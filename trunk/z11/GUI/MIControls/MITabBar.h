/*
 * =====================================================================================
 *
 *       Filename:  MITabBar.h
 *
 *    Description:  TabBar class implements a control for selecting one of two or more buttons ,
 *					called items
 *
 *        Version:  1.0
 *        Created:  02/06/2012 12:39:23 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#include <vector>
#include "../Base/MIView.h"
#include "MITabBarItem.h"
namespace z11
{
	class MITabBarDelegate ;
	typedef std::vector<MITabBarItem *> MITabBarItemArray;
	class MITabBar: public MIView
	{
		public:
			MITabBarDelegate *delegate;
			/**
			 * @brief items :The items displayed on the tab bar.
			 *
			 * @return the array of the items
			 */
			std::vector<MITabBarItem *> items()
			{
				return m_items;
			}
			MGPROPERTY( MITabBarItem * ,m_selectedItem,selectedItem);
			/**
			 * @brief setItems : Sets the items on the tab bar
			 *
			 * @param items: The items to display on the tab bar.
			 */
			void setItems(const std::vector<MITabBarItem *>& items)
			{
				m_items=items;
			}

			/**
			 * @brief beginCustomizingItems :Presents a modal view allowing the user to 
			 *		  customize the tab bar by adding, removing, and rearranging
			 *
			 * items on the tab bar.
			 *
			 * @param items: The items to display on the modal view that can be rearranged.
			 */
			void beginCustomizingItems(const std::vector<MITabBarItem *>& items)
			{
				//@todo do nothing
			}
			
			/**
			 * @brief endCustomizingAnimated :Dismisses the modal view used to modify items on the tab bar.
			 *
			 * @param animated: If YES, animates the transition; otherwise, does not.
			 *
			 * @return YES if items on the tab bar changed; otherwise, NO.
			 */
			bool endCustomizingAnimated(bool animated)
			{
				return true;
			}

			/**
			 * @brief isCustomizing : Returns a Boolean value indicating whether the user 
			 *	is customizing the tab bar.
			 *
			 * @return : YES if the user is currently customizing the items on the tab bar; otherwise, NO.
			 */
			bool isCustomizing()
			{
				return m_is_customizing;
			}

		protected:
			std::vector<MITabBarItem *> m_items;
			bool m_is_customizing;
	};


	class MITabBarDelegate
	{
		public :
			/**
			 * @brief Sent to the delegate before the customizing modal view is displayed.
			 *
			 * @param tabBar The tab bar that is being customized.
			 * @param items The items on the customizing modal view.
			 */
			virtual void tabBarWillBeginCustomizingItems(MITabBar *tabBar,const MITabBarItemArray& items)=0;
			
			/**
			 * @brief tabBarDidBeginCustomizingItems :Sent to the delegate after the 
			 * customizing modal view is displayed.
			 *
			 * @param tabBar The tab bar that is being customized.
			 * @param items The items on the customizing modal view.
			 */
			virtual void tabBarDidBeginCustomizingItems(MITabBar *tabBar,const MITabBarItemArray &items)=0;

			/**
			 * @brief tabBarWillEndCustomizingItems :Sent to the delegate before the 
			 * customizing modal view is dismissed.
			 *
			 * @param tabBar The tab bar that is being customized.
			 * @param items The items on the customizing modal view.
			 */
			virtual void tabBarWillEndCustomizingItems(MITabBar *tabBar,const MITabBarItemArray &items)=0;

			/**
			 * @brief tabBarDidEndCustomizingItems :Sent to the delegate after the 
			 * customizing modal view is dismissed.
			 *
			 * @param tabBar The tab bar that is being customized.
			 * @param items The items on the customizing modal view.
			 */
			virtual void tabBarDidEndCustomizingItems(MITabBar *tabBar,const MITabBarItemArray &items)=0;
			
			/**
			 * @brief tabBarDidSelectItem :Sent to the delegate when the user selects 
			 * a tab bar item. (required)
			 *
			 * @param tabBar The tab bar that is being customized.
			 * @param item The tab bar item that was selected.
			 */
			virtual void tabBarDidSelectItem(MITabBar *tabBar,MITabBarItem *item)=0;
	};
}
