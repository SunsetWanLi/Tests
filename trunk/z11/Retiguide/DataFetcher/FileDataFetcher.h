#ifndef _FILE_DATA_FETCHER_H_
#define _FILE_DATA_FETCHER_H_

#include "FileBuffer.h"
#include "../Common/Types/DataFetcherTypes/TypeDefs.h"
#include "TypeDefs.h"
#include "CommonBase/MGWString.h"

using namespace z11;

namespace GIS_Embedded
{

	class FileDataFetcher
	{
	private:
		FileBuffer *m_file_buffer;
		FclsGlobalIndex *m_fcls_global_index;

	public:
		FileDataFetcher();
		~FileDataFetcher();
		int init(FileParam &file_param, NetParam &net_param, FclsGlobalIndex *fcls_global_index);
		int close();
		int getTerrsConf(byte_8 **, int *size);
		int getWorkSpaceConf(const z11::MGWString & workspace_name, byte_8 **, int *size);

//		int getBitmap(const z11::MGWString & map_name, HBITMAP &map_handler);
//		int releaseBitmap(HBITMAP map_handler);
// 		int getBitmap(const z11::MGWString & map_name, MyBITMAP* bit_map);
// 		int releaseBitmap(MyBITMAP bit_map);

		int getSettings(byte_8 **, int *size);
		int getDefaultSettings(byte_8 **, int *size);
		int saveSettings(byte_8 *, int size);
//		int savePathPoints(vector<NO_ID> *graph_grl,Pos2DEx &start,Pos2DEx &end,double direction);//lw
//		void delPathPoints();//lw
//		int outputMyPi(vector<z11::MGWString> &pi);
//		int inputMyPi(vector<z11::MGWString> &pi);
//
		int getProgramLanguage(byte_8 **, int *size);
//		int getFixedRadar(byte_8 **, int *size);
//		int getUnFixedRadar(byte_8 **, int *size);
//		//GRF
		int getGGPointFileInfos(z11::MGWString &key,int language_index,int offset,byte_8**data, int *size);
		int getGGPointTriggerArea(z11::MGWString &key, int offset,byte_8 **data, int *size);
		int getGGPointData(z11::MGWString &key, byte_8 **data, int *size);
	};

}
#endif