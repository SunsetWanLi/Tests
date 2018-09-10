#ifndef _RENDER_LABELRENDER_GEOLABELPOSTRANS_H
#define _RENDER_LABELRENDER_GEOLABELPOSTRANS_H

#include "../../Map/ScreenPixMap.h"
#include "Retiguide/Common/Share/Scope.h"
#include "CommonBase/MGStructure.h"
#include <map>
#include <math.h>
#include <string>

using namespace std;
using namespace z11;

namespace GIS_Embedded
{

	class GeoLabelPosTrans
	{
	private:
		static int lineCutOff(vector<z11::MGPoint>* ret,int* longest_space,int* longest_pos_index,
								const z11::MGPoint* control_point,const int pos_sum,const z11::MGRect2& rect);

		static int lineCutOff(vector<z11::MGPoint>* ret,vector<int>* ret_dis,
								const z11::MGPoint* control_point,const int pos_sum,const z11::MGRect2& rect);
		static void polygonCutOff(z11::MGPoint* center_XY,double* escape,int* line_length,
								const z11::MGPoint* control_point,const int pos_sum,const z11::MGRect2& rect,Scope& scope);
		static bool polygonCutOff(vector<z11::MGPoint>* ret,const z11::MGPoint* control_point,const int pos_sum,const z11::MGRect2& rect);
		static void polygonCutOff(z11::MGPoint* center_XY,double* escape,int* line_length,
								const z11::MGPoint* control_point,const int pos_sum,const z11::MGRect2& rect);
		static int removeQiDianX(vector<ScreenPos2D> &ret_pos,const vector<ScreenPos2D> &control_point,const int pos_num,const int minx,const int maxx);
		static int removeQiDianY(vector<ScreenPos2D> &ret_pos,const vector<ScreenPos2D> &control_point,const int pos_num,const int miny,const int maxy);
		static int fourRemoveQiDianX(vector<ScreenPos2D> &ret_pos,const vector<ScreenPos2D> &control_point,const int pos_num,const int minx,const int maxx);
		static int fourRemoveQiDianY(vector<ScreenPos2D> &ret_pos,const vector<ScreenPos2D> &control_point,const int pos_num,const int miny,const int maxy);

		static int getDistance(const z11::MGPoint& p1,const z11::MGPoint& p2)
		{
			int dis_x = (int)(p1.x - p2.x);
			int dis_y = (int)(p1.y - p2.y);
			return (int)(sqrt(dis_x*dis_x + dis_y*dis_y+0.0));
		}

		static int computeScreenPos(z11::MGPoint* pos, const z11::MGPoint& p1, const z11::MGPoint& p2, const int distance, const int label_height, const double radian);
		static int computeScreenPos(z11::MGPoint* leftTop, z11::MGPoint* from, z11::MGPoint* to, const z11::MGPoint& p1, const z11::MGPoint& p2, const int longestSpace, const z11::MGSize& labelSize, const double radian);

	public:
		static int getStringExtent(z11::MGSize* size,const std::string &label,const int font_sz);

		static int transGeometryLinePos(vector<z11::MGPoint>* label_pos,vector<std::string>* labels,vector<int>* escapes,
										const std::string& label,const int font_size,const z11::MGPoint* control_point,const int pos_sum,ScreenPixMap& screen_map);

		static int transGeometryLinePosChs(vector<z11::MGPoint>* label_pos,vector<std::string>* labels,
										const std::string& label,const int font_size,const z11::MGPoint* control_point,const int pos_sum,ScreenPixMap& screen_map);

		static int transGeometryPointPos(vector<z11::MGPoint>* label_pos,vector<std::string>* labels,vector<int>* escapes,
										const std::string& label,const int font_size,const z11::MGPoint& control_point,ScreenPixMap& screen_map);
		static int transGeometryPolygonPos(vector<z11::MGPoint>* label_pos,vector<std::string>* labels,vector<int>* escapes,
										const std::string& label,const int font_size,const z11::MGPoint* control_point,const int pos_sum,ScreenPixMap& screen_map);
		static int transGeometryPolygonPos(vector<z11::MGPoint>* label_pos,vector<wchar_t>* labels,
										const wstring& label,const int font_size,const z11::MGPoint* control_point,const int pos_sum,ScreenPixMap& screen_map);
	};

}


#endif