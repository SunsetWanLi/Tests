/*
 * =====================================================================================
 *
 *       Filename:  MIBarItem.h
 *
 *    Description:  BarItem is an abstract superclass for items added to a bar that 
 *					appears at the bottom of the screen.
 *
 *        Version:  1.0
 *        Created:  02/06/2012 12:54:31 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#include <string>
#include "../../CommonBase/CommonDef.h"
#include "../../CommonBase/MGStructure.h"
#include "../Base/MIObject.h"
#include <string>
namespace z11
{
	class MGImage;
	class MIBarItem:public MIObject
	{
		public :
			virtual ~MIBarItem()=0;
			MIBarItem():m_enabled(true),
				image(NULL),
				tag(0)
			{

			}
			bool isEnabled()
			{
				return m_enabled;
			}
			void setImage(MGImage *_image)
			{
				image=_image;
			}
			const MGImage *getImage()
			{
				return image;
			}
			void setImageInsets(const MGEdgeInsets &image_insets)
			{
				m_image_insets=image_insets;
			}
			const MGEdgeInsets &getImageInsets()
			{
				return m_image_insets;
			}
			void setTitle(const std::string &_title)
			{
				m_title=_title;
			}
			const std::string &getTitle()
			{
				return m_title;
			}
			int tag;
		protected:
			bool m_enabled;
			MGImage *image;
			MGEdgeInsets m_image_insets;
			std::string m_title;
	};
}
