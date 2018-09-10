
#include "NaviProcessor.h"
#include "NaviHandler.h"
#include "NaviPathContext.h"
//#include "../../TopoData/NaviNWTypes/CNW_Intersection.h"
#include "NaviHandlerParam.h"

using namespace std;
using namespace IS_GIS;
namespace GIS_Embedded
{
NaviProcessor * NaviProcessor::m_instance = NULL;
Syncable NaviProcessor::m_syn;

NaviProcessor * NaviProcessor::getInstance()
{
	if(m_instance == NULL)
	{
		//Syncable::InnerSyncable inner_syn(m_syn);
		if(m_instance == NULL)
		{
			m_instance = new NaviProcessor();
		}
	}
	return m_instance;
}

NaviProcessor::~NaviProcessor() // delete the pointer in m_inner_handlers
{
	for(map<int, NaviHandler*>::iterator it = m_inner_handlers.begin(), it_end = m_inner_handlers.end();
		it != it_end; ++it)
		delete it->second, it->second = NULL;
	m_inner_handlers.clear();
	m_reg_handlers.clear();
}

int NaviProcessor::analyze( const NaviAnalyzeParam & ana_param,NavigationInfo * navi_info)
{
	int ret = 0;
	const NaviPathContext & navi_path_context = ana_param.m_navi_path_context;

	NWTypeEnum type = navi_path_context.getNextCNWJunctionType();
	
	NaviHandler * nv_handler = NULL;
	map<int, NaviHandler *>::iterator it = m_reg_handlers.find(type);
	if(it != m_reg_handlers.end())
	{
		nv_handler = it->second;
	}
	else
	{
		it = m_inner_handlers.find(type);
		if(it != m_inner_handlers.end())
			nv_handler = it->second;
	}

	if(nv_handler != NULL)
	{
		nv_handler->crtNaviInfo(ana_param, navi_info);
		ret = 0;
	}
	else
		ret = -1;

	return ret;
}

int NaviProcessor::navigate(int cmd, NaviActParam & act_param)
{
	int ret = -1;
	if(cmd == (int)NHCT_NO_HANDLE)
		return ret;

	NaviPathContext & navi_path_context = act_param.m_navi_path_context;

	//PER_REF(CNW_Object) cnw_is = navi_path_context.getNextCNWJunction();
	//if(cnw_is.isNull())
	//	return ret;

	//int no_type = cnw_is->NWType();
	NWTypeEnum type = navi_path_context.getNextCNWJunctionType();

	NaviHandler * nv_handler = NULL;	
	map<int, NaviHandler *>::iterator it = m_reg_handlers.find(type);
	if(it != m_reg_handlers.end())
	{
		nv_handler = it->second;
	}
	else
	{
		it = m_inner_handlers.find(type);
		if(it != m_inner_handlers.end())
			nv_handler = it->second;
	}

	if(nv_handler != NULL)
	{
		ret = nv_handler->actWithNaviInfo(cmd,act_param);
		//ret = 0;
	}
	else
		ret = -1;

	return ret;
}

// 以下两个注册/注销接口用于扩展的Junction类型， navi_handler在外部进行new/delete操作
NaviHandler * NaviProcessor::regNaviHandler(int nv_obj_type, NaviHandler * new_handler)
{
	NaviHandler * ret = NULL;
	if(m_inner_handlers.find(nv_obj_type) != m_inner_handlers.end())
	{
		ret = m_inner_handlers[nv_obj_type];
		m_inner_handlers[nv_obj_type] = new_handler;
	}
	return ret;
}

NaviHandler * NaviProcessor::unRegNaviHandler(int nv_obj_type)
{
	NaviHandler * ret = NULL;
	if(m_inner_handlers.find(nv_obj_type) != m_inner_handlers.end())
	{
		m_inner_handlers.erase(nv_obj_type);
	}
	return ret;
}

void NaviProcessor::init() //用于初始化已知的几种导航元素
{
	m_inner_handlers[(int)CNW_INTERSECTION_TYPE] = new CNW_InterSectionHandler();
	m_inner_handlers[(int)CNW_FINAL_TYPE] = new CNW_FinalHandler();
}

}
