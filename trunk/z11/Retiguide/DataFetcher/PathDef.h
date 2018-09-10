#ifndef _PATH_DEF_H_
#define _PATH_DEF_H_

#include <string>
#include "../Common/Types/DataFetcherTypes/TypeDefs.h"
#include "TypeDefs.h"
#include "CommonBase/MGWString.h"

using namespace z11;
using namespace std;

namespace GIS_Embedded
{

	class PathDef
	{
	private:
		static const string MainFolder;
		static const string ConfigFolder;
		static const string DataFolder;
		static const string BitmapFolder;
		static const string QueryFolder;
		static const string FileIndexFileName;	//文本文件
		static const string TerrsConfZipFileName;
		static const string NameQueryFileName;
		static string WorkspaceZipFileName(const z11::MGWString &datasrc_id);
		static const string FclsInfoFileName;
		static const string TopoDataFolder;
		static string ImageDataFileName(const z11::MGWString &fclsId, int index);
		static string MetaDataFileName(int index);
		static const string TopoDataFileName;
		static string BitmapFileName(const z11::MGWString &map_name);
		static const string FileDatabaseName;
		static const string NWFileDatabaseName;
		static const string NWGridDataBaseName;
		static const string NWGridConfFileName;
		static const string IDRangeConfFileName;
		static const string SettingsFileName;
		static const string ProgramLanguageName;
		static const string DefaultSettingsFileName;
		static const string MapConfigFileName;
		static const string NWRelatedFclsLocatorFileName;
		static const string NWMappingFileDatabaseName;
		static const string NetConfigName;
		static const string NameQueryExConfigFileName;
		static const string NameQueryExIndexFileName;
		static const string NameQueryExFileName;
		static const string FclsGlobalIndexName;
		static const string TMCConfigFileName;
		static const string PathPointsFileName;//lw
		static const string OutInPutMyPiFileName;//dao ru daochu
		static const string FixedRadarFileName;
		static const string UnFixedRadarFileName;
		//GRF
		static const string GGPointDataFolder;
		static const string GGGuiderFilesFolder;		
		static const string GGSoundFileFolder;
		static const string GGConfigFolder;
		static const string GGPointConfFileName;
		static const string GGScenicAreaConfFileName;
		static const string GGIndoorAreaPointGroupConfFileName;
		static const string GGPairPointGroupConfFileName;
		static const string SightButtonConf1Name;
		static const string SightButtonConf0Name;
		static const string counterlist;

	public:
		static string MainFolderPath(FileParam &);
		static string ConfigFolderPath(FileParam &);
		static string DataFolderPath(FileParam &);
		static string TopoDataFolderPath(FileParam &);
		static string QueryFolderPath(FileParam &);
		static string DatasrcFolderPath(FileParam &, const z11::MGWString & fcls_id);
		static string FclsFolderPath(FileParam &, const z11::MGWString & fcls_id);
		static string BitmapFolderPath(FileParam &);

		static string NameQueryFilePath(FileParam &);
		static string TerrsConfFilePath(FileParam &);
		static string FileIndexFilePath(FileParam &);
		static string WorkspaceFilePath(FileParam &, const z11::MGWString & workspace_name);
		static string PublishFilePath(FileParam &file_param, const z11::MGWString & fcls_id);
		static string FclsInfoFilePath(FileParam &, const z11::MGWString & fcls_id);
		static string TopoDataFilePath(FileParam &);
		static string ImageDataFilePath(FileParam &file_param, const z11::MGWString & fcls_id, int index);
		static string MetaDataFilePath(FileParam &, const z11::MGWString & fcls_id, int index);
		static string BitmapFilePath(FileParam &, const z11::MGWString & map_name);
		static string FileDatabasePath(FileParam &file_param);
		static string NWFileDatabasePath(FileParam &file_param);
		static string NWMappingFileDatabasePath(FileParam &file_param, const z11::MGWString &grl);
		static string NetConfigPath(FileParam &file_param);
		static string NameQueryExConfigFilePath(FileParam &file_param);
		static string NameQueryExIndexFilePath(FileParam &file_param, z11::MGWString &type);
		static string NameQueryExIndexFilePath(FileParam &file_param, z11::MGWString &type, z11::MGWString &language);
		static string NameQueryExFilePath(FileParam &file_param, z11::MGWString &type);
		static string NameQueryExFilePath(FileParam &file_param, z11::MGWString &type, z11::MGWString &language);
		static string NameQueryPartMatchIndexFilePath(FileParam &file_param, z11::MGWString &type, z11::MGWString &language);
		static string NameQueryPartMatchDataFilePath(FileParam &file_param, z11::MGWString &language);
		static string NWDataFilePath(FileParam &file_param, const NO_ID grl);
		static string NWRelatedFclsLocatorFilePath(FileParam &file_param);
		static string NWMappingFilePath(FileParam &file_param, const z11::MGWString &grl);
		static string NWGridDataBasePath(FileParam &file_param);
		static string NWGridConfFilePath(FileParam &file_param);
		static string IDRangeConfFilePath(FileParam &file_param);
		static string SettingsFilePath(FileParam &file_param);
		static string ProgramLanguagePath(FileParam &file_param);
		static string MapConfigFilePath(FileParam &file_param);
		static string DefaultSettingsFilePath(FileParam &file_param);
		static string FclsGlobalIndexFilePath(FileParam &file_param);
		static string PathPointsFilePath(FileParam &file_param);//lw
		static string OutInPutMyPi(FileParam &file_param);//dao chu
		static int GridId(int fcls_id, int index);
		static string FixedRadarFilePath(FileParam &file_param);
		static string UnFixedRadarFilePath(FileParam &file_param);

		static string TMCConfigFilePath(FileParam &file_param);;

		//GRF
		static string GGPointFilePath(FileParam &file_param, z11::MGWString &key);
		static string GGPointTriggerAreaPath(FileParam &file_param, z11::MGWString &key);
		static string GGPointFileInfosPath(FileParam &file_param,z11::MGWString &key, int language_index);
		static string GuiderSoundFilesPath(FileParam &file_param,string filename);
		static string GGPointConfFilePath(FileParam &file_param);
		static string GGPointSoundFilePath(FileParam &file_param);
		static string ScenicAreaConfigPath(FileParam &file_param);
		static string GGPointConfPath(FileParam &file_param);
		static string IndoorAreaPointGroupConfigPath(FileParam &file_param);
		static string PairPointGroupConfigPath(FileParam &file_param);
		static string SightButtonConf1Path(FileParam &file_param);
		static string SightButtonConf0Path(FileParam &file_param);
		static string CounterListPath(FileParam &file_param);
	};

}

#endif