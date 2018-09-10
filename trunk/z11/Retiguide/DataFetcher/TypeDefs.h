#ifndef _TYPE_DEFS_H_
#define _TYPE_DEFS_H_

#include <assert.h>
#include <string>
#include <map>
#include <sstream>
#include <fstream>
#include <ctime>
#include <vector>
#include "../Common/Share/Scope.h"
#include "../Common/Types/MapConfigType.h"
#include "../Common/Types/Settings.h"
#include "../Common/Types/ProgramLanguage.h"
#include "CommonBase/MGWString.h"

//GRF
#include "../Common/Types/GeoTypes/GuiderGeoPointTrigger.h"
using namespace std;
using namespace z11;
#undef NEED_LOG
//#define NEED_LOG

#define DISK_MAX_SIZE 2000000000 //bytes 最大文件容量字节数

#define RAM_MAX_SIZE 1000000 //bytes 最大地理数据内存容量字节数
#define NW_DATA_MAX_SIZE 5000000 //bytes 最大网络数据内存容量字节数
#define NW_GRID_MAX_SIZE 1000000 //bytes 最大网络格子数据内存容量字节数
#define GM_LINE_MAX_SIZE 1000000 //bytes 最大几何线对象数据内存容量字节数

#define LOW_AVAIL_PHYS 11000000	//bytes 低于此空间大小则表示空间不足
using namespace std;

namespace GIS_Embedded
{
	template<class T> class p_Ref;

	class FclsGlobalIndex
	{
	private:
		typedef pair<std::string, vector<std::string> > DatasrcNode;
		vector<DatasrcNode> datasrc_nodes;

	public:
 		int calculateIndex(const z11::MGWString &datasrc, const z11::MGWString &fcls);
 		int calculateIndex(const z11::MGWString &fcls_id);
		vector<pair<std::string, vector<std::string> > > getAllFclsNames()
		{
			return datasrc_nodes;
		}


		int inputConf(const string &file_name);
	};


	struct ConnectionInfo
	{
		ConnectionInfo();
		int blockCount;
		int totalSize;
	};

	struct FeatureClassConf
	{
		//from publish file --> *_pl.xml
		z11::MGWString Fcls_Id;
		z11::MGWString Data_Provider;
		int Geo_Sum;
		bool Zip_Compress;
		bool Gm_Double_Compress;
		int Point_Compress;

		//from FclsInfo.info OR _pl.xml OR full map setting
		double left;
		double top;
		double right;
		double bottom;
		int rows;
		int cols;
		int image_size;
	};

	////GRF
	struct GGPointBlock{
		~GGPointBlock();
		vector<GuiderGeoPoint*> *ggp_objs;
		int ref_count;
		int size;
		int chance_left;
	};

	struct GGPointConf
	{
		map<z11::MGWString,GGPTypeT> conf;
	};
	typedef map<z11::MGWString, vector<z11::MGWString> > ScenicAreaMap;

	struct SightButtonConf
	{
		z11::MGWString name;
		int		buttonID;
		int		x;
		int		y;
		int		nWidth;
		int		nHeight;
	};
}

#endif