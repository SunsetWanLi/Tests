#ifndef _RADAR_H
#define _RADAR_H

#include "basic_types.h"
#include <vector>
#include <map>

using namespace std;

namespace GIS_Embedded{

	class Radar
	{

	public:
		int size;
		pair<unsigned int,Pos2DEx> radar_pos;
		std::map<unsigned int,vector<Pos2DEx> > road_id_points;//need new vector?

	public:
		void addRadarPoint(pair<unsigned int,Pos2DEx> point)
		{
			std::map<unsigned int,vector<Pos2DEx> >::iterator it = road_id_points.find(point.first);
			if (it != road_id_points.end())
			{
				(*it).second.push_back(point.second);
			}
			else
				road_id_points[point.first].push_back(point.second);
		}
		void getRadarPoint(unsigned int road_id,vector<Pos2DEx>& radar_pionts)
		{
			std::map<unsigned int,vector<Pos2DEx> >::iterator it = road_id_points.find(road_id);
			if (it != road_id_points.end())
				radar_pionts.insert(radar_pionts.end(),(*it).second.begin(),(*it).second.end());
		}
	};
}

#endif