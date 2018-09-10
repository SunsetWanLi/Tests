//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (MGURLConnection.h)  
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
#pragma once
#ifndef _MGURLConnection_H_
#define _MGURLConnection_H_
#include "../../../CommonBase/MGData.h"
#include "../../../CommonBase/MGDataReader.h"
#include "MGURLRequest.h"
#include "MGURLResponse.h"
#include <map>
#include "../../Thread/MGMutex.h"
namespace curlpp
{
	class Easy;
};
namespace z11
{
	class MGURLConnectionDelegate;
	class MGURLDownloadImpl;
	class MGURLConnectionThread;
	enum MGURLConnectionState
	{
		MGURLConnectionStateInit,
		MGURLConnectionStateRunning,
		MGURLConnectionStateCancel,
		MGURLConnectionStateFinish
	};
	class MGURLConnection
	{
	public:
		/**
		* \brief 从指定request获取数据
		* \param request: The URL request to load.The request object is deep-copied as part of the initialization process.
		*				Changes made to request after this method returns do not affect the request that is used for the loading process.
		* \return 返回获取到的数据
		*/
		static MGData *sendSynchronousRequest(const MGURLRequest *request,MGURLResponse **response);
		static MGURLConnection* connectionWithRequest(const MGURLRequest *request,MGURLConnectionDelegate *delegate);
		~MGURLConnection();
		void cancel();
		MGURLConnection();
		void initWithRequest(const MGURLRequest *request,MGURLConnectionDelegate *delegate,bool startImmediately=true);
		MGURLRequest *request();
	private:
		void start();
		MGURLConnectionDelegate *m_delegate;
		MGURLConnection(MGURLRequest *request,MGURLConnectionDelegate *delegate);
		MGURLRequest *m_request;
		curlpp::Easy* easy_request;
		MGMutex write_locker;
	private:
		MGURLConnectionState m_state;
		MGMutableURLDictionary header_response_map;
		friend size_t MGURLConnectionHeaderCallback(MGURLConnection *connection, char* ptr, size_t size, size_t nmemb);
		friend size_t MGURLConnectionWriteCallback(MGURLConnection *connection, char* ptr, size_t size, size_t nmemb);
		friend size_t MGURLConnectionReadCallback(MGURLConnection *connection, char* ptr, size_t size, size_t nmemb);
		friend class MGURLDownloadImpl;
		friend class MGURLConnectionThread;
		int receive_bytes;
		int total_bytes;
		MGURLResponse *m_response;
		MGURLConnectionThread *m_thread;
		MGDataReader *m_data_reader;
	};
	class MGURLConnectionDelegate
	{
	public:
		virtual void connectionDidReceiveResponse(MGURLConnection *connection,MGURLResponse *response)=0;
		virtual void connectionDidReceiveData(MGURLConnection *connection,MGData *data)=0;
		virtual void connectionDidFinishLoading(MGURLConnection *connection)=0;
		virtual void connectionDidCancelLoading(MGURLConnection *connection)=0;
		virtual void connectionDidFailWithError(MGURLConnection *connection,const MGError&error);
	};
}

#endif //_MGURLConnection_H_
