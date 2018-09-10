/*
 * =====================================================================================
 *
 *       Filename:  MINavigationItem.h
 *
 *    Description:  The UINavigationItem class encapsulates information about a navigation 
 *    item pushed on a UINavigationBar object’s stack.
 *
 *        Version:  1.0
 *        Created:  02/07/2012 01:03:18 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#include "../Base/MIObject.h"
#include "MIBarButtonItem.h"
namespace z11
{
	class MINavigationItem: public MIObject
	{
		/**
		 * @brief initWithTitle : Returns a navigation item initialized with the specified title.
		 *
		 * @param title : The string to set as the navigation item’s title displayed in the center 
		 * of the navigation bar.
		 *
		 * @return A new UINavigationItem object initialized with the specified title.
		 */
		static MINavigationItem *initWithTitle(const std::string &title)
		{
			MINavigationItem *ret=new MINavigationItem();
			return ret;
		}

		/**
		 * @brief The navigation item’s title displayed in the center of the navigation bar.
		 */
		std::string m_title;

		/**
		 * @brief A single line of text displayed at the top of the navigation bar.
		 */
		std::string m_propt;

		/**
		 * @brief The bar button item to use when this item is represented by a back button 
		 * on the navigation bar.(retain)
		 */
		MIBarButtonItem *m_backBarButtonItem;
		
		/**
		 * @brief A Boolean value that determines whether the back button is hidden.
		 */
		bool m_hidesBackButton;

		/**
		 * @brief setHidesBackButton : Sets whether the back button is hidden, optionally 
		 * animating the transition.
		 *
		 * @param hidesBackButton : YES if the back button is hidden when this navigation 
		 * item is the top item; otherwise, NO.
		 * @param animated : YES to animate the transition; otherwise, NO.
		 */
		void setHidesBackButton(bool hidesBackButton,bool animated)
		{
			m_hidesBackButton=hidesBackButton;
		}

		/**
		 * @brief A custom view displayed in the center of the navigation bar when this item is the top item.(retain)
		 */
		MIView * m_titleView;


		/**
		 * @brief A custom bar item displayed on the left of the navigation bar when this item is the top item.
		 */
		MIBarButtonItem * m_leftBarButtonItem;

		/**
		 * @brief A custom bar item displayed on the right of the navigation bar when this item is the top item.
		 */
		MIBarButtonItem * m_rightBarButtonItem;

		/**
		 * @brief setLeftBarButtonItem : Sets the custom bar item, optionally animating the transition to the view. 
		 *
		 * @param item : A custom bar item to display on the left of the navigation bar.
		 * @param animated : YES to animate the transition to the custombar itemwhen this itembecomes the top item; 
		 * otherwise, NO.
		 */
		void setLeftBarButtonItem(MIBarButtonItem *item,bool animated)
		{
			m_leftBarButtonItem=item;
		}

		/**
		 * @brief setRightBarButtonItem : Sets the custom bar item, optionally animating the transition to the view.
		 *
		 * @param item : A custom bar item to display on the right of the navigation bar.
		 * @param animated : YES to animate the transition to the custombar itemwhen this itembecomes the top item; 
		 * otherwise, NO.
		 */
		void setRightBarButtonItem(MIBarButtonItem *item,bool animated)
		{
			m_rightBarButtonItem=item;
		}
	};
}
