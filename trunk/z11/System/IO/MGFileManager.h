/*
 * =====================================================================================
 *
 *       Filename:  MGFileManager.h
 *
 *    Description:  提供通用的文件系统调用
 *
 *        Version:  1.0
 *        Created:  07/13/2012 01:46:41 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#pragma once
#include "../../CommonBase/MGData.h"
#include "../../CommonBase/MGWString.h"
#include <vector>
namespace z11
{
	class MGFileManager
	{
	public:
		static MGFileManager *defaultManager();
		bool fileExistsAtPath(const std::string &path);
		bool fileExistsAtPath(const MGWString &path);
		bool createDirectoryAtPath(const std::string &path,bool createIntermediates = true );
		std::vector<std::string> contentsOfDirectoryAtURL(const std::string &path);
		bool removeItemAtPath(const std::string &path);
	};
}
