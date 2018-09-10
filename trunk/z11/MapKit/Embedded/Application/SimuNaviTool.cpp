#include "SimuNaviTool.h"
#include "../BasicType/Line.h"
#include "PathResult.h"
#include "../../../Retiguide/Common/CRS/GlobalCrt.h"
#include "../../GPSWrapper/GPS_Info.h"
#include "../../../Retiguide/SpatialMath/SpatialMath.h"
#include "../../../System/Shared/MGTime.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288 
#endif
using namespace IS_GIS;
using namespace GIS_Embedded;
using namespace z11;

namespace Embedded
{
	int SimuNaviTool::transPath2GPSData(const PathResult &route, const Embedded::SimParam &sim_param, vector<GPS_Info> *v_gps)
	{
		v_gps->clear();
		//by lw
		z11::MGTime cur_time = z11::MGTime::Now();
		z11::MGTime::Exploded tmp_exploded;
		cur_time.explode(tmp_exploded, false);

		GPS_Info gps;
		gps.utc_date.yy = tmp_exploded.year;
		gps.utc_date.mm = tmp_exploded.month;
		gps.utc_date.dd = tmp_exploded.day_of_month;
		gps.utc_time.hh = tmp_exploded.hour;
		gps.utc_time.mm = tmp_exploded.minute;
		gps.utc_time.ss = tmp_exploded.second;


// 		SYSTEMTIME sys_time;
// 		GetSystemTime(&sys_time);
//
// 		GPS_Info gps;
// 		gps.utc_date.yy = (int)sys_time.wYear;
// 		gps.utc_date.mm = (int)sys_time.wMonth;
// 		gps.utc_date.dd = (int)sys_time.wDay;
//
// 		gps.utc_time.hh = (int)sys_time.wHour;
// 		gps.utc_time.mm = (int)sys_time.wMinute;
// 		gps.utc_time.ss = (int)sys_time.wSecond;

		vector<ScreenPos2D> ctrl_points;
		vector<ScreenPos2D> new_ctrl_points;
		vector<double> dirs;

		GlobalCrt crt;

		//const vector<Line> &lines = route.getLines();

		for(uint_32 i = 0; i < route.m_segments.size(); ++i)
		{
			//const Line* cur_l = &lines[i];
			ctrl_points.clear();
			new_ctrl_points.clear();
			dirs.clear();
			//cur_l->getPoints(&ctrl_points);
			ctrl_points = route.m_segments[i].m_ctrl_points;
			double leng = route.m_segments[i].getLength();
			if(leng/sim_param.max_stop_per_line > sim_param.m_speed)
				getInterCtrlPoints(ctrl_points,leng/sim_param.max_stop_per_line, &new_ctrl_points, &dirs );
			else
				getInterCtrlPoints(ctrl_points,sim_param.m_speed, &new_ctrl_points, &dirs );

			for(uint_32 j = 0; j < new_ctrl_points.size(); ++j )
			{
				ScreenPos2D &cur_pos = new_ctrl_points[j];

				double geo_x, geo_y;
				crt.prjToGeo(cur_pos.d0, cur_pos.d1, &geo_x, &geo_y);

				gps.longitude = geo_x;
				gps.latitude = geo_y;

				gps.heading = dirs[j] *180 / M_PI;
				gps.speed = sim_param.m_speed;
				gps.utc_time.ss += 10;
				gps.utc_time.mm += gps.utc_time.ss/60;
				gps.utc_time.ss %= 60;

				v_gps->push_back(gps);
			}
		}

		int MAX_COUNT = 60;
		if(v_gps->size() > MAX_COUNT)	//减少过多路径点的情况 使模拟导航速度加快
		{
			vector<GPS_Info> tmp = *v_gps;
			v_gps->clear();
			double inter = ((double)tmp.size()) / (MAX_COUNT - 1);
			for(int i = 0; i < MAX_COUNT - 1; i ++)
			{
				v_gps->push_back(tmp[uint_32(inter * i)]);
			}
			v_gps->push_back(tmp[tmp.size() - 1]);
		}

		return 0;
	}

	int SimuNaviTool::getInterCtrlPoints(const vector<ScreenPos2D> & ctrl_points, double inter_dis, vector<ScreenPos2D> * inter_ctrl_points, vector<double> *dirs)
	{
		if(ctrl_points.empty())
			return -1;

		if(ctrl_points.size() == 1)
		{
			//inter_ctrl_points->push_back(*ctrl_points[0]);
			return 0;
		}

		//inter_ctrl_points->push_back(*ctrl_points[0]);
		int part_dis = 0;
		for(uint_32 i = 1, size = ctrl_points.size(); i < size; ++i)
		{
			ScreenPos2D pre_pos = ctrl_points[i-1];
			ScreenPos2D cur_pos = ctrl_points[i];
			if(i == size - 1)
			{
				//不正好在路口
				//inter_ctrl_points->push_back(cur_pos);
				ScreenPos2D last_pos;
				last_pos.d0 = (cur_pos.d0 + pre_pos.d0) / 2;
				last_pos.d1 = (cur_pos.d1 + pre_pos.d1) / 2;
				double dir = GIS_Embedded::SpatialMath::calLineNAngle(pre_pos, cur_pos);
				//lw
				//double dir;
				//if ()
				//{
				//}

				dirs->push_back(dir);
				inter_ctrl_points->push_back(last_pos);
				dirs->push_back(dir);
				inter_ctrl_points->push_back(cur_pos);
				break;
			}
			double dis = GIS_Embedded::SpatialMath::getDistance(pre_pos, cur_pos);
			part_dis += dis;

			if(dis < inter_dis)
			{
				if(part_dis > inter_dis)
				{
					part_dis = 0;
					dirs->push_back(GIS_Embedded::SpatialMath::calLineNAngle(pre_pos, cur_pos));
					inter_ctrl_points->push_back(cur_pos);
				}
				continue;
			}

			//zj below is: dis > inter_dis

			vector<ScreenPos2D> inter_pos;
			getInterpolations(pre_pos, cur_pos,inter_dis, &inter_pos);

			for(uint_32 inter_i = 0, inter_size = inter_pos.size(); inter_i < inter_size; ++ inter_i)
			{
				dirs->push_back(GIS_Embedded::SpatialMath::calLineNAngle(pre_pos, cur_pos));
				inter_ctrl_points->push_back(inter_pos[inter_i]);
			}
			//inter_ctrl_points->push_back(cur_pos);
			part_dis = 0;
		}

		return 0;
	}

	int SimuNaviTool::getInterpolations(const ScreenPos2D &start_pos, const ScreenPos2D &end_pos, double inter_dis, std::vector<ScreenPos2D> *inter_pols)
	{
		double dis = GIS_Embedded::SpatialMath::getDistance(start_pos, end_pos);
		if(dis < 2 * inter_dis)
			return 0;

		uint_32 inter_size = dis / inter_dis - 1;
		for(uint_32  i = 0; i < inter_size; ++i)
		{
			ScreenPos2D inter_pos;
			inter_pos.d0 = start_pos.d0 + (end_pos.d0 - start_pos.d0) * ((i+1) * inter_dis) / dis;
			inter_pos.d1 = start_pos.d1 + (end_pos.d1 - start_pos.d1) * ((i+1) * inter_dis) / dis;

			inter_pols->push_back(inter_pos);
		}

		return 0;
	}

}
