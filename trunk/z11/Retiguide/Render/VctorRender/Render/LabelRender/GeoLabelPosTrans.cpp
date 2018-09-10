#include "GeoLabelPosTrans.h"
#include "Retiguide/Common/Util/Algorithmic.h"
#include "Retiguide/Common/String_Tools.h"
#include "GraphicCore/MGFont.h"
#include "System/Shared/Utility.h"
#include "Retiguide/SpatialMath/SpatialMath.h"
#include "GraphicCore/MGCanvas.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288 
#endif
using namespace z11;


namespace GIS_Embedded
{

	int GeoLabelPosTrans::getStringExtent(z11::MGSize* size,const std::string& label,const int font_sz)
	{
		static MGCanvas canvas(NULLREF);
		MGRect bounds;
		canvas.measureText(label,font_sz,&bounds);
		*size = MGSize(bounds.width,bounds.height);
		return 0;
	}

	int GeoLabelPosTrans::lineCutOff(vector<z11::MGPoint>* ret,int* longest_space,int* longest_pos_index,
								const z11::MGPoint* control_point,const int pos_sum,const z11::MGRect2& rect)
	{
		int ret_sum = 0;
		ret->resize(pos_sum);

		z11::MGPoint last_pos,ret1,ret2;
		int i =0;
		for(; i<pos_sum-1; i++)
		{
			if(Algorithmic::lineCutOffByRect(&ret1, &ret2, 
							control_point[i].x, control_point[i].y, control_point[i+1].x, control_point[i+1].y, rect))
			{
				//若裁剪了，则进行如下处理   lw
				*longest_space = getDistance(ret1, ret2);
				*longest_pos_index = ret_sum;
				last_pos = ret2;

				(*ret)[ret_sum++] = ret1;
				(*ret)[ret_sum++] = ret2;
				break;
			}

		}
		for(i++; i<pos_sum-1; i++)
		{
			if(!Algorithmic::lineCutOffByRect(&ret1, &ret2, 
							control_point[i].x, control_point[i].y, control_point[i+1].x, control_point[i+1].y, rect))
				break;

			if(!((ret1.x == last_pos.x) && (ret1.y == last_pos.y)))
				break;

			int tmp_dis = getDistance(ret1, ret2);
			if(tmp_dis > *longest_space)
			{
				*longest_space = tmp_dis;
				*longest_pos_index = ret_sum-1;
			}

			last_pos = ret2;
			
			(*ret)[ret_sum++] = ret2;
		}

		return ret_sum;
	}

	int GeoLabelPosTrans::lineCutOff(vector<z11::MGPoint>* ret,vector<int>* ret_dis,
								const z11::MGPoint* control_point,const int pos_sum,const z11::MGRect2& rect)
	{
		int ret_sum = 0;
		ret->resize(pos_sum);

		ret_dis->clear();
		ret_dis->push_back(0);

		z11::MGPoint last_pos,ret1,ret2;
		int i =0;
		for(; i<pos_sum-1; i++)
		{
			if(Algorithmic::lineCutOffByRect(&ret1, &ret2, 
							control_point[i].x, control_point[i].y, control_point[i+1].x, control_point[i+1].y, rect))
			{
				ret_dis->push_back(ret_dis->at(ret_dis->size()-1)+getDistance(ret1, ret2));

				last_pos = ret2;

				(*ret)[ret_sum++] = ret1;
				(*ret)[ret_sum++] = ret2;
				break;
			}

		}
		for(i++; i<pos_sum-1; i++)
		{
			if(!Algorithmic::lineCutOffByRect(&ret1, &ret2, 
							control_point[i].x, control_point[i].y, control_point[i+1].x, control_point[i+1].y, rect))
				break;

			if(!((ret1.x == last_pos.x) && (ret1.y == last_pos.y)))
				break;

			ret_dis->push_back(ret_dis->at(ret_dis->size()-1)+getDistance(ret1, ret2));

			last_pos = ret2;
			
			(*ret)[ret_sum++] = ret2;
		}

		return ret_sum;
	}

	//int GeoLabelPosTrans::polygonCutOff(vector<z11::MGPoint>* ret,int* longest_space,int* longest_pos_index,
	//							const z11::MGPoint* control_point,const int pos_sum,const z11::MGRect2& rect)
	//{

	//}
	void GeoLabelPosTrans::polygonCutOff(z11::MGPoint* center_XY,double* escape,int* line_length,
								const z11::MGPoint* control_point,const int pos_sum,const z11::MGRect2& rect) //lw
	{
		vector<ScreenPos2D>	screen_pos2D;
		int min_x=480,min_y=800,max_x=0,max_y=0;

		z11::MGPoint last_pos,ret1,ret2;
		UserScreenRect temp_rect(rect.left,rect.top,rect.right,rect.bottom) ;
		for (int i = 0 ; i < pos_sum-1 ; i++)
		{
			Algorithmic::polygonCutOff(control_point[i].x,control_point[i].y,control_point[i+1].x,control_point[i+1].y,temp_rect,&screen_pos2D);
		}

		for (int j = 0 ; j < screen_pos2D.size(); j++)
		{
			if (screen_pos2D[j].X < min_x)
				min_x = screen_pos2D[j].X;
			else if(screen_pos2D[j].X > max_x)
				max_x = screen_pos2D[j].X;
			if(screen_pos2D[j].Y < min_y)
				min_y = screen_pos2D[j].Y;
			else if(screen_pos2D[j].Y > max_y)
				max_y = screen_pos2D[j].Y;
		}
		*line_length = max_x - min_x;
		*escape = 0;
		center_XY->x = (max_x+min_x)/2;
		center_XY->y = (max_y+min_y)/2;

	}

	bool GeoLabelPosTrans::polygonCutOff(vector<z11::MGPoint>* ret,const z11::MGPoint* control_point,const int pos_sum,const z11::MGRect2& rect)
	{
		int i,j;
		vector<ScreenPos2D>	screen_pos2D;
		vector<ScreenPos2D>	filter_screen_pos2D;//用来删除切割后重复的点

		z11::MGPoint last_pos,ret1,ret2;
		UserScreenRect temp_rect(rect.left,rect.top,rect.right,rect.bottom) ;
		for (i = 0 ; i < pos_sum-1 ; i++)
		{
			Algorithmic::polygonCutOff(control_point[i].x,control_point[i].y,control_point[i+1].x,control_point[i+1].y,temp_rect,&screen_pos2D);
		}
		if (screen_pos2D.size() <= 0)
			return false;

		vector<ScreenPos2D> screen_pos_in_scope;
		int min_x=screen_pos2D[0].X,min_y=screen_pos2D[0].Y,max_x=0,max_y=0;
		filter_screen_pos2D.resize(0);
		filter_screen_pos2D.push_back(screen_pos2D[0]);

		for (j = 1 ; j < screen_pos2D.size(); j++)
		{
			if (!screen_pos2D[j].equal(screen_pos2D[j-1]))
				filter_screen_pos2D.push_back(screen_pos2D[j]);

			if (screen_pos2D[j].X < min_x)
				min_x = screen_pos2D[j].X;
			else if(screen_pos2D[j].X > max_x)
				max_x = screen_pos2D[j].X;
			if(screen_pos2D[j].Y < min_y)
				min_y = screen_pos2D[j].Y;
			else if(screen_pos2D[j].Y > max_y)
				max_y = screen_pos2D[j].Y;
		}

		int len_x = max_x - min_x;
		int len_y = max_y - min_y;
		int mid_num = 0;
		Pos2DEx start_pos,end_pos;
		z11::MGPoint mid_pos;
		vector<Pos2DEx> tmp_pos;
		vector<Pos2DEx> pos_in_scope;
		pos_in_scope.resize(0);
		int filter_screen_size = filter_screen_pos2D.size();
		if (len_x >= len_y)
		{
			int pos_num_in_scope = fourRemoveQiDianX(screen_pos_in_scope,filter_screen_pos2D,filter_screen_size,min_x,max_x);
			if (pos_num_in_scope <=2)
				return false;
			pos_in_scope.reserve(pos_num_in_scope);
			for (i = 0; i<pos_num_in_scope ;i++)
			{
				Pos2DEx pos;
				pos.d0 = screen_pos_in_scope[i].X;
				pos.d1 = screen_pos_in_scope[i].Y;
				pos_in_scope.push_back(pos);
			}

			ret->resize(len_x/5+1);

			for (i=min_x ; i<=max_x ;)
			{
				start_pos.d0 = i;
				start_pos.d1 = min_y;
				end_pos.d0 = i;
				end_pos.d1 = max_y;

				i += 5;

				SpatialMath::isLineIntersectsPolygon(start_pos,end_pos,pos_in_scope,tmp_pos);
				if (tmp_pos.size() != 2)
					continue;
				else
				{
					mid_pos.x = (tmp_pos[0].d0 + tmp_pos[1].d0)/2;
					mid_pos.y = (tmp_pos[0].d1 + tmp_pos[1].d1)/2;
					(*ret)[mid_num++] = mid_pos;
				}
			}
		}
		else
		{
			int pos_num_in_scope = fourRemoveQiDianY(screen_pos_in_scope,filter_screen_pos2D,filter_screen_size,min_x,max_x);
			if (pos_num_in_scope <=2)
				return false;
			pos_in_scope.reserve(pos_num_in_scope);
			for (i = 0; i<pos_num_in_scope ;i++)
			{
				Pos2DEx pos;
				pos.d0 = screen_pos_in_scope[i].X;
				pos.d1 = screen_pos_in_scope[i].Y;
				pos_in_scope.push_back(pos);
			}

			ret->resize(len_y/5+1);

			for (i=min_y ; i<=max_y ;)
			{
				start_pos.d0 = min_x;
				start_pos.d1 = i;
				end_pos.d0 = max_x;
				end_pos.d1 = i;

				i += 5;

				SpatialMath::isLineIntersectsPolygon(start_pos,end_pos,pos_in_scope,tmp_pos);
				if (tmp_pos.size() != 2)
					continue;
				else
				{
					mid_pos.x = (tmp_pos[0].d0 + tmp_pos[1].d0)/2;
					mid_pos.y = (tmp_pos[0].d1 + tmp_pos[1].d1)/2;
					(*ret)[mid_num++] = mid_pos;
				}
			}
		}
		ret->resize(mid_num);
		if (mid_num > 0)
			 return true;
		else
			return false;
	}

	void GeoLabelPosTrans::polygonCutOff(z11::MGPoint* center_XY,double* escape,int* line_length,
								const z11::MGPoint* control_point,const int pos_sum,const z11::MGRect2& rect,Scope& scope) //lw
	{
		vector<ScreenPos2D>	screen_pos2D;
		int min_x=480,min_y=800,max_x=0,max_y=0;

		z11::MGPoint last_pos,ret1,ret2;
		UserScreenRect temp_rect(rect.left,rect.top,rect.right,rect.bottom) ;
		for (int i = 0 ; i < pos_sum-1 ; i++)
		{
			Algorithmic::polygonCutOff(control_point[i].x,control_point[i].y,control_point[i+1].x,control_point[i+1].y,temp_rect,&screen_pos2D);
		}

		for (int j = 0 ; j < screen_pos2D.size(); j++)
		{
			if (screen_pos2D[j].X < min_x)
				min_x = screen_pos2D[j].X;
			else if(screen_pos2D[j].X > max_x)
				max_x = screen_pos2D[j].X;
			if(screen_pos2D[j].Y < min_y)
				min_y = screen_pos2D[j].Y;
			else if(screen_pos2D[j].Y > max_y)
				max_y = screen_pos2D[j].Y;
		}
		*line_length = max_x - min_x;
		*escape = 0;
		center_XY->x = (max_x+min_x)/2;
		center_XY->y = (max_y+min_y)/2;
		Scope tmp_scope(min_x,min_y,max_x,max_y);
		scope = tmp_scope;
	}

	int GeoLabelPosTrans::computeScreenPos(z11::MGPoint* pos, const z11::MGPoint& p1, const z11::MGPoint& p2, const int distance, const int label_height, const double radian)
	{
        int total_distance = getDistance(p1,p2);
		if(total_distance == 0)
			return -1;

		int x_dis = p2.x - p1.x;
		int y_dis = p2.y - p1.y;

        int x = p1.x + distance * x_dis / total_distance;
        int y = p1.y + distance * y_dis / total_distance;

		double half_height = label_height / 2;

		if(radian == 0)
			y -= (int)half_height;
		else
		{
			x -= int(half_height * sin(radian));
			y -= int(half_height * cos(radian));
		}

		pos->x = x;
		pos->y = y;

		return 0;
	}

	int GeoLabelPosTrans::computeScreenPos(z11::MGPoint* leftTop, z11::MGPoint* from, z11::MGPoint* to, 
							const z11::MGPoint& p1, const z11::MGPoint& p2, const int longestSpace, const z11::MGSize& labelSize, const double radian)
	{
        int total_distance = getDistance(p1,p2);
		if(total_distance == 0)
			return -1;

		int x_dis = p2.x - p1.x;
		int y_dis = p2.y - p1.y;

		int distance = (longestSpace - labelSize.width) / 2;

        int x = p1.x + distance * x_dis / total_distance;
        int y = p1.y + distance * y_dis / total_distance;

		from->x = x;
		from->y = y;

		double half_height = labelSize.height / 2;

		if(radian == 0)
		{
			y -= (int)half_height;

			to->x = from->x + labelSize.width;
			to->y = from->y;
		}
		else
		{
			x -= int(half_height * sin(radian));
			y -= int(half_height * cos(radian));

			to->x = from->x + labelSize.width * cos(radian);
			to->y = from->y - labelSize.width * sin(radian);
		}

		leftTop->x = x;
		leftTop->y = y;

		return 0;
    }
	int GeoLabelPosTrans::transGeometryLinePos(vector<z11::MGPoint>* label_pos,vector<std::string>* labels,vector<int>* escapes,
										const std::string& label,const int font_size,const z11::MGPoint* control_point,const int pos_sum,ScreenPixMap& screen_map)

	{
		if(label.empty() || pos_sum<=0)
            return -1;

		int label_sum = label.length();

		static vector<z11::MGPoint> label_control_pos;
		int longest_space;
		int longest_pos_index;

		int contral_sum = lineCutOff(&label_control_pos,&longest_space,&longest_pos_index,control_point,pos_sum,screen_map.m_screen_rect);

        if(contral_sum <= 1)
            return -1;

		double line_length = getDistance(label_control_pos[0],label_control_pos[contral_sum-1]);

		z11::MGSize size;
		getStringExtent(&size,label,font_size);

		if(size.width/2 >= line_length)
			return -1;
		
		static z11::MGPoint begin,end;
		begin = label_control_pos[longest_pos_index];
		end = label_control_pos[longest_pos_index + 1];

		if(begin.x > end.x)
		{
			begin = label_control_pos[longest_pos_index + 1];
			end = label_control_pos[longest_pos_index];
		}

		int escape;
		double radian;
		if(begin.x == end.x)
		{
			radian = M_PI / 2;
			escape = 90;
		}
		else
		{
			double tan_value = - (double)(begin.y - end.y) / (double)(begin.x - end.x);
			radian = atan(tan_value);
			escape = (int)(radian*180/M_PI);
		}

		z11::MGPoint left_top,from,to;
		if(computeScreenPos(&left_top, &from, &to, begin, end, longest_space, size, radian) <0)
			return -1;

 		//if(screen_map.testDirty(from,to,size))
 		//	return -1;
        //screen_map for what? modify by GRF 12/4/26

		if(escape<0)
			escape += 360;

		//screen_map.setDirty(from, to, size);
        //modify by GRF 12/4/26
        
		label_pos->push_back(left_top);
		labels->push_back(label);
		escapes->push_back(escape);


		return 0;
	}
	int GeoLabelPosTrans::transGeometryLinePosChs(vector<z11::MGPoint>* label_pos,vector<std::string>* labels,
										const std::string& label,const int font_size,const z11::MGPoint* control_point,const int pos_sum,ScreenPixMap& screen_map)
	{
		if(label.empty() || pos_sum<=0)
            return -1;

		static vector<z11::MGPoint> label_control_pos;
		static vector<int> label_control_pos_dis;
		int contral_sum = lineCutOff(&label_control_pos,&label_control_pos_dis,control_point,pos_sum,screen_map.m_screen_rect);

        if(contral_sum <= 1)
            return -1;

		int label_length = label.length();

        int inter_distance = (int)(label_control_pos_dis[label_control_pos_dis.size() - 1] / (label_length + 1));
		
		//if(inter_distance < 16)
		//	return -1;

        z11::MGPoint& begin_point = label_control_pos[0];
        z11::MGPoint& end_point = label_control_pos[contral_sum - 1];
		bool reverse;
		if(::abs(begin_point.x - end_point.x) > ::abs(begin_point.x - end_point.x))
        {
            if(begin_point.x < end_point.x)
                reverse = false;
            else
                reverse = true;
        }
		else if(begin_point.y < end_point.y)
            reverse = false;
        else
            reverse = true;
        
        int ctrl_point_index = 0;   

		z11::MGSize size;
		getStringExtent(&size,label.substr(0, 1),font_size);

		static vector<z11::MGPoint> strlabel_pos;
		static vector<std::string> str_label;
		strlabel_pos.resize(label_length);
		str_label.resize(label_length);
        for(int i = 0; i < label_length; i++)
        {
            int sub_string_dis = 0;

            for(sub_string_dis = (i + 1) * inter_distance; 
					(ctrl_point_index < contral_sum ) && (label_control_pos_dis[ctrl_point_index] <= sub_string_dis); 
					ctrl_point_index++);

			if( computeScreenPos(&strlabel_pos[i],label_control_pos[ctrl_point_index - 1], 
												label_control_pos[ctrl_point_index], 
												sub_string_dis - label_control_pos_dis[ctrl_point_index - 1], size.height, 0) < 0)
				continue;
            
			strlabel_pos[i].x -= size.width/2;
			strlabel_pos[i].y -= size.height/2;

 			if(screen_map.testDirty(strlabel_pos[i], size))
 				return -1;


			char tmp_char_i = label[i];
			char tmp_char = label[label_length - i - 1];

			std::string tmp_str_i ;
			std::string tmp_str;

			//lww yao gai
			tmp_str_i.assign(&tmp_char_i);
			tmp_str.assign(&tmp_char);
 			str_label[i] = reverse?(tmp_str):(tmp_str_i);
        }

        for(int i = 0; i < label_length; i++)
			screen_map.setDirty(strlabel_pos[i], size);
		label_pos->insert(label_pos->end(),strlabel_pos.begin(),strlabel_pos.end());
		labels->insert(labels->end(),str_label.begin(),str_label.end());
		return 0;

	}

	int GeoLabelPosTrans::transGeometryPointPos(vector<z11::MGPoint>* label_pos,vector<std::string>* labels,vector<int>* escapes,
										const std::string& label,const int font_size,const z11::MGPoint& control_point,ScreenPixMap& screen_map)
	{
		if(label.empty())
            return -1;

		z11::MGSize size;
		getStringExtent(&size,label,font_size);

		UserScreenRect rect;
		rect._left = control_point.x + 10;
		rect._right = control_point.x + size.width +10;
		rect._top = control_point.y - size.height/2;
		rect._bottom = control_point.y + size.height/2;
		
 		if(screen_map.testDirty(rect))
 			return -1;

		screen_map.setDirty(rect);

		static z11::MGPoint point;
		point.x = rect._left;
		point.y = rect._top;
		label_pos->push_back(point);
		labels->push_back(label);
		escapes->push_back(0);
		return 0;
	}

	int GeoLabelPosTrans::transGeometryPolygonPos(vector<z11::MGPoint>* label_pos,vector<std::string>* labels,vector<int>* escapes, 
										const std::string& label,const int font_size,const z11::MGPoint* control_point,const int pos_sum,ScreenPixMap& screen_map)
	{
		if(label.empty() || pos_sum<=0)
			return -1;

		int label_sum = label.length();
		z11::MGPoint center_XY;
		double escape;
		int line_length;
		Scope cur_scope;
		MGRect2 screen_rect;
#define SCREEN_SIZE 8192
		screen_rect.left=-(SCREEN_SIZE-256)/2;
		screen_rect.top=-(SCREEN_SIZE-256)/2;
		screen_rect.right=screen_rect.left+SCREEN_SIZE;
		screen_rect.bottom=screen_rect.top+SCREEN_SIZE;
		
		polygonCutOff(&center_XY,&escape,&line_length,control_point,pos_sum,screen_rect,cur_scope);
		//polygonCutOff(&center_XY,&escape,&line_length,control_point,pos_sum,screen_map.m_screen_rect);
		z11::MGSize size;
		getStringExtent(&size,label,font_size);

		if(size.width/2 >= line_length)
			return -1;
		UserScreenRect rect;
		rect._left = center_XY.x + 10;
		rect._right = center_XY.x + size.width +10;
		rect._top = center_XY.y - size.height/2;
		rect._bottom = center_XY.y + size.height/2;
		Scope temp_scope(rect._left,rect._bottom,rect._right,rect._top);

		if(screen_map.testDirty(rect))
			return -1;

		screen_map.setDirty(rect);
		label_pos->push_back(center_XY);
		labels->push_back(label);
		escapes->push_back(escape);

		return 0;
	}

	int GeoLabelPosTrans::removeQiDianX(vector<ScreenPos2D> &ret_pos,const vector<ScreenPos2D> &control_point, const int pos_num, const int minx, const int maxx)
	{
		if (pos_num <= 2)
			return -1;
		ret_pos.resize(0);
		int num = 2;
		ret_pos.push_back(control_point[0]);

		for (int i=1; i<pos_num-2;i++)
		{
			if (control_point[i].X == minx || control_point[i].X == maxx || (control_point[i].X - control_point[i-1].X) * (control_point[i+1].X - control_point[i].X) > 0)
			{
				ret_pos.push_back(control_point[i]);
				num++;
			}
		}
		ret_pos.push_back(control_point[pos_num-1]);

		return num;
	}
	int GeoLabelPosTrans::removeQiDianY(vector<ScreenPos2D> &ret_pos,const vector<ScreenPos2D> &control_point, const int pos_num, const int miny, const int maxy)
	{
		if (pos_num <= 2)
			return -1;
		ret_pos.resize(0);
		int num = 2;
		ret_pos.push_back(control_point[0]);

		for (int i=1; i<pos_num-2;i++)
		{
			if (control_point[i].Y == miny || control_point[i].Y == maxy || (control_point[i].Y - control_point[i-1].Y) * (control_point[i+1].Y - control_point[i].Y) > 0)
			{
				ret_pos.push_back(control_point[i]);
				num++;
			}
		}
		ret_pos.push_back(control_point[pos_num-1]);

		return num;
	}

	int GeoLabelPosTrans::fourRemoveQiDianX(vector<ScreenPos2D> &ret_pos,const vector<ScreenPos2D> &control_point,const int pos_num,const int minx,const int maxx) 
	{
  		vector<ScreenPos2D> tmp_rotation_polygon;
		vector<ScreenPos2D> tmp_control_points;
		tmp_control_points.resize(0);
		int tmp_num = -1;
		int four_tmp_num = -1;
		ScreenPos2D tmp_centroid_polygon;
		//第一次岐点删除
		tmp_num = removeQiDianX(ret_pos,control_point,pos_num,minx,maxx);
		//第二次岐点删除，逆时针旋转45度
		if (SpatialMath::calculateCentroidOfPolygon(ret_pos,tmp_num,tmp_centroid_polygon))
		{
			tmp_control_points.insert(tmp_control_points.begin(),ret_pos.begin(),ret_pos.end());
			SpatialMath::rotationPolygon(tmp_rotation_polygon,tmp_control_points,tmp_control_points.size(),M_PI/4,tmp_centroid_polygon);
			four_tmp_num = removeQiDianX(tmp_control_points,tmp_rotation_polygon,tmp_rotation_polygon.size(),minx,maxx);
			//第三次岐点删除，顺时针旋转67.5度
			if (SpatialMath::calculateCentroidOfPolygon(tmp_control_points,four_tmp_num,tmp_centroid_polygon))
			{
				SpatialMath::rotationPolygon(tmp_rotation_polygon,tmp_control_points,tmp_control_points.size(),-3*M_PI/8,tmp_centroid_polygon);
				four_tmp_num = removeQiDianX(tmp_control_points,tmp_rotation_polygon,tmp_rotation_polygon.size(),minx,maxx);
				//第四次起点删除，逆时针旋转22.5度
				if (SpatialMath::calculateCentroidOfPolygon(tmp_control_points,four_tmp_num,tmp_centroid_polygon))
				{
					SpatialMath::rotationPolygon(tmp_rotation_polygon,tmp_control_points,tmp_control_points.size(),M_PI/8,tmp_centroid_polygon);
					four_tmp_num = removeQiDianX(tmp_control_points,tmp_rotation_polygon,tmp_rotation_polygon.size(),minx,maxx);
				}
			}
		}
		if (four_tmp_num >= 3)
		{
			ret_pos.resize(0);
			ret_pos.insert(ret_pos.begin(),tmp_control_points.begin(),tmp_control_points.end());
			return four_tmp_num;
		}

		return tmp_num;
	}

	int GeoLabelPosTrans::fourRemoveQiDianY(vector<ScreenPos2D> &ret_pos,const vector<ScreenPos2D> &control_point,const int pos_num,const int miny,const int maxy)
	{
		vector<ScreenPos2D> tmp_rotation_polygon;
		vector<ScreenPos2D> tmp_control_points;
		tmp_control_points.resize(0);
		int tmp_num = -1;
		int four_tmp_num = -1;
		ScreenPos2D tmp_centroid_polygon;
		//第一次岐点删除
		tmp_num = removeQiDianY(ret_pos,control_point,pos_num,miny,maxy);
		//第二次岐点删除，逆时针旋转45度
		if (SpatialMath::calculateCentroidOfPolygon(ret_pos,tmp_num,tmp_centroid_polygon))
		{
			tmp_control_points.insert(tmp_control_points.begin(),ret_pos.begin(),ret_pos.end());
			SpatialMath::rotationPolygon(tmp_rotation_polygon,tmp_control_points,tmp_control_points.size(),M_PI/4,tmp_centroid_polygon);
			four_tmp_num = removeQiDianY(tmp_control_points,tmp_rotation_polygon,tmp_rotation_polygon.size(),miny,maxy);
			//第三次岐点删除，顺时针旋转67.5度
			if (SpatialMath::calculateCentroidOfPolygon(tmp_control_points,four_tmp_num,tmp_centroid_polygon))
			{
				SpatialMath::rotationPolygon(tmp_rotation_polygon,tmp_control_points,tmp_control_points.size(),-3*M_PI/8,tmp_centroid_polygon);
				four_tmp_num = removeQiDianY(tmp_control_points,tmp_rotation_polygon,tmp_rotation_polygon.size(),miny,maxy);
				//第四次起点删除，逆时针旋转22.5度
				if (SpatialMath::calculateCentroidOfPolygon(tmp_control_points,four_tmp_num,tmp_centroid_polygon))
				{
					SpatialMath::rotationPolygon(tmp_rotation_polygon,tmp_control_points,tmp_control_points.size(),M_PI/8,tmp_centroid_polygon);
					four_tmp_num = removeQiDianY(tmp_control_points,tmp_rotation_polygon,tmp_rotation_polygon.size(),miny,maxy);
				}
			}
		}
		if (four_tmp_num >= 3)
		{
			ret_pos.resize(0);
			ret_pos.insert(ret_pos.begin(),tmp_control_points.begin(),tmp_control_points.end());
			return four_tmp_num;
		}
		return tmp_num;
	}

}