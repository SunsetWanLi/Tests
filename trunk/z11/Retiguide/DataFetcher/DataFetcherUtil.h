#ifndef _DATA_FETCHER_UTIL_H_
#define _DATA_FETCHER_UTIL_H_

#include "CommonBase/MGWString.h"
#include <string>
using namespace z11;
using namespace std;

namespace GIS_Embedded
{
	class DataFetcherUtil
	{
	public:
		static z11::MGWString get_FclsName_From_FclsId(const z11::MGWString &fcls_id);
		static z11::MGWString get_DatasrcName_From_FclsId(const z11::MGWString &fcls_id);
		//static string wstring_to_string(const z11::MGWString &wstr);
		//static z11::MGWString string_to_wstring(const string &str);
		static z11::MGWString getDatasrcName_From_Grl(const z11::MGWString &grl);
		static z11::MGWString getDatasetId_From_FclsId(const z11::MGWString &fcls_id);
	};
}

#endif