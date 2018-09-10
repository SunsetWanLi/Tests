#ifndef _FCLS_INFO_ADAPTER_H_
#define _FCLS_INFO_ADAPTER_H_

#include "../Common/Types/FeatureClassInfo.h"

namespace GIS_Embedded
{

	class FclsInfoAdapter
	{
	public:
		static int translate(byte_8 *data, int size, int grid_side_divider, FeatureClassInfo **fcls_info);

	};

}

#endif