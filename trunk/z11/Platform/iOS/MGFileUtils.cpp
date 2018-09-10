#define __MG_PLATFORM_FILEUTILS_CPP__
#include "../MGFileUtilsCommon_cpp.h"
#include "../../System/Shared/MGLog.h"
#include "../../System/IO/MGBundle.h"
#include "../../System/Shared/MGTime.h"
#include "../../System/Shared/MGLog.h"
using namespace std;

namespace z11
{

	// record the resource path
	static  std::string s_pszResourcePath;

	static MGFileUtils* s_pFileUtils = NULL;

	MGFileUtils* MGFileUtils::sharedFileUtils()
	{
		if (s_pFileUtils == NULL)
		{
			s_pFileUtils = new MGFileUtils();
		}
		return s_pFileUtils;
	}

	void MGFileUtils::purgeFileUtils()
	{
		if (s_pFileUtils != NULL)
		{
			s_pFileUtils->purgeCachedEntries();
		}

		delete_and_set_null(s_pFileUtils);
	}

	void MGFileUtils::purgeCachedEntries()
	{

	}

	void MGFileUtils::setResourcePath(const std::string &pszResourcePath)
	{
		assert(!pszResourcePath.empty());
		//CCAssert(pszResourcePath != NULL, "[FileUtils setResourcePath] -- wrong resource path");
		//CCAssert(strlen(pszResourcePath) <= MAX_PATH, "[FileUtils setResourcePath] -- resource path too long");

		s_pszResourcePath=pszResourcePath;
	}

	MGData* MGFileUtils::getFileData(const std::string &file_name,size_t offset_pos,size_t length)
	{
		if (file_name.empty())
		{
			return NULL;
		}
		unsigned char* pBuffer = NULL;
		
		size_t size = 0;
		do 
		{
			// read the file from hardware
			FILE *fp = fopen(file_name.c_str(), "rb");
			MG_BREAK_IF(!fp);

			fseek(fp,0,SEEK_END);
			size = ftell(fp);
			fseek(fp,0,SEEK_SET);
			pBuffer = new unsigned char[size+1];
			pBuffer[size]=0;
			size = fread(pBuffer,sizeof(unsigned char), size,fp);
			fclose(fp);
		} while (0);


		MGData *data=NULL;
		if (! pBuffer )
		{
			std::string msg = "Get data from file("+file_name+") failed!";

			MGLogD(msg.c_str());
		}
		else
		{
			data=MGData::dataWithBytesNoCopy(pBuffer,size);
		}
		return data;
	}

	string MGFileUtils::getWriteablePath()
	{
		// return the path that the exe file saved in

		return MGBundle::mainBundle()->tmpPathWithFileName("");
	}

	std::string MGFileUtils::fullPathFromRelativePath(const std::string &pszRelativePath)
	{
		return MGBundle::mainBundle()->pathWithFileName(pszRelativePath);
	}

	bool MGFileUtils::fileExistsAtPath( const std::string &path )
	{
        if (path.length() == 0) {
            return false;
        }
        bool ret = false;
		if( access(path.c_str (), F_OK ) == 0 )
		{
			ret = true;
		}
        return ret;
	}
}
