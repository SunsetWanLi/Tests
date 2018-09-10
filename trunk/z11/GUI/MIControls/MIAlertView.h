/*
 * =====================================================================================
 *
 *       Filename:  MIAlertView.h
 *
 *    Description:  Use the MIAlertView class to display an alert message to the user.
 *
 *        Version:  1.0
 *        Created:  02/06/2012 04:27:22 PM
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
namespace z11
{
	class MILabel;
	class MITextView;
	class MIAlertViewDelegate;
	class MIAlertView:public MIView
	{
		public :


			/**
			 * @brief initWithTitle : Convenience method for initializing an alert view.
			 *
			 * @param title : The string that appears in the receiver’s title bar.
			 * @param message : Descriptive text that provides more details than the title.
			 * @param delegate : The receiver’s delegate or nil if it doesn’t have a delegate.
			 * @param cancelButtonTitle : The title of the cancel button or nil if there is 
			 * no cancel button.
			 * @param otherButtonTitles : The title of another button,should end with NULL
			 *
			 * @return Newly initialized alert view.
			 */
			static MIAlertView *initWithTitle(const std::string &title,
					const std::string & message,
					MIAlertViewDelegate *delegate,
					const std::string & cancelButtonTitle,
					const char *otherButtonTitles,...);

			/**
			 * @brief title : The string that appears in the receiver’s title bar.
			 *
			 * @return 
			 */
			const std::string & title();
			
			/**
			 * @brief message : Descriptive text that provides more details than the title.
			 *
			 * @return 
			 */
			const std::string & message();
			
			/**
			 * @brief isVisible : A Boolean value that indicates whether the receiver is 
			 * displayed. (read-only)
			 *
			 * @return If YES, the receiver is displayed; otherwise, NO.
			 */
			bool isVisible();

			/**
			 * @brief addButtonWithTitle : Adds a button to the receiver with the given title.
			 *
			 * @param title : The title of the new button.
			 *
			 * @return The index of the new button. Button indices start at 0 and increase 
			 * in the order they are added.
			 */
			int_32 addButtonWithTitle(const std::string &title);
			
			/**
			 * @brief numberOfButtons : The number of buttons on the alert view. (read-only)
			 *
			 * @return 
			 */
			int_32 numberOfButtons();

			/**
			 * @brief buttonTitleAtIndex : Returns the title of the button at the given index.
			 *
			 * @param buttonIndex : The index of the button. The button indices start at 0.
			 *
			 * @return : The title of the button specified by index buttonIndex.
			 */
			const std::string & buttonTitleAtIndex(int_32 buttonIndex);

			/**
			 * @brief cancelButtonIndex : The index number of the cancel button.
			 *
			 * @return The button indices start at 0. If -1, then the index is not set.
			 */
			int_32 cancelButtonIndex();

			/**
			 * @brief firstOtherButtonIndex : The index of the first other button. (read-only)
			 *
			 * @return The button indices start at 0. If -1, then the index is not set.
			 */
			int_32 firstOtherButtonIndex();

			/**
			 * @brief show : Displays the receiver using animation.
			 */
			void show();

			/**
			 * @brief dismissWithClickedButtonIndex : Dismisses the receiver, optionally with animation.
			 *
			 * @param buttonIndex : The index of the button that was clicked just before invoking 
			 * this method. The button indices start at 0.
			 */
			void dismissWithClickedButtonIndex(int_32 buttonIndex);

			/**
			 * @brief The receiver’s delegate or nil if it doesn’t have a delegate.
			 */
			MIAlertViewDelegate *delegate;
			MIVIEW_DECLARE;
		private:
			MILabel *m_title_label;
			MITextView *m_message_view;
			MIAlertView();
			std::vector< std::string > m_button_titles;
			bool m_visible;
			std::string m_title;
			std::string m_message;
	};

	class MIAlertViewDelegate
	{
		public:
			/**
			 * @brief alertViewClickedButtonAtIndex : Sent to the delegate when the user 
			 * clicks a button on an alert view.
			 *
			 * @param alertView : The alert view containing the button.
			 * @param buttonIndex : The position of the clicked button. The button indices start at 0.
			 */
			virtual void alertViewClickedButtonAtIndex(MIAlertView *alertView,int_32 buttonIndex)
			{
			}
			
			/**
			 * @brief willPresentAlertView : Sent to the delegate before a model view is presented 
			 * to the user.
			 *
			 * @param alertView : The alert view that is about to be displayed.
			 */
			virtual void willPresentAlertView(MIAlertView *alertView)
			{}

			/**
			 * @brief didPresentAlertView : Sent to the delegate after an alert view is presented 
			 * to the user.
			 *
			 * @param alertView : The alert view that was displayed.
			 */
			virtual void didPresentAlertView(MIAlertView *alertView)
			{}

			/**
			 * @brief alertViewWillDismissWithButtonIndex : Sent to the delegate before an alert 
			 * view is dismissed.
			 *
			 * @param alertView : The alert view that is about to be dismissed.
			 * @param buttonIndex : The index of the button that was clicked. The button indices 
			 * start at 0. If this is the cancel button index, the alert view is canceling. If -1,
			 * the cancel button index is not set.
			 */
			virtual void alertViewWillDismissWithButtonIndex(MIAlertView *alertView,int_32 buttonIndex)
			{}

			/**
			 * @brief alertViewDidDismissWithButtonIndex : Sent to the delegate after an alert view 
			 * is dismissed from the screen.
			 *
			 * @param alertView : The alert view that was dismissed.
			 * @param buttonIndex : The index of the button that was clicked. The button indices 
			 * start at 0. If this is the cancel button index, the alert view is canceling. If -1,
			 * the cancel button index is not set.
			 */
			virtual void alertViewDidDismissWithButtonIndex(MIAlertView *alertView,int_32 buttonIndex)
			{}

			/**
			 * @brief alertViewCancel : Sent to the delegate before an alert view is canceled.
			 *
			 * @param alertView : The alert view that will be canceled.
			 */
			virtual void alertViewCancel(MIAlertView *alertView)
			{}
	};
}
