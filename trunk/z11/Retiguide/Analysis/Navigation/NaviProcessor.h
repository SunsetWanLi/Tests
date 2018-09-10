#ifndef _GIS_NAVI_PROCESSOR_H_
#define _GIS_NAVI_PROCESSOR_H_

//#include "../../TopoData/NaviNWTypes/CNW_Object.h"
//#include "../../Common/Types/Syncable.h"
#include "../../../System/Shared/Syncable.h"
#include "NaviHandlerParam.h"
#include "../../../MapKit/GPSWrapper/GPS_Info.h"

#include <map>

//struct GPS_Info;
//using z11::Syn

namespace GIS_Embedded
{
class NaviPathContext;
class NavigationInfo;
class NaviHandler;

class NaviProcessor
{
public:
	static NaviProcessor * getInstance();
	~NaviProcessor(); // delete the pointer in m_inner_handlers
protected:
	NaviProcessor()
	{
		init();
	}
	NaviProcessor(const NaviProcessor & r_val);
	NaviProcessor & operator=(const NaviProcessor & r_val);
public:
	int analyze(const NaviAnalyzeParam & ana_param, NavigationInfo * navi_info);
	// zgr, 2008-05-22, 目前：路口放大和路径重算都不在这里处理
	int navigate(int cmd, NaviActParam & act_param);

	// 以下两个注册/注销接口用于扩展的Junction类型， navi_handler在外部进行new/delete操作
	NaviHandler * regNaviHandler(int nv_obj_type, NaviHandler * navi_handler); // 返回值为 旧的处理对象，注意新旧不要注册两次，否则可能会导致delete两次
	NaviHandler * unRegNaviHandler(int nv_obj_type);
private:
	void init(); //用于初始化已知的几种导航元素
private:
	// singleton
	static NaviProcessor * m_instance;
	static Syncable m_syn;
private:
	// when searching the handler, search in the m_reg_handlers firstly, 
	//	if there is handler in the reg_handlers, use it;
	//  or not find the handler in the inner_handlers, if also not find, do nothing, and return error_code
	// here, not use Responsibility Chain Pattern( use handler queue )
	std::map<int, NaviHandler *> m_inner_handlers, m_reg_handlers; 
};

}
#endif