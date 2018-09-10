#pragma once
class SkDevice;
#include "CGBitmap.h"
namespace z11
{
	class CGDevice
	{
	public:
		CGDevice(const CGBitmap&bitmap);
		~CGDevice(void);
	private:
		SkDevice *impl;
	};
}