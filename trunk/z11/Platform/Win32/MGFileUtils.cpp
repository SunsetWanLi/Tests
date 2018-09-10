#define __MG_PLATFORM_FILEUTILS_CPP__
#include "../MGFileUtilsCommon_cpp.h"
#include <winsock2.h>
#include <windows.h>
#include "../../System/Shared/MGLog.h"
#include "../../System/IO/MGBundle.h"
#include "../../System/Shared/Utility.h"
#include <io.h>
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
			if (offset_pos>=size)
			{
				fclose(fp);
				break;
			}
			if (length==npos || length>size-offset_pos)
			{
				length=size-offset_pos;
			}
			
			fseek(fp,offset_pos,SEEK_SET);
			
			pBuffer = new unsigned char[length+1];
			pBuffer[length]=0;
			size = fread(pBuffer,sizeof(unsigned char), length,fp);
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
		//string real_path = Utility::ustr2astr(path);
		string real_path = (path);
		bool ret = false;
		if (_access (real_path.c_str (), 0 ) != -1 )
			ret = true;
		return ret;
	}
}
