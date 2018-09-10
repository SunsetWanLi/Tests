#ifndef _CORE_WORKSPACE_WSANNOTATIONLAYERSTYLE_H
#define _CORE_WORKSPACE_WSANNOTATIONLAYERSTYLE_H

#include"WSLabelStyle.h"
#include "../basic_types.h"

using namespace GIS_Embedded;

namespace GIS_Embedded{
class WSAnnotationLayerStyle
{

public:
	WSLabelStyle Label;
	vector<GO_ID> GOID_VECTOR;

    WSAnnotationLayerStyle()
    {
    }
};

}
#endif