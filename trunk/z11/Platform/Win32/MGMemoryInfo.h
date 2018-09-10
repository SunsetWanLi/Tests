#pragma once
#include "../MGPlatformConfig.h"
#if MG_TARGET_PLATFORM == MG_PLATFORM_WIN32
namespace z11
{
	class MGMemoryInfo
	{
	public:
		static int getCurrentHeapSize();
	};
}
#endif