#ifndef _RADAR_ADAPTER_H_
#define _RADAR_ADAPTER_H_

#include <map>
#include "../Common/Types/basic_types.h"
#include "../Common/Types/Radar.h"
using namespace std;

namespace GIS_Embedded
{
	class RadarAdapter
	{
	public:
		static int translate(byte_8 *data, int size,Radar * radar);
		//static int translate(byte_8 *data, int size, map<int,Pos2D> * radar);
		//static int pack(map<int,Pos2D> &radar, byte_8 **data, int *size);
// 	private:
// 		static uint_32 packSize(map<int,Pos2D> &radar);

	};
}

#endif