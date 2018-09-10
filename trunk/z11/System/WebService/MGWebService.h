//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (MGWebService.h)  
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
#ifndef _MGWEBSERVICE_H_
#define _MGWEBSERVICE_H_
#include "../Thread/MGThread.h"
#include "MGSoapObject.h"
#include "MGRequestSoapObject.h"
#include <vector>
#include <set>
#include "../Thread/MGMutex.h"
namespace z11
{
	class MGWebService;
	class MGWebServiceDelegate
	{
	public:
		bool m_quit;
		MGMutex locker;
		MGWebServiceDelegate();
		virtual ~MGWebServiceDelegate();
		virtual void didGetResponse(MGRequestSoapObject *request_soap,MGSoapObject* response)=0;
	};
	class MGWebServiceSystemDelegate
	{
	public:
		virtual MGSoapObject* handle(MGRequestSoapObject *)=0;
	};
	class MGWebService
	{
	public:
		MGWebService();
		MGPROPERTY(std::string,m_namespace,Namespace);
		MGPROPERTY(std::string,m_url,URL);
	public:
		void requestWebService(const std::string& method,
			const std::vector<std::string>* keys,
			const std::vector<std::string>* values);
		void setDelegate(MGWebServiceDelegate *_delegate);

		static void setSystemDelegate(MGWebServiceSystemDelegate *_delegate);
		static MGWebServiceSystemDelegate* getSystemDelegate();
	private:
		MGWebServiceDelegate* delegate;
		static MGWebServiceSystemDelegate *m_system_delegate;
		bool handle();

	public:
	private:
		//static set<MGWebService *> m_webservice_pool;
	};
}

#endif //_MGWEBSERVICE_H_
