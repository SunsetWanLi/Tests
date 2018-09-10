//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (MGWebServiceThread.h)  
/// @brief (本文件实现的功能的简述)  
///  
///(本文件实现的功能的详述)  
///  
/// @version 0.1   (版本声明)  
/// @author        (Gu RongFang)  
/// @date          (11-12-29)  
///  
///  
///    修订说明：最初版本  
////////////////////////////////////////////////////////////////////////// 
#ifndef _MGWEBSERVICETHREAD_H_
#define _MGWEBSERVICETHREAD_H_
#include "../Thread/MGThread.h"
#include "MGWebService.h"
#include "../Thread/MGMutex.h"
#include <set>
namespace z11
{
	class MGWebServiceThread:public MGThread
	{
	public:
		MGWebServiceThread(MGRequestSoapObject *service);//retain
		virtual ~MGWebServiceThread();
		virtual int    readyToRun();
	protected:
		MGRequestSoapObject *m_request_soap_object;
		//static MGMutex pool_locker;
		//static std::set<MGWebServiceThread *> m_pool;
		//static std::set<MGWebServiceThread *> m_delete_pool;
		virtual bool threadLoop();
	};

}

#endif //_MGWEBSERVICETHREAD_H_
