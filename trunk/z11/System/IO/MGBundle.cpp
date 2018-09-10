
//
//  MGBundle.cpp
//  System
//
//  Created by Wang ZhiPeng on 11-6-4.
//  Copyright 2011骞?Zhejiang University. All rights reserved.
//

#include "MGBundle.h"
#include "../Shared/MGLog.h"
#include "../../Platform/MGPlatformConfig.h"
#include "../Shared/Utility.h"
namespace z11 
{
	string MGBundle::main_bundle_path = "";
	string MGBundle::storage_card_bundle_path = "";
    string MGBundle::ext_storage_card_bundle_path = "";
    string MGBundle::shared_bundle_path = "";
    string MGBundle::iOS_data_bundle_path = "";
	MGBundle *MGBundle::_instance=NULL;
    MGBundle::MGBundle()
    {
        current_bundle_path = main_bundle_path.append("/");
    }
    MGBundle::~MGBundle()
    {
        
    }
    /*Bundles*/
    MGBundle* MGBundle::mainBundle()
    {
		if (_instance==NULL)
		{
			_instance=new MGBundle();
		}
		return _instance;
    }
    string MGBundle::documentsPath()
    {
        return documentsPathWithFileName("");
    }
    string MGBundle::resourcesPath()
	{
        return resourcesPathWithFileName("");
    }
    string MGBundle::tmpPath()
	{
        return tmpPathWithFileName("");
    }
    string MGBundle::readablePath()
	{
        return readablePathWithFileName("");
    }
    string MGBundle::userFilePath()
	{
        return userFilePathWithFileName("");
    }

	string MGBundle::pathWithFileName(const string& filename)
	{
        string ret = current_bundle_path;
		return ret.append(filename);
	}

	/*  返回layout目录所在的bundle  */
	string MGBundle::layoutsPathWithFileName( const string& filename )
	{
		string ret = current_bundle_path + LAYOUT_BUNDLE;
		return ret.append(filename);
	}

	std::string MGBundle::drawablePathWithFileName( const string& filename )
	{
		string ret = current_bundle_path + DRAWABLE_BUNDLE;
		return ret.append(filename);
	}
    
    std::string MGBundle::readablePathWithFileName( const string& filename )
	{
#if  MG_TARGET_PLATFORM == MG_PLATFORM_IOS
		string ret = current_bundle_path + READABLE_BUNDLE;
#elif  MG_TARGET_PLATFORM == MG_PLATFORM_ANDROID
        string ret = current_bundle_path + READABLE_BUNDLE;
#else //Win32
        string ret = "";
		if (!current_bundle_path.empty()) {
            ret = current_bundle_path + READABLE_BUNDLE;
		}
#endif
		return ret.append(filename);
	}

	std::string MGBundle::storageCardPathWithFileName( const string& filename )
	{
#if  MG_TARGET_PLATFORM == MG_PLATFORM_IOS
		string ret = iOS_data_bundle_path + "/Library/Caches/";
        return ret.append(filename);
#else
		string ret = storage_card_bundle_path;
		return ret.append(filename);
#endif
	}
    
    std::string MGBundle::extStorageCardPathWithFileName( const string& filename )
	{
#if  MG_TARGET_PLATFORM == MG_PLATFORM_IOS
		string ret = iOS_data_bundle_path + "/Library/Caches/";
        return ret.append(filename);
#else
		string ret = ext_storage_card_bundle_path;
		return ret.append(filename);
#endif
	}
    
    std::string MGBundle::sharedPathWithFileName( const string& filename )
	{
#if  MG_TARGET_PLATFORM == MG_PLATFORM_IOS
		string ret = iOS_data_bundle_path + "/Library/Caches/";
        return ret.append(filename);
#else
		string ret = shared_bundle_path;
		return ret.append(filename);
#endif
	}

	std::string MGBundle::tmpPathWithFileName( const string&filename )
	{
#if  MG_TARGET_PLATFORM == MG_PLATFORM_IOS
		string ret = iOS_data_bundle_path + "/" + TMP_BUNDLE;
#elif  MG_TARGET_PLATFORM == MG_PLATFORM_ANDROID
        string ret = current_bundle_path + TMP_BUNDLE;
#else //Win32
        string ret = "";
		if (!current_bundle_path.empty()) {
            ret = current_bundle_path + "../" + TMP_BUNDLE;
		}
#endif
        return ret.append(filename);
	}

	std::string MGBundle::documentsPathWithFileName( const string&filename )
	{
#if  MG_TARGET_PLATFORM == MG_PLATFORM_IOS
		string ret = iOS_data_bundle_path + "/Library/Caches/" + DOCUMENTS_BUNDLE;
#elif  MG_TARGET_PLATFORM == MG_PLATFORM_ANDROID
        string ret = current_bundle_path + DOCUMENTS_BUNDLE;
#else //Win32
        string ret = "";
		if (!current_bundle_path.empty()) {
            ret = current_bundle_path + "../" + DOCUMENTS_BUNDLE;
		}
#endif
		return ret.append(filename);
	}
    
    string MGBundle::resourcesPathWithFileName(const string&filename)
    {
#if  MG_TARGET_PLATFORM == MG_PLATFORM_IOS
		string ret = iOS_data_bundle_path + "/Library/Caches/" + RESOURCES_BUNDLE;
#elif  MG_TARGET_PLATFORM == MG_PLATFORM_ANDROID
        string ret = current_bundle_path + RESOURCES_BUNDLE;
#else //Win32
        string ret = "";
		if (!current_bundle_path.empty()) {
            ret = current_bundle_path + "../" + RESOURCES_BUNDLE;
		}
#endif
		return ret.append(filename);
    }
    
    string MGBundle::userFilePathWithFileName(const string&filename)
    {
#if  MG_TARGET_PLATFORM == MG_PLATFORM_IOS
		string ret = iOS_data_bundle_path + "/Documents/";
        return ret.append(filename);
#else
		string ret = storage_card_bundle_path;
		return ret.append(filename);
#endif
    }


} //namespace z11