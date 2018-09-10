#include "PathDef.h"
#include "DataFetcherUtil.h"
#include "System/Shared/MGPath.h"
#include "System/Shared/Utility.h"
#include "CommonBase/MGWString.h"

#include <algorithm>
#include <sstream>

using namespace z11;
using namespace std;

namespace GIS_Embedded
{
	const string PathDef::MainFolder = "GIS_Embedded/";
	const string PathDef::ConfigFolder = "conf/";
	const string PathDef::DataFolder = "data/";
	const string PathDef::BitmapFolder = "image/";
	const string PathDef::TopoDataFolder = "Topo/";
	const string PathDef::FileIndexFileName = "fileindex.fi";	//文本文件
	const string PathDef::TerrsConfZipFileName = "TerrsConf.tc";
	const string PathDef::FclsInfoFileName = "FclsInfo.info";
	const string PathDef::TopoDataFileName = "TopoData.topo";
	const string PathDef::QueryFolder = "Query/";
	const string PathDef::NameQueryFileName = "namequery.nq";
	const string PathDef::FileDatabaseName = "geodata.fdb";
	const string PathDef::NWFileDatabaseName = "Network.fdb";
	const string PathDef::NWRelatedFclsLocatorFileName = "Related.fclocs";
	const string PathDef::NetConfigName = "netconfig.cfg";
	const string PathDef::NameQueryExConfigFileName = "NqConfig";
	const string PathDef::NameQueryExIndexFileName = "namequery.index";
	const string PathDef::NameQueryExFileName = "namequery.qdb";
	const string PathDef::NWGridDataBaseName = "NaviNWGridData.odb";
	const string PathDef::NWGridConfFileName = "NaviNWGridData.cfg";
	const string PathDef::IDRangeConfFileName = "NaviNWInit.cfg";
	const string PathDef::SettingsFileName = "Settings.cfg";
	const string PathDef::ProgramLanguageName = "Language.lan";
	const string PathDef::DefaultSettingsFileName = "DefaultSettings.cfg";
	const string PathDef::MapConfigFileName = "MapConfig.cfg";
	const string PathDef::FclsGlobalIndexName = "FclsGlobalIndex.cfg";
	const string PathDef::TMCConfigFileName = "TMC_Config.cfg";
	const string PathDef::PathPointsFileName = "PathPoints.cfg";//lw
	const string PathDef::OutInPutMyPiFileName = "MyPi.cfg";//lw
	const string PathDef::FixedRadarFileName = "FixedRadar.cfg";
	const string PathDef::UnFixedRadarFileName = "UnFixedRadar.cfg";
	//GRF
	const string PathDef::GGPointDataFolder = "GuiderGeoPoint/";
	const string PathDef::GGGuiderFilesFolder = "GuiderFiles/" ;	
	const string PathDef::GGSoundFileFolder = "SoundFiles/";
	const string PathDef::GGConfigFolder = "Config/";
	const string PathDef::GGScenicAreaConfFileName ="ScenicArea.conf";
	const string PathDef::GGPointConfFileName = "GGPoint.conf"; //是否触发
	const string PathDef::GGIndoorAreaPointGroupConfFileName = "IndoorAreaPointGroup.conf";
	const string PathDef::GGPairPointGroupConfFileName = "pairPointGroup.conf";
	const string PathDef::SightButtonConf1Name = "SightButton1.conf";
	const string PathDef::SightButtonConf0Name = "SightButton0.conf";
	const string PathDef::counterlist = "CounterList.conf";

	string PathDef::WorkspaceZipFileName(const z11::MGWString &datasrc_id)
	{
		string ret_name = Utility::wstr2ustr(datasrc_id);		
		return ret_name += ".ws";
	}

	string PathDef::MetaDataFileName(int index)
	{
		string ret("GIS_");
		ostringstream iss;
		iss<<index;
		ret += iss.str();
		ret += ".PZK";
		return ret;
	}

	string PathDef::ImageDataFileName(const z11::MGWString &fclsId, int index)
	{
		z11::MGWString wfcls_name = DataFetcherUtil::get_FclsName_From_FclsId(fclsId);
		string ret = Utility::wstr2ustr(wfcls_name);
		ret += "_";
		ret += z11::Utility::BuiltInTypeToString<int>(index);
		ret += ".jpg";
		return ret;
	}

	string PathDef::BitmapFileName(const z11::MGWString &map_name)
	{
		string ret = Utility::wstr2ustr(map_name);
		ret += ".bmp";
		return ret;
	}

	int PathDef::GridId(int fcls_id, int index)
	{
		return (fcls_id << 16) | (index & 0xffff);
	}

	string PathDef::MainFolderPath(FileParam &file_param)
	{
		string main_folder = file_param.fileDirectory;
		main_folder += MainFolder;
		return main_folder;
	}

	string PathDef::ConfigFolderPath(FileParam &file_param)
	{
		string conf_folder = MainFolderPath(file_param);
		conf_folder += ConfigFolder;
		return conf_folder;
	}

	string PathDef::DataFolderPath(FileParam &file_param)
	{
		string data_folder = MainFolderPath(file_param);
		data_folder += DataFolder;
		return data_folder;
	}

	string PathDef::BitmapFolderPath(FileParam &file_param)
	{
		string ret = MainFolderPath(file_param);
		ret += BitmapFolder;
		return ret;
	}

	string PathDef::DatasrcFolderPath(FileParam &file_param, const z11::MGWString & fcls_id)
	{
		//返回存入文件数据库的路径：如“SUZHOU/”（无“/SD/GIS_Embedded/data/”）
		string ret;
		const z11::MGWString datasrc = DataFetcherUtil::get_DatasrcName_From_FclsId(fcls_id);
		ret += Utility::wstr2ustr(datasrc);
		ret += "/";
		return ret;
	}

	string PathDef::QueryFolderPath(FileParam &file_param)
	{
		string ret = DataFolderPath(file_param);
		ret += QueryFolder;
		return ret;
	}

	string PathDef::FclsFolderPath(FileParam &file_param, const z11::MGWString & fcls_id)
	{
		string ret = DatasrcFolderPath(file_param, fcls_id);
		const z11::MGWString fcls =  DataFetcherUtil::get_FclsName_From_FclsId(fcls_id);
		ret += Utility::wstr2ustr(fcls);
		ret += "/";
		return ret;
	}

	string PathDef::TopoDataFolderPath(FileParam &file_param)
	{
		string ret = DataFolderPath(file_param);
		ret += TopoDataFolder;
		return ret;
	}

	string PathDef::TerrsConfFilePath(FileParam &file_param)
	{
		string ret = ConfigFolderPath(file_param);
		ret += TerrsConfZipFileName;
		return ret;
	}

	string PathDef::FileIndexFilePath(FileParam &file_param)
	{
		string ret = ConfigFolderPath(file_param);
		ret += FileIndexFileName;
		return ret;
	}

	string PathDef::WorkspaceFilePath(FileParam &file_param, const z11::MGWString & workspace_name)
	{
		string ret = ConfigFolderPath(file_param);
		ret += WorkspaceZipFileName(workspace_name);
		return ret;
	}

	string PathDef::PublishFilePath(FileParam &file_param, const z11::MGWString & fcls_id)
	{
		string ret = ConfigFolderPath(file_param);
		ret += Utility::wstr2ustr(DataFetcherUtil::get_DatasrcName_From_FclsId(fcls_id));
		ret += "_pl.xml";
		return ret;
	}

	string PathDef::TopoDataFilePath(FileParam &file_param)
	{
		string ret = TopoDataFolderPath(file_param);
		ret += TopoDataFileName;
		return ret;
	}

	string PathDef::FclsInfoFilePath(FileParam &file_param, const z11::MGWString & fcls_id)
	{
		string ret = FclsFolderPath(file_param, fcls_id);
		ret += FclsInfoFileName;

		return ret;
	}

	string PathDef::MetaDataFilePath(FileParam &file_param, const z11::MGWString & fcls_id, int index)
	{
		string ret = FclsFolderPath(file_param, fcls_id);
		ret +=  MetaDataFileName(index);

		return ret;
	}

	string PathDef::ImageDataFilePath(FileParam &file_param, const z11::MGWString & fcls_id, int index)
	{
		string ret = DataFolderPath(file_param);
		ret += FclsFolderPath(file_param, fcls_id);
		ret += ImageDataFileName(fcls_id, index);

		return ret;
	}

	string PathDef::BitmapFilePath(FileParam &file_param, const z11::MGWString & map_name)
	{
		string ret = BitmapFolderPath(file_param);
		ret += BitmapFileName(map_name);
		return ret;
	}

	string PathDef::NameQueryFilePath(FileParam &file_param)
	{
		string ret = QueryFolderPath(file_param);
		ret += NameQueryFileName;
		return ret;
	}

	string PathDef::FileDatabasePath(FileParam &file_param)
	{
		string ret = DataFolderPath(file_param);
		ret += FileDatabaseName;
		return ret;
	}

	string PathDef::NWFileDatabasePath(FileParam &file_param)
	{
		string ret = TopoDataFolderPath(file_param);
		ret += NWFileDatabaseName;
		return ret;
	}

	string PathDef::NWMappingFileDatabasePath(FileParam &file_param, const z11::MGWString &grl)
	{
		string full_grl = Utility::wstr2ustr(grl);
		string ret = TopoDataFolderPath(file_param);
		string fcls_loc = full_grl.substr(0, full_grl.find_last_of(':'));
		replace(fcls_loc.begin(), fcls_loc.end(), ':', '#');
		ret += fcls_loc;
		ret += ".fdb";
		return ret;
	}

	string PathDef::NetConfigPath(FileParam &file_param)
	{
		string ret = ConfigFolderPath(file_param);
		ret += NetConfigName;
		return ret;
	}

	string PathDef::NameQueryExConfigFilePath( FileParam &file_param )
	{
		string ret = QueryFolderPath(file_param);
		ret += NameQueryExConfigFileName;
		return ret;
	}

	string PathDef::NameQueryExIndexFilePath(FileParam &file_param, z11::MGWString &type)
	{
		string ret = QueryFolderPath(file_param);
		string str_type = Utility::wstr2ustr(type);
		ret += str_type;
		ret += ".index";
		//ret += NameQueryExIndexFileName;
		return ret;
	}

	string PathDef::NameQueryExIndexFilePath(FileParam &file_param, z11::MGWString &type, z11::MGWString &language)
	{
		string ret = QueryFolderPath(file_param);
		string str_type = Utility::wstr2ustr(type);
		ret += str_type;
		ret += "_";
		string str_lan = Utility::wstr2ustr(language);
		ret += str_lan;
		ret += ".index";
		//ret += NameQueryExIndexFileName;
		return ret;
	}

	string PathDef::NameQueryExFilePath(FileParam &file_param, z11::MGWString &type)
	{
		string ret = QueryFolderPath(file_param);
		string str_type = Utility::wstr2ustr(type);
		ret += str_type;
		ret += ".qdb";
		//ret += NameQueryExFileName;
		return ret;
	}

	string PathDef::NameQueryExFilePath(FileParam &file_param, z11::MGWString &type, z11::MGWString &language)
	{
		string ret = QueryFolderPath(file_param);
		string str_type = Utility::wstr2ustr(type);
		ret += str_type;
		ret += "_";
		string str_lan = Utility::wstr2ustr(language);
		ret += str_lan;
		ret += ".qdb";
		//ret += NameQueryExFileName;
		return ret;
	}

	string PathDef::NameQueryPartMatchIndexFilePath(FileParam &file_param, z11::MGWString &type, z11::MGWString &language)
	{
		string ret = QueryFolderPath(file_param);
		string str_type = Utility::wstr2ustr(type);
		ret += str_type;
		ret += "_";
		string str_lan = Utility::wstr2ustr(language);
		ret += str_lan;
		ret += ".idx";
		//ret += NameQueryExIndexFileName;
		return ret;
	}

	string PathDef::NameQueryPartMatchDataFilePath(FileParam &file_param, z11::MGWString &language)
	{
		string ret = QueryFolderPath(file_param);
		ret += "All_";
		string str_lan = Utility::wstr2ustr(language);
		ret += str_lan;
		ret += ".dat";
		//ret += NameQueryExFileName;
		return ret;
	}

	string PathDef::NWDataFilePath(FileParam &file_param, const NO_ID grl)
	{
		//string ret = Utility::wstr2ustr(grl);
		//char s[100];
		string ret = z11::Utility::BuiltInTypeToString<NO_ID>(grl);
		return ret;
	}

	string PathDef::NWRelatedFclsLocatorFilePath(FileParam &file_param)
	{
		string ret = TopoDataFolderPath(file_param);
		ret += NWRelatedFclsLocatorFileName;
		return ret;
	}

	string PathDef::NWMappingFilePath(FileParam &file_param, const z11::MGWString &grl)
	{
		string ret = Utility::wstr2ustr(grl);
		ret = ret.substr(ret.find_last_of(':') + 1);
		return ret;
	}

	string PathDef::NWGridDataBasePath(FileParam &file_param)
	{
		string ret = TopoDataFolderPath(file_param);
		ret += NWGridDataBaseName;
		return ret;
	}

	string PathDef::NWGridConfFilePath(FileParam &file_param)
	{
		string ret = TopoDataFolderPath(file_param);
		ret += NWGridConfFileName;
		return ret;
	}

	string PathDef::IDRangeConfFilePath(FileParam &file_param)
	{
		string ret = TopoDataFolderPath(file_param);
		ret += IDRangeConfFileName;
		return ret;
	}

	string PathDef::SettingsFilePath(FileParam &file_param)
	{
		string ret = ConfigFolderPath(file_param);
		ret += SettingsFileName;
		return ret;
	}

	string PathDef::FixedRadarFilePath(FileParam &file_param)
	{
		string ret = DataFolderPath(file_param);
		ret += FixedRadarFileName;
		return ret;
	}
	string PathDef::UnFixedRadarFilePath(FileParam &file_param)
	{
		string ret = DataFolderPath(file_param);
		ret += UnFixedRadarFileName;
		return ret;
	}

	string PathDef::ProgramLanguagePath(FileParam &file_param)
	{
		string ret = ConfigFolderPath(file_param);
		ret += ProgramLanguageName;
		return ret;
	}

	string PathDef::MapConfigFilePath(FileParam &file_param)
	{
		string ret = ConfigFolderPath(file_param);
		ret += MapConfigFileName;
		return ret;
	}

	string PathDef::DefaultSettingsFilePath(FileParam &file_param)
	{
		string ret = ConfigFolderPath(file_param);
		ret += DefaultSettingsFileName;
		return ret;
	}

	string PathDef::FclsGlobalIndexFilePath( FileParam &file_param )
	{
		string ret = ConfigFolderPath(file_param);
		ret += FclsGlobalIndexName;
		return ret;
	}

	string PathDef::TMCConfigFilePath( FileParam &file_param )
	{
		string ret = ConfigFolderPath(file_param);
		ret += TMCConfigFileName;
		return ret;
	}

	string PathDef::PathPointsFilePath(FileParam &file_param) //lw
	{
		string ret = ConfigFolderPath(file_param);
		ret += PathPointsFileName;
		return ret;
	}

	string PathDef::OutInPutMyPi(FileParam &file_param)//
	{
		string ret = ConfigFolderPath(file_param);
		ret += OutInPutMyPiFileName;
		return ret;
	}
	string PathDef::GGPointConfFilePath(FileParam &file_param)
	{
		string ret = MainFolderPath(file_param)+GGPointDataFolder;;
		ret += GGPointConfFileName;
		return ret;
	}
	string PathDef::GGPointFilePath(FileParam &file_param, z11::MGWString &key)
	{

		string ret = MainFolderPath(file_param)+GGPointDataFolder;
		ret +=  "GeoPoint/" + Utility::wstr2astr(key);
		ret += "_point";
		return ret;
	}
	string PathDef::GGPointTriggerAreaPath(FileParam &file_param,z11::MGWString &key)
	{
		string ret = MainFolderPath(file_param) +GGPointDataFolder;
		ret += "TriggerArea/";
		ret += Utility::wstr2ustr(key);
		ret += "_trigger";
		return ret; //to do
	}
	string PathDef::GGPointFileInfosPath(FileParam &file_param,z11::MGWString &key, int language_index)
	{
		string ret = MainFolderPath(file_param) +GGPointDataFolder;
		ret += "IntroduceFileInfos/";
		ret += Utility::wstr2ustr(key);
		ret += "_description_";
		//char buf[2];
		//_itoa_s(language_index,buf,2,10);
		//string str(buf);	
		ret +=  z11::Utility::BuiltInTypeToString<int>(language_index);
		return ret; //to do
	}
	string PathDef::GGPointSoundFilePath(FileParam &file_param)
	{
		string ret  = MainFolderPath(file_param) +GGPointDataFolder;
		ret += GGSoundFileFolder;
		return ret;
	}
	string PathDef::ScenicAreaConfigPath(FileParam &file_param)
	{
		string ret  = MainFolderPath(file_param) +GGPointDataFolder;
		ret += GGConfigFolder;
		ret +=GGScenicAreaConfFileName;
		return ret;
	}
	string PathDef::IndoorAreaPointGroupConfigPath(FileParam &file_param)
	{
		string ret  = MainFolderPath(file_param) +GGPointDataFolder;
		ret += GGConfigFolder;
		ret +=GGIndoorAreaPointGroupConfFileName;
		return ret;
	}

	string PathDef::SightButtonConf1Path(FileParam &file_param)
	{
		string ret  = MainFolderPath(file_param) +GGPointDataFolder;
		ret += GGConfigFolder;
		ret +=SightButtonConf1Name;
		return ret;
	}
	string PathDef::CounterListPath(FileParam &file_param)
	{
		string ret  = MainFolderPath(file_param) +GGPointDataFolder;
		ret += GGConfigFolder;
		ret +=counterlist;
		return ret;
	}

	string PathDef::SightButtonConf0Path(FileParam &file_param)
	{
		string ret  = MainFolderPath(file_param) +GGPointDataFolder;
		ret += GGConfigFolder;
		ret +=SightButtonConf0Name;
		return ret;
	}
	string PathDef::PairPointGroupConfigPath(FileParam &file_param)
	{
		string ret  = MainFolderPath(file_param) +GGPointDataFolder;
		ret += GGConfigFolder;
		ret +=GGPairPointGroupConfFileName;
		return ret;
	}

	string PathDef::GGPointConfPath(FileParam &file_param)
	{
		string ret  = MainFolderPath(file_param) +GGPointDataFolder;
		ret += GGConfigFolder;
		ret += GGPointConfFileName;
		return ret;
	}
	string PathDef::GuiderSoundFilesPath(FileParam &file_param,string filename)
	{
		string ret = MainFolderPath(file_param) + GGGuiderFilesFolder;
		ret += filename;
		return ret;
	}

	//string PathDef::MetaDataFilePath(string &fcls_id, int index)
	//{
	//	string ret = PathDef::DataFolder;

	//}

}