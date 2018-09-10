#include "../Common/Types/DataFetcherTypes/TypeDefs.h"
#include "DataFetcherUtil.h"
#include "FileBuffer.h"
#include "PathDef.h"
#include <fstream>
#include <ctime>
#include "DataWriter.h"
#include "System/Shared/MGLog.h"
using namespace std;

namespace GIS_Embedded
{

	FileBuffer::FileBuffer()
	{
		m_file_index = new FileIndexData();
	}

	FileBuffer::~FileBuffer()
	{
		delete m_file_index;
	}

	int FileBuffer::init(FileParam &file_param)
	{
		m_file_param = file_param;
		string main_folder = PathDef::MainFolderPath(m_file_param);
		if(!FileAdapter::folderExist(main_folder))
		{
			if(FileAdapter::createFolder(main_folder) < 0)
				return -1;
		}

		string conf_folder = PathDef::ConfigFolderPath(m_file_param);
		if(!FileAdapter::folderExist(conf_folder))
		{
			if(FileAdapter::createFolder(conf_folder) < 0)
				return -1;
		}

		string data_folder = PathDef::DataFolderPath(m_file_param);
		if(!FileAdapter::folderExist(data_folder))
		{
			if(FileAdapter::createFolder(data_folder) < 0)
				return -1;
		}

		return 0;
		//create bmp folder is no means
		string bmp_folder = PathDef::BitmapFolderPath(m_file_param);
		if(!FileAdapter::folderExist(bmp_folder))
		{
			if(FileAdapter::createFolder(bmp_folder) < 0)
				return -1;
		}
		return 0;
	}

	int FileBuffer::getTerrsConf(byte_8 **data, int *size)
	{
		string file_path = PathDef::TerrsConfFilePath(m_file_param);
		if(FileAdapter::readFileData(file_path, data, size) < 0)	//读取对应文件;
			return -1;
		return 0;
	}

	int FileBuffer::getWorkSpaceConf(const z11::MGWString & workspace_name, byte_8 **data, int *size)
	{
		string file_path = PathDef::WorkspaceFilePath(m_file_param, workspace_name);
		if(FileAdapter::readFileData(file_path, data, size) < 0)	//读取对应文件;
			return -1;
		return 0;
	}

	//int FileBuffer::getBitmap(const z11::MGWString &map_name, HBITMAP &map_handler)
	//{
	//	string file_path = PathDef::BitmapFilePath(m_file_param, map_name);
	//	z11::MGWString wfile_path = DataFetcherUtil::string_to_wstring(file_path);
	//	map_handler = SHLoadDIBitmap(wfile_path.c_str());
	//	//*map_handler = bit_map;
	//	if(map_handler == NULL)
	//		return -1;
	//	return 0;
	//}

	//int FileBuffer::releaseBitmap(HBITMAP map_handler)
	//{
	//	if(NULL == map_handler)
	//		return -1;
	//	DeleteObject(map_handler);
	//	return 0;
	//}
// 	int FileBuffer::getBitmap(const z11::MGWString &map_name, MyBITMAP* bit_map)
// 	{
// 		string file_path = PathDef::BitmapFilePath(m_file_param, map_name);
// 		z11::MGWString wfile_path = DataFetcherUtil::string_to_wstring(file_path);
// 
// 
// #ifdef _G_RENDER_ENGINE
// 		//GXLoadBitmap(PTSTR pstrFileName, GBITMAP** ppOut);
// 		PGBITMAP bitmap=NULL;//Test GRF
// 		::GXLoadBitmap(wfile_path.c_str(),&bitmap);
// 		*bit_map = bitmap;
// #else
// 		*bit_map = SHLoadDIBitmap(wfile_path.c_str());
// 		if(*bit_map == NULL)
// 			return -1;
// #endif
// 		return 0;
// 	}
// 	int FileBuffer::releaseBitmap(MyBITMAP bit_map)
// 	{
// 		if(NULL == bit_map)
// 			return -1;
// #ifdef _G_RENDER_ENGINE
// 		PGBITMAP pgbmp;
// 		pgbmp = (PGBITMAP)bit_map;
// 		if(pgbmp->pBits)	
// 		{
// 			delete[] pgbmp->pBits;
// 			//free(pgbmp->pBits);
// 			pgbmp->pBits = NULL;
// 		}
// #else
// 		DeleteObject(bit_map);
// #endif
// 		return 0;
// 	}

	int FileBuffer::availableSize() const
	{
		return DISK_MAX_SIZE - m_file_index->curr_used_space;//返回剩余空间大小
	}
	int FileBuffer::spaceFree(int size)	//文件替换策略 size表示需要的空间数
	{
		return 0;
	//	//map<z11::MGWString, FileInfoData*>::iterator min_iter = m_file_index->file_info.begin();
	//	//min_iter->second;

	//	while(availableSize() < size)	//文件存储容量不够
	//	{
	//		//文件替换,根据FileIndex中的文件读取时间信息,删除最长没有访问的文件;
	//		map<z11::MGWString, FileInfoData*>::iterator min_iter = m_file_index->file_info.begin();
	//		time_t min_time = min_iter->second->last_access_time;
	//		map<z11::MGWString, FileInfoData*>::iterator iter = ++ min_iter;
	//		while(iter != m_file_index->file_info.end())
	//		{
	//			if(iter->second->last_access_time < min_time)
	//			{
	//				min_time = iter->second->last_access_time;
	//				min_iter = iter;
	//			}
	//			iter ++;
	//		}
	//		if(FileAdapter::deleteGeoFileDB(iter->second->file_path) < 0)	//不能删除
	//		{
	//			iter->second->last_access_time = /*time(0)*/1;	//设为当前时间,即刚刚访问
	//		}
	//		else
	//		{
	//			m_file_index->curr_used_space -= iter->second->file_size;	//更改已使用文件字节数等数据;
	//			delete min_iter->second;
	//			m_file_index->file_info.erase(min_iter);
	//		}
	//	}
	//	return 0;
	}

	int FileBuffer::getSettings(byte_8 **data, int *size)
	{
		string file_path = PathDef::SettingsFilePath(m_file_param);
		if(FileAdapter::readFileData(file_path, data, size) < 0)
			return -1;
		return 0;
	}

//	int FileBuffer::getFixedRadar(byte_8 **data,int *size)
//	{
//		string file_path = PathDef::FixedRadarFilePath(m_file_param);
//		if (FileAdapter::readFileData(file_path,data,size) < 0)
//			return -1;
//		return 0;
//	}
//	int FileBuffer::getUnFixedRadar(byte_8 **data,int *size)
//	{
//		string file_path = PathDef::UnFixedRadarFilePath(m_file_param);
//		if(FileAdapter::readFileData(file_path,data,size) < 0)
//			return -1;
//		return 0;
//	}
//	void FileBuffer::delPathPoints()
//	{
//		string file_path = PathDef::PathPointsFilePath(m_file_param);
//		ifstream in_file;
//		in_file.open(file_path.c_str(),ios_base::in);
//		if(in_file.is_open())
//		{
//			in_file.close();
//			LPCWSTR file_name = (LPCWSTR)String_Tools::strToWStr(file_path).c_str();
//			DeleteFile(file_name);
//		}
//
//	}
//

	int FileBuffer::getProgramLanguage(byte_8 **data, int *size)
	{
		string file_path = PathDef::ProgramLanguagePath(m_file_param);
		if(FileAdapter::readFileData(file_path, data, size) < 0)
			return -1;
		return 0;
	}

	int FileBuffer::getDefaultSettings(byte_8 **data, int *size)
	{
		string file_path = PathDef::DefaultSettingsFilePath(m_file_param);
		if(FileAdapter::readFileData(file_path, data, size) < 0)
			return -1;
		return 0;
	}

	int FileBuffer::saveSettings(byte_8 *data, int size)
	{
// 		string path = PathDef::SettingsFilePath(m_file_param);
// 		if(FileAdapter::saveFile(path, data, size) < 0)
// 			return -1;
		return 0;
	}
	
//	int FileBuffer::savePathPoints(vector<NO_ID> *graph_grl,Pos2DEx &start,Pos2DEx &end,double direction) //lw
//	{
//		string path = PathDef::PathPointsFilePath(m_file_param);
//		int sum = graph_grl->size();
//		if (sum ==0)
//			return -1;
//		ofstream out(path.c_str(),ios::trunc);
//		out<<start.d0<<' '<<start.d1<<' '<<end.d0<<' '<<end.d1<<' '<<direction<<' ';
//		for (int i=0 ;i < sum-1; i++)
//		{
//			NO_ID temp_grl = (*graph_grl)[i];
//			out<<temp_grl<<' ';
//		}
//		NO_ID temp_grl = (*graph_grl)[sum-1];
//		out<<temp_grl;
//		out.close();
//		return 0;
//	}
//
//	int FileBuffer::outputMyPi(std::vector<z11::MGWString> &pi)//导出
//	{
//		string path = PathDef::OutInPutMyPi(m_file_param);
//		int sum = pi.size();
//		if (sum ==0)
//			return -1;
//		ofstream out(path.c_str(),ios::trunc);
//		z11::MGWString name;
//		int_32 x,y;
//		for (int i=0; i<sum; i++)
//		{
//			z11::MGWString label = pi[i];
//			String_Tools::transToLabelPos(label,&name,&x,&y);
//			out<<label.c_str()<<' '<<x<<' '<<y<<' ';
//		}
//		out.close();
//		return 0;
//	}
//	int FileBuffer::inputMyPi(std::vector<z11::MGWString> &pi)//导入
//	{
//		string file_path = PathDef::PathPointsFilePath(m_file_param);
//		ifstream in_file;
//		in_file.open(file_path.c_str(),ios_base::in);
//		if(in_file.is_open())
//		{
//			while(!in_file.eof())
//			{
//				z11::MGWString new_label;	
//				string name;
//				int_32 x,y;
//				in_file>>name>>x>>y;
//				z11::MGWString tmp_name = String_Tools::strToWStr(name);
//				String_Tools::transToLabelStr(tmp_name,x,y,false,&new_label);
//				pi.push_back(new_label);
//			}
//			return 0;
//		}
//		return -1;
//	}

	void FileBuffer::toLower(z11::MGWString &name)
	{
		int size = name.size();
		for(int i = 0; i < size; i ++)
		{
			if('A' <= name[i] && name[i] <= 'Z')
				name[i] += 'a'-'A';
		}
	}

	int FileBuffer::getGGPointFileInfos( z11::MGWString &key,int language_index,int offset, byte_8**data, int *size )
	{
		string file_path = PathDef::GGPointFileInfosPath(m_file_param,key,language_index);
		if (FileAdapter::readFileData(file_path,offset,data,size) < 0)
			return -1;
		return 0;
	}

	int FileBuffer::getGGPointTriggerArea( z11::MGWString &key, int offset,byte_8 **data, int *size )
	{
		string file_path = PathDef::GGPointTriggerAreaPath(m_file_param,key);
		if (FileAdapter::readFileData(file_path,offset,data,size) < 0)
			return -1;
		return 0;
	}

	int FileBuffer::getGGPointData( z11::MGWString &key, byte_8 **data, int *size )
	{
		string file_path = PathDef::GGPointFilePath(m_file_param, key);

		if (FileAdapter::readFileData(file_path,data,size) < 0)
			return -1;
		return 0;
	}

}