//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (MGURLDownloadThread.h)  
/// @brief (���ļ�ʵ�ֵĹ��ܵļ���)  
///  
///(���ļ�ʵ�ֵĹ��ܵ�����)  
///  
/// @version 0.1   (�汾����)  
/// @author        (Gu RongFang)  
/// @date          (11-12-29)  
///  
///  
///    �޶�˵��������汾  
////////////////////////////////////////////////////////////////////////// 
#pragma once
#ifndef _MGURLDownloadThread_H_
#define _MGURLDownloadThread_H_
#include "../../Thread/MGMutex.h"
#include "../../Thread/MGSemaphore.h"
#include "../../Thread/MGThread.h"
#include "MGURLDownload.h"
#include "MGURLConnection.h"
#include <queue>
#include <stdio.h>
namespace z11
{
	class MGURLDownloadThread:public MGThread
	{
	public:
		static void start();
		static void stop();
		static void enqueue(MGURLDownload *download);
		virtual bool threadLoop();
		virtual void requestExit();
		~MGURLDownloadThread();
	private:
		static MGURLDownloadThread *_instance;
		static std::queue<MGURLDownload*> download_queue;
		static MGMutex m_queue_locker;
		static int start_num;
	private:
		MGSemaphore m_queue_semphore;
		MGURLDownloadThread();
	};

	class MGURLDownloadImpl:public MGURLConnectionDelegate
	{
	public:
		//MGURLConnection *m_connection;
		MGURLDownload *m_download;
		FILE *file;
		MGURLDownloadImpl(MGURLDownload *download);
		~MGURLDownloadImpl();
		//void cancel();
		virtual void connectionDidReceiveResponse(MGURLConnection *connection,MGURLResponse *response);
		virtual void connectionDidReceiveData(MGURLConnection *connection,MGData *data);
		virtual void connectionDidFinishLoading(MGURLConnection *connection);
		virtual void connectionDidCancelLoading(MGURLConnection *connection);
		virtual void connectionDidFailWithError(MGURLConnection *connection,const MGError&error);
	};
}

#endif //_MGURLDownloadThread_H_
