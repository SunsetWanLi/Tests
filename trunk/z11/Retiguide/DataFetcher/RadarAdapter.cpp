#include "RadarAdapter.h"
#include "DataReader.h"
#include "DataWriter.h"
#include "../Common/CRS/GlobalCrt.h"

using namespace std;

namespace GIS_Embedded
{
	int RadarAdapter::translate(byte_8 *data, int size,  Radar * radar)
	{
		int radar_path_size;
		DataReader read;
		read.setStartPointer(data);
		radar_path_size = read.getInt();
		pair<unsigned int,Pos2DEx> tmp_radar_pos;

		GlobalCrt crt;
		unsigned int line_id;
		Pos2D pos;
		Pos2DEx tmp_pos;

		for (int i = 0; i< radar_path_size; i++)
		{
			line_id = read.getInt64();
			int radar_point = read.getInt();
			for (int j=0; j<radar_point; j++)
			{
 				pos.d0 = read.getDouble();
 				pos.d1 = read.getDouble();

				int_32 d0 = tmp_pos.d0;
				int_32 d1 = tmp_pos.d1;

 				crt.geoToPrj(pos.d0,pos.d1,&d0,&d1);

 				tmp_radar_pos = make_pair(line_id,tmp_pos);
 				radar->addRadarPoint(tmp_radar_pos);
			}
		}
		return 0;
	}
// 	int RadarAdapter::translate(byte_8 *data, int size,  map<int,Pos2D> * radar)
// 	{
// 		int radar_size;
// 		DataReader read;
// 		read.setStartPointer(data);
// 		radar_size = read.getInt();
//
// 		int line_id;
// 		Pos2D pos;
// 		for (int i = 0; i< radar_size; i++)
// 		{
// 			line_id = read.getInt();
// 			pos.d0 = read.getDouble();
// 			pos.d1 = read.getDouble();
// 			radar->insert(map<int,Pos2D>::value_type(line_id,pos));
// 		}
//
// 		return 0;
// 	}

}
