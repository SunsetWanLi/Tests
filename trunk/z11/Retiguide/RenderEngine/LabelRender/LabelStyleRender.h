#ifndef _CORE_RENDERSTYLE_LABELSTYLERENDER_H
#define _CORE_RENDERSTYLE_LABELSTYLERENDER_H


#include "GraphicCore/MGGraphicEngine.h"
#include "GraphicCore/MGFont.h"
#include "Retiguide/Common/String_Tools.h"
#include "Retiguide/Common/Share/UserScreenRect.h"
#include "System/Shared/Utility.h"
#include "CommonBase/MGWString.h"

using namespace z11;

namespace GIS_Embedded{
class LabelStyleRender
{

public:
   
	LabelStyleRender( )
	{
		m_is_draw_bg = false;
		m_label_clor = RGBAColorMake(255,0,0);
		m_bg_clor = RGBAColorMake(0,0,0);
        m_font_size=15;
		m_pFont = MGFont::create(m_font_size,m_label_clor);
	}
    LabelStyleRender( const LabelStyleRender & rhs)
    {
        m_is_draw_bg=rhs.m_is_draw_bg;
        m_label_clor=rhs.m_label_clor;
        m_bg_clor=rhs.m_bg_clor;
        m_pFont=MGFont::create(rhs.m_pFont->getFontSize(), rhs.m_pFont->getColor());
    }
    LabelStyleRender & operator =(const LabelStyleRender & rhs)
    {
        if(this!=&rhs)
        {
            m_is_draw_bg=rhs.m_is_draw_bg;
            m_label_clor=rhs.m_label_clor;
            m_bg_clor=rhs.m_bg_clor;
            m_pFont=MGFont::create(rhs.m_pFont->getFontSize(), rhs.m_pFont->getColor());
        }
        return *this;
    }
    ~LabelStyleRender()
    {
        delete_and_set_null(m_pFont);
    }

	void setLabelColor(int r,int g,int b)
	{
		m_label_clor = RGBAColorMake(r,g,b);
	}

	void setLabelBgColor(int color)
	{
		m_bg_clor = RGBAColorMake((color&0xff0000)>>16, (color&0xff00)>>8, (color&0xff));
	}

	void setLabelColor(int color)
	{
		m_label_clor = RGBAColorMake((color&0xff0000)>>16, (color&0xff00)>>8, (color&0xff));
		m_pFont->setColor(m_label_clor);

	}

	void setLabelBgColor(int r,int g,int b)
	{
		m_bg_clor = RGBAColorMake(r,g,b);
	}
    
    void setFontSize(int font_size)
	{
        m_font_size=font_size;
		m_pFont->setFaceSize(font_size);
	}
	int getFontSize()
	{
		return m_pFont->getFontSize();
	}
	RGBAColorRef getLabelColor()
	{
		return m_pFont->getColor();
	}
 	void setFontName(z11::MGWString &font_name)
 	{
		m_font_name = font_name;
 	}
	void setFontWeight(short weight)
	{
		m_font_weight = weight;
	}

	//缁樺埗浠ヤ竴瀹氳搴﹀€炬枩鐨勫瓧绗︿�?
	void drawString(const std::string& label, int left, int top , int escape);

	//缁樺埗浠锛寉涓哄乏涓婅鐨勫瓧�?
	void drawCharacter(const std::string& label, int left, int top, int escape);
    
	//姘村钩缁樺埗浠锛寉涓轰腑蹇冪偣鐨勫瓧绗︿覆
	void drawStringCenterXY(const std::string& label, int center_x, int center_y);
// 	void drawStringCenterXY(const wstring& label, int center_x, int center_y,int length);
    
	void getStringExtent(const std::string& str, int *width, int *height);

      void getStringBound(const std::string &text,UserScreenRect *rect)
  	{
  		rect->_left = 0;
  		rect->_top = 0;
		//long right,bottom;
  		getStringExtent(text,&rect->_right,&rect->_bottom);
  	}

private:
	void textOut(const string& label,const MGRect& rect,int escape);

public:
	bool m_is_draw_bg;
	int m_font_size;
	z11::MGWString m_font_name;
	short m_font_weight;

private:
	MGFont *m_pFont;
	RGBAColorRef m_label_clor;
	RGBAColorRef m_bg_clor;
};
}
#endif
