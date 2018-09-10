#include "FileDataFetcher.h"

namespace GIS_Embedded
{
	FileDataFetcher::FileDataFetcher()
	{
		m_file_buffer = new FileBuffer();
	}

	FileDataFetcher::~FileDataFetcher()
	{
		delete m_file_buffer;
	}

	int FileDataFetcher::init(FileParam &file_param, NetParam &net_param, FclsGlobalIndex *fcls_global_index)
	{
		m_fcls_global_index = fcls_global_index;
		if(m_file_buffer->init(file_param) < 0) //test the ( GIS_Embedded, GIS_Embedded/conf , GIS_Embedded/data, GIS/Embedded/image)folder is exist
			return -1;
		return 0;
	}

	int FileDataFetcher::getTerrsConf(byte_8 **data, int *size)
	{
		if(m_file_buffer->getTerrsConf(data, size) < 0)
			return -1;
		return 0;
	}

	int FileDataFetcher::getWorkSpaceConf(const z11::MGWString & workspace_name, byte_8 **data, int *size)
	{
		if(m_file_buffer->getWorkSpaceConf(workspace_name, data, size) < 0)
			return -1;
		return 0;
	}

	//int FileDataFetcher::getBitmap(const z11::MGWString &map_name, HBITMAP &map_handler)
	//{
	//	if(m_file_buffer->getBitmap(map_name, map_handler) < 0)
	//		return -1;
	//	//TODO: fetch bitmap from network (when necessary)
	//	return 0;
	//}

	//int FileDataFetcher::releaseBitmap(HBITMAP map_handler)
	//{
	//	if(m_file_buffer->releaseBitmap(map_handler) < 0)
	//		return -1;
	//	return 0;
	//}
// 	int FileDataFetcher::getBitmap(const z11::MGWString & map_name, MyBITMAP* bit_map)
// 	{
// 		if(m_file_buffer->getBitmap(map_name, bit_map) < 0)
// 			return -1;
// 		return 0;
// 	}
// 	int FileDataFetcher::releaseBitmap(MyBITMAP bit_map)
// 	{
// 		if(m_file_buffer->releaseBitmap(bit_map) < 0)
// 			return -1;
// 		//TODO: fetch bitmap from network (when necessary)
// 		return 0;
// 	}

	int FileDataFetcher::getSettings(byte_8 **data, int *size)
	{
		if(m_file_buffer->getSettings(data, size) < 0)
			return -1;
		return 0;
	}
//	int FileDataFetcher::getFixedRadar(byte_8 **data, int *size)
//	{
//		if (m_file_buffer->getFixedRadar(data,size) < 0)
//			return -1;
//		return 0;
//	}
//	int FileDataFetcher::getUnFixedRadar(byte_8 **data, int *size)
//	{
//		if (m_file_buffer->getUnFixedRadar(data,size) < 0)
//			return -1;
//		return 0;
//	}

	int FileDataFetcher::getProgramLanguage(byte_8 **data, int *size)
	{
		if(m_file_buffer->getProgramLanguage(data, size) < 0)
			return -1;
		return 0;
	}

	int FileDataFetcher::getDefaultSettings(byte_8 **data, int *size)
	{
		if(m_file_buffer->getDefaultSettings(data, size) < 0)
			return -1;
		return 0;
	}

	int FileDataFetcher::saveSettings(byte_8 *data, int size)
	{
		if(m_file_buffer->saveSettings(data, size) < 0)
			return -1;
		return 0;
	}

//	void FileDataFetcher::delPathPoints()
//	{
//		m_file_buffer->delPathPoints();
//	}
//
//	int FileDataFetcher::savePathPoints(vector<NO_ID> *graph_grl,Pos2DEx &start,Pos2DEx &end,double direction)//lw
//	{
//		if (m_file_buffer->savePathPoints(graph_grl,start,end,direction)<0)
//			return -1;
//		return 0;
//	}
//	int FileDataFetcher::outputMyPi(std::vector<z11::MGWString> &pi)
//	{
//		if(m_file_buffer->outputMyPi(pi)<0)
//			return -1;
//		return 0;
//	}
//	int FileDataFetcher::inputMyPi(vector<z11::MGWString> &pi)
//	{
//		if(m_file_buffer->inputMyPi(pi)<0)
//			return -1;
//		return 0;
//	}
//
	int FileDataFetcher::getGGPointFileInfos( z11::MGWString &key,int language_index,int offset,byte_8**data, int *size )
	{
		if(m_file_buffer->getGGPointFileInfos(key,language_index,offset,data,size) <0 )
			return -1;

		return 0;
	}

	int FileDataFetcher::getGGPointTriggerArea( z11::MGWString &key, int offset,byte_8 **data, int *size )
	{
		if (m_file_buffer->getGGPointTriggerArea(key,offset,data,size)<0)
			return -1;
		return 0;
	}

	int FileDataFetcher::getGGPointData( z11::MGWString &key, byte_8 **data, int *size )
	{
		if (m_file_buffer->getGGPointData(key,data,size)<0)
			return -1;
		return 0;
	}

}