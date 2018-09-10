#ifndef _TERRCTRL_ADAPTER_H_
#define _TERRCTRL_ADAPTER_H_

#include <fstream>
#include <vector>
#include "../Common/Types/DataFetcherTypes/TypeDefs.h"
#include "TypeDefs.h"
#include "../Common/Types/WorkSpaceTypes/TerrCtrl.h"
#include "../Common/Share/Scope.h"
#include "DataReader.h"

using namespace std;

namespace GIS_Embedded
{

	class TerrCtrlAdapter
	{
	private:
		static DataReader *read;
		static void getTerrLevel(DataReader &read,TerrLevel **terr_level);
		static void getTerr(DataReader &read,Terr **terr);

	public:
		static int translate(byte_8 *data, int size, TerrCtrl **terrCtrl);
	};

}

#endif