/*
 * =====================================================================================
 *
 *       Filename:  MIAlertView.cpp
 *
 *    Description:  Use the MIAlertView class to display an alert message to the user.
 *
 *        Version:  1.0
 *        Created:  02/08/2012 11:08:07 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
 #include "MIAlertView.h"
#include "../../System/Shared/MGLog.h"
#include <stdarg.h>
namespace z11 {
	z11::MIAlertView::MIAlertView():delegate(NULL) {
        class_name = "MIAlertView";
		m_title_label=NULL;
		m_message_view=NULL;
	}
	
	MIAlertView *z11::MIAlertView::initWithTitle(const std::string& title, 
			const std::string& message, 
			MIAlertViewDelegate* delegate, 
			const std::string& cancelButtonTitle, 
			const char *otherButtonTitles,...) 
	{
		MIAlertView *ret= new MIAlertView();
		ret->m_title=title;
		ret->m_message=message;
		ret->delegate=delegate;
		ret->m_button_titles.push_back(cancelButtonTitle);
		va_list arg_ptr;
		va_start(arg_ptr,otherButtonTitles);
		while(otherButtonTitles!=NULL)
		{
			ret->m_button_titles.push_back(otherButtonTitles);
			otherButtonTitles=va_arg(arg_ptr, const char *);
		}
		va_end(arg_ptr);
		//for(size_t i=0;i<ret->m_button_titles.size();i++)
		//	MGLogD(ret->m_button_titles[i].c_str());
		return ret;
	}
	void MIAlertView::drawself()
	{
//		MGLogD("MIAlerView::drawself");
		MGGraphicEngine *engine = MGGraphicEngine::sharedGraphicEngine();
		engine->drawFilledRoundRect(m_bounds,RGBAColorMake(128,128,128,128));

	}
	void MIAlertView::initWithFrame(MGRect rect)
	{
		MIView::initWithFrame(rect);

		//MGRect _rect=rect;
		//m_title_label	= new MILabel();
		//m_title_label->setText(m_title);
		//_rect.x+=rect.width/4;
		//_rect.width=rect.width/2;
		//_rect.y+=rect.height/4;
		//_rect.height=rect.height/8;
		//m_title_label->initWithFrame(_rect);

		//m_message_view	= new MITextView();
		//m_message_view->setText(m_message);
		//m_message_view->initWithFrame(_rect);
	}
	const std::string& z11::MIAlertView::title() {
		return m_title;
	}
	const std::string& z11::MIAlertView::message() {
		return m_message;
	}
	bool z11::MIAlertView::isVisible() {
		return m_visible;
	}
	int_32 z11::MIAlertView::addButtonWithTitle(const std::string& title) {
		int_32 index_of_new_button=m_button_titles.size();
		m_button_titles.push_back(title);
		return index_of_new_button;
	}
	int_32 z11::MIAlertView::numberOfButtons() {
		return m_button_titles.size();
	}
	const std::string& z11::MIAlertView::buttonTitleAtIndex(int_32 buttonIndex) {
		return m_button_titles.at(buttonIndex);
	}
	int_32 z11::MIAlertView::cancelButtonIndex() {
		return 0;
	}
	int_32 z11::MIAlertView::firstOtherButtonIndex() {
		return -1;
	}
	void z11::MIAlertView::show() {
	}
	void z11::MIAlertView::dismissWithClickedButtonIndex(int_32 buttonIndex) {
	
	}
}
