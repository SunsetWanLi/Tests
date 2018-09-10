// common_types.h
// zzj, Jan. 19, 2003
// define the common data types

#ifndef GIS_Embeded_COMMON_TYPES_H_
#define GIS_Embeded_COMMON_TYPES_H_

#include "../Types/basic_types.h"
#include "basic_geo_type.h"
//#include "CommonBase/MGStructure.h"
#include <math.h>
#include <vector>

using namespace std ;
namespace GIS_Embedded
{

// 几何对象ID
// IMPORTANT! : if change the typedef of gm_id or go_id,
// don't forget to chage that in basic_pack.h
typedef int_64 GM_ID;
// 地理对象ID
typedef int_64 GO_ID;

typedef bool_32 gm_bool;

typedef int_64 TIN_ID;

typedef int_64 TimeStamp;
typedef uint_32 TS_Type_Enum ;
const TS_Type_Enum TS_TYPE_GM = 0;
const TS_Type_Enum TS_TYPE_ATTR = 1;
const GO_ID MAX_GO_ID = 0x7fffffffffffffffULL;

struct GO_ID_ARRAY
{
	GO_ID first,last;
	GO_ID_ARRAY(){first = 0;last = 0;}
	GO_ID_ARRAY(GO_ID f,GO_ID l):first(f),last(l){}
	GO_ID_ARRAY(const GO_ID_ARRAY &right):first(right.first),last(right.last){}

	bool operator == (const GO_ID_ARRAY &right) const
	{	return first == right.first && last == right.last;	}
	bool operator != (const GO_ID_ARRAY &right) const
	{	return !(*this == right);	}

	GO_ID_ARRAY & operator = (const GO_ID_ARRAY &right)
	{
		first = right.first;
		last = right.last;
		return *this;
	}
};

// 3维点。仅记录在给定坐标系下的坐标值。实际位置需要结合坐标系才能决定。
struct Pos3D
{
	double d0, d1, d2;

	Pos3D(){}
	Pos3D(double x, double y, double z) : d0(x), d1(y), d2(z){}
	Pos3D(const Pos3D &right) : d0(right.d0), d1(right.d1), d2(right.d2){}

	bool operator == (const Pos3D &right) const
	{	return d0 == right.d0 && d1 == right.d1 && d2 == right.d2;	}
	bool operator < (const Pos3D &right) const
	{	
		return	d0 < right.d0 || (d0 == right.d0 &&
				(d1 < right.d1 || (d1 == right.d1 && d2 == right.d2)));	
	}

	bool operator != (const Pos3D &right) const
	{	return !(*this == right);	}

	Pos3D & operator = (const Pos3D &right)
	{
		d0 = right.d0; d1 = right.d1 ; d2 = right.d2 ;
		return *this;
	}
};
struct SPosition
{
	int_32 x;
	int_32 y;
};
// 2维点。仅记录在给定坐标系下的坐标值。实际位置需要结合坐标系才能决定。
// struct Pos2D
// {
// 	double d0, d1;
// 
// public:
// 	Pos2D(){}
// 	Pos2D(double x, double y) : d0(x), d1(y){}
// 	Pos2D(const Pos2D &right) : d0(right.d0), d1(right.d1){}
// 
// 	bool operator == (const Pos2D &right) const
// 	{	return d0 == right.d0 && d1 == right.d1;	}
// 	 //this < does not mean 'less', it only provides some sortability which is useful for
// 	 //associative containers such as set, map, etc.
// 	bool operator < (const Pos2D &right) const
// 	{	return d0 < right.d0 || d0 == right.d0 && d1 < right.d1;	}
// 	bool operator != (const Pos2D &right) const
// 	{	return !(*this == right);	}
// 
// 	Pos2D & operator = (const Pos2D &right)
// 	{
// 		d0 = right.d0; d1 = right.d1 ;
// 		return *this;
// 	}
// 
// 	Pos2D & operator = (const Pos3D &right)
// 	{
// 		d0 = right.d0; d1 = right.d1 ;
// 		return *this;
// 	}
// };


// these below used in TIN
typedef Pos3D TIN_Point ;
/*
三角形的三个顶点按照逆时针的方向排列,
p0


p1     p2
它的边t0,t1, t2分别对应p0, p1, p2具有一定的关系,点边相对

*/
struct TIN_Triangle
{
	TIN_ID p[3];
	TIN_ID t[3];

	bool operator == ( const TIN_Triangle & tr ) const
	{ return p[0] == tr.p[0] && p[1] == tr.p[1] && p[2] == tr.p[2]
	&& t[0] == tr.t[0] && t[1] == tr.t[1] && t[2] == tr.t[2] ; }

	TIN_Triangle(TIN_ID a0 = -1, TIN_ID a1 = -1, TIN_ID a2 = -1,
				TIN_ID b0 = -1, TIN_ID b1 = -1, TIN_ID b2 = -1) 
	{
		p[0] = a0; p[1] = a1; p[2] = a2;
		t[0] = b0; t[1] = b1; t[2] = b2;
	}
};

struct ContourLineNode
{
	//form a loop
	bool_32 is_closed;
	std::vector<Pos3D> point_array;
};

struct ContourLine
{
	// the structure of the contour line	
	double elevation;
	std::vector<ContourLineNode> contourline_array;

};


struct Measure
{
	//	how to?
};

struct Length
{
	Measure measure;
	double value;
};

typedef Length Distance;

struct Area
{
	Measure measure;
	double value;	
};

struct Volume
{
	Measure measure;
	double value;
};

// type to discribe a GM object
struct GM_ObjDiscription
{
	GM_ID id;
//	GM_Type type
};

struct Rect2D
{
	Pos2D left_low;
	Pos2D right_top;
};
typedef Rect2D GM_Envelope;

struct Rect2DEx
{
	Pos2DEx left_low;
	Pos2DEx right_top;
};

struct Cube3D
{
	Pos3D left_low ;
	Pos3D right_top ;
} ;

// 逆时针方向环绕
struct Rect3D
{
	Pos3D p0 ;
	Pos3D p1 ;
	Pos3D p2 ;
	Pos3D p3 ;
};

}	// namespace GIS_Embedded

#endif //GIS_Embeded_COMMON_TYPES_H_