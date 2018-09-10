#ifndef _CORE_WORKSPACE_WORKSPACE_H
#define _CORE_WORKSPACE_WORKSPACE_H
#include<vector>
#include<string>
#include"WSLayer.h"
#include "CommonBase/MGWString.h"

using namespace std;
using namespace z11;

namespace GIS_Embedded{

class CrossType
{
public:
	int m_bg_color;
	int m_arrow_width;
	int m_arrow_color;
	vector<WSCrossLayer> m_cross_layers;
};

class WorkSpace
{
public:
	z11::MGWString m_workspace_id;
	int m_bg_color;
	vector<WSLayer> m_ws_layers;

	CrossType m_cross_type;
};
}

#endif