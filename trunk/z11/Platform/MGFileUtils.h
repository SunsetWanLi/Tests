#ifndef __MG_FILEUTILS_PLATFORM_H__
#define __MG_FILEUTILS_PLATFORM_H__

#include <string>
#include "MGPlatformMacros.h"
#include "../CommonBase/MGData.h"
namespace z11
{
	//! @brief  Helper class to handle file operations
	class  MGFileUtils
	{
	public :
		static const size_t npos=-1;
	public:
		static MGFileUtils* sharedFileUtils();
		static void purgeFileUtils();

		void purgeCachedEntries();
		/**
		@brief Get resource file data
		@param[in]  pszFileName The resource file name which contain the path
		@param[in]  pszMode The read mode of the file
		@param[out] pSize If get the file data succeed the it will be the data size,or it will be 0
		@return if success,the pointer of data will be returned,or NULL is returned
		@warning If you get the file data succeed,you must delete[] it after used.
		*/
		MGData* getFileData(const std::string &file_name,size_t offset_pos=0,size_t length=npos);
		/**
		@brief Get resource file data from zip file
		@param[in]  pszFileName The resource file name which contain the relative path of zip file
		@param[out] pSize If get the file data succeed the it will be the data size,or it will be 0
		@return if success,the pointer of data will be returned,or NULL is returned
		@warning If you get the file data succeed,you must delete[] it after used.
		*/
		MGData* getFileDataFromZip(const std::string &zip_file_name,const std::string &file_name,size_t offset_pos=0,size_t length=npos);

		/**
		@brief   Generate the absolute path of the file.
		@param   pszRelativePath     The relative path of the file.
		@return  The absolute path of the file.
		@warning We only add the ResourcePath before the relative path of the file.
		If you have not set the ResourcePath,the function add "/NEWPLUS/TDA_DATA/UserData/" as default.
		You can set ResourcePath by function void setResourcePath(const char *pszResourcePath);
		*/
		std::string fullPathFromRelativePath(const std::string &pszRelativePath);

		/**
		@brief  Set the ResourcePath,we will find resource in this path
		@param pszResourcePath  The absolute resource path
		@warning Don't call this function in android and iOS, it has not effect.
		In android, if you want to read file other than apk, you shoud use invoke getFileData(), and pass the 
		absolute path.
		*/
		void setResourcePath(const std::string &resourcePath);

		/**
		@brief   Get the writeable path
		@return  The path that can write/read file
		*/
		std::string getWriteablePath();

		bool fileExistsAtPath(const std::string &path );
		bool fileExistsAtPathInZip(const std::string &zip_file_name,const std::string &path );
        bool fileExistsAtPathInBundle(const std::string &zip_file_name,const std::string &path );
	};

	// end of platform group
	/// @}

}

#endif    // __MG_FILEUTILS_PLATFORM_H__
