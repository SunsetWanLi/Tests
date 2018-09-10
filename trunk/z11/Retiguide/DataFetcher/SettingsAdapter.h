#ifndef _SETTINGS_ADAPTER_H_
#define _SETTINGS_ADAPTER_H_

#include "../Common/Types/Settings.h"

namespace GIS_Embedded
{
	class SettingsAdapter
	{
	public:
		static int translate(byte_8 *data, int size, Settings ** settings);
		static int pack(Settings &settings, byte_8 **data, int *size);
	private:
		static uint_32 packSize(Settings &settings);

	};
}

#endif