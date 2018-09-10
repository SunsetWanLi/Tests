#ifndef _FILE_ADAPTER_H_
#define _FILE_ADAPTER_H_

#include <stdio.h>
#include "../Common/Types/basic_types.h"
#include "../Common/Types/basic_types.h"
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <map>
#include <string>

using namespace std;

namespace GIS_Embedded
{

	class FileAdapter
	{
	public:
		static bool folderExist(const string &folder_name);
		static int createFolder(const string &folder_name);
		static int fileSize(FILE *fp);
		static int readFileData(const string &file_path, byte_8** data, int *size);
		static int deleteFile(const string &file_path);
		static int saveFile(const string &file_path, byte_8 *data, int size);
	
		//GRF
		static 	int readFileData(const string &file_path,int offset,byte_8 **data, int *size);
	};

}

#endif