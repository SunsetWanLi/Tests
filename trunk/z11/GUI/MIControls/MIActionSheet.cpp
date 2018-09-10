/*
 * =====================================================================================
 *
 *       Filename:  MIActionSheet.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  02/06/2012 10:13:16 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#include "MIActionSheet.h"
namespace z11 {
	MIActionSheet* z11::MIActionSheet::initWithTitle(const std::string& title,
			MIActionSheetDelegate* delegate,
			const std::string& cancelButtonTitle,
			const std::string& destructiveButtonTitle,
			const std::vector<std::string>& otherButtonTitles) 
	{
		MIActionSheet *ret=new MIActionSheet();
		ret->m_button_titles.push_back(cancelButtonTitle);
		ret->m_button_titles.push_back(destructiveButtonTitle);
		ret->m_button_titles.insert(ret->m_button_titles.end(),otherButtonTitles.begin(),otherButtonTitles.end());
		ret->m_delegate = delegate;
		return ret;
	}
	void MIActionSheet::drawself()
	{
		MGGraphicEngine *engine=MGGraphicEngine::sharedGraphicEngine();
		MGRect rect=m_bounds;
		float width=m_bounds.width/4;
		rect.x=rect.x+width;
		rect.width=width*2;
		rect.height=30;
		engine->drawRoundRect(m_bounds,RGBAColorMake(255,255,255));
		for(size_t i=0;i<m_button_titles.size();i++)
		{
			rect.y+=40;
			if(i==cancelButtonIndex())
				engine->drawFilledRoundRect(rect,RGBAColorMake(255,0,0));
			else
				engine->drawFilledRoundRect(rect,RGBAColorMake(128,128,128));

			engine->drawText(m_button_titles[i],NULL,rect);
		}
		
	}
	void MIActionSheet::initWithFrame(MGRect rect)
	{
		MIView::initWithFrame(rect);
	}
	const std::string& z11::MIActionSheet::title() 
	{
		return m_title;
	}
	bool z11::MIActionSheet::isVisible() 
	{
		return true;
	}
	int_32 z11::MIActionSheet::addButtonWithTitle(const string& title) 
	{
		m_button_titles.push_back(title);
		return m_button_titles.size()-1;
	}
	int_32 z11::MIActionSheet::numberOfButtons() 
	{
		return m_button_titles.size();
	}
	const std::string& z11::MIActionSheet::buttonTitleAtIndex(int_32 buttonIndex) 
	{
		return m_button_titles.at(buttonIndex);
	}
	int_32 z11::MIActionSheet::cancelButtonIndex() 
	{
		return 0;
	}
	int_32 z11::MIActionSheet::destructiveButtonIndex() 
	{
		return -1;
	}
	int_32 z11::MIActionSheet::firstOtherButtonIndex() 
	{
		return -1;
	}
	void z11::MIActionSheet::showFromTabBar(MITabBar* view) 
	{
	}
	void z11::MIActionSheet::showInView(MIView* view) 
	{
	}
	void z11::MIActionSheet::showFromBarButtonItem(MIBarButtonItem* item) 
	{
	}
	void z11::MIActionSheet::showFromRectInView(const MGRect& rect, MIView* view) 
	{
	}
	void z11::MIActionSheet::dismissWithClickedButtonIndex(int_32 buttonIndex) 
	{
	}
	MIActionSheet::MIActionSheet():m_delegate(NULL)
	{
        class_name = "MIActionSheet";
	}
	
}
