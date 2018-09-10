#ifndef _CORE_SHARE_TRANSFORMER_H
#define _CORE_SHARE_TRANSFORMER_H

#include "../Share/Scope.h"
#include "../Types/basic_types.h"
#include "../Types/Fraction.h"
#include "../Share/ScreenPos2D.h"
#include "./Coordinate/Coordinate.h"
#include "CommonBase/MGStructure.h"
#include<vector>
#include<math.h>
using namespace std;

namespace GIS_Embedded{

#define ANTIALIAS_SCALE 2

#define COORDINATE_CLIP_BUFFER 30

struct CoordinateTable
{
	int mScreenWidth;
	int mRow;
	int mCol;

	struct RowTable
	{
		short Y;
		short XSpace;
		vector<pair<short,short> > XRowTable;
	}*mRowTable;

};

//struct CoordinateTable
//{
//	int mScreenWidth;
//	int mRow;
//	int mCol;
//	MGPoint* mTable;
//};

class Transformer
{
    
private:
	Fraction _Sx;
    Fraction _Sy;
    int _Tx;
    int _Ty;
    double _offset_angle;
	double _v_cos;
	double _v_sin;
	int _visual_state;
	int _screen_pix_width;
    int _screen_pix_height;
    Scope _screen_scope;
	int _x_center;
	int _y_center;

	Coordinate mCoordinate;
	int mWidth;
	int mHeight;

	double _rotato_x_para;
	double _rotato_y_para;
	//raster
	double m_scale_width;
	double m_scale_height;
	bool m_is_pymid;

private:
	static vector<CoordinateTable> s_CoordinateTable;
	int m_TableIndex;

public:

    Transformer()
	{   
		_offset_angle = 0;
		_visual_state = 0;

		_rotato_x_para = 0;
		_rotato_y_para = 0;

		m_is_pymid = false;
	}

	~Transformer() {}

// 	static int load3DCoordinateTable();
    
    void setTansformPara(const Scope& screen_scope,double offset_angle,int width,int height); // visual_state对应与俯视图0，斜视图1
	void setTansformPara(double scale_width,double scale_height)
	{
		m_scale_width = scale_width;
		m_scale_height = scale_height;
		m_is_pymid = true;
	}
    void getScreenScope(Scope *scope)const
	{
    	scope->setScope(_screen_scope);
	}

    int getScreenPixWidth()const
	{
    	return _screen_pix_width;
	}
    
    int getScreenPixHeight()const
	{
    	return _screen_pix_height;
	}

	double getScreenOffsetAngle()const
	{
		return _offset_angle;
	}

	int translateFromScrToMap(int screen_x,int screen_y,int_32 *map_x,int_32 *map_y);
	void translateFromScrDisToMap(int x_begin,int x_end,int y_begin,int y_end,int_32 *map_x_dis,int_32 *map_y_dis);

	int transfromMapToScr(int_32 x,int_32 y,long *screen_x,long *screen_y)const;
	int transfromMapToScr(int_32 x,int_32 y,float *screen_x,float *screen_y)const;
	int transformPos2DsToArray(vector<Pos2DEx>& pos2ds,int sum, z11::MGPoint* points)const;
//by lw for cross
	int transformPos2DsToArray(vector<Pos2DEx>& pos2ds, GIS_Embedded::Scope scope,z11::MGPoint* points)const;

	int transformPos2DsToArray(vector<Pos2D>& pos2ds,int sum, z11::MGPoint* points)const;
	int transformPos2DsToArray(vector<Pos2DEx>& pos2ds,z11::MGPoint* points)const;
// 	int transformPointsToMgPoints(MGPoint* points,int sum,vector<MGPoint>& mg_points)const;//lw
// 	int transformPos2DsToMgPoints(vector<Pos2DEx>& pos2ds,int sum,vector<MGPoint>& points)const;//lw

	int transformPixDisToDistance(int pix_dis) const
	{
		return pix_dis * _Sx.numerator / _Sx.denominator;
	}

	int transformDistanceToPix(int dis) const
	{
		return dis*_Sx.denominator/_Sx.numerator;
	}

	int transCoordinateScope(Scope* dataScope, Scope* transScope, int left,int top,int right,int bottom,const Scope& viewScope,bool isRotato);

public:
	static int clipByRect(z11::MGPoint* dest_pts, const z11::MGPoint* src_pts, int src_sum, int x1, int y1, int x2, int y2);

private:
	long calXFrom3DTable(CoordinateTable& table,int width,int height)const;

};



}

#endif