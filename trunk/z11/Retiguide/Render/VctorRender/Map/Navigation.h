#ifndef _APP_NOMFC_NAVIGATION_H
#define _APP_NOMFC_NAVIGATION_H

#include "Retiguide/DataFetcher/DataFetcher.h"
#include "Retiguide/Common/CRS/GlobalCrt.h"
#include "Retiguide/Common/Types/MapConfigType.h"
#include "Retiguide/Common/Types/ProgramLanguage.h"
#include "CommonBase/CommonDef.h"
#include "System/Shared/Syncable.h"
#include "System/LocationService/MGLocation.h"

#include <deque>
#include <algorithm>
#include <map>
#include "MapKit/GPSWrapper/GPS_Info.h"
#include "MapKit/GPSWrapper/GPS_State.h"
#include "MapKit/GPSWrapper/GPSDataRecord.h"
#include "MapKit/GPSWrapper/GPS_Standard.h"
#include "MapKit/GPSWrapper/GPS_FromLog.h"
#include "MapKit/GPSWrapper/GPS_FromRoute.h"
#include "MapKit/GPSWrapper/GPS_Filter.h"


using namespace IS_GIS;
using namespace std;
using namespace z11;
namespace Embedded
{
	class Pathfinding;
}

namespace GIS_Embedded{

	struct CrossInfo
	{
		Pos2DEx host_pos;
		Pos2DEx cross_center;
		std::string path_name;
		int host_dir_clock;
		int turn_dir_type;
		int path_length_all;
		int path_length_left;

		CrossInfo()
		{
			host_pos.d0 = 0;
			host_pos.d1 = 0;
			cross_center.d0 = 0;
			cross_center.d1 = 0;
			path_name = "";
			host_dir_clock = 0;
			turn_dir_type = 1;
			path_length_all = 1;
			path_length_left = 0;
		}
	};
	struct NearFinal
	{
		bool m_is_near_final;
		Pos2DEx m_last_near_pos;

		NearFinal():m_is_near_final(false)
		{}
		void setNearFinal(const Pos2DEx & pos)
		{
			m_is_near_final = true;
			m_last_near_pos = pos;
		}
		void reset()
		{
			m_is_near_final = false;
			m_last_near_pos.d0 = 0;
			m_last_near_pos.d1 = 0;
		}
	};
	class NavigationInfo;
	class Navigation{

	private:
		Pathfinding *m_path_finding;
		PathResult *_path_res;
		vector< vector<Pos2DEx> > _path_points;

		int m_bUserCancel;
 		NearFinal m_near_final; 
		bool _is_real_navi;
		bool _is_simu_navi;
		bool _is_cross_map_shown;

		int _current_scale;

		GPS_State _gps_simulant_state;
		GPS_Wrapper *_gps_simulant; //zj 2010-12-30  new GPS_FromLog();
		GPS_Wrapper *_gps_from_route; //zj 2010-12-30 new GPS_FromRoute();
		GPS_Filter * m_gps_filter;
		GPS_Info mGpsInfo;



		//cross
		CrossInfo m_cross_info;
		GIS_Embedded::Pos2DEx m_navi_host;

	private:
		static Navigation* m_instance;
		static Syncable* m_syn;

	public:
		bool naviOnPositionChange(MGLocation &location,Pos2DEx& host_pos,Pos2DEx& dest_pos);
		void updateTrack(const Pos2DEx &pos,double dir,int remain_dis);
		bool getCrossData(CrossInfo& cross_info);

	private:
		double calRemainDistance(const Pos2DEx & cur_pos, const Pos2DEx & dest_pos);
		int calCtrlPointInSegment(const Pos2DEx &cur_pos);//lw 计算当前点在pathResult的第几个segment，用来计算剩余真实距�?wrong
		void reRoute(const Pos2DEx & cur_pos, double direction, const Pos2DEx & dest_pos, uint_32 bind_to_ps_index);
		int reCalPathPoints(const Pos2DEx & cur_pos, double dir, const Pos2DEx & dest_pos, uint_32 bind_to_ps_index,
			vector<Pos2DEx> * path_ctrl_points);
		bool isReach(const Pos2DEx & cur_pos, const Pos2DEx & dest_pos/*,const vector< vector<Pos2DEx> > & _path_points*/);
		void reachFinal() ;// reach the path end
		void reachTarget();//  reach the dest
		bool isReachedFinal(const Pos2DEx & pos);
		void toShowCross(const NavigationInfo &navi_info, Pos2DEx& host_pos,double direction);
		int updateHostPos(double prj_x, double prj_y, double dir_degree);

	
	public:

		Navigation();
		~Navigation();

		static Navigation* getInstance();

		int searchShortestPath();
		int searchShortestPath(Pos2DEx host_pos,Pos2DEx dest_pos);//lw 2011/03/08  

		int startNavigation(MGLocation &gps,Pos2DEx &dest_pos);
		void endNavigation();
		void startNaviSimulate(Pos2DEx &dest_pos);

		void reFreshMap();
		void resizeMap(int scale);


		inline bool isCrossOn()const //用来判断是否已经存在路口放大
		{
			return _is_cross_map_shown;
		}
		inline void setIsCrossOn(bool flag)
		{
			_is_cross_map_shown = flag;
		}

		inline void userCancelRouting()
		{
			m_bUserCancel = 2;
// 			_is_path_searching = false;
		}
		inline int isUserCancelRouting()
		{
			return m_bUserCancel;
		}
		inline void setUserCancel(int flag)
		{
			m_bUserCancel = flag;
		}
		void getPathPoints(vector<vector<Pos2DEx> >& path);
		GIS_Embedded::Pos2DEx& getNaviHost()
		{
			return m_navi_host;
		}

	};


}

#endif