#include "MGPath.h"
#include "Utility.h"
#include <stdio.h>
using namespace  std;
namespace z11
{
	static std::vector<std::string> rdp_table;

	static std::string root_driver_path;
	static std::string image_path;
	static std::string geo_data_path;
	static std::string current_working_path;
	static std::string temp_path;
	static std::string config_path;
	static std::string shared_library_path;
	static std::string resource_path;

	static wstring w_root_driver_path;
	static wstring w_image_path;
	static wstring w_geo_data_path;
	static wstring w_current_working_path;
	static wstring w_temp_path;
	static wstring w_config_path;
	static wstring w_shared_library_path;
	static wstring w_resource_path;

	MGPath *MGPath::_instance=NULL;
	MGPath::MGPath(void)
	{
	}

	MGPath::~MGPath(void)
	{
	}

	MGPath* MGPath::getInstance()
	{
		if (_instance==NULL)
		{
			_instance=new MGPath();
			_instance->Init();
		}
		return _instance;
	}

	void MGPath::Init()
	{
		root_driver_path = "/Storage Card/";
		current_working_path=getCurrentWorkingDir();

		string RDP_SD = "/SD/";
		string RDP_STORAGE_CARD = "/Storage Card/";
		string RDP_STORAGE_CAED_CHS = "/存储卡/";
		string RDP_SDMMC = "/SDMMC/";
		string RDP_STORAGECARD = "/StorageCard/";
		string RDP_SD_MEMORY = "/SD Memory/";
		string RDP_SD_MEMORY_CHS	=	"/内存/";
		string RDP_SD_MEMORY_TW	=	"/存储卡/";
		string ANDOIRD_PATH="/sdcard/";
		string RDP_PROGRAM_STORE = "/ProgramStore/";

		if(rdp_table.empty())
		{
			rdp_table.push_back(current_working_path+"/");
			rdp_table.push_back(RDP_SDMMC);
			rdp_table.push_back(RDP_SD);
			rdp_table.push_back(RDP_STORAGE_CARD);
			rdp_table.push_back(RDP_STORAGE_CAED_CHS);
			rdp_table.push_back(RDP_STORAGECARD);
			rdp_table.push_back(RDP_SD_MEMORY);
			rdp_table.push_back(RDP_SD_MEMORY_CHS);
			rdp_table.push_back(RDP_SD_MEMORY_TW);
			rdp_table.push_back(ANDOIRD_PATH);
			rdp_table.push_back(RDP_PROGRAM_STORE);

		}

		size_t i = 0;
		size_t size = rdp_table.size();

		for(; i < size; ++i)
		{
			string tmp = rdp_table.at(i);
			tmp += "MobileApp";
			if(isFolderExist(tmp))
			{
				root_driver_path = rdp_table.at(i);
				break;
			}
		}
		if (i==size)
		{
			root_driver_path="/sdcard/";
		}
		#ifdef ANDROID_NDK
		if(isFolderExist("/sdcard/"))
		{
			root_driver_path="/sdcard/";
		}
		else if(isFolderExist("/data/data/com.z11.mobile.framework/files/"))
		{
			root_driver_path="/data/data/com.z11.mobile.framework/files/";
		}
		#endif

		geo_data_path = root_driver_path+"GIS_Embedded/";
		image_path = geo_data_path+"image/";
		temp_path=geo_data_path+"temp/";
		config_path=geo_data_path+"conf/";
		shared_library_path=geo_data_path+"Include/";
		resource_path=geo_data_path+"sound";

		w_root_driver_path = Utility::asci_to_unicode(root_driver_path.c_str());
		w_image_path = Utility::asci_to_unicode(image_path.c_str());
		w_geo_data_path = Utility::asci_to_unicode(geo_data_path.c_str());
		w_current_working_path = Utility::asci_to_unicode(current_working_path.c_str());
		w_temp_path = Utility::asci_to_unicode(temp_path.c_str());
		w_config_path = Utility::asci_to_unicode(config_path.c_str());
		w_shared_library_path = Utility::asci_to_unicode(shared_library_path.c_str());
		w_resource_path = Utility::asci_to_unicode(resource_path.c_str());
	}

	const std::string& MGPath::getRootDriverPath()
	{
		return root_driver_path;
	}

	const std::string& MGPath::getImagePath()
	{
		return image_path;
	}

	const std::string& MGPath::getGeoDataPath()
	{
		return geo_data_path;
	}

	const std::string& MGPath::getCurrentWorkingDir()
	{
#ifdef WIN32
		wchar_t szFull[_MAX_PATH];
		::GetModuleFileNameW(NULL, szFull, sizeof(szFull)/sizeof(wchar_t));
		wstring strPath(szFull);
		current_working_path = Utility::wstr2astr(strPath);
		current_working_path = current_working_path.substr(0,current_working_path.find_last_of('\\') );
		for (size_t pos=current_working_path.find_first_of('\\');pos!=string::npos;pos=current_working_path.find_first_of('\\'))
		{
			current_working_path[pos]='/';
		}
#endif
		return current_working_path;
	}

	const std::string& MGPath::getTempDirectoryPath()
	{
		return temp_path;
	}

	const std::string& MGPath::getConfigPath()
	{
		return config_path;
	}

	const std::string& MGPath::getSharedLibraryPath()
	{
		return shared_library_path;
	}

	const std::string& MGPath::getResourcePath()
	{
		return resource_path;
	}

	const wchar_t* MGPath::getWRootDriverPath()
	{
		return &w_root_driver_path[0];
	}

	const wchar_t* MGPath::getWImagePath()
	{
		return &w_image_path[0];

	}

	const wchar_t* MGPath::getWGeoDataPath()
	{
		return &w_geo_data_path[0];
	}

	const wchar_t* MGPath::getWCurrentWorkingDir()
	{
		return &w_current_working_path[0];
	}

	const wchar_t* MGPath::getWTempDirectoryPath()
	{
		return &w_temp_path[0];
	}

	const wchar_t* MGPath::getWConfigPath()
	{
		return &w_config_path[0];
	}

	const wchar_t* MGPath::getWSharedLibraryPath()
	{
		return &w_shared_library_path[0];
	}

	const wchar_t* MGPath::getWResourcePath()
	{
		return &w_resource_path[0];
	}

	bool MGPath::isFolderExist( const std::string & folder_name )
	{
#if defined(_WIN32) || defined(_WIN64)
		WIN32_FIND_DATAW  wfd;
		bool rValue = false;
		void * hFind = FindFirstFileW(Utility::astr2wstr(folder_name).c_str(), &wfd);
		if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			rValue = true;
		}
		FindClose(hFind);
		return rValue;
#else
		FILE *fp=fopen((folder_name+"_test_folder_exist_can_be_delete").c_str(),"w");
		if (fp!=NULL)
		{
			fclose(fp);
			return true;
		}
		return false;
#endif
	}

}
