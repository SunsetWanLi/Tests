#include "MGFileManager.h"
#include "../Shared/Utility.h"
#include "../../Platform/MGFileUtils.h"
namespace z11
{

	bool z11::MGFileManager::fileExistsAtPath( const std::string &path )
	{
		return MGFileUtils::sharedFileUtils()->fileExistsAtPath(path);
	}

    
    static MGFileManager manager;
	MGFileManager * MGFileManager::defaultManager()
	{
		return &manager;
	}

	bool MGFileManager::createDirectoryAtPath( const std::string &path,bool createIntermediates )
	{
		return Utility::createDirOnPath(path);
	}

	bool MGFileManager::fileExistsAtPath( const MGWString &path )
	{
		return fileExistsAtPath(z11::Utility::wstr2ustr(path));
	}

	std::vector<std::string> MGFileManager::contentsOfDirectoryAtURL( const std::string &path )
	{
		std::vector<std::string> ret;
		Utility::getFilesInDir(path,ret);
		return ret;
	}

	bool MGFileManager::removeItemAtPath( const std::string &path )
	{
		return false;
		//Utility::delAFile()
	}
}