#include "TypeDefs.h"
#include "DataFetcherUtil.h"
#include "System/Shared/Utility.h"
#include "Retiguide/Common/String_Tools.h"
#include "CommonBase/MGWString.h"
#include "CommonBase/MGData.h"
#include "System/Shared/MGLog.h"
#include <stdio.h>
#include <iostream>
using namespace std;
using namespace GIS_Embedded;
using namespace z11 ;

namespace GIS_Embedded
{
	int FclsGlobalIndex::calculateIndex( const z11::MGWString &fcls_id )
	{
		int tmp = fcls_id.find_first_of(L':');
		tmp = fcls_id.size();
		z11::MGWString wdatasrc = fcls_id.substr(0, fcls_id.find_first_of(L':'));
		tmp = wdatasrc.size();
		z11::MGWString wfcls = fcls_id.substr(fcls_id.find_last_of(L':') + 1);
		tmp = wfcls.size();

        //wcout<<"find layer "<<wfcls<<endl;
		return calculateIndex(wdatasrc, wfcls);
	}

	int FclsGlobalIndex::calculateIndex( const z11::MGWString &datasrc, const z11::MGWString &fcls)
	{
		string temp_datasrc = Utility::wstr2ustr(datasrc);
		string temp_fcls = Utility::wstr2ustr(fcls);

		unsigned datasrc_key, fcls_key;
		int i, j, ret = 0;
		bool ok = false;
		for(i = 0; i < datasrc_nodes.size(); i ++)
		{
			for(j = 0; j < datasrc_nodes[i].second.size(); j ++)
			{	
				if(datasrc_nodes[i].first == temp_datasrc && datasrc_nodes[i].second[j] == temp_fcls)
				{
					ok = true;
					break;
				}
			}
			if(ok)
				break;
		}
		if(!ok)
		{
			assert(0);
			return -1;
		}
		datasrc_key = i;
		fcls_key = j;
		if(datasrc_key >= 256 || fcls_key >= 256)
		{
			assert(0);
			return -1;
		}

		ret |= (datasrc_key << 8);
		ret |= fcls_key;

		return ret;

	}
	int FclsGlobalIndex::inputConf( const string &file_name )
	{
		MGData *data=MGData::dataWithContentsOfFile(file_name);
		assert(data!=NULL);
		istringstream ifstr((char *)data->bytes());
		delete_and_set_null(data);
		int datasrc_size = -1, fcls_size = -1;
		string datasrc, fcls;
		ifstr>>datasrc_size;
		assert(ifstr.good());
		datasrc_nodes.resize(datasrc_size);
		unsigned datasrc_id, fcls_id;
		for(int i = 0; i <datasrc_size; i ++)
		{
			ifstr>>datasrc_id>>datasrc;
			datasrc_nodes[datasrc_id].first = datasrc;//wdatasrc;
			ifstr>>fcls_size;
			datasrc_nodes[datasrc_id].second.resize(fcls_size);

			for(int j = 0; j < fcls_size; j ++)
			{
				ifstr>>fcls_id>>fcls;
				datasrc_nodes[datasrc_id].second[fcls_id] = fcls;//wfcls;
			}
		}
		return 0;
	}

// 	z11::MGWString FclsGlobalIndex::calculateFclsID( int fcls_index )
// 	{
//  		int datasrc_index = fcls_index>>8;
//  		int sub_fcls_index = fcls_index & 0xff;
//  		if(datasrc_index >= datasrc_nodes.size())
//  		{
//  			assert(0);
//  			return WS(WS(L""));
//  		}
//  		z11::MGWString datasrc_name = datasrc_nodes[datasrc_index].first;
//  		if(sub_fcls_index >= datasrc_nodes[datasrc_index].second.size())
//  		{
//  			assert(0);
//  			return WS(WS(L""));
//  		}
//  		z11::MGWString fcls_name = datasrc_nodes[datasrc_index].second[sub_fcls_index];
//  		z11::MGWString ret = datasrc_name + z11::MGWString(WS(L":")) + datasrc_name + z11::MGWString(WS(L":")) + fcls_name;
//  		return ret;
// 	}
}