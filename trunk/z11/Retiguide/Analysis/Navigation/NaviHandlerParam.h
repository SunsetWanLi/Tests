#ifndef _NAVI_HANDLER_PARAM_H_
#define _NAVI_HANDLER_PARAM_H_

#include "../../Common/Types/basic_types.h"

#include "NavigationInfo.h"
#include "../../../MapKit/GPSWrapper/GPS_Info.h"
#include "NaviPathContext.h"

using namespace IS_GIS;

namespace GIS_Embedded
{
class NaviMap;

struct NaviAnalyzeParam
{
	NaviAnalyzeParam()
	{}

	NaviAnalyzeParam(const GPS_Info & gps_info, const NaviPathContext & navi_path_context, const Pos2DEx & target_pos)
		:m_gps_info(gps_info), m_navi_path_context(navi_path_context), m_target_pos(target_pos)
	{}

	GPS_Info m_gps_info;
	NaviPathContext m_navi_path_context;
	Pos2DEx m_target_pos;
};

struct NaviActParam
{
	NaviActParam()
	{}
	NaviActParam(const NaviPathContext & navi_path_context, const NavigationInfo & navi_info/*, NaviMap * navi_map,*/ /*const Pos2DEx & cur_pos*/)
		: m_navi_path_context(navi_path_context), m_navi_info(navi_info)/*, m_navi_map(navi_map),*//* m_cur_pos(cur_pos)*/
	{}

	NaviPathContext m_navi_path_context;
	NavigationInfo m_navi_info;
	//NaviMap * m_navi_map;
	//Pos2DEx m_cur_pos;
};

enum NaviHandleCmdType
{
	NHCT_NO_HANDLE = 0x00,
	NHCT_SHOW_CROSS = 0x01,
	NHCT_SPEAK = 0x02,
	NHCT_REROUTE = 0x04
};

}

#endif