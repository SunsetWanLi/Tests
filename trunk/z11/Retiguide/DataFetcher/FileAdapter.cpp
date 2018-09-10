#include "FileAdapter.h"
#include <fstream>
#include <ctime>
#include <iostream>
#include <algorithm>
#include "PathDef.h"
#include "System/Shared/Utility.h"
#include "Platform/MGFileUtils.h"
#include "System/Shared/MGLog.h"
using z11::MGLogD;
using namespace std;

namespace GIS_Embedded
{
	bool FileAdapter::folderExist(const string &folder_name)
	{
		return z11::Utility::isFileORDirExist(folder_name);
	}
	int FileAdapter::createFolder(const string &folder_name)
	{
		return z11::Utility::createDirOnPath(folder_name);
		//assert(0);
		return -1;
	}
	int FileAdapter::fileSize(FILE *fp)
	{
		int pos;
		int pos_cur;
		pos_cur = ftell(fp);
		fseek(fp,0,SEEK_END);
		pos = ftell(fp);
		fseek(fp , pos_cur, SEEK_SET);
		return pos;
	}

	int FileAdapter::readFileData(const string &file_path, byte_8** data, int *size)
	{
		MGData *input_data=MGData::dataWithContentsOfFile(file_path);
		if (input_data!=NULL)
		{
			*data=(byte_8*)input_data->bytes();
			*size=input_data->length();
			input_data->initWithBytesNoCopy(NULL,0);
			delete_and_set_null(input_data);
			return 0;
		}
		else
			return -1;
	}

	int FileAdapter::readFileData( const string &file_path,int offset,byte_8 **data, int *size )
	{
		MGLogD("FileAdapter::readFileData");
		assert(0);
		FILE* fp_in;
        fp_in = fopen(file_path.c_str(), "rb");
		if(fp_in == NULL)
		{
			return -1;
		}
		fseek(fp_in,offset,SEEK_SET);
		byte_8 *tempdata = new byte_8[*size];

		fread(tempdata,*size,1,fp_in);
		*data = tempdata;

		fclose(fp_in);

		return 0;
	}

	int FileAdapter::deleteFile(const string &file_path)
	{
		assert(0);
		return -1;
	}

	int FileAdapter::saveFile(const string &file_path, byte_8 *data, int size)
	{
		ofstream ofstrm(file_path.c_str(), ios::binary);
		if(!ofstrm.is_open())
			return -1;

		char *tmp_data = (char *)data;
		ofstrm.write(tmp_data, size);
		ofstrm.close();
		return 0;
	}
}