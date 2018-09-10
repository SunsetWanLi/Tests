#include "DataFetcher.h"
#include "TerrCtrlAdapter.h"
#include "FclsInfoAdapter.h"
#include "WorkspaceAdapter.h"
#include "MapConfigAdapter.h"
#include "SettingsAdapter.h"
#include "ProgramLanguageAdapter.h"
#include "PathDef.h"
#include "FileAdapter.h"
#include "FileDataFetcher.h"
#include "RamBuffer.h"
#include "RadarAdapter.h"
//GRF
#include "GuiderGeoPointAdapter.h"
#include <fstream>
#include <algorithm>
#include <vector>
#include <map>
#include "GGPointBuffer.h"
#include "System/Shared/Utility.h"
#include "System/Shared/MGLog.h"
#include "CommonBase/MGData.h"
#include "CommonBase/MGDataReader.h"
#include "System/Shared/MGPath.h"
//#include "../../include/MGFoundation.h"

using namespace std;
using namespace z11;

namespace GIS_Embedded
{

	ofstream DF_file;

	DataFetcher* DataFetcher::m_instance = NULL;

	Syncable* DataFetcher::m_syn = new Syncable();

#ifdef _DEBUG
	int DataFetcher::times_exchange_memory =0;
#endif


	DataFetcher::DataFetcher()
	{
	    m_file_data_fetcher = NULL;
		m_ram_buffer = NULL;
		m_settings = NULL;
		m_languages = NULL;
		m_map_config = NULL;
		m_fcls_global_index = NULL;
		m_gg_point_buffer = NULL;

		init();
	}

	DataFetcher::~DataFetcher()
	{
		delete m_file_data_fetcher;
		delete m_ram_buffer;
		delete m_settings;
		delete m_map_config;
		delete m_fcls_global_index;
	}

	DataFetcher* DataFetcher::getInstance()
	{
		if(m_instance == NULL)
		{
			Syncable::AutoLock inner_syn(m_syn);
			if(m_instance ==NULL)
			{
				m_instance = new DataFetcher();
			}
		}
		return m_instance;
	}
	int DataFetcher::init()
	{
		Syncable::AutoLock inner_syn(m_syn);

		//CoInitializeEx(NULL, COINIT_MULTITHREADED);

		NetParam net_param;
		FileParam file_param;
#ifdef ANDROID_NDK
		m_file_param.fileDirectory = z11::MGBundle::mainBundle()->pathWithFileName("");//"/sdcard/";//MGPath::getInstance()->getRootDriverPath();//z11::MGBundle::mainBundle()->pathWithFileName("");
		file_param.fileDirectory = z11::MGBundle::mainBundle()->pathWithFileName("");//"/sdcard/";//MGPath::getInstance()->getRootDriverPath();//z11::MGBundle::mainBundle()->pathWithFileName("");
#else
		m_file_param.fileDirectory = z11::MGBundle::mainBundle()->pathWithFileName("");
		file_param.fileDirectory = z11::MGBundle::mainBundle()->pathWithFileName("");        
#endif
		string path = PathDef::NetConfigPath(file_param);//find the NetConfigPath $Resource/GIS_Embedded/conf/netconfig.cfg
		MGData *data=MGData::dataWithContentsOfFile(path);
		istringstream fin((char *)data->bytes());
		delete_and_set_null(data);

		string tmp_online, tmp_server_ip, tmp_passwd;
		fin>>tmp_online;
		if(tmp_online == "true" || tmp_online == "TRUE")
			net_param.online = true;
		else
			net_param.online = false;
		fin>>tmp_server_ip;
		net_param.server_ip = tmp_server_ip;
		fin>>net_param.port;
		fin>>net_param.user;
		fin>>tmp_passwd;
		net_param.passwd = tmp_passwd;
//		fin.close();

		path = PathDef::MapConfigFilePath(file_param);//find the MapConfigFilePath $Resource/GIS_Embedded/conf/MapConfig.cfg
		MGData *map_config_data=MGData::dataWithContentsOfFile(path);
		if(!map_config_data)
			return -1;
		istringstream map_config_fin((char *)map_config_data->bytes());
		delete_and_set_null(map_config_data);
		MapConfigAdapter::translate(map_config_fin, &m_map_config); //reading GIS_Embedded/conf/MapConfig.cfg to m_map_config

		path = PathDef::FclsGlobalIndexFilePath(file_param);//find the Global Index File $Resource/GIS_Embedded/conf/FclsGlobalIndex.cfg
		m_fcls_global_index = new FclsGlobalIndex();
		if(m_fcls_global_index->inputConf(path) < 0)
			return -1;

		m_file_data_fetcher = new FileDataFetcher();
		m_file_data_fetcher->init(file_param, net_param, m_fcls_global_index);//test the ( GIS_Embedded, GIS_Embedded/conf , GIS_Embedded/data, GIS/Embedded/image)folder is exist

		if(initSettings() < 0)	//reading GIS_Embedded/conf/Settings.cfg to m_setting
			if(loadDefaultSetting() < 0) //reading GIS_Embedded/conf/DefaultSettings.cfg
				m_settings = new Settings();

		initFixedRadar();		//do nothing
		initUnFixedRadar();		//do nothing
		initProgramLanguage();	//reading GIS_Embedded/conf/Language.lan to m_languages
		m_languages->changeLanguage(*m_settings); //select index of m_languages to m_current_language

		m_ram_buffer = new RamBuffer(); //doesn't use RamBuffer
		if(m_ram_buffer->init() < 0)
			return -1;
		m_buffers.push_back(m_ram_buffer);

		m_lastLanguageIndex = m_languages->m_current_language->INDEX; //save current language 's index
		//GRF
 		m_gg_point_buffer = new GGPointBuffer();
 		return 0;
	}

// 	int DataFetcher::spaceCheckAndFree(int need_size)
// 	{
// 			MEMORYSTATUS mst;
// 			mst.dwLength = sizeof(MEMORYSTATUS);
// 			GlobalMemoryStatus(&mst);
//
// #ifdef NEED_LOG
// 			LOG_INFO("    available space:");
// 			LOG_INFO_ENDL(mst.dwAvailPhys);
// #endif
//
// 			/*//int avail = 32000000 -(m_mst.dwAvailPhys - mst.dwAvailPhys) ; */
//
// 			if(mst.dwAvailPhys < LOW_AVAIL_PHYS  || mst.dwAvailVirtual < LOW_AVAIL_PHYS)
// 			{
// #ifdef _DEBUG
// 				times_exchange_memory++;
// #endif
//
// 				sort(m_buffers.begin(), m_buffers.end(), availSizeLesser);
// 				int i;
// 				for(i = 0; i < m_buffers.size(); ++ i)
// 				{
// 					if(m_buffers[i]->spaceFree(LOW_AVAIL_PHYS /*LOW_AVAIL_PHYS - mst.dwAvailPhys*/ + need_size) == 0)
// 						break;
// 				}
// //#ifdef NEED_LOG
// 				if(i == m_buffers.size())
// 				{
// 					LOG_INFO_ENDL("  !!!UNABLE TO FREE MORE SPACE!!!");
// 					LOG_nw_block();
// 				}
//
//
// 				LOG_INFO("  space after free:");
// 				LOG_INFO_ENDL(mst.dwAvailVirtual/*mst.dwAvailPhys*/);
// //#endif
//
// 			}
//
// 			return 0;
// 	}
//
//	int DataFetcher::close()
//	{
//		return m_file_data_fetcher->close();
//	}
//
//	int DataFetcher::LOG_nw_block()
//	{
//#ifdef NEED_LOG
//		Syncable::AutoLock inner_syn(m_syn);
//		hash_map<NO_ID, NWDataBlock *>::iterator iter;
//		for(iter = m_nw_ram_buffer->m_hash_blocks.begin(); iter != m_nw_ram_buffer->m_hash_blocks.end(); ++ iter)
//		{
//			LOG_INFO("   block start id:");
//			LOG_INFO(iter->second->start_id);
//			LOG_INFO("    block reference count:");
//			LOG_INFO_ENDL(iter->second->ref_count);
//		}
//#endif
//		return 0;
//	}
//
//	int DataFetcher::calcParam(NO_ID (*func_ptr)(const NO_ID id), const int id, const int max_obj_count, vector<NWTypeEnum> &nw_type/*out*/, vector<NO_ID> &need_id_begin/*out*/, vector<NO_ID> &need_id_last/*out*/)
//	{
//		int i, block_id_begin = func_ptr(id), block_id_end = block_id_begin + max_obj_count;
//		for(i = 0; i < m_id_range_conf.conf.size(); ++ i)
//		{
//			NO_ID start_id = m_id_range_conf.conf[i].start_id;
//			NO_ID end_id = start_id + m_id_range_conf.conf[i].id_sum;	//the next id of the last id in this type of data
//
//			NO_ID tmp_id_begin, tmp_id_last;
//			tmp_id_begin = func_ptr(id);
//			tmp_id_last = tmp_id_begin + max_obj_count - 1;
//
//			if(rangeCross(start_id, end_id, block_id_begin, block_id_end))
//			{
//				nw_type.push_back(m_id_range_conf.conf[i].nw_type);
//
//				if(tmp_id_begin < start_id)
//					need_id_begin.push_back(start_id);
//				else
//					need_id_begin.push_back(tmp_id_begin);
//
//				if(tmp_id_last >= end_id)
//					need_id_last.push_back(end_id -1);
//				else
//					need_id_last.push_back(tmp_id_last);
//			}
//		}
//
//		if(nw_type.size() == 0)
//			return -1;
//
//		return 0;
//	}
//

	int DataFetcher::getTerrsConf(TerrCtrl **terr_ctrl)
	{
		Syncable::AutoLock inner_syn(m_syn);
		*terr_ctrl = NULL;
		byte_8 *data;
		int size;
		if(m_file_data_fetcher->getTerrsConf(&data, &size) < 0)
			return -1;
		if(TerrCtrlAdapter::translate(data, size, terr_ctrl) < 0)
			return -1;
		delete []data;
		return 0;

	}

	int DataFetcher::getWorkSpaceConf(const z11::MGWString & workspace_name, WorkSpace *work_space)
	{
		Syncable::AutoLock inner_syn(m_syn);
		//work_space = NULL;
		byte_8 *data;
		int size;
		if(m_file_data_fetcher->getWorkSpaceConf(workspace_name, &data, &size) < 0)
			return -1;
		if(WorkSpaceAdapter::translate(data, size, work_space) < 0)
			return -1;
		delete []data;
		return 0;
	}

// 	int DataFetcher::getFclsInfoConf(int fcls_id, FeatureClassInfo **fcls_info)
// 	{
// 		Syncable::AutoLock inner_syn(m_syn);
// 		*fcls_info = NULL;
// 		byte_8 *data;
// 		//int type;
// 		int size;
// 		if(m_file_data_fetcher->getFclsInfoConf(fcls_id, &data, &size) < 0)
// 			return -1;
// 		if(FclsInfoAdapter::translate(data, size, m_map_config->grid_side_divider, fcls_info) < 0)
// 			return -1;
// 		delete []data;
// 		return 0;
// 	}

// 	int DataFetcher::getBitmap(const z11::MGWString &map_name, HBITMAP &map_handler)
// 	{
// 		Syncable::AutoLock inner_syn(m_syn);
// 		map_handler = NULL;
// 		if(m_ram_buffer->getBitmap(map_name, map_handler) < 0)
// 		{
// 			if(m_file_data_fetcher->getBitmap(map_name, map_handler) < 0)
// 				return -1;
//
// 			m_ram_buffer->saveBitmapRam(map_name,map_handler); //loujt..09-04-30
// 		}
// 		return 0;
// 	}

// 	int DataFetcher::releaseBitmapHBITMAP(const z11::MGWString map_name)
// 	{
// 		Syncable::AutoLock inner_syn(m_syn);
// 		HBITMAP map_handler;
// 		map_handler = NULL;
// 		if(m_ram_buffer->getBitmap(map_name, map_handler) < 0)
// 			return -1;
// 		if(m_file_data_fetcher->releaseBitmap(map_handler) < 0)
// 			return -1;
// 		if(m_ram_buffer->deleteBitmapRam(map_name) < 0)
// 		{
// 			return -1;
// 		}
// 		return 0;
// 	}

 	//int DataFetcher::getBitmap(const z11::MGWString &map_name, MyBITMAP *bit_map)
 	//{
 	//	Syncable::AutoLock inner_syn(m_syn);
 	//	*bit_map = NULL;

 	//	if(m_ram_buffer->getBitmap(map_name, bit_map) < 0)
 	//	{
 	//		if(m_file_data_fetcher->getBitmap(map_name, bit_map) < 0)
 	//			return -1;

 	//		m_ram_buffer->saveBitmapRam(map_name,*bit_map); //loujt..09-04-30
 	//	}
 	//	return 0;
 	//}

//	int DataFetcher::releaseBitmapMyBITMAP(const z11::MGWString map_name)
//	{
//
////		Syncable::AutoLock inner_syn(m_syn);
////		MyBITMAP *bit_map;
////		bit_map = NULL;
////		if(m_ram_buffer->getBitmap(map_name, bit_map) < 0)
////			return -1;
////		if(m_file_data_fetcher->releaseBitmap(bit_map) < 0)
////			return -1;
////		if(m_ram_buffer->deleteBitmapRam(map_name) < 0)
////		{
////			return -1;
////		}
////		return 0;
//		Syncable::AutoLock inner_syn(m_syn);
//		MyBITMAP bit_map;
//		bit_map = NULL;
//		if(m_ram_buffer->getBitmap(map_name, &bit_map) < 0)
//			return -1;
//		if(m_file_data_fetcher->releaseBitmap(bit_map) < 0)//
//			return -1;
//		if(m_ram_buffer->deleteBitmapRam(map_name) < 0)//
//		{
//			return -1;
//		}
//		return 0;
//	}
	int DataFetcher::initSettings()
	{
		Syncable::AutoLock inner_syn(m_syn);
		byte_8 *data;
		int size;
		if(m_file_data_fetcher->getSettings(&data, &size) < 0)
			return -1;
		if(SettingsAdapter::translate(data, size, &m_settings) < 0)
			return -1;
		delete []data;
		return 0;
	}

	int DataFetcher::initFixedRadar()
	{
// 		Syncable::AutoLock inner_syn(m_syn);
// 		byte_8 *data;
// 		int size;
// 		if (m_file_data_fetcher->getFixedRadar(&data,&size) < 0)
// 			return -1;
// 		if(RadarAdapter::translate(data,size,&m_fixed_radar) < 0)
// 			return -1;
// 		delete []data;
		return 0;
	}
	int DataFetcher::initUnFixedRadar()
	{
// 		Syncable::AutoLock inner_syn(m_syn);
// 		byte_8 *data;
// 		int size;
// 		if (m_file_data_fetcher->getUnFixedRadar(&data,&size) < 0)
// 			return -1;
// 		if(RadarAdapter::translate(data,size,&m_unfixed_radar) < 0)
// 			return -1;
// 		delete []data;
		return 0;

	}

	int DataFetcher::getFixedRadar(unsigned int path_id,vector<Pos2DEx> &fix_radar_points)
	{
		m_fixed_radar.getRadarPoint(path_id,fix_radar_points);
		return 0;
	}

	int DataFetcher::getUnFixedRadar(unsigned int path_id, std::vector<Pos2DEx> &unfix_radar_points)
	{
		m_unfixed_radar.getRadarPoint(path_id,unfix_radar_points);
		return 0;
	}

	int DataFetcher::initProgramLanguage()
	{
		Syncable::AutoLock inner_syn(m_syn);
		byte_8 *data;
		int size;
		if(m_file_data_fetcher->getProgramLanguage(&data, &size) < 0)
			return -1;

		if(ProgramLanguageAdapter::translate(data, size, &m_languages) < 0)
			return -1;
		delete []data;
		return 0;
	}

	int DataFetcher::loadDefaultSetting()
	{
		Syncable::AutoLock inner_syn(m_syn);
		byte_8 *data;
		int size;
		if(m_file_data_fetcher->getDefaultSettings(&data, &size) < 0)
			return -1;
		if(SettingsAdapter::translate(data, size, &m_settings) < 0)
			return -1;
		delete []data;
		return 0;
	}

	int DataFetcher::saveSettings()
	{
		Syncable::AutoLock inner_syn(m_syn);
		byte_8 *data;
		int size;
		if(SettingsAdapter::pack(*m_settings, &data, &size) < 0)
			return -1;
		if(m_file_data_fetcher->saveSettings(data, size) < 0)
			return -1;
		delete []data;
		return 0;
	}

	int DataFetcher::outputMyPi(vector<z11::MGWString> &pi)
	{
// 		Syncable::AutoLock inner_syn(m_syn);
// 		if(m_file_data_fetcher->outputMyPi(pi)<0)
// 			return -1;
		return 0;
	}

	int DataFetcher::inputMyPi(vector<z11::MGWString> &pi)
	{
// 		Syncable::AutoLock inner_syn(m_syn);
// 		if(m_file_data_fetcher->inputMyPi(pi)<0)
// 			return -1;
		return 0;
	}


	int DataFetcher::getGGPoint(z11::MGWString type_key, GGPointBlock ** gg_point_block)
	{
 		Syncable::AutoLock inner_syn(m_syn);
 		if (m_gg_point_buffer->getBlock(type_key,gg_point_block) <0)
 		{
 			byte_8 *data = NULL;
 			int size;
 			vector<GuiderGeoPoint*> *gg_points = NULL;	//
 			if(m_file_data_fetcher->getGGPointData(type_key,&data,&size) < 0)
 				return -1;
 			if(m_gg_point_conf.conf.size() == 0)
 				getGGPointConf(&m_gg_point_conf);
			map<z11::MGWString,GGPTypeT>::iterator it = m_gg_point_conf.conf.find(type_key);
			if(it == m_gg_point_conf.conf.end())
				return -2;
 			GGPTypeT typeT =it->second;// GGPT_NOTRIGGER;//GGPT_TRIGGER;//m_gg_point_conf.conf[type_key];
 			if(GuiderGeoPointAdapter::translate(typeT,data,size,&gg_points) < 0)	//
 				return -1;

 			delete data;
 			m_gg_point_buffer->saveRam(type_key,gg_points,size);
 			m_gg_point_buffer->getBlock(type_key,gg_point_block);
 		}
		return 0;

	}
	int DataFetcher::getGGPointTriggerArea(z11::MGWString &key, int offset,int size,vector<PolygonArea> **tri_area)
	{
 		Syncable::AutoLock inner_syn(m_syn);
 		byte_8 *data = NULL;
 		//int size;
 		if (m_file_data_fetcher->getGGPointTriggerArea(key,offset,&data,&size) <0)
 			return -1;

 		if (GuiderGeoPointAdapter::translateTrigerArea(data,size,tri_area) <0)
 			return -1;

 		delete data;
		return 0;
	}
	int DataFetcher::getGGPointFileInfos(z11::MGWString &key,int language_index, int offset,int size,FileInfos **file_info)
	{
 		Syncable::AutoLock inner_syn(m_syn);
 		byte_8 *data = NULL;
 		//int size;
 		if (m_file_data_fetcher->getGGPointFileInfos(key,language_index,offset,&data,&size) < 0)
 			return -1;

 		if (GuiderGeoPointAdapter::translateFileInfos(data,size,file_info) <0)
 			return -1;


 		delete data;

		return 0;//to do
	}
	int DataFetcher::getGGPointFromName(z11::MGWString &name , GuiderGeoPoint** p)
	{
 		Syncable::AutoLock inner_syn(m_syn);
 		if(m_gg_point_conf.conf.size() == 0)
 			getGGPointConf(&m_gg_point_conf);
 		map<z11::MGWString,GGPTypeT>::iterator it = m_gg_point_conf.conf.begin();
 		while (m_gg_point_buffer->getGGPoint(name,p)<0 )
 		{
 			if(it==m_gg_point_conf.conf.end())
 				return -1;
 			z11::MGWString type_key = it->first;

 			++it;

 			byte_8 *data = NULL;
 			int size;
 			vector<GuiderGeoPoint*> *gg_points = NULL;

 			GGPointBlock * block;


 			if(m_gg_point_buffer->getBlock(type_key,&block) == 0)
 				continue;

 			if(m_file_data_fetcher->getGGPointData(type_key,&data,&size) < 0)
 				return -1;
 			//GGPTypeT typeT = GGPT_TRIGGER;//m_gg_point_conf.conf[type_key];

 			GGPTypeT typeT =m_gg_point_conf.conf[type_key];

 			if(GuiderGeoPointAdapter::translate(typeT,data,size,&gg_points) < 0)//
 				return -1;

 			delete data;
 			m_gg_point_buffer->saveRam(type_key,gg_points,size);

 		}
		return 0;

	}

	int DataFetcher::getScenicAreaMap(ScenicAreaMap &scenicAreaMap,vector<z11::MGWString> &scenicAreaNames)
	{
 		int ret = 0;
 		string path = PathDef::ScenicAreaConfigPath(m_file_param);
 		ifstream fin(path.c_str());
 		assert(fin.is_open());
 		int num1=0,num2;
 		z11::MGWString name1,name2;
		z11::MGAString nm1, nm2;
 		fin>>num1;
 		while(num1-- && !fin.eof())
 		{
 			fin>>nm1;
			name1 = Utility::astr2wstr(nm1);
			fin>>num2;
 			vector<z11::MGWString> secnicPoints;
 			while(num2--&&!fin.eof() )
 			{
 				fin>>nm2;
				name2 = Utility::astr2wstr(nm2);
 				secnicPoints.push_back(name2);
 			}
 			scenicAreaMap[name1] = secnicPoints;
 			scenicAreaNames.push_back(name1);
 		}
 		if(!(fin.eof() && num1==-1))
 			ret = -1;
		return ret;
	}
	int DataFetcher::getPairPointGroup(ScenicAreaMap &pairPointMap)
	{
		int ret = 0;
		string path = PathDef::PairPointGroupConfigPath(m_file_param);
		ifstream fin(path.c_str());
		assert(fin.is_open());
		int num1=0,num2;
		z11::MGWString name1,name2;
		z11::MGAString nm1, nm2;
		fin >> num1;
		while(num1-- && !fin.eof())
		{
			fin >> nm1;
			name1 = Utility::astr2wstr(nm1);
			fin >> num2;
			vector<z11::MGWString> secnicPoints;
			while(num2--&&!fin.eof())
			{
				fin >> nm2;
				name2 = Utility::astr2wstr(nm2);
				secnicPoints.push_back(name2);
			}
			pairPointMap[name1] = secnicPoints;
		}

		if(!fin.eof() && num1==-1)
			ret = -1;
		return ret;

	}
	int DataFetcher::getIndoorPointGroup(ScenicAreaMap &indoorPointMap)
	{
		int ret = 0;
		string path = PathDef::IndoorAreaPointGroupConfigPath(m_file_param);
		ifstream fin(path.c_str());
		assert(fin.is_open());
		int num1=0,num2;
		z11::MGWString name1,name2;
		z11::MGAString nm1, nm2;
		fin>>num1;
		while(num1-- && !fin.eof())
		{
			fin>>nm1;
			name1 = Utility::astr2wstr(nm1);
			fin>>num2;
			vector<z11::MGWString> secnicPoints;
			while(num2--&&!fin.eof() )
			{
				fin>>nm2;
				name2 = Utility::astr2wstr(nm2);
				secnicPoints.push_back(name2);
			}
			indoorPointMap[name1] = secnicPoints;
		}

		if(!(fin.eof() && num1==-1))
			ret = -1;
		return ret;
	}

	int DataFetcher::getSightButtonConf1(vector<SightButtonConf> &sightbuttons)
	{
		int ret = 0;
		//string path = PathDef::SightButtonConf1Path(m_file_param);
		//wifstream fin(path.c_str());
		//assert(fin.is_open());
		//int num2;
		//z11::MGWString name1,name2;
		//fin>>num2;
		//while(num2-- && !fin.eof())
		//{
		//	SightButtonConf temp;
		//	fin>>temp.name;
		//	fin>>temp.buttonID;
		//	fin>>temp.x;
		//	fin>>temp.y;
		//	fin>>temp.nWidth;
		//	fin>>temp.nHeight;
		//	sightbuttons.push_back(temp);
		//}
		//if(!(fin.eof() && num2==-1))
		//	ret = -1;
		return ret;
	}

	int DataFetcher::getSightButtonConf0(vector<SightButtonConf> &sightbuttons)
	{
		int ret = 0;
		string path = PathDef::SightButtonConf0Path(m_file_param);
		ifstream fin(path.c_str());
		assert(fin.is_open());
		int num2;
		//z11::MGWString name1,name2;
		//z11::MGAString nm1, nm2;
		z11::MGAString nm;
		fin>>num2;
		while(num2-- && !fin.eof())
		{
			SightButtonConf temp;
			//fin>>temp.name;
			fin >> nm;
			temp.name = Utility::astr2wstr(nm);
			fin>>temp.buttonID;
			fin>>temp.x;
			fin>>temp.y;
			fin>>temp.nWidth;
			fin>>temp.nHeight;
			sightbuttons.push_back(temp);
		}
		if(!(fin.eof() && num2==-1))
			ret = -1;
		return ret;
	}

	int DataFetcher::getCounterList(vector<z11::MGWString> &countlist)
	{
		int ret = 0;
		string path = PathDef::CounterListPath(m_file_param);
		MGDataReader reader(path);
		assert(reader.is_open());
		istringstream &fin=reader.getStringStream();
		int num2;
		//z11::MGWString name1,name2;
		fin >> num2;
		while(num2-- && !fin.eof())
		{
			z11::MGAString temp;
			fin >> temp;
			countlist.push_back(Utility::astr2wstr(temp));
		}
		if(!(fin.eof() && num2==-1))
			ret = -1;
		return ret;
	}

	int DataFetcher::getGGPointConf(GGPointConf *gg_point_conf)
	{
		Syncable::AutoLock inner_syn(m_syn);
		gg_point_conf->conf.clear();

		string path = PathDef::GGPointConfPath(m_file_param);
		MGData *data=MGData::dataWithContentsOfFile(path);
		assert(data!=NULL);
		istringstream fin((char *)data->bytes());
		delete_and_set_null(data);
		z11::MGWString name,sign;
		z11::MGAString name1, sign1;
		GGPTypeT type;
		int ret = 0;

		while( !fin.eof())
		{
			fin >> name1;
			fin >> sign1;
			name = z11::Utility::astr2wstr(name1);
			sign = z11::Utility::astr2wstr(sign1);
			if(sign == WS(L"true") || sign == WS(L"TRUE"))
				type = GGPT_TRIGGER;
			else if (sign == WS(L"false") || sign == WS(L"FALSE"))
				type = GGPT_NOTRIGGER;
			else
			{
				// file error!
				ret = -1;
				break;
			}
			gg_point_conf->conf[name] = type;
		}

		return ret;

	}

	int DataFetcher::getGGPointTypes(vector<z11::MGWString> &types)
	{
		types.clear();
		if(m_gg_point_conf.conf.size() == 0)
			getGGPointConf(&m_gg_point_conf);
		map<z11::MGWString,GGPTypeT>::iterator it = m_gg_point_conf.conf.begin();
		while(it!= m_gg_point_conf.conf.end())
		{
			types.push_back(it->first);
			it++;
		}
		return 0;
	}

//	//int DataFetcher::getFclsInfoConf(const z11::MGWString & fcls_id, FeatureClassInfo **fcls_info)
//	//{
//	//	Syncable::AutoLock inner_syn(m_syn);
//	//	*fcls_info = NULL;
//	//	byte_8 *data;
//	//	int type;
//	//	int size;
//	//	if(m_file_data_fetcher->getFclsInfoConf(fcls_id, &type, &data, &size) < 0)
//	//		return -1;
//	//	if(FclsInfoAdapter::translate(type, data, size, fcls_info) < 0)
//	//		return -1;
//	//	delete []data;
//	//	return 0;
//	//}
//


}
