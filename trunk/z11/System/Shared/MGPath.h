//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (MGPath.h)  
/// @brief (本文件实现的功能的简述)  
///  
///(本文件实现的功能的详述)  
///  
/// @version 0.1   (版本声明)  
/// @author        (Gu RongFang)  
/// @date          (11-07-22)  
///  
///  
///    修订说明：最初版本  
////////////////////////////////////////////////////////////////////////// 
#ifndef _MGPATH_H_
#define _MGPATH_H_
#pragma once
#include "../../CommonBase/MGObject.h"
#include <string>
namespace z11
{
	//中文的路径名在android下会失败，所以尽量不要使用中文路径名

	class MGPath:public MGObject
	{
	public:
		static MGPath*getInstance();
		static bool isFolderExist(const std::string & folder_name);
	public:
		void addRootDriverPath(const std::string & root_driver_path);
		void setRootDriverPath(const std::string & root_driver_path);

		//单字节版本 
		const std::string& getRootDriverPath();
		const std::string& getImagePath();
		const std::string& getGeoDataPath();		//返回的是地图数据所在路径(GIS_Embedded/data) 
		const std::string& getCurrentWorkingDir();	//返回的是GIS_Embedded目录所在路径 
		const std::string& getTempDirectoryPath();
		const std::string& getConfigPath();
		const std::string& getSharedLibraryPath();
		const std::string& getResourcePath();

		//宽字节版本, 
		const wchar_t* getWRootDriverPath();
		const wchar_t* getWImagePath();
		const wchar_t* getWGeoDataPath();
		//返回的是GIS_Embedded目录所在路径 
		const wchar_t* getWCurrentWorkingDir();
		const wchar_t* getWTempDirectoryPath();
		const wchar_t* getWConfigPath();
		const wchar_t* getWSharedLibraryPath();
		const wchar_t* getWResourcePath();
	private:
		static MGPath *_instance;
		void Init();
		MGPath(void);
		~MGPath(void);

	};
}

#endif //_MGPATH_H_
