#ifndef _COMMON_TYPE_DEFS_H_
#define _COMMON_TYPE_DEFS_H_
#include "System/IO/MGBundle.h"
#include "CommonBase/MGWString.h"
using namespace z11;

#include <string>
#include <map>
#include <ctime>
#include <vector>

using namespace std;



namespace GIS_Embedded
{

	struct NetParam	//网络连接环境变量
	{
		bool online;//是否连接入网络
		string server_ip;
		unsigned port;/*uint_32*/
		string user;
		string passwd;
		int notify_port;/*int_32*/

	};

	struct FileParam//文件读取环境变量
	{
		string fileDirectory;	//配置文件总路径，如"\\sd\\";
		//etc...
	};

	struct FileInfoData
	{
		FileInfoData();
		FileInfoData(string &file_path, int file_size, time_t last_access_time);
		string file_path;
		int file_size;
		//string last_access_time; //最近读取时间
		time_t last_access_time;
	};

	struct FileIndexData	// 在程序退出时将对应信息存回索引文件 保存各文件信息和文件总大小
	{
		~FileIndexData();
		int curr_used_space;	//所有文件总大小
		map<z11::MGWString, FileInfoData*> file_info;	//各文件信息 string: fcls_id + "_" + index
	};
}

#endif
