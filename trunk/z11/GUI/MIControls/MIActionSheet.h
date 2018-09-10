/*
 * =====================================================================================
 *
 *       Filename:  MIActionSheet.h
 *
 *    Description:  Use the UIActionSheet class to present the user with a set of alternatives 
 *    for how to proceed with a given task.
 *
 *        Version:  1.0
 *        Created:  02/06/2012 06:11:47 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#pragma once
#include "../Base/MIView.h"
#include "../Base/MIInterface.h"
namespace z11
{
	typedef enum
	{
		MIActionSheetStyleAutomatic			= -1,
		MIActionSheetStyleDefault			= MIBarStyleDefault,
		MIActionSheetStyleBlackOpaque		= MIBarStyleBlack,
		MIActionSheetStyleBlackTranslucent	= MIBarStyleBlack,
	}MIActionSheetStyle;

	class MIActionSheetDelegate;
	class MITabBar;
	class MIBarButtonItem;
	class MIActionSheet:public MIView
	{
		public :
			MIActionSheet();
			/**
			 * @brief initWithTitle : Initializes the action sheet using the specified starting parameters.
			 *
			 * @param title : A string to display in the title area of the action sheet
			 * @param delegate : The receiver’s delegate object.
			 * @param cancelButtonTitle : The title of the cancel button
			 * @param destructiveButtonTitle : The title of the destructive button.
			 * @param otherButtonTitles : The titles of any additional buttons you want to add
			 *
			 * @return A newly initialized action sheet.
			 */
			static MIActionSheet *initWithTitle(const std::string & title,
					MIActionSheetDelegate *delegate,
					const std::string & cancelButtonTitle,
					const std::string & destructiveButtonTitle,
					const std::vector<std::string> & otherButtonTitles);
			
			/**
			 * @brief title : The string that appears in the receiver’s title bar.
			 *
			 * @return 
			 */
			const std::string & title();

			/**
			 * @brief isVisible : A Boolean value that indicates whether the receiver is displayed
			 * . (read-only)
			 *
			 * @return If YES, the receiver is displayed; otherwise, NO.
			 */
			bool isVisible();
			
			/**
			 * @brief addButtonWithTitle : Adds a custom button to the action sheet.
			 *
			 * @param title : The title of the new button.
			 *
			 * @return The index of the new button. Button indices start at 0 and increase 
			 * in the order they are added.
			 */
			int_32 addButtonWithTitle(const string & title)	;

			/**
			 * @brief numberOfButtons : The number of buttons on the action sheet. (read-only)
			 *
			 * @return 
			 */
			int_32 numberOfButtons();

			/**
			 * @brief buttonTitleAtIndex : Returns the title of the button at the specified index.
			 *
			 * @param buttonIndex : The index of the button. The button indices start at 0.
			 *
			 * @return The title of the button specified by index buttonIndex.
			 */
			const std::string & buttonTitleAtIndex(int_32 buttonIndex);

			/**
			 * @brief cancelButtonIndex : The index number of the cancel button.
			 *
			 * @return Button indices start at 0. The default value of this property is normally 
			 * -1, which indicates that no cancel button has been set.
			 */
			int_32 cancelButtonIndex();

			/**
			 * @brief destructiveButtonIndex : The index number of the destructive button.
			 *
			 * @return Button indices start at 0. The default value of this property is normally -1, 
			 * which indicates that no destructive button has been set. 
			 */
			int_32 destructiveButtonIndex()	;

			/**
			 * @brief firstOtherButtonIndex : The index of the first custom button. (read-only)
			 *
			 * @return Button indices start at 0. The default value of this property is -1, 
			 * which indicates that there are no other custom buttons.
			 */
			int_32 firstOtherButtonIndex();

			/**
			 * @brief showFromTabBar : Displays an action sheet that originates from the specified tab bar.
			 *
			 * @param view : The tab bar from which the action sheet originates.
			 */
			void showFromTabBar(MITabBar *view);

			//showFromToolBar
			
			/**
			 * @brief showInView : Displays an action sheet that originates from the specified view.
			 *
			 * @param view : The view from which the action sheet originates.
			 */
			void showInView(MIView *view);

			/**
			 * @brief showFromBarButtonItem : Displays an action sheet that originates from the 
			 * specified bar button item.
			 *
			 * @param item : The bar button item from which the action sheet originates.
			 */
			void showFromBarButtonItem(MIBarButtonItem *item);

			/**
			 * @brief showFromRectInView : Displays an action sheet that originates from the specified view.
			 *
			 * @param rect : The portion of view from which to originate the action sheet.
			 * @param view : The view from which to originate the action sheet.
			 */
			void showFromRectInView(const MGRect &rect ,MIView *view);

			/**
			 * @brief dismissWithClickedButtonIndex : Dismisses the action sheet immediately 
			 * using an optional animation.
			 *
			 * @param buttonIndex : The index of the button that was clicked. Button indices start at 0.
			 */
			void dismissWithClickedButtonIndex(int_32 buttonIndex);
			MIActionSheetStyle		m_actionSheetStyle;
			MIActionSheetDelegate *	m_delegate;
			MIVIEW_DECLARE;
		private:
			std::string m_title;
			std::vector< std::string > m_button_titles;
	};

	class MIActionSheetDelegate
	{
		public :
			/**
			 * @brief actionSheetClickedButtonAtIndex : Sent to the delegate when 
			 * the user clicks a button on an action sheet.
			 *
			 * @param actionSheet : The action sheet containing the button.
			 * @param buttonIndex : The position of the clicked button. The button indices start at 0.
			 */
			virtual void actionSheetClickedButtonAtIndex(MIActionSheet *actionSheet,
					int_32 buttonIndex)=0;

			/**
			 * @brief willPresentActionSheet : Sent to the delegate before an action sheet 
			 * is presented to the user.
			 *
			 * @param actionSheet : The action sheet that is about to be displayed.
			 */
			virtual void willPresentActionSheet(MIActionSheet * actionSheet)=0;

			/**
			 * @brief didPresentActionSheet : Sent to the delegate after an action sheet 
			 * is presented to the user.
			 *
			 * @param actionSheet : The action sheet that was displayed.
			 */
			virtual void didPresentActionSheet(MIActionSheet *actionSheet)=0;

			/**
			 * @brief actionSheetWillDismissWithButtonIndex : Sent to the delegate before 
			 * an action sheet is dismissed.
			 *
			 * @param actionSheet : The action sheet that is about to be dismissed.
			 * @param buttonIndex : The index of the button that was clicked. If this is the 
			 * cancel button index, the action sheet is canceling. If -1, the cancel button 
			 * index is not set.
			 */
			virtual void actionSheetWillDismissWithButtonIndex(MIActionSheet *actionSheet,
					int_32 buttonIndex)=0;
			
			/**
			 * @brief actionSheetWillDismissWithButtonIndex : Sent to the delegate after 
			 * an action sheet is dismissed.
			 *
			 * @param actionSheet : The action sheet that was dismissed.
			 * @param buttonIndex : The index of the button that was clicked. If this is the 
			 * cancel button index, the action sheet is canceling. If -1, the cancel button 
			 * index is not set.
			 */
			virtual void actionSheetDidDismissWithButtonIndex(MIActionSheet *actionSheet,
					int_32 buttonIndex)=0;

			/**
			 * @brief actionSheetCancel : Sent to the delegate before an action sheet is canceled.
			 *
			 * @param actionSheet : The action sheet that will be canceled.
			 */
			virtual void actionSheetCancel(MIActionSheet *actionSheet)=0;
	};
}
