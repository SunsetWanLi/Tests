#define __MG_PLATFORM_FILEUTILS_CPP__
#include "../MGFileUtilsCommon_cpp.h"
#include "../../System/Shared/MGTime.h"
#include "../../System/Shared/MGLog.h"
#include "../../System/IO/MGBundle.h"
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
		//assert(!pszResourcePath.empty());
		//CCAssert(pszResourcePath != NULL, "[FileUtils setResourcePath] -- wrong resource path");
		//CCAssert(strlen(pszResourcePath) <= MAX_PATH, "[FileUtils setResourcePath] -- resource path too long");

		 if ((pszResourcePath.empty() ) || pszResourcePath.find(".apk") == string::npos)
		{
			MGLogD("wrong resource path");
			return;
		}
	
		s_pszResourcePath=pszResourcePath;
	}

	MGData* MGFileUtils::getFileData(const std::string &pszFileName,size_t offset_pos,size_t length)
	{
		MGData* pData = 0;
		string fullPath(pszFileName);

		if (fullPath.empty())
		{
			return 0;
		}

		if( access(fullPath.c_str (), F_OK ) == 0 )
		{
			do 
			{
				// read rrom other path than user set it
				FILE *fp = fopen(fullPath.c_str(), "rb");
				MG_BREAK_IF(!fp);

				unsigned long size;
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
				unsigned char *_pData = new unsigned char[length+1];
				_pData[length]=0;
				size = fread(_pData,sizeof(unsigned char), length,fp);
				fclose(fp);

				if (size)
				{
					pData=MGData::dataWithBytesNoCopy(_pData,size);
				}            
			} while (0);        
		}
		else
		{
			if (fullPath[0] == '/')
			{
				string bundle_path=MGBundle::mainBundle()->pathWithFileName("");
				if(fullPath.find(bundle_path)==string::npos)
				{
					return 0;
				}
				else
					fullPath.erase(0,bundle_path.size());
			}
			if (fullPath[0] != '/')
			{
				// read from apk
				fullPath.insert(0, "assets/");
				//MGLogD("getFileDataFromZip(%s,%s)",s_pszResourcePath.c_str(), fullPath.c_str());
				pData =  MGFileUtils::getFileDataFromZip(s_pszResourcePath, fullPath,offset_pos,length);
			}
		}

		return pData;
	}

	string MGFileUtils::getWriteablePath()
	{
		// return the path that the exe file saved in

		return MGBundle::mainBundle()->tmpPathWithFileName("");
	}

	std::string MGFileUtils::fullPathFromRelativePath(const std::string &pszRelativePath)
	{
		return pszRelativePath;
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
		else
		{
			string fullPath(path);
			string bundle_path=MGBundle::mainBundle()->pathWithFileName("");
			if(fullPath.find(bundle_path)==string::npos)
			{
				return false;
			}
			if (fullPath[0] == '/')
			{
				fullPath.erase(0,bundle_path.size());
			}
			fullPath.insert(0, "assets/");
			ret = fileExistsAtPathInBundle(s_pszResourcePath,fullPath);
		}
        return ret;
	}
}
