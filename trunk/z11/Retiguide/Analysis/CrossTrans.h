#ifndef _RENDER_CROSSTRANS_H
#define _RENDER_CROSSTRANS_H

#include "../Common/Share/Scope.h"
#include "../SpatialMath/SpatialMath.h"
#include "../Render/VctorRender/Map/MapFrame.h"
#include "TransformRoad.h"
#include "../../MapKit/Embedded/Network/AdjacencyList.h"
#include <map>

using namespace std;
using namespace Embedded::Network;
using namespace Embedded;

namespace GIS_Embedded
{
	class CrossRoad
	{
	public:
		vector<GIS_Embedded::Pos2DEx> m_external_line;
		vector<GIS_Embedded::Pos2DEx> m_center_line;
		vector<vector<GIS_Embedded::Pos2DEx> > m_compartmentation_lines;//compartmentations数组中按从左至右排列虚线,0最左，sum-1最右
		int m_grade;
		int m_width;

	public://used when calIntersectRoads
		int_32 m_id;
		map<int_32,pair<double,pair<GIS_Embedded::Pos2DEx,GIS_Embedded::Pos2DEx> > > m_intersectRoads;
	};

	class CrossTrans
	{
	private:
		static int initExternalLine(vector<GIS_Embedded::Pos2DEx>* road_external,vector<pair<GIS_Embedded::Pos2DEx, int> >* left_pos,vector<pair<GIS_Embedded::Pos2DEx, int> >* right_pos,const int road_width,const vector<GIS_Embedded::Pos2DEx>& src_line);
		static int getExternalPosTwoPointLine(vector<GIS_Embedded::Pos2DEx>* road_external,vector<pair<GIS_Embedded::Pos2DEx, int> >* left_pos,vector<pair<GIS_Embedded::Pos2DEx, int> >* right_pos,const int road_width,const vector<GIS_Embedded::Pos2DEx>& src_line);
		static int getExternalPosMoreTwoPointLine(vector<GIS_Embedded::Pos2DEx>* road_external,vector<pair<GIS_Embedded::Pos2DEx, int> >* left_pos,vector<pair<GIS_Embedded::Pos2DEx, int> >* right_pos,const int road_width,const vector<GIS_Embedded::Pos2DEx>& src_line);
		static int initCompartmentationLines(vector<vector<GIS_Embedded::Pos2DEx> >* road_compart,const int grade,const vector<pair<GIS_Embedded::Pos2DEx, int> >& left_pos,const vector<pair<GIS_Embedded::Pos2DEx, int> >& right_pos,const vector<GIS_Embedded::Pos2DEx>& src_line);
		static int initCompartmentationLines(vector<vector<GIS_Embedded::Pos2DEx> >* road_compart,const int no, const int tot,const vector<pair<GIS_Embedded::Pos2DEx, int> >& left_pos,const vector<pair<GIS_Embedded::Pos2DEx, int> >& right_pos,const vector<GIS_Embedded::Pos2DEx>& src_line);

	private:
		static void transRoad(CrossRoad* cross_road,const vector<GIS_Embedded::Pos2DEx>& line,const GIS_Embedded::Pos2DEx& start_pos,const GIS_Embedded::Pos2DEx& end_pos,int grade,int width);
		static void calIntersectRoads(vector< Line* >& topo_data, vector< pair<Pos2DEx, Pos2DEx> > dataBEs, vector<CrossRoad*>& tpID_road);
		static void cutRoadByConnRoads(int road_id,set<int>& roads_conn,Pos2DEx& cross_pos,vector<CrossRoad*>& tpID_road);
		//返回两条射线（由交叉口为起点）的角度差，正表示在src的右侧，负表示src的左侧,(-PI,PI)
		static double calAngle(CrossRoad& src,CrossRoad& road_conn,GIS_Embedded::Pos2DEx& cross_pos,bool src_from_cross);
		static void intersectRoadLines(CrossRoad& road_src,CrossRoad& road_conn,bool isleft,bool src_from_cross,GIS_Embedded::Pos2DEx& cross_pos);
		static void intersectLines(vector<GIS_Embedded::Pos2DEx>& src,bool src_from_cross,const vector<GIS_Embedded::Pos2DEx>& conn);
		//static void mergeRoads(Line*& Line*,const Scope& scope,vector<GIS_Embedded::Pos2DEx>* data_merged,vector<uint64_t>* ids_merged);
		static bool isHeadNearCross(const GIS_Embedded::Pos2DEx& cross,const vector<GIS_Embedded::Pos2DEx>& line);
		static int getRoadFRC(Line* data);

	private:
		static void cutArrowInScope(const vector<GIS_Embedded::Pos2DEx>& line,const Scope& scope,vector<GIS_Embedded::Pos2DEx>* ret);
		static int testPath(const vector<GIS_Embedded::Pos2DEx>& line,const Scope& scope,int begin,vector<GIS_Embedded::Pos2DEx>* ret);
		static void cutPathInScope(const vector<GIS_Embedded::Pos2DEx>& line,const Scope& scope);//lw
		static bool isNoContain(GIS_Embedded::Pos2DEx temp_point);
		static void caculatePathTrack(const vector<GIS_Embedded::Pos2DEx> line,vector<GIS_Embedded::Pos2DEx>* ret);

	public:
		static int crossRoadsTrans(vector<CrossRoad*>* cross_roads,vector<Line* >& data,const Scope& scope,const MapFrame& map_frame);
		static int arrowTrans(const vector<GIS_Embedded::Pos2DEx>& path_line, const int arrow_width,const Scope& scope,vector<GIS_Embedded::Pos2DEx>* arrow_line);
		
		void setPathTrack()
		{
			path_size =0;
			path_track.clear();
		}
		static void resetPathRemain()
		{
			m_is_not_firstPaint = false;
			//path_remain.clear();
			//path_size = 0;
		}
	public:
		static int path_size;
		static vector<GIS_Embedded::Pos2DEx> path_track;

		static vector<bool> path_remain;
		static bool m_is_not_firstPaint;
	};
}
#endif