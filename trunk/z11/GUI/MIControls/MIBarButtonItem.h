/*
 * =====================================================================================
 *
 *       Filename:  MIBarButtonItem.h
 *
 *    Description:  A bar button item is a button specialized for placement on a UIToolbar 
 *    or UINavigationBar object.
 *
 *        Version:  1.0
 *        Created:  02/06/2012 11:33:54 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#include "MIBarItem.h"
#include <set>
namespace z11
{
	typedef enum {
	    MIBarButtonSystemItemDone,
	    MIBarButtonSystemItemCancel,
	    MIBarButtonSystemItemEdit,
	    MIBarButtonSystemItemSave,
	    MIBarButtonSystemItemAdd,
	    MIBarButtonSystemItemFlexibleSpace,
	    MIBarButtonSystemItemFixedSpace,
	    MIBarButtonSystemItemCompose,
	    MIBarButtonSystemItemReply,
	    MIBarButtonSystemItemAction,
	    MIBarButtonSystemItemOrganize,
	    MIBarButtonSystemItemBookmarks,
	    MIBarButtonSystemItemSearch,
	    MIBarButtonSystemItemRefresh,
	    MIBarButtonSystemItemStop,
	    MIBarButtonSystemItemCamera,
	    MIBarButtonSystemItemTrash,
	    MIBarButtonSystemItemPlay,
	    MIBarButtonSystemItemPause,
	    MIBarButtonSystemItemRewind,
	    MIBarButtonSystemItemFastForward,
	    MIBarButtonSystemItemUndo,        // iOS 3.0 and later
	    MIBarButtonSystemItemRedo,        // iOS 3.0 and later
	    MIBarButtonSystemItemPageCurl,    // iOS 4.0 and later
	} MIBarButtonSystemItem;
	
	typedef enum {
		MIBarButtonItemStylePlain,
		MIBarButtonItemStyleBordered,
		MIBarButtonItemStyleDone,
	} MIBarButtonItemStyle;

	class MIView;
	class MIBarButtonItem:public MIBarItem
	{
		public:
			/**
			 * @brief initWithBarButtonSystemItem : Creates and returns a new item containing the specified system item.
			 *
			 * @param systemItem : The system item to use as the first item on the bar
			 * @param handler : The object that receives the action message and 
			 * The action to send to target when this item is selected.
			 *
			 * @return A newly initialized item containing the specified system item
			 */
			static MIBarButtonItem * initWithBarButtonSystemItem(MIBarButtonItem systemItem,SELHandler handler)
			;
			
			/**
			 * @brief initWithCustomView : Creates and returns a new item using the specified custom view.
			 *
			 * @param customView : A custom view representing the item.
			 *
			 * @return Newly initialized item with the specified properties.
			 */
			static MIBarButtonItem * initWithCustomView(MIView *customView)
			;
			
			/**
			 * @brief initWithImage : Creates and returns a new item using the specified image and other properties.
			 *
			 * @param image : The item’s image. If nil an image is not displayed.
			 * @param style : The style of the item
			 * @param handler : The object that receives the action message and 
			 * The action to send to target when this item is selected.
			 *
			 * @return Newly initialized item with the specified properties.
			 */
			static MIBarButtonItem * initWithImage(MGImage *image ,MIBarButtonItemStyle style,SELHandler handler)
			;

			/**
			 * @brief initWithTitle : Creates and returns a new item using the specified title and other properties.
			 *
			 * @param title : The item’s title. If nil a title is not displayed.
			 * @param style : The style of the item
			 * @param handler : The object that receives the action message and 
			 * The action to send to target when this item is selected.
			 *
			 * @return 
			 */
			static MIBarButtonItem * initWithTitle(const std::string &title,MIBarButtonItemStyle style,SELHandler handler)
			;

			MIBarButtonItem()
			;

			/**
			 * @brief customView : A custom view representing the item.
			 *
			 * @return 
			 */
			MIView *customView()
			;

		private:
			/**
			 * @brief The style of the item.
			 */
			MIBarButtonItemStyle style;

			/**
			 * @brief The object that receives an action when the item is selected and
			 * The selector defining the action message to send to the target object when the user taps this bar
			 * button item.
			 */
			SELHandler m_handler;

			/**
			 * @brief The set of possible titles to display on the bar button.
			 */
			std::set<std::string> possibleTitles;


			/**
			 * @brief The width of the item.
			 */
			float width;

	};
}
