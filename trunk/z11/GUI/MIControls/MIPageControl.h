/*
 * =====================================================================================
 *
 *       Filename:  MIPageControl.h
 *
 *    Description:  You use the UIPageControl class to create and manage page controls
 *
 *        Version:  1.0
 *        Created:  02/06/2012 08:37:51 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#include "MIControl.h"
namespace z11
{
	class MIPageControl:public MIControl
	{
		public :
			MIPageControl()
				:m_current_page(0),hidesForSinglePage(true)
			{}

			/**
			 * @brief currentPage : The current page, shown by the receiver as a white dot.
			 *
			 * @return The property value is an integer specifying the current page shown minus one;
			 */
			int_32 currentPage()
			{
				return m_current_page;
			}

			/**
			 * @brief numberOfPages : The number of pages the receiver shows (as dots).
			 *
			 * @return The value of the property is the number of pages for the page control 
			 * to show as dots.
			 */
			int_32 numberOfPages()
			{
				return m_number_of_pages;
			}

			/**
			 * @brief A Boolean value that controls whether the page indicator is hidden 
			 * when there is only one page.
			 */
			bool hidesForSinglePage;
			
			/**
			 * @brief A Boolean value that controls when the current page is displayed.
			 */
			bool defersCurrentPageDisplay;

			/**
			 * @brief updateCurrentPageDisplay : Updates the page indicator to the current page.
			 */
			void updateCurrentPageDisplay()
			{
			}

			/**
			 * @brief sizeForNumberOfPages : Returns the size the receiver’s bounds should be 
			 * to accommodate the given number of pages.
			 *
			 * @param pageCount : The number of pages to fit in the receiver’s bounds.
			 *
			 * @return The minimum size required to display dots for the page count.
			 */
			virtual MGSize sizeForNumberOfPages(int_32 pageCount)
			{
				return m_size_of_pages.at(pageCount);
			}

			
		private :
			std::vector<MGSize> m_size_of_pages;
			int_32 m_number_of_pages;
			int_32 m_current_page;
	}
}
