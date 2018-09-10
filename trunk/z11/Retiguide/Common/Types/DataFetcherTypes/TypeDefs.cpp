

#include "TypeDefs.h"
//#include <map>

namespace GIS_Embedded
{

	FileInfoData::FileInfoData()
	{
		file_path = "";
		file_size = -1;
		last_access_time = -1;
	}

	FileInfoData::FileInfoData(string &file_path, int file_size, time_t last_access_time)
	{
		this->file_path = file_path;
		this->file_size = file_size;
		this->last_access_time = last_access_time;
	}

	FileIndexData::~FileIndexData()
	{
		map<z11::MGWString, FileInfoData*>::iterator iter;
		for(iter = file_info.begin(); iter != file_info.end(); iter ++)
		{
			delete iter->second;
		}
	}

}