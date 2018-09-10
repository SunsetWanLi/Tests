//
//  MGBundle.h
//  System
//
//  Created by Wang ZhiPeng on 11-6-4.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//
/*
 应用程序束，束一般是指一个目录下的所有资源，其基准是目录路径，但是提供了对该路径下各种资源快速访问的方法。
 应用程序一般包含如下固定Bundles:
 Documents //用户可写文件区域，保存用户数据//
 tmp //应用程序生成临时文件，程序重启后会被清理//
 Resources //用保存的资源文件，如从网络下载的图片//
 readable //应用程序只读文件
 
/===============mainBundle===============/
iOS:        /iOS.App
Android:    /data/data/PACKAGE_NAME/files/Android.App
Win32:      Apps/APPNAME/Resources

/===============tmpPathWithFileName(filename)===============/
iOS:        mainBundle/../tmp/filename
Android:    mainBundle/tmp/filename
Win32:      mainBundle/../tmp/filename

/===============storageCardPathWithFileName(filename)===============/
iOS:        mainBundle/../Library/Caches/filename
Android:    /sdcard/Android/data/PACKAGE_NAME/filename(不存在使用mainBundle)
Win32:      mainBundle/../sdcard/filename
 
/===============sharedPathWithFileName(filename)===============/
iOS:        Shared/Library/Caches/z11/filename   //暂不可用
Android:    /sdcard/Shared/z11/filename(不存在使用mainBundle)
Win32:      C:/Shared/z11/filename

/===============documentsPathWithFileName(filename)===============/
iOS:        mainBundle/../Library/Caches/documents/filename
Android:    mainBundle/documents/filename
Win32:      mainBundle/../documents/filename

/===============readablePathWithFileName(filename)===============/
iOS:        mainBundle/readable/filename
Android:    mainBundle/readable/filename
Win32:      mainBundle/readable/filename
*/
#pragma once

#include "../../CommonBase/MGObject.h" 
#include <string>

using namespace std;

namespace z11
{
#define DOCUMENTS_BUNDLE "documents/"
#define TMP_BUNDLE "tmp/"
#define RESOURCES_BUNDLE "Resources/"
#define READABLE_BUNDLE "readable/"
#define DRAWABLE_BUNDLE "drawable/"
#define LAYOUT_BUNDLE "layout/"
    class MGBundle : public MGObject
    {
	public:
        virtual ~MGBundle();
    protected:
		MGBundle();
    protected:
		string current_bundle_path;
    /* 共享属性 */  
    public:
		static string main_bundle_path;
		static string storage_card_bundle_path;
        static string ext_storage_card_bundle_path;
        static string shared_bundle_path;
        static string iOS_data_bundle_path;
    public:
        /*  返回应用程序执行文件和资源文件所在的Bundle*/
        static MGBundle* mainBundle();
        /*  返回Documents目录所在bundle*/
        string documentsPath();
        /*  返回Resources目录所在的bundle*/
        string resourcesPath();
        /*  返回tmp目录所在的bundle */
        string tmpPath();
		/*  返回readable目录所在bundle*/
		string readablePath();
        /*  返回用户文件目录所在bundle*/
		string userFilePath();
		/*  返回layout目录所在的bundle  */
		string layoutsPathWithFileName(const string& filename);
		/*  返回drawable目录所在bundle*/
		string drawablePathWithFileName(const string& filename);
		/*  返回mainBundle目录所在bundle*/
		string pathWithFileName(const string& filename);
        /*  返回一个可写的目录所在bundle,
            对于iOS来说是mainBundle/../Library/Caches/documents目录，
            对于Android来说是mainBundle/documents目录，
            对于Win32就是mainBundle/documents目录*/
		string documentsPathWithFileName(const string&filename);
		string resourcesPathWithFileName(const string&filename);
		string tmpPathWithFileName(const string&filename);
        string readablePathWithFileName(const string& filename);

		/*  返回存储卡位置 */
		string storageCardPathWithFileName(const string& filename);
        /*  返回外部存储卡位置 */
		string extStorageCardPathWithFileName(const string& filename);
        
        /* 返回应用间共享文件位置，iOS暂不可用 */
        string sharedPathWithFileName(const string& filename);
        
        /*  返回用户文件位置，用户文件需要被iCloud等备份 */
        string userFilePathWithFileName(const string& filename);
	private:
		static MGBundle* _instance;
    };
} //namespace z11