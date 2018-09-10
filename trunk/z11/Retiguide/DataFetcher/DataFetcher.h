#ifndef _DATA_FETCHER_H__
#define _DATA_FETCHER_H__

#include "FileDataFetcher.h"
#include "../Common/Types/Radar.h"
#include "../Common/Types/WorkSpaceTypes/WorkSpace.h"
#include "../Common/Types/WorkSpaceTypes/TerrCtrl.h"
#include "../Common/Types/FeatureClassInfo.h"
#include "System/Shared/Syncable.h"
#include "TypeDefs.h"
#include "IBuffer.h"
#include "CommonBase/MGWString.h"

#include <vector>
#include <fstream>
#include <map>
//GRF
#include "../Common/Types/DataFetcherTypes/TypeDefs.h"

using namespace std;
using namespace z11;

#ifdef _DEBUG
#define _LOG_INFO
#endif

namespace GIS_Embedded
{
	class FileDataFetcher;
	class RamBuffer;
	class NWDataBuffer;
	class NWGridBuffer;
	class GMLineBuffer;
	//GRF
	struct FileInfos;
	class GGPointBuffer;

	class DataFetcher	//负责从MemBuffer取数据 如果没有则从FileBufferFetcher读 不关心文件存在情况和网络读取情况 它们均由FileBufferFetcher管理 
	{
 	private:
 		FileDataFetcher *m_file_data_fetcher;
 		vector<IBuffer *> m_buffers;
 		RamBuffer *m_ram_buffer;
 		Settings *m_settings;
 		ProgramLanguage *m_languages;
 		MapConfigType *m_map_config;
 		//GRF
 		FileParam m_file_param;
 		FclsGlobalIndex *m_fcls_global_index;
		GGPointBuffer *m_gg_point_buffer; 
		GGPointConf m_gg_point_conf;
 
 		static DataFetcher* m_instance;
 		static Syncable* m_syn;
 
 		//雷达有关
 		Radar m_fixed_radar;
 		Radar m_unfixed_radar;
 
 		int m_lastLanguageIndex;
 
 // 		int spaceCheckAndFree(int need_size);
 
 		static bool availSizeLesser(IBuffer *buffer1, IBuffer *buffer2)
 		{
 			return buffer1->availableSize() < buffer2->availableSize();
 		}
 
 	private:
 		DataFetcher();
 		int init();
 
 	public:
 
 		static int times_exchange_memory;
 		static DataFetcher* getInstance();
 
 
 		~DataFetcher();
 
 		int close();
 		int LOG_nw_block();
 		int getTerrsConf(TerrCtrl **terr_ctrl);
 		int getWorkSpaceConf(const z11::MGWString & workspace_name, WorkSpace *work_space);
  		int getFclsInfoConf(int fcls_id, FeatureClassInfo **fcls_info);
//  		int getBitmap(const z11::MGWString &map_name, HBITMAP &map_handler);
//  		int releaseBitmapHBITMAP(const z11::MGWString map_name);
 
//   		int getBitmap(const z11::MGWString &map_name, MyBITMAP *bit_map);
//  		int releaseBitmapMyBITMAP(const z11::MGWString map_name);
 
 		int getFclsGlobalIndex(FclsGlobalIndex **fcls_global_index)
 		{
 			//初始化的时候取到，此处直接赋值并返回。
 			*fcls_global_index = m_fcls_global_index;
 			return 0;
 		}
 
 		int initSettings();
 		//RADAR
 		int initFixedRadar();
 		int initUnFixedRadar();
 		int getFixedRadar(unsigned int path_id,vector<Pos2DEx> &fix_radar_points);
 		int getUnFixedRadar(unsigned int path_id,vector<Pos2DEx> &unfix_radar_points);
 
 		int loadDefaultSetting();
 		int	getSettings(Settings **settings)
 		{
 			*settings = m_settings;
 			return 0;
 		}
 
 		int initProgramLanguage();
 		int getProgramLanguage(ProgramLanguage **lang)
 		{
 			*lang = m_languages;
 			return 0;
 		}
 		int saveSettings();
 		int outputMyPi(vector<z11::MGWString> &pi);
 		int inputMyPi(vector<z11::MGWString> &pi);
 
 		int getMapConfig(MapConfigType **map_config)
 		{
 			*map_config = m_map_config;
 			return 0;
 		}
 
 		//GRF
 		int getGGPointFileInfos(z11::MGWString &key,int language_index, int offset,int size,FileInfos **file_info);
 		int getGGPointTriggerArea(z11::MGWString &key, int offset,int size,vector<PolygonArea> **tri_area);
 		int getGGPoint(z11::MGWString type_key, GGPointBlock ** gg_point_block);
 		int getGGPointFromName(z11::MGWString &name , GuiderGeoPoint** p);
 		int getScenicAreaMap(ScenicAreaMap &scenicAreaMap,vector<z11::MGWString> &scenicAreaNames);
 		int getIndoorPointGroup(ScenicAreaMap &indoorPointMap);
 		int getPairPointGroup(ScenicAreaMap &pairPointMap);
 		int getGGPointConf(GGPointConf *gg_point_conf);
 		int getGGPointTypes(vector<z11::MGWString> &types);
 
 		int getSightButtonConf1(vector<SightButtonConf> &sightbuttons);
 		int getSightButtonConf0(vector<SightButtonConf> &sightbuttons);
 
 		int getCounterList(vector<z11::MGWString> &counterlist);
 		
	};

}

#endif