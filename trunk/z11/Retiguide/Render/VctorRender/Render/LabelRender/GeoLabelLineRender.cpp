#include "GeoLabelLineRender.h"
#include "Retiguide/Common/Util/Algorithmic.h"
#include "Retiguide/Common/Util/PureMath.h"
#include "Retiguide/Common/String_Tools.h"
#include "Retiguide/Analysis/TransformRoad.h"
#include "MapKit/Embedded/BasicType/Line.h"
#include "System/Shared/Utility.h"
#include "CommonBase/MGWString.h"
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288 
#endif
using namespace z11;
using namespace Embedded;

namespace GIS_Embedded
{

	vector<int> GeoLabelLineRender::_distance;

	vector<ScreenPos2D> GeoLabelLineRender::_label_ctrl_points;

	int GeoLabelLineRender::_longest_sub_segment_index;
	double GeoLabelLineRender::_longest_sub_segment_length;

	vector<UserScreenRect> GeoLabelLineRender::_rect_queue;
	vector<ScreenPos2D> GeoLabelLineRender::_pos_queue;
	vector<std::string> GeoLabelLineRender::_label_char_queue;
	vector<int> GeoLabelLineRender::_label_escape_queue;

	void GeoLabelLineRender::render(StyleRender& mg, ScreenPixMap& screen_pix_map)
    {
		z11::MGWString label = _geo->getName();
 			if(label.empty())
             return;
 
 		if(label[0]<127)
 		{
 			renderEnLabel(mg,screen_pix_map);
 			return;
 		}
 		renderChineseLabel(mg, screen_pix_map);
	}

	void GeoLabelLineRender::renderChineseLabel(StyleRender& mg, ScreenPixMap& screen_pix_map)
	{
 		z11::MGWString wlabel = _geo->getName();

		std::string label = Utility::wstr2ustr(wlabel);
 
 		_rect_queue.clear();
 		_pos_queue.clear();
 		_label_char_queue.clear();
 		_label_escape_queue.clear();
 		
 		int label_length = label.length();
         getLabelCtrlPoints();
         if(_label_ctrl_points.size() == 0)
             return;
         computeCtrlPointsDistance();
 
 		//inter_dis瑕佹牴鎹腑鑻辨枃鍙樺寲
         int inter_distance = (int)(_distance[_distance.size() - 1] / (label_length + 1));
 		
 		if(inter_distance < 16)
 				return;
 
         int ctrl_points_sum = _label_ctrl_points.size();
 
         ScreenPos2D begin_point = _label_ctrl_points[0];
         ScreenPos2D end_point = _label_ctrl_points[ctrl_points_sum - 1];
         if(abs(begin_point.X - end_point.X) > abs(begin_point.Y - end_point.Y))
         {
             if(begin_point.X < end_point.X)
                 _reverse = false;
             else
                 _reverse = true;
         } else
         if(begin_point.Y < end_point.Y)
             _reverse = false;
         else
             _reverse = true;
         
         int ctrl_point_index = 0;   
 
 		int label_width, label_height;
 		mg.getStringExtent(label.substr(0, 1), &label_width, &label_height);
 
         for(int i = 0; i < label_length; i++)
         {
             int sub_string_dis = 0;
 			int aasum = _distance.size();
 
             for(sub_string_dis = (i + 1) * inter_distance; 
 					(ctrl_point_index < ctrl_points_sum ) && (_distance[ctrl_point_index] <= sub_string_dis); 
 					ctrl_point_index++);
 			
 			ScreenPos2D pos;
 			computeScreenPos(_label_ctrl_points[ctrl_point_index - 1], 
 												_label_ctrl_points[ctrl_point_index], 
 												sub_string_dis - _distance[ctrl_point_index - 1], label_height, 0, 
 												&pos);
             
 			//if(screen_pix_map.testDirty(pos, 0, label_width, label_height))
 				return;
 			//screen_pix_map.setDirty(pos, 0, label_width, label_height);
 
 			UserScreenRect rect;
 			rect._left = pos.X;
 			rect._right = label_width + pos.X;
 			rect._top = pos.Y;
 			rect._bottom = label_height + pos.Y;
 
 			_rect_queue.push_back(rect);
 			_pos_queue.push_back(pos);
 			_label_escape_queue.push_back(0);

			std::string tmp_char;
 			if(!_reverse)
				tmp_char = label[i];
 			else	// if(conflictTest(mg, screen_pix_map, pos, label.substr(label_length - i - 1, 1)))
				tmp_char = label[label_length - i - 1];

			_label_char_queue.push_back(tmp_char); 
         }
 
         drawStringByCharacter(mg, screen_pix_map);
    }

	void GeoLabelLineRender::renderEnLabel(StyleRender &mg,ScreenPixMap &screen_pix_map)
	{
		
		z11::MGWString wlabel = _geo->getName();

		std::string label = Utility::wstr2ustr(wlabel);
 
 
 		int label_sum = label.length();

 		getLabelCtrlPoints();
 		int contral_sum = _label_ctrl_points.size();
         if(contral_sum <= 1)
             return;
 		ScreenPos2D &pos_begin = _label_ctrl_points[0];
 		ScreenPos2D &pos_end = _label_ctrl_points[contral_sum-1];
 		int x_length = pos_begin.X - pos_end.X;
 		int y_length = pos_begin.Y - pos_end.Y;
 		double line_length = sqrt(0.0+x_length * x_length + y_length * y_length);	//鍙互鑰冭檻浣跨敤鏈€闀垮瓙绾挎闀?
 		int label_width, label_height;
 		mg.getStringExtent(label, &label_width, &label_height);
 
 		if( label_width >= line_length)
 			return;
 		
 		ScreenPos2D &tmp_begin = _label_ctrl_points[_longest_sub_segment_index];
 		ScreenPos2D &tmp_end = _label_ctrl_points[_longest_sub_segment_index + 1];
 
 		ScreenPos2D *sub_pos_begin;
 		ScreenPos2D *sub_pos_end;
 		if(tmp_begin.X < tmp_end.X)	//淇濊瘉鍐欏嚭鐨勫瓧绗︿覆浠庡乏鍒板彸
 		{
 			sub_pos_begin = &_label_ctrl_points[_longest_sub_segment_index];
 			sub_pos_end = &_label_ctrl_points[_longest_sub_segment_index + 1];
 		}
 		else
 		{
 			sub_pos_begin = &_label_ctrl_points[_longest_sub_segment_index + 1];
 			sub_pos_end = &_label_ctrl_points[_longest_sub_segment_index];
 		}
 
 		int angle;
 		double radian;
 		if((*sub_pos_begin).X == (*sub_pos_end).X)
 		{
 			radian = M_PI / 2;
 			angle = 90;
 		}
 		else
 		{
 			double tan_value = - (double)((*sub_pos_begin).Y - (*sub_pos_end).Y) / (double)((*sub_pos_begin).X - (*sub_pos_end).X);
 			radian = atan(tan_value);
 			angle = (int)(radian*180/M_PI);
 		}
 
 		int label_start_length = (int)((_longest_sub_segment_length - label_width) / 2);
 		
 		ScreenPos2D pos;
 		computeScreenPos(*sub_pos_begin, *sub_pos_end, label_start_length, label_height, radian, &pos);
 
         //if(screen_pix_map.testDirty(pos, radian, label_width, label_height)/*conflictTest(mg, screen_pix_map, pos, label)*/)
             return;
 
 		if(angle<0)
 			angle += 360;
 
 		//screen_pix_map.setDirty(pos, radian, label_width, label_height);
 		mg.drawString(pos.X, pos.Y, angle, label);

	}


	void GeoLabelLineRender::getLabelCtrlPoints()
    {
		Embedded::Line* line = dynamic_cast< Embedded::Line*> (_geo);
		std::vector<Embedded::MapPos2D > points;
		if(line)
		{
			points.clear();
			line->getPoints(&points);
		}
		static vector<Pos2DEx> control_pos2d(points.size());//points.size()
		control_pos2d.clear();

		for (uint_32 k=0; k<points.size(); k++)
		{
			control_pos2d[k].d0 = points[k].d0;
			control_pos2d[k].d1 = points[k].d1;
		}
 
  		static vector<Pos2DEx> control_pos2d_filtered;
  		control_pos2d_filtered.clear();
  		int filter_width = _transformer->transformPixDisToDistance(5);	
  		if (filter_width == 0)
  			filter_width = 1;
  		TransformRoad::filtrationPointDouglas(control_pos2d, filter_width, control_pos2d_filtered);//loujt,09-01-14
 
 		int pos2d_sum = control_pos2d_filtered.size();
 		static int pos_size = 20;
 		static z11::MGPoint* pos_array = new z11::MGPoint[pos_size];
 		if(pos2d_sum > pos_size)
 		{
 			delete [] pos_array;
 			pos_array = new z11::MGPoint[pos2d_sum];
 			pos_size = pos2d_sum;
 		}
 		
 		_transformer->transformPos2DsToArray(control_pos2d_filtered,pos_array);
 
         ScreenPos2D last_p;
 
 		_label_ctrl_points.clear();
 
 		//todo: pos2ds杩欐牱鐢ㄤ細鏈夊緢澶歯ew鍔ㄤ�?瑕佷笉瑕佸垏鍓插彲鍐嶈€冭檻 鏄笉鏄墍鏈夌殑鐐归兘瑕佺畻杩?鏄笉鏄簲璇ュ湪杩炵画涓€娈靛彲鍒囧壊鐨勫垏鍓蹭簡灏卞彲浠?
         UserScreenRect screen_rect(0, 0, _transformer->getScreenPixWidth()-1, _transformer->getScreenPixHeight() - 1);
 
 		static vector<ScreenPos2D> pos2ds;
 		int i;
 		for(i = 0; i < pos2d_sum - 1; i ++)
 		{
 			pos2ds.clear();
 			Algorithmic::lineCutOff(pos_array[i].x, pos_array[i].y, pos_array[i+1].x, pos_array[i+1].y, screen_rect,&pos2ds);
 			if(!pos2ds.empty())
 			{
 				ScreenPos2D &pos0 = pos2ds[0];
 				ScreenPos2D &pos1 = pos2ds[1];
 
 				_label_ctrl_points.push_back(pos0);
 				_label_ctrl_points.push_back(pos1);
 				
 				_longest_sub_segment_length = pos0.distance(pos1);
 				_longest_sub_segment_index = _label_ctrl_points.size() - 2;
 
 				last_p = pos2ds[1];
 				break;
 			}
 		}
 		for(i ++; i< pos2d_sum - 1; i ++)
 		{
 			pos2ds.clear();
 			Algorithmic::lineCutOff(pos_array[i].x, pos_array[i].y, pos_array[i+1].x, pos_array[i+1].y, screen_rect,&pos2ds);
 			if(pos2ds.empty())
 				break;
 			ScreenPos2D &p0 = pos2ds[0];
 			ScreenPos2D &p1 = pos2ds[1];
 
 			if(!p0.equal(last_p))
 				break;
 
 			_label_ctrl_points.push_back(p1);
 
 			int tmp_dis = p0.distance(p1);
 			if(tmp_dis > _longest_sub_segment_length)
 			{
 				_longest_sub_segment_length = tmp_dis;
 				_longest_sub_segment_index = _label_ctrl_points.size() - 2;
 			}
 			last_p = p1;
 		}

    }


	void GeoLabelLineRender::computeCtrlPointsDistance()
    {
		_distance.clear();
		_distance.push_back(0);
        int sum = _label_ctrl_points.size();
        for(int i = 1; i < sum; i++)
        {
            ScreenPos2D p1 = _label_ctrl_points[i - 1];
            ScreenPos2D p2 = _label_ctrl_points[i];
			_distance.push_back(p1.distance(p2) + _distance[i - 1]);
        }

    }


	void GeoLabelLineRender::computeScreenPos(ScreenPos2D& p1, ScreenPos2D& p2, int distance, int label_height, double radian, ScreenPos2D *pos)
    {
        int total_distance = p1.distance(p2);
		if(total_distance == 0)
		{
			pos->X = p1.X;
			pos->Y = p1.Y;
		}

		int x_dis = p2.X - p1.X;
		int y_dis = p2.Y - p1.Y;

        int x = p1.X + distance * x_dis / total_distance;
        int y = p1.Y + distance * y_dis / total_distance;

		double half_height = label_height / 2;

		if(radian == 0)
			y -= half_height;
		else
		{
			x -= int(half_height * sin(radian));
			y -= int(half_height * cos(radian));
		}

		pos->X = x;
		pos->Y = y;
    }

	void GeoLabelLineRender::drawStringByCharacter( StyleRender& mg, ScreenPixMap& screen_pix_map )
	{
	    for(unsigned int i = 0, size = _label_char_queue.size(); i < size; i++)
	    {
	        UserScreenRect &rect = _rect_queue[i];
	        ScreenPos2D &pos = _pos_queue[i];
			std::string label_char = _label_char_queue[i];
			int escape = _label_escape_queue[i];
	
	        screen_pix_map.setDirty(rect);
	        mg.drawCharacter(pos.X, pos.Y, escape, label_char);
	
	    }
	}

}