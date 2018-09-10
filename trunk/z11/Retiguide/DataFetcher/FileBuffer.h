#ifndef _FILE_BUFFER_H_
#define _FILE_BUFFER_H_

#include "../Common/Types/basic_types.h"
#include "../Common/Types/DataFetcherTypes/TypeDefs.h"
#include "../Common/Types/basic_types.h"
#include "TypeDefs.h"
#include "IBuffer.h"
#include "FileAdapter.h"
#include "CommonBase/MGWString.h"

using namespace z11;

namespace GIS_Embedded
{
	class XmlReader;
	class FileBuffer : public IBuffer
	{
	public:
		FileBuffer();
		~FileBuffer();
		int init(FileParam &file_param);
		int getTerrsConf(byte_8 **data, int *size);
		int getWorkSpaceConf(const z11::MGWString & workspace_name, byte_8 **data, int *size);
//		int getBitmap(const z11::MGWString &map_name, HBITMAP &map_handler);
//		int releaseBitmap(HBITMAP map_handler);
// 		int getBitmap(const z11::MGWString &map_name, MyBITMAP* bit_map);
// 		int releaseBitmap(MyBITMAP bit_map);
		int getSettings(byte_8 **data, int *size);
		int getProgramLanguage(byte_8 **data, int *size);
		int getDefaultSettings(byte_8 **data, int *size);

		int availableSize() const;
		int spaceFree(int size);	//文件替换策略 size表示需要的空间数

		int saveSettings(byte_8 *data, int size);
//		int savePathPoints(vector<NO_ID> *graph_grl,Pos2DEx &start,Pos2DEx &end,double direction);//lw
//		void delPathPoints();//lw
//		int outputMyPi(vector<z11::MGWString> &pi);//dao chu
//		int inputMyPi(vector<z11::MGWString> &pi);//dao ru
//		int getFixedRadar(byte_8 **data,int *size);
//		int getUnFixedRadar(byte_8 **data,int *size);
//		//GRF
		int getGGPointFileInfos(z11::MGWString &key,int language_index,int offset, byte_8**data, int *size);
		int getGGPointTriggerArea(z11::MGWString &key, int offset,byte_8 **data, int *size);
		int getGGPointData(z11::MGWString &key, byte_8 **data, int *size);
	private:
		void toLower(z11::MGWString &name);

		FileIndexData *m_file_index;	//索引信息

		FileParam m_file_param;

	};

}

#endif