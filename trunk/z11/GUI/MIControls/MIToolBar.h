/*
 * =====================================================================================
 *
 *       Filename:  MIToolBar.h
 *
 *    Description:  A toolbar is a control that displays one ormore buttons, called toolbar items. 
 *    A toolbar momentarily highlights or does not change the appearance of an item when tapped.
 *
 *        Version:  1.0
 *        Created:  02/06/2012 09:26:51 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#include "../Base/MIView.h"
#include "../Base/MIInterface.h"
namespace z11
{
	typedef std::vector<MIBarButtonItem *> MIBarButtonItemArray;
	class MIToolBar
	{
		public:
			/**
			 * @brief The toolbar style that specifies its appearance.
			 */
			MIBarStyle barStyle;

			/**
			 * @brief tintColor : The color used to tint the bar.
			 *
			 * @return The default value is nil.
			 */
			MGColor *tintColor()
			{
				return m_tint_color;
			}

			/**
			 * @brief isTranslucent : A Boolean value that indicates whether 
			 * the toolbar is translucent (YES) or not (NO).
			 *
			 * @return Applying translucence to a toolbar is intended primarily for 
			 * landscape orientation, when you want the user to be able to view 
			 * the area beneath the toolbar.
			 */
			bool isTranslucent()
			{
				return m_translucent;
			}
			
			/**
			 * @brief items : The items displayed on the toolbar.
			 *
			 * @return The items, instances of UIBarButtonItem, that are visible 
			 * on the toolbar in the order they appear in this array.
			 */
			const MIBarButtonItemArray & items()
			{
				return m_items;
			}
			
			/**
			 * @brief setItems : Sets the items on the toolbar by animating the changes.
			 *
			 * @param _items : The items to display on the toolbar.
			 */
			void setItems(const MIBarButtonItemArray & _items)
			{
				m_items=_items;
			}

			
			MIToolBar()
				:barStyle(MIBarStyleDefault),m_tint_color(NULL),m_translucent(false)
			{}
		private:
			MIBarButtonItemArray m_items;
			bool m_translucent;
			MGColor *m_tint_color;
	}
}
