#ifndef __RASTERIMAGE_COMMON_TYPES_H__
#define __RASTERIMAGE_COMMON_TYPES_H__
#if defined(_WIN32) || defined(_WIN64)
#ifdef WINCE
#include <winsock2.h>
#include <windows.h>
#else
#endif
#endif
#include "basic_types.h"
#include <vector>

using namespace std ;
namespace Raster_GIS
{

// ���ζ���ID
// IMPORTANT! : if change the typedef of gm_id or go_id,
// don't forget to chage that in basic_pack.h
typedef int_64 GM_ID;
// �������ID
typedef int_64 GO_ID;

typedef bool_32 gm_bool;

typedef int_64 TIN_ID;

typedef int_64 TimeStamp;
typedef uint_32 TS_Type_Enum ;
const TS_Type_Enum TS_TYPE_GM = 0;
const TS_Type_Enum TS_TYPE_ATTR = 1;
const GO_ID MAX_GO_ID = 0x7fffffffffffffffLL;

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

// 3ά�㡣����¼�ڸ�������ϵ�µ�����ֵ��ʵ��λ����Ҫ�������ϵ���ܾ�����
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
// 2ά�㡣����¼�ڸ�������ϵ�µ�����ֵ��ʵ��λ����Ҫ�������ϵ���ܾ�����
struct Pos2D
{
	double d0, d1;

public:
	Pos2D(){}
	Pos2D(double x, double y) : d0(x), d1(y){}
	Pos2D(const Pos2D &right) : d0(right.d0), d1(right.d1){}

	bool operator == (const Pos2D &right) const
	{	return d0 == right.d0 && d1 == right.d1;	}
	 //this < does not mean 'less', it only provides some sortability which is useful for
	 //associative containers such as set, map, etc.
	bool operator < (const Pos2D &right) const
	{	return d0 < right.d0 || (d0 == right.d0 && d1 < right.d1);	}
	bool operator != (const Pos2D &right) const
	{	return !(*this == right);	}

	Pos2D & operator = (const Pos2D &right)
	{
		d0 = right.d0; d1 = right.d1 ;
		return *this;
	}

	Pos2D & operator = (const Pos3D &right)
	{
		d0 = right.d0; d1 = right.d1 ;
		return *this;
	}
};


// these below used in TIN
typedef Pos3D TIN_Point ;
/*
�����ε��������㰴����ʱ��ķ�������,
p0


p1     p2
���ı�t0,t1, t2�ֱ��Ӧp0, p1, p2����һ���Ĺ�ϵ,������

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

struct Cube3D
{
	Pos3D left_low ;
	Pos3D right_top ;
} ;

// ��ʱ�뷽����
struct Rect3D
{
	Pos3D p0 ;
	Pos3D p1 ;
	Pos3D p2 ;
	Pos3D p3 ;
};

}	// namespace Raster_GIS

#endif //IS_GIS_COMMON_TYPES_H_