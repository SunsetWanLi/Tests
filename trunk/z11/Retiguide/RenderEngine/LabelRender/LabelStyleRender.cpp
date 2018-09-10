#include "LabelStyleRender.h"

namespace GIS_Embedded
{
void LabelStyleRender::drawString(const std::string& label, int left, int top , int escape)
{
	int width,height;
	getStringExtent(label,&width,&height);

	MGRect label_rect;
	label_rect.x = left,label_rect.y = top;
	label_rect.width = width,label_rect.height = height;

	textOut(label,label_rect,escape);
}

void LabelStyleRender::drawStringCenterXY(const std::string& label, int center_x, int center_y)
{
	int width,height;
	getStringExtent(label,&width,&height);
	MGRect label_rect;
	label_rect.x = center_x - width/2,label_rect.y = center_y - height/2;
	label_rect.width = width,label_rect.height = height;

	textOut(label,label_rect,0);
}

void LabelStyleRender::drawCharacter(const std::string& label, int left, int top, int escape)
{
	int width,height;
	getStringExtent(label,&width,&height);

	MGRect label_rect;
	label_rect.x = left,label_rect.y = top;
	label_rect.width = width,label_rect.height = height;
 
 	textOut(label,label_rect,escape);
}


void LabelStyleRender::textOut(const string& label, const MGRect& rect,int escape)
{
	MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();

	if (graphic_engine == NULL) {
		return;
	}
	graphic_engine->drawText(label,m_pFont,rect);
}

void LabelStyleRender::getStringExtent( const std::string& str, int *width, int *height )
{
	MGSize str_size = m_pFont->getStringWidthAndHeight(str,999999);
	*width = (int)str_size.width,*height = (int)str_size.height;
}

}