 #include "CrossTrans.h"
 #include "../Common/Util/Algorithmic.h"
 #include "../Common/String_Tools.h"
 #include "../../MapKit/Embedded/BasicType/Line.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288 
#endif
 using namespace Embedded;

 namespace GIS_Embedded
 {
	vector<bool> CrossTrans::path_remain(65535);
	vector<Pos2DEx> CrossTrans::path_track(127);
	int CrossTrans::path_size;
	bool CrossTrans::m_is_not_firstPaint;

	int CrossTrans::initExternalLine(vector<Pos2DEx>* road_external,vector<pair<Pos2DEx, int> >* left_pos,vector<pair<Pos2DEx, int> >* right_pos,const int road_width,const vector<Pos2DEx>& src_line)
	{
		assert(src_line.size() > 1);
		road_external->reserve(src_line.size() * 4 + 4);
		if(src_line.size() == 2)
			getExternalPosTwoPointLine(road_external,left_pos,right_pos,road_width,src_line);
		else
			getExternalPosMoreTwoPointLine(road_external,left_pos,right_pos,road_width,src_line);

		return 0;
	}

	int CrossTrans::getExternalPosTwoPointLine(vector<Pos2DEx>* road_external,vector<pair<Pos2DEx, int> >* left_pos,vector<pair<Pos2DEx, int> >* right_pos,const int road_width,const vector<Pos2DEx>& src_line)
	{
		Pos2DEx pos1, pos2;
		SpatialMath::offsetSegment(src_line[0], src_line[1], -road_width/2, &pos1, &pos2);
		Pos2DEx pos3(Pos2DEx(src_line[1].d0 + src_line[1].d0 - pos2.d0, src_line[1].d1 + src_line[1].d1 - pos2.d1)),
			pos4(Pos2DEx(src_line[0].d0 + src_line[0].d0 - pos1.d0, src_line[0].d1 + src_line[0].d1 - pos1.d1));
		left_pos->push_back(make_pair(pos1, 0));
		left_pos->push_back(make_pair(pos2, 1));
		right_pos->push_back(make_pair(pos4, 0));
		right_pos->push_back(make_pair(pos3, 1));
		TransformRoad::getDestLineFromSegment(src_line, *road_external, road_width, *left_pos, *right_pos);

		return 0;
	}

	int CrossTrans::getExternalPosMoreTwoPointLine(vector<Pos2DEx>* road_external,vector<pair<Pos2DEx, int> >* left_pos,vector<pair<Pos2DEx, int> >* right_pos,const int road_width,const vector<Pos2DEx>& src_line)
	{
		static vector<Pos2DEx> side0(127), side1(127);
		TransformRoad::offsetAllSegment(src_line, side0, side1, road_width/2);
		left_pos->reserve(src_line.size() * 2);
		right_pos->reserve(src_line.size() * 2);
		TransformRoad::intersectAllSegment(src_line, side0, side1, *left_pos, *right_pos);
		TransformRoad::getDestLineFromSegment(src_line, *road_external, road_width, side0, side1, *left_pos, *right_pos);

		return 0;
	}

	// m_grade = 0 单车道
	// m_grade = 1 双向2车道
	// m_grade = 2 双向4车道
	// m_grade = 3 双向6车道
	// m_grade = 4 双向8车道
	int CrossTrans::initCompartmentationLines(vector<vector<Pos2DEx> >* road_compart,
					const int grade,
					const vector<pair<Pos2DEx, int> >& left_pos,
					const vector<pair<Pos2DEx, int> >& right_pos,
					const vector<Pos2DEx>& src_line)
	{
		if(grade == 0)
			return 0;
		//assert(grade > 0 && grade <= 4);
		
		if(grade > 1)
			road_compart->resize(grade*2 - 2);

		for(int i = 1; i < grade; ++i)
		{
			initCompartmentationLines(road_compart,i, grade,left_pos,right_pos,src_line);
		}

		return 0;
	}

	int CrossTrans::initCompartmentationLines(vector<vector<Pos2DEx> >* road_compart,
												 const int no, 
												 const int tot,
												 const vector<pair<Pos2DEx, int> >& left_pos,
												 const vector<pair<Pos2DEx, int> >& right_pos,
												 const vector<Pos2DEx>& src_line)
	{
		road_compart->at(tot-no-1).resize(left_pos.size());
		for(int i = 0; i < left_pos.size(); ++i)
		{
			int pos_index = left_pos.at(i).second;
			road_compart->at(tot-no-1).at(i).d0 = src_line.at(pos_index).d0 + ( left_pos.at(i).first.d0 - src_line.at(pos_index).d0) * no  / tot;
			road_compart->at(tot-no-1).at(i).d1 = src_line.at(pos_index).d1 + ( left_pos.at(i).first.d1 - src_line.at(pos_index).d1) * no  / tot;
		}

		road_compart->at(tot+no-2).resize(right_pos.size());
		for(int i = 0; i < right_pos.size(); ++i)
		{
			int pos_index = right_pos.at(i).second;

			road_compart->at(tot+no-2).at(i).d0 = src_line.at(pos_index).d0 + ( right_pos.at(i).first.d0 - src_line.at(pos_index).d0) * no  / tot;
			road_compart->at(tot+no-2).at(i).d1 = src_line.at(pos_index).d1 + ( right_pos.at(i).first.d1 - src_line.at(pos_index).d1) * no  / tot;
		}

		return 0;
	}

	void CrossTrans::transRoad(CrossRoad* cross_road,const vector<Pos2DEx>& line,const Pos2DEx& start_pos,const Pos2DEx& end_pos,int grade,int width)
	{
		static vector<Pos2DEx> line_add(255);
		line_add.erase(line_add.begin()+1,line_add.end());
		line_add.insert(line_add.end(),line.begin(),line.end());
		if(SpatialMath::getDistanceSquare(line[0],start_pos) < SpatialMath::getDistanceSquare(line[0],end_pos))
		{
			line_add[0] = start_pos;//line_add.insert(line_add.begin(),start_pos);
			line_add.insert(line_add.end(),end_pos);
		}
		else
		{
			line_add[0] = end_pos;//line_add.insert(line_add.begin(),end_pos);
			line_add.insert(line_add.end(),start_pos);
		}

		cross_road->m_grade = grade;
		cross_road->m_width = width;
		cross_road->m_center_line.clear();
		cross_road->m_intersectRoads.clear();
		cross_road->m_compartmentation_lines.clear();
		cross_road->m_external_line.clear();
		TransformRoad::filtrationPointDouglas(line_add, width/2, cross_road->m_center_line);

		static vector<pair<Pos2DEx, int> > left_pos, right_pos;
		left_pos.clear(),right_pos.clear();
		initExternalLine(&(cross_road->m_external_line),&left_pos,&right_pos,width,cross_road->m_center_line);
		initCompartmentationLines(&(cross_road->m_compartmentation_lines),grade,left_pos,right_pos,cross_road->m_center_line);
		
	}

	void CrossTrans::calIntersectRoads(vector< Line* >& topo_data, vector< pair<Pos2DEx, Pos2DEx> > dataBEs, vector<CrossRoad*>& tpID_road)
	{
		assert(topo_data.size() == dataBEs.size() && topo_data.size() == tpID_road.size() );
		set<int> roads_conn; //存储索引值
		for(int i = 0; i < topo_data.size(); i++)
		{
			roads_conn.clear();
			for(int j=0; j < topo_data.size(); j++)
			{
				if(dataBEs[i].first == dataBEs[j].first && i != j)
					roads_conn.insert(j);
				else if(dataBEs[i].first == dataBEs[j].second && i != j)
					roads_conn.insert(j);
			}
			
			cutRoadByConnRoads(i,roads_conn,dataBEs[i].first,tpID_road);


			roads_conn.clear();
			for(int j=0; j < topo_data.size(); j++)
			{
				if(dataBEs[i].second == dataBEs[j].first && i != j)
					roads_conn.insert(j);
				else if(dataBEs[i].second == dataBEs[j].second && i != j)
					roads_conn.insert(j);
			}

			cutRoadByConnRoads(i,roads_conn,dataBEs[i].second,tpID_road);

		}
	}

	void CrossTrans::cutRoadByConnRoads(int road_id,set<int>& roads_conn,Pos2DEx& cross_pos,vector<CrossRoad*>& tpID_road)
	{
		if(roads_conn.size() <=1)
			return;

		CrossRoad* cur = tpID_road[road_id];
		vector<Embedded::uint_64> conn_ids(roads_conn.begin(),roads_conn.end());

		double left=2*M_PI,right=2*M_PI;
		CrossRoad *left_road=NULL,*right_road=NULL;
		bool src_start_from_cross;//标记是否由路口出发

		src_start_from_cross = isHeadNearCross(cross_pos,cur->m_center_line);

		double angle;
		for(int i=0,sum=conn_ids.size();i<sum;i++)
		{
			CrossRoad* road_conn = tpID_road[conn_ids[i]];
			
			if(road_conn->m_intersectRoads.find(cur->m_id) != road_conn->m_intersectRoads.end()) //角度已经计算过
			{
				angle = -road_conn->m_intersectRoads[cur->m_id].first;
			}
			else
			{
				angle = calAngle(*cur,*road_conn,cross_pos,src_start_from_cross);//大于0为右方
				cur->m_intersectRoads[road_conn->m_id] = make_pair(angle,make_pair(Pos2DEx(),Pos2DEx()));
			}

			if(angle > 0 && angle<right)
			{
				right = angle;
				right_road = road_conn;
			}
			if(angle<0 && (-angle)<left)
			{
				left = -angle;
				left_road = road_conn;
			}
		}

		if(left_road!=NULL && left<3)
		{
			intersectRoadLines(*cur,*left_road,true,src_start_from_cross,cross_pos);
		}
		if(right_road!=NULL && right<3)
		{
			intersectRoadLines(*cur,*right_road,false,src_start_from_cross,cross_pos);
		}

	}

	//返回两条射线（由交叉口为起点）的角度差，正表示在src的右侧，负表示src的左侧,(-PI,PI)
	double CrossTrans::calAngle(CrossRoad& src,CrossRoad& road_conn,Pos2DEx& cross_pos,bool src_from_cross)
	{
		assert(src.m_center_line.size() > 1);
		assert(road_conn.m_center_line.size() > 1);
		Pos2DEx pos1(src.m_center_line[1]),pos2(road_conn.m_center_line[1]);
		if(!src_from_cross)
			pos1 = src.m_center_line[src.m_center_line.size()-2];
		if(!isHeadNearCross(cross_pos,road_conn.m_center_line)/*SpatialMath::getDistanceSquare(road_conn.m_center_line.front(),cross_pos) > SpatialMath::getDistanceSquare(road_conn.m_center_line.back(),cross_pos)*/)
			pos2 = road_conn.m_center_line[road_conn.m_center_line.size()-2];

		double angle1=SpatialMath::calLineNAngle(cross_pos,pos1);
		double angle2=SpatialMath::calLineNAngle(cross_pos,pos2);

		double angle =  angle1-angle2;
		if(abs(angle)<M_PI)
			angle = -angle;
		else if(angle >= M_PI)
			angle = 2*M_PI-angle;
		else if(angle <= -M_PI)
			angle = -2*M_PI-angle;

		return angle;
	}

	// m_grade = 0 单车道
	// m_grade = 1 双向2车道
	// m_grade = 2 双向4车道
	// m_grade = 3 双向6车道
	// m_grade = 4 双向8车道
	void CrossTrans::intersectRoadLines(CrossRoad& road_src,CrossRoad& road_conn,bool isleft,bool src_from_cross,Pos2DEx& cross_pos)
	{
		static vector<Pos2DEx> bisector(2);
		if(road_conn.m_intersectRoads.find(road_src.m_id) != road_conn.m_intersectRoads.end())
		{
			bisector[0] = road_conn.m_intersectRoads[road_src.m_id].second.first;
			bisector[1] = road_conn.m_intersectRoads[road_src.m_id].second.second;
		}
		else
		{
			bisector[0] = cross_pos;
			double a_sin = sin(road_src.m_intersectRoads[road_conn.m_id].first/2);
			double a_cos = cos(road_src.m_intersectRoads[road_conn.m_id].first/2);

			Pos2DEx pos1,pos2;
			if(src_from_cross)
			{
				pos1 = road_src.m_center_line[0];
				pos2 = road_src.m_center_line[1];
			}
			else
			{
				pos1 = road_src.m_center_line[road_src.m_center_line.size()-1];
				pos2 = road_src.m_center_line[road_src.m_center_line.size()-2];
			}
			bisector[1].d0 = (pos2.d0-pos1.d0)*a_cos + (pos2.d1-pos1.d1)*a_sin + pos1.d0;
			bisector[1].d1 = (pos2.d1-pos1.d1)*a_cos - (pos2.d0-pos1.d0)*a_sin + pos1.d1;

			road_src.m_intersectRoads[road_conn.m_id].second.first = bisector[0];
			road_src.m_intersectRoads[road_conn.m_id].second.second = bisector[1];
		}

		int start,end;
		if(isleft == src_from_cross)
		{//left
			start=0;
			end=road_src.m_compartmentation_lines.size()/2;
		}
		else
		{//right
			start=road_src.m_compartmentation_lines.size()/2;
			end=road_src.m_compartmentation_lines.size();
		}

		for(;start<end;start++)
			intersectLines(road_src.m_compartmentation_lines[start],src_from_cross,bisector);
	}

	void CrossTrans::intersectLines(vector<Pos2DEx>& src,bool src_from_cross,const vector<Pos2DEx>& conn)
	{
#define TOLERANCE 2 //不能小于2

		Pos2DEx cross_pos(0,0);
		if(src_from_cross)
		{
			for(int i = 1, i_sum = src.size(); i < i_sum; i++)
			{
				if(SpatialMath::isLineCross(src[i-1], src[i], conn[0], conn[1], &cross_pos))
				{
					if(SpatialMath::pointOnSection(src[i-1], src[i], cross_pos, TOLERANCE)>=0) //最后一个参数得重新设置
					{
						src.erase(src.begin(),src.begin()+i);
						src.insert(src.begin(),cross_pos);
						return;
					}
				}
			}
		}
		else
		{
			for(int i = src.size()-1; i >0; i--)
			{
				if(SpatialMath::isLineCross(src[i], src[i-1], conn[0], conn[1], &cross_pos))
				{
					if(SpatialMath::pointOnSection(src[i-1], src[i], cross_pos, TOLERANCE)>=0) //最后一个参数得重新设置
					{
						src.erase(src.begin()+i,src.end());
						src.insert(src.end(),cross_pos);
						return;
					}
				}

			}
		}
	
	}

	bool CrossTrans::isHeadNearCross(const Pos2DEx& cross,const vector<Pos2DEx>&  line)
	{
		if(SpatialMath::getDistanceSquare(cross,line.front())
			<= SpatialMath::getDistanceSquare(cross,line.back()))
		{
			return true;
		}	
		else
		{
			return false;
		}
	}

	int CrossTrans::crossRoadsTrans(vector<CrossRoad*>* cross_roads,vector<Line* >& data,const Scope& scope,const MapFrame& map_frame)
	{
		int road_sum =0;
		//vector<CrossRoad*> id_road;
		//Pathfinding* pathfinding = Pathfinding::getInstance();

		//MapPos2D lefLow(scope.m_left, scope.m_bottom), rightTop(scope.m_right, scope.m_top);
		//Rect rect(lefLow, rightTop);		

		road_sum = data.size();
		if(cross_roads->size() < road_sum)
		{
			int i = cross_roads->size();
			cross_roads->resize(road_sum);
			for(;i<road_sum;i++)
				cross_roads->at(i) = new CrossRoad();
		}

		vector< pair<Pos2DEx, Pos2DEx> > dataBEs;

		for(int i=0;i<road_sum;i++)
		{
			
			vector<Pos2DEx> ctrl_points;
			data[i]->getPoints(&ctrl_points);
			dataBEs.push_back(make_pair(ctrl_points[0], ctrl_points[ctrl_points.size()-1]));
			
			int frc = getRoadFRC(data[i]);
			const CrossLayerStyle& style = map_frame.getCrossLayerStyleByFrc(frc);
			transRoad(cross_roads->at(i), ctrl_points, ctrl_points[0], ctrl_points[ctrl_points.size() - 1],style.m_grade,style.m_width);

			//id_road[data[i].roadID] = cross_roads->at(i);
			//cross_roads->at(i)->m_id = i;
		}

		calIntersectRoads(data, dataBEs, *cross_roads);

		return road_sum;
	}

	
	int CrossTrans::getRoadFRC(Line* data)
	{
		return 3;
		//return data.getFrc();
	}

	int CrossTrans::arrowTrans(const vector<Pos2DEx>& path_line, const int arrow_width,const Scope& scope,vector<Pos2DEx>* arrow_line)
	{
		static vector<Pos2DEx> line_cut(127);
		line_cut.clear();
		Scope scope_cut(scope);
		scope_cut.resizeScope(0.65);

		//vector<Pos2DEx> pathTrack;
		//caculatePathTrack(path_line,&pathTrack);//之前用来判断已走过路径，现在直接传入剩余路径
		//cutPathInScope(path_line,scope_cut);
		//if (pathTrack.size() < 2)
			cutArrowInScope(path_line,scope_cut,&line_cut);
		//else
		//	cutArrowInScope(pathTrack,scope_cut,&line_cut);

		//if (line_cut.size()==1)
		//{
		//	line_cut.clear();
		//	cutArrowInScope(path_line,scope_cut,&line_cut);
		//}

		if (line_cut.size() < 2)  //lw
			return -1;

		static vector<Pos2DEx> line_filted(63);
		line_filted.clear();
		TransformRoad::filtrationPointDouglas(line_cut,arrow_width/2,line_filted);

		static vector<Pos2DEx> side0(127), side1(127);
		TransformRoad::offsetAllSegment(line_filted, side0, side1, arrow_width/2);
		vector<Pos2DEx> ctl_pos_0, ctl_pos_1;
		ctl_pos_0.reserve(line_filted.size() * 2);
		ctl_pos_1.reserve(line_filted.size() * 2);
		TransformRoad::intersectAllSegment(line_filted, side0, side1, ctl_pos_0, ctl_pos_1);
		arrow_line->reserve(ctl_pos_0.size() + ctl_pos_1.size() + 8);
		Pos2DEx left_out_pos, left_in_pos, right_out_pos, right_in_pos, forward_pos;
		SpatialMath::pointOffsetLine(line_filted.at(line_filted.size() - 2), line_filted.at(line_filted.size() - 1), 
			line_filted.at(line_filted.size() - 1), -arrow_width*6/5, &left_out_pos);
		SpatialMath::pointOffsetLine(line_filted.at(line_filted.size() - 2), line_filted.at(line_filted.size() - 1), 
			line_filted.at(line_filted.size() - 1), -arrow_width/2, &left_in_pos);
		SpatialMath::pointOffsetLine(line_filted.at(line_filted.size() - 2), line_filted.at(line_filted.size() - 1), 
			line_filted.at(line_filted.size() - 1), arrow_width*6/5, &right_out_pos);
		SpatialMath::pointOffsetLine(line_filted.at(line_filted.size() - 2), line_filted.at(line_filted.size() - 1), 
			line_filted.at(line_filted.size() - 1), arrow_width/2, &right_in_pos);

		SpatialMath::pointOffsetLine(right_out_pos, left_out_pos, 
			Pos2DEx((left_out_pos.d0 + right_out_pos.d0) / 2, (left_out_pos.d1 + right_out_pos.d1) / 2), arrow_width*3/2, &forward_pos);
		arrow_line->push_back(side0[0]);
		copy(ctl_pos_0.begin(), ctl_pos_0.end(), back_inserter(*arrow_line));
		arrow_line->push_back(left_in_pos);
		arrow_line->push_back(left_out_pos);
		arrow_line->push_back(forward_pos);
		arrow_line->push_back(right_out_pos);
		arrow_line->push_back(right_in_pos);
		copy(ctl_pos_1.rbegin(), ctl_pos_1.rend(), back_inserter(*arrow_line));
		arrow_line->push_back(side1[0]);
		arrow_line->push_back(arrow_line->front());

		return 0;
	}

	void CrossTrans::caculatePathTrack(const vector<Pos2DEx> line,vector<Pos2DEx>* ret)
	{
		for (int index=path_size ; index < line.size(); index++)
		{
			ret->push_back(line[index]);
		}
	}

	bool CrossTrans::isNoContain(Pos2DEx temp_point)
	{
		for (int index = 0 ; index <= path_size; index++)
		{
			if (CrossTrans::path_track[index].d0 == temp_point.d0 && CrossTrans::path_track[index].d1 == temp_point.d1)
				return false;
		}
		return true;
	}

	void CrossTrans::cutPathInScope(const vector<Pos2DEx>& line,const Scope& scope)
	{
		int sum = line.size();
		if (sum < 2)
			return;
		for(int index = 0 ; index < sum ; )
		{
			if (scope.isContain(line[index].d0,line[index].d1))
			{
				if (isNoContain(line[index]))
				{
					path_track.push_back(line[index]);
					path_size++;
				}
				index++;
			}
			else if (!(scope.isContain(line[index].d0,line[index].d1)) && scope.isContain(line[index+1].d0,line[index+1].d1))
			{
				if (isNoContain(line[index]))
				{
					path_track.push_back(line[index]);
					path_track.push_back(line[index+1]);
					path_size+=2;
				}
				index+=2;
			}
			else 
				break;
		}
	}
	
	void CrossTrans::cutArrowInScope(const vector<Pos2DEx>& line,const Scope& scope,vector<Pos2DEx>* ret)
	{
		int sum = line.size();
		if(sum < 2)
			return;

		Pos2DEx last_pos(0,0);
		bool last_in_scope = false; //只要有一个在scope中即为true
		bool last2_in_scope = false; //两个都在scope中才为true
		vector<Pos2DEx> ret_cut_off;
		for(int index=0; index < sum - 1; index ++)
		{
			if(Algorithmic::lineCutOff(line[index].d0, line[index].d1, line[index + 1].d0, line[index + 1].d1, scope))
			{
				ret_cut_off.clear();
				bool is2in = Algorithmic::lineCutOff(line[index].d0, line[index].d1, line[index + 1].d0, line[index + 1].d1,scope,&ret_cut_off);

				ret->push_back(ret_cut_off[0]);
				last_pos = ret_cut_off[1];

				if(last2_in_scope && !is2in)
					break;

				last_in_scope = true;
				last2_in_scope = is2in;
			}
			else if(last_in_scope)
				break;
		}
		ret->push_back(last_pos);
		return;
	}


int CrossTrans::testPath(const vector<Pos2DEx>& line,const Scope& scope,int begin,vector<Pos2DEx>* ret)
{
	int sum = line.size();
	int index;
	Pos2DEx last_pos(0,0);
	bool last_in_scope = false; //只要有一个在scope中即为true
	bool last2_in_scope = false; //两个都在scope中才为true
	vector<Pos2DEx> ret_cut_off;
	for(index=begin; index < sum - 1; index ++)
	{//两点中有一点在scope，即进入判断
		if(Algorithmic::lineCutOff(line[index].d0, line[index].d1, line[index + 1].d0, line[index + 1].d1, scope)/* && !path_remain[index]*/)
		{
			ret_cut_off.clear();
			//只有两点都在scope，is2in才为true
			bool is2in = Algorithmic::lineCutOff(line[index].d0, line[index].d1, line[index + 1].d0, line[index + 1].d1,scope,&ret_cut_off);

			ret->push_back(ret_cut_off[0]);
			last_pos = ret_cut_off[1];

			if(last2_in_scope && !is2in)//三个点中，前两个在scope，第三个不在scope
				break;

			last_in_scope = true;
			last2_in_scope = is2in;
		}
		else if(last_in_scope) 
			break;
	}
	if(last_pos.d0 && last_pos.d1)
	{
		ret->push_back(last_pos);	
		return index;
	}
	else
		return sum;
}

}




		//vector<Pos2DEx> *src,*conn;

		//if(isleft == src_from_cross)
		//	src = &(road_src.m_compartmentation_lines[0]);
		//else
		//	src = &(road_src.m_compartmentation_lines[1]);

		//if(isleft == conn_from_cross)
		//	conn = &(road_conn.m_compartmentation_lines[1]);
		//else
		//	conn = &(road_conn.m_compartmentation_lines[0]);

		//intersectLines(*src,src_from_cross,*conn,conn_from_cross);

		//if(isleft == src_from_cross)
		//{
		//	if(isleft == conn_from_cross)
		//	{
		//		intersectLines(src.m_compartmentation_lines[0],src_from_cross,road_conn.m_compartmentation_lines[1],conn_from_cross);
		//	}
		//	else
		//	{
		//		intersectLines(src.m_compartmentation_lines[0],src_from_cross,road_conn.m_compartmentation_lines[0],conn_from_cross);
		//	}
		//}
		//else
		//{
		//	if(isleft == conn_from_cross)
		//	{
		//		intersectLines(src.m_compartmentation_lines[1],src_from_cross,road_conn.m_compartmentation_lines[1],conn_from_cross);
		//	}
		//	else
		//	{
		//		intersectLines(src.m_compartmentation_lines[1],src_from_cross,road_conn.m_compartmentation_lines[0],conn_from_cross);
		//	}
		//}

	//void CrossTrans::mergeRoads(line& line,const Scope& scope,vector<Pos2DEx>* data_merged,vector<uint64_t>* ids_merged)
	//{
	//	uint64_t cur_id = line->m_id;
	//	set<uint64_t> roads_conn;
	//	line* road_conn;

	//	GeometryLine* gm_line = (GeometryLine*)line->getGeoObject()->_geometry;
	//	bool isFromStartToEnd = isHeadNearCross(line->m_start->m_pos,gm_line->_control_pos2d);
	//	data_merged->insert(data_merged->end(),gm_line->_control_pos2d.begin(),gm_line->_control_pos2d.end());

	//	if(scope.isContain(line->m_start->m_pos.d0,line->m_start->m_pos.d1)
	//		&& line->m_start->m_start_road_elements.size() <=2
	//		&& line->m_start->m_end_road_elements.size() <=2)
	//	{
	//		for(int j=0,j_sum=line->m_start->m_start_road_elements.size();j<j_sum;j++)
	//		{
	//			if(line->m_start->m_start_road_elements[j]->m_id != cur_id)
	//			{
	//				roads_conn.insert(line->m_start->m_start_road_elements[j]->m_id);
	//				road_conn = &(line->m_start->m_start_road_elements[j]);
	//			}
	//		}
	//		for(int j=0,j_sum=line->m_start->m_end_road_elements.size();j<j_sum;j++)
	//		{
	//			if(line->m_start->m_end_road_elements[j]->m_id != cur_id
	//				&& roads_conn.find(line->m_start->m_end_road_elements[j]->m_id) == roads_conn.end())
	//			{
	//				roads_conn.insert(line->m_start->m_end_road_elements[j]->m_id);
	//				road_conn = &(line->m_start->m_end_road_elements[j]);
	//			}
	//		}

	//		if(roads_conn.size() ==1)
	//		{
	//			GeometryLine* gm_line_conn = (GeometryLine*)(*road_conn)->getGeoObject()->_geometry;
	//			bool fromStart = isHeadNearCross(line->m_start->m_pos,gm_line_conn->_control_pos2d);
	//			if(fromStart ==isFromStartToEnd)
	//				data_merged->insert(data_merged->begin(),gm_line_conn->_control_pos2d.begin(),gm_line_conn->_control_pos2d.end());
	//			else
	//				data_merged->insert(data_merged->end(),gm_line_conn->_control_pos2d.begin(),gm_line_conn->_control_pos2d.end());
	//		}
	//	}

	//	roads_conn.clear();
	//	if(scope.isContain(line->m_end->m_pos.d0,line->m_start->m_pos.d1)
	//		&& line->m_end->m_start_road_elements.size() <=2
	//		&& line->m_end->m_end_road_elements.size() <=2)
	//	{
	//		for(int j=0,j_sum=line->m_end->m_start_road_elements.size();j<j_sum;j++)
	//		{
	//			if(line->m_end->m_start_road_elements[j]->m_id != cur_id)
	//			{
	//				roads_conn.insert(line->m_end->m_start_road_elements[j]->m_id);
	//				road_conn = &(line->m_end->m_start_road_elements[j]);
	//			}
	//		}
	//		for(int j=0,j_sum=line->m_end->m_end_road_elements.size();j<j_sum;j++)
	//		{
	//			if(line->m_end->m_end_road_elements[j]->m_id != cur_id
	//				&& roads_conn.find(line->m_end->m_end_road_elements[j]->m_id) == roads_conn.end())
	//			{
	//				roads_conn.insert(line->m_end->m_end_road_elements[j]->m_id);
	//				road_conn = &(line->m_end->m_end_road_elements[j]);
	//			}
	//		}

	//		if(roads_conn.size() ==1)
	//		{
	//			GeometryLine* gm_line_conn = (GeometryLine*)(*road_conn)->getGeoObject()->_geometry;
	//			bool fromStart = isHeadNearCross(line->m_start->m_pos,gm_line_conn->_control_pos2d);
	//			if(fromStart !=isFromStartToEnd)
	//				data_merged->insert(data_merged->begin(),gm_line_conn->_control_pos2d.begin(),gm_line_conn->_control_pos2d.end());
	//			else
	//				data_merged->insert(data_merged->end(),gm_line_conn->_control_pos2d.begin(),gm_line_conn->_control_pos2d.end());
	//		}
	//	}
/*}*/