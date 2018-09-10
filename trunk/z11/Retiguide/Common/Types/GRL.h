#ifndef _COMMON_TYPES_GRL_H
#define _COMMON_TYPES_GRL_H

#include <string>
#include "CommonBase/MGWString.h"

using namespace std;
using namespace z11;

namespace GIS_Embeded
{
class GRL {
	
private:

	z11::MGWString _data_src;
	z11::MGWString _data_set;
	z11::MGWString _feature_layer;
	z11::MGWString _goid;

public:

	GRL();

	void deCodeGRL  (z11::MGWString grl);


	z11::MGWString getFeatureLayerId()const;

	z11::MGWString toString()const;

	long getGoid()const;
	
};

}

#endif