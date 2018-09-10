#pragma once
#include "../CommonBase/MGStructure.h"
#include <vector>
#include <string>
namespace z11
{
	class MGFont;
	class MGFontLayout
	{
	public:
		MGFontLayout(void);
		virtual ~MGFontLayout(void);
		virtual std::vector<MGRect> getStringWidthAndHeight(const std::string& text)
		{
            std::vector<MGRect> ret;
			return ret;
		}
		virtual void setFont(MGFont *font)
		{
			m_font=font;
		}
		virtual void setLineLength(const float width)
		{
			
		}
	private:
		MGFont *m_font;
	};
}