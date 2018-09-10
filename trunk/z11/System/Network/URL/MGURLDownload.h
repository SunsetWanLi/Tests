//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (MGURLDownload.h)  
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
#ifndef _MGURLDownload_H_
#define _MGURLDownload_H_
#include "MGURLRequest.h"
#include "../../../CommonBase/MGData.h"
namespace z11
{
	class MGURLDownloadDelegate;
	class MGURLDownloadImpl;
	class MGURLConnection;
	//this object will be auto delete by MGURLDownloadThread
	class MGURLDownload
	{
	public:
		//Creating a Download Instance,The request object is deep-copied as part of the initialization process
		void initWithRequest(const MGURLRequest *_request,MGURLDownloadDelegate *delegate);

		void setDeletesFileUponFailure(bool _deletesFileUponFailure);
		bool deletesFileUponFailure();

		//Setting the Destination Path
		void setDestination(const std::string&path);
		void setAllowOverwrite(bool allowOverwrite);
		std::string getDestinationPath();


		//Getting Download Properties
		MGURLRequest *request();

		MGURLDownload();
		virtual ~MGURLDownload();

		//Resuming Partial Downloads
		void initWithResumeData(MGData *resume_data,MGURLDownloadDelegate *delegate,const std::string &path);
		MGData *resumeData();

		//Canceling a Download
		void cancel();
	private:
		//not ready


		//Manager Download thread
		static void startDownloadThread();
		static void stopDownloadThread();
	private:
		MGURLConnection *m_connection;
		MGURLRequest *m_request;
		bool resume_mode;
		MGURLDownloadDelegate *m_delegate;
		bool m_deletesFileUponFailure;
		bool m_allowOverwrite;
		std::string m_download_path;
		int m_receive_bytes;
		friend class MGURLDownloadImpl;
		MGURLDownloadImpl *impl;
		void decideDestinationWithSuggestedFilename( const std::string &file_name );
	};
	class MGURLDownloadDelegate
	{
	public: 
		virtual void downloadDecideDestinationWithSuggestedFilename(MGURLDownload *download,const std::string &file_name);
		virtual void downloadDidReceiveData(MGURLDownload *download,int receive_bytes,int total_bytes);
		virtual void downloadDidFinishDownloading(MGURLDownload *download);
		virtual void downloadDidCancelDownloading(MGURLDownload *download);
		virtual void downloadDidFailWithError(MGURLDownload *download,const MGError&error);
	};
}

#endif //_MGURLDownload_H_
