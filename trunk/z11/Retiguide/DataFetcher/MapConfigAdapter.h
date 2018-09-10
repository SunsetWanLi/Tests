#ifndef _MAP_CONFIG_ADAPTER_H_
#define _MAP_CONFIG_ADAPTER_H_

#include "../Common/Types/MapConfigType.h"
#include "CommonBase/MGWString.h"
#include <fstream>
#include <string>

using namespace std;
using namespace z11;

namespace GIS_Embedded
{
	class MapConfigAdapter
	{
	public:
		static int translate(istream &ifstrm, MapConfigType ** map_config);
		
	private:
		static int getVectorInt(istream &ifstrm, vector<int> *ret);
		//static int getVectorWstr(ifstream &ifstrm, vector<z11::MGWString> *ret);
		static int getInt(istream &ifstrm, int *ret);
		static int getDouble(istream &ifstrm, double *ret);
		static int getWstring(istream &ifstrm,string *ret);
		static int getPos2D(istream &ifstrm, Pos2D *ret);

		static char tmp[1000];

	};
}

#endif