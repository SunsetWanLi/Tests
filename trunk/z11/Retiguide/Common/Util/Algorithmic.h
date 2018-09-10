#ifndef _CORE_SHARE_ALGORITHMIC_H
#define _CORE_SHARE_ALGORITHMIC_H

#include<vector>
#include "../Share/UserScreenRect.h"
#include "../Share/ScreenPos2D.h"
#include "../Share/Scope.h"
#include "../Types/basic_types.h"
#include "Transformer.h"


using namespace std;

namespace GIS_Embedded{
class Algorithmic
{

public:
	static const int LEFT = 1;
    static const int RIGHT = 2;
    static const int BOTTOM = 4;
    static const int TOP = 8;

public:
	Algorithmic(){}

    static bool lineCutOff(double x1, double y1, double x2, double y2, const Scope& select_scope);

    static int encode(int x, int y, UserScreenRect& screen_scope);

	static unsigned char encode(int x,int y,const z11::MGRect2& screen);

    static void lineCutOff(int x1, int y1, int x2, int y2, UserScreenRect& screen_scope,vector<ScreenPos2D>* screen_pos2d);

	//loujt 08-04-10 模仿上面两个接口，处理投影坐标
	static int encode(double x,double y,const Scope &screen_scope);
	//如两个点都在scope中，则返回true
	static bool lineCutOff(double x1,double y1,double x2,double y2,const Scope &screen_scope,vector<Pos2D> *screen_pos2d);

    static void polygonCutOff(int x1, int y1, int x2, int y2, UserScreenRect& screen_scope,vector<ScreenPos2D>* screen_pos2d);

    static int encodeY(double value, double y);

    static int encodeX(double value, double x);

    static int lineCutByXline(Pos2D& pos1, Pos2D& pos2, double y,Pos2D *pos);

    static int lineCutByYline(Pos2D& pos1, Pos2D& pos2, double x,Pos2D* pos);

    static int computeLineIntersect(Pos2D& p1, Pos2D& p2, Pos2D& p3, Pos2D& p4,Pos2D *pos);

	static int transAngleToClock(double angleInDegree);


	////////////////////////////////////////
	static bool lineCutOff(int x1, int y1, int x2, int y2, const Scope& select_scope);

	//loujt 08-04-10 模仿上面两个接口，处理投影坐标
	static int encode(int x,int y,const Scope &screen_scope);
	//如两个点都在scope中，则返回true
	static bool lineCutOff(int x1,int y1,int x2,int y2,const Scope &screen_scope,vector<Pos2DEx> *screen_pos2d);

	static int encodeY(int value, int y);

	static int encodeX(int value, int x);

	static int lineCutByXline(Pos2DEx& pos1, Pos2DEx& pos2, int y,Pos2DEx *pos);

	static int lineCutByYline(Pos2DEx& pos1, Pos2DEx& pos2, int x,Pos2DEx* pos);

	static int computeLineIntersect(Pos2DEx& p1, Pos2DEx& p2, Pos2DEx& p3, Pos2DEx& p4,Pos2DEx *pos);

	/////////////////////////////////////////////////////

	//直接修改p1,p2的值，如与rect相交返回true，反之返回false
	static bool lineCutOffByRect(z11::MGPoint* ret1, z11::MGPoint* ret2, int x1, int y1, int x2, int y2, const z11::MGRect2 & rect);

	//the scale is 2, and 32-bits bitmap
	static int hBitMapAntiAliasing(int scren_width,int scren_height,byte_8* src,byte_8* dest);

};
}
#endif