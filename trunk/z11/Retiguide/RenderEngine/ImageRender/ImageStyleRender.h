#ifndef _RENDERENGINE_IMAGERENDER_IMAGERENDERRENDER_H
#define _RENDERENGINE_IMAGERENDER_IMAGERENDERRENDER_H

#include "GraphicCore/MGGraphicEngine.h"
#include "GraphicCore/MGImage.h"

using namespace z11;


namespace GIS_Embedded
{
	class ImageStyleRender
	{
	public:
		static void drawImage( MGImage * image, const MGRect &rect, RGBAColorRef blend, DrawImageMode mode);
 		static void drawImageCenterXY(MGImage* image,long center_x,long center_y); 
		static void drawImageLeftTop(MGImage* image,long left,long top);
		static void drawStretchLeftTop(MGImage* image,long left,long top,long width,long height);
		static void getImageSize(MGImage* image,long *width,long *height);
	};

}
#endif