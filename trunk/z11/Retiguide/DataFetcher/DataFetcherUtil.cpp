#include "DataFetcherUtil.h"
#include "System/Shared/Utility.h"
#include <string>
#include "Retiguide/Common/String_Tools.h"

using namespace std;

namespace GIS_Embedded
{

	z11::MGWString DataFetcherUtil::get_FclsName_From_FclsId(const z11::MGWString &fcls_id)	//无"GOS(1.0):"等字段
	{
		return fcls_id.substr(fcls_id.find_last_of(':')+1);
	}

	z11::MGWString DataFetcherUtil::get_DatasrcName_From_FclsId(const z11::MGWString &fcls_id)	//无"GOS(1.0):"等字段
	{
		return fcls_id.substr(0, fcls_id.find_first_of(':'));
	}

	//string DataFetcherUtil::wstring_to_string(const z11::MGWString &wstr)	//无"GOS(1.0):"等字段
	//{
	//	return Utility::wstr2ustr(wstr);
	//}

	//z11::MGWString DataFetcherUtil::string_to_wstring(const string &str)
	//{
	//	return Utility::ustr2wstr(str);
	//}

	z11::MGWString DataFetcherUtil::getDatasrcName_From_Grl(const z11::MGWString &grl)	//无"GOS(1.0):"等字段
	{
		return grl.substr(0, grl.find_first_of(':'));
	}

	z11::MGWString DataFetcherUtil::getDatasetId_From_FclsId(const z11::MGWString &fcls_id)
	{
		return fcls_id.substr(0, fcls_id.find_last_of(':'));
	}
}