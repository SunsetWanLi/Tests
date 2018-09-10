#include "ImageStyleRender.h"
#include "../../../GraphicCore/MGImage.h"

using namespace z11;

namespace GIS_Embedded
{
	void ImageStyleRender::drawImage( MGImage * image, const MGRect &rect, RGBAColorRef blend, DrawImageMode mode)
	{
		MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();

		if (graphic_engine == NULL) {
			return;
		}
		image->sendDataToContext();
		graphic_engine->drawImage(image,rect,blend,mode);
	}
	void ImageStyleRender::drawImageCenterXY(MGImage* image,long center_x,long center_y)
	{
		MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();

		if (graphic_engine == NULL) {
			return;
		}

		MGRect rect;
		int width = image->getWidth();
		int height = image->getHeight();
		rect.x = center_x - width/2;
		rect.y = center_y - height/2;
		rect.width = width;
		rect.height = height;

		image->sendDataToContext();
		graphic_engine->drawImage(image,rect);
	}

	void ImageStyleRender::drawStretchLeftTop(MGImage* image,long left,long top,long width,long height)
	{
		MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();

		if (graphic_engine == NULL) {
			return;
		}

		MGRect rect;
		rect.x = left;
		rect.y = left;
		rect.width = width;
		rect.height = width;
		image->sendDataToContext();

		graphic_engine->drawImage(image,rect);

	}

	void ImageStyleRender::drawImageLeftTop(MGImage* image,long left,long top)
	{
		MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();

		if (graphic_engine == NULL) {
			return;
		}

		MGRect rect;
		rect.x = left;
		rect.y = top;
		rect.width = (int)(image->getWidth());
		rect.height = (int)(image->getHeight());
		image->sendDataToContext();

		graphic_engine->drawImage(image,rect);

	}
	void ImageStyleRender::getImageSize(MGImage* image,long *width,long *height)
	{
		*width = (long)(image->getWidth());
		*height = (long)(image->getHeight());

	}
}