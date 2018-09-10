#pragma once
#include <zxing/LuminanceSource.h>
namespace z11
{
	class MGImage;
	class MGImageSource:public zxing::LuminanceSource
	{
	private:
		MGImage *image_;
		int width;
		int height;
	public:
		MGImageSource(MGImage *image);
		~MGImageSource();

		int getWidth() const;
		int getHeight() const;
		unsigned char* getRow(int y, unsigned char* row);
		unsigned char* getMatrix();
	};

}