#ifndef _GIS_NAVI_HANDLER_H_
#define _GIS_NAVI_HANDLER_H_
#include "../../Common/Types/basic_types.h"
#include "NaviHandlerParam.h"
#include "../../../MapKit/GPSWrapper/GPS_Info.h"
#include "../../../CommonBase/MGWString.h"

#include <string>

using namespace z11;

namespace GIS_Embedded
{

class NavigationInfo;

class NaviHandler
{
public:
	virtual ~NaviHandler()
	{}
	// create the navi_info through analyze on the basis of ~~
	virtual int crtNaviInfo(const NaviAnalyzeParam & navi_ana_param,  NavigationInfo * navi_info) = 0;
	// act on the navi_info
	virtual int actWithNaviInfo(int cmd, NaviActParam & navi_act_param) = 0; 

	virtual z11::MGWString toString(const NavigationInfo & navi_info)const
	{
		return z11::MGWString();
	}
public:
	static int NAV_TOLERANCE_CROSS; 
	static const double NAVI_DISTANCE_HINT_NEXT; 

private:
	static int NAV_DISTANCE_SOUNT_UNIT;
	static int NAV_DISTANCE_SOUNT_UPPER;
};

class CNW_JunctionHandler : public NaviHandler
{
public:
	virtual ~CNW_JunctionHandler()
	{}
	// create the navi_info through analyze on the basis of ~~
	virtual int crtNaviInfo(const NaviAnalyzeParam & navi_ana_param,  NavigationInfo * navi_info) = 0;
	// act on the navi_info
	virtual int actWithNaviInfo(int cmd, NaviActParam & navi_act_param) = 0; 
protected:
	//void toSpeak(NaviActParam & navi_act_param);
	int toSpeak(NaviActParam & navi_act_param);
	void toShowCross(NaviActParam & navi_act_param);
};

class CNW_InterSectionHandler:public CNW_JunctionHandler
{
public:

	CNW_InterSectionHandler()
	{}
	virtual ~CNW_InterSectionHandler()
	{}

	virtual int crtNaviInfo(const NaviAnalyzeParam & navi_ana_param,  NavigationInfo * navi_info)
	{
		return CNW_JunctionHandler::crtNaviInfo(navi_ana_param, navi_info);
	};
	virtual int actWithNaviInfo(int cmd, NaviActParam & navi_act_param)
	{
		return CNW_JunctionHandler::actWithNaviInfo(cmd,navi_act_param);
	}
private:
//	static void reset()
//	{
//		last_st = SONT_UNKNOWN;
//		to_sound_pre = true;
//		to_show_cross = true;
//	}
//private:
//	// zgr, 2008-01-13, GPS_MAX_OFF_SET:
//	static double GPS_MAX_OFF_SET;
//	// zgr, 2008-01-14,
//	static z11::MGWString NAV_INFO_PREFIX;
//	static z11::MGWString NAV_INFO_DIR[18]; // eight dir;
//
//private:
//	static SoundType last_st;
//	static PathCross last_cross;
//private:
//	bool toUpdateSound(DirectionType * dir_type, SoundType cur_type)const;
};

class CNW_FinalHandler: public CNW_JunctionHandler
{
public:
	CNW_FinalHandler()
	{}
	virtual ~CNW_FinalHandler()
	{}
	
	virtual int crtNaviInfo(const NaviAnalyzeParam & navi_ana_param,  NavigationInfo * navi_info)
	{
		return CNW_JunctionHandler::crtNaviInfo(navi_ana_param, navi_info);
	};
	virtual int actWithNaviInfo(int cmd, NaviActParam & navi_act_param)
	{
		return CNW_JunctionHandler::actWithNaviInfo(cmd,navi_act_param);
	}
};

}

#endif