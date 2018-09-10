//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (MIDownloadManager.h)  
/// @brief (本文件实现的功能的简述)  
///  
///(本文件实现的功能的详述)  
///  
/// @version 0.1   (版本声明)  
/// @author        (Gu RongFang)  
/// @date          (11-09-28)  
///  
///  
///    修订说明：最初版本  
////////////////////////////////////////////////////////////////////////// 
#ifndef _MIDownloadManager_H_
#define _MIDownloadManager_H_
#pragma once

#include "../Base/MIView.h"
#include <string>
namespace z11
{

	class MITableView;
	class MIDownloadManagerViewDelegate;
	class MIDownloadManagerView : public MIView
	{
	public:
		MIDownloadManagerView(void);
		~MIDownloadManagerView(void);
		void setDownloadFolder(const std::string &download_folder);
		void addTask(const std::string &url);
		void addTask(const std::string &url,const std::string &name);
	public:
		MIVIEW_DECLARE;
	private:
		MITableView *m_table_view;
		std::string m_download_folder;
		MIDownloadManagerViewDelegate *delegate;
	};
	class MIDownloadManagerViewDelegate
	{
		virtual void downloadManagerDidFinishDownload()
		{

		}
	};
}

#endif //_MIDownloadManager_H_
