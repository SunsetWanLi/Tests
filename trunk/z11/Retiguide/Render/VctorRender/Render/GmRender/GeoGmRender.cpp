#include "GeoGmRender.h"

namespace GIS_Embedded
{

	z11::MGPoint* GeoGmRender::m_gm_buff = new z11::MGPoint[1000];
	int GeoGmRender::m_gm_buff_size = 1000;
	int GeoGmRender::m_buff_valid = 0;

}