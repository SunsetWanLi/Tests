//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (MINavigationBar.h)  
/// @brief (±¾ÎÄ¼þÊµÏÖµÄ¹¦ÄÜµÄ¼òÊö)  
///  
///(±¾ÎÄ¼þÊµÏÖµÄ¹¦ÄÜµÄÏêÊö)  
///  
/// @version 0.1   (°æ±¾ÉùÃ÷)  
/// @author        (Gu RongFang)  
/// @date          (11-11-11)  
///  
///  
///    ÐÞ¶©ËµÃ÷£º×î³õ°æ±¾  
////////////////////////////////////////////////////////////////////////// 
#ifndef _MINAVIGATIONBAR_H_
#define _MINAVIGATIONBAR_H_
#pragma once
#include "../Base/MIView.h"
#include "../Base/MIInterface.h"
#include "MINavigationItem.h"
namespace z11
{
	typedef std::vector <MINavigationItem *> MINavigationItemArray;
	class MINavigationBarDelegate;
	class MINavigationBar :public MIView
	{
	public:
		MINavigationBar(void);
		~MINavigationBar(void);

		/**
		 * @brief The appearance of the navigation bar.
		 */
		MIBarStyle barStyle;		

		/**
		 * @brief The color used to tint the bar.
		 */
		MGColor *tintColor;

		/**
		 * @brief A Boolean value indicating whether the navigation bar is only partially opaque.
		 */
		bool translucent;

		/**
		 * @brief The navigation bar¡¯s delegate object.
		 */
		MINavigationBarDelegate *delegate;
		
		/**
		 * @brief pushNavigationItem : Pushes the given navigation item onto the receiver¡¯s stack 
		 * and updates the navigation bar.
		 *
		 * @param item : The navigation item to push on the stack.
		 * @param animated : YES if the navigation bar should be animated; otherwise, NO.
		 */
		void pushNavigationItem(MINavigationItem *item,bool animated)
		{

		}

		/**
		 * @brief popNavigationItem : Pops the top item from the receiver¡¯s stack and updates the navigation bar.
		 *
		 * @param animated : YES if the navigation bar should be animated; otherwise, NO.
		 */
		void popNavigationItem(bool animated)
		{

		}

		/**
		 * @brief setItems : Replaces the navigation items currently managed by the navigation 
		 * bar with the specified items.
		 *
		 * @param items : The UINavigationItem objects to place in the stack.
		 */
		void setItems(const MINavigationItemArray & items)
		{
			m_items=items;
		}

		/**
		 * @brief topItem : The navigation item at the top of the navigation bar¡¯s stack. (read-only)
		 *
		 * @return 
		 */
		MINavigationItem * topItem()
		{
			return m_items.back();
		}

		/**
		 * @brief backItem : The navigation item that is immediately below the topmost item 
		 * on navigation bar¡¯s stack. (read-only)
		 *
		 * @return 
		 */
		MINavigationItem * backItem()
		{
			return m_items.front();
		}
		/**
		 * @brief An array of navigation items managed by the navigation bar.
		 */
		std::vector<MINavigationItem *> m_items;
	};

	class MINavigationBarDelegate
	{
		public :
			/**
			 * @brief navigationBarShouldPushItem : Returns a Boolean value indicating 
			 * whether the navigation bar should push an item.
			 *
			 * @param navigationBar : The navigation bar that the item is being pushed onto.
			 * @param item : The navigation item that is being pushed.
			 *
			 * @return : YES if the item should be pushed; otherwise, NO.
			 */
			virtual bool navigationBarShouldPushItem(MINavigationBar *navigationBar,MINavigationItem *item)
			{
				return true;
			}

			/**
			 * @brief navigationBarDidPushItem : Tells the delegate that an item was pushed onto the navigation bar.
			 *
			 * @param navigationBar : The navigation bar that the item is being pushed onto.
			 * @param item : The navigation item that is being pushed.
			 */
			virtual void navigationBarDidPushItem(MINavigationBar *navigationBar, MINavigationItem *item)
			{

			}

			/**
			 * @brief navigationBarShouldPopItem : Returns a Boolean value indicating whether the navigation 
			 * bar should pop an item.
			 *
			 * @param navigationBar : The navigation bar that the item is being popped from.
			 * @param item : The navigation item that is being popped.
			 *
			 * @return YES if the item should be popped; otherwise, NO.
			 */
			virtual bool navigationBarShouldPopItem(MINavigationBar *navigationBar,MINavigationItem *item)
			{
				return true;
			}

			/**
			 * @brief navigationBarDidPopItem : Tells the delegate that an item was popped from the navigation bar.
			 *
			 * @param navigationBar : The navigation bar that the item is being popped from.
			 * @param item : The navigation item that is being popped.
			 */
			virtual void navigationBarDidPopItem(MINavigationBar *navigationBar,MINavigationItem *item)
			{}
	};

}

#endif //_MINAVIGATIONBAR_H_
