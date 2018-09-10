#pragma once
#ifndef GIS_BASIC_NW_TYPE_H_
#define GIS_BASIC_NW_TYPE_H_
#include <functional>

#include "BuildInTypes.h"

namespace GIS_Embedded
{

#define UNSIGNED_INIT_VALUE -1
    
#define DOUBLE_MAX 1.7976931348623157E+308

#ifdef DOUBLE_ZERO
#undef DOUBLE_ZERO
#endif
#define DOUBLE_ZERO /*0.000000001*/0.0000001	//changed by wdc for cross analyse

enum RepositoryType
{
	REPO_FILES,
	REPO_DATABASE
};

enum RasterType
{
	RT_GRID,
	RT_BMP,
	RT_JPG,
	RT_TIFF
};

//the first bit is "0"->node
//the first bit is "1"->edge
//#define MIN_NODE_ID (0)
//#define MIN_EDGE_ID (0x8000000000000000)
//extern const NO_ID MIN_NODE_ID;
//extern const NO_ID MIN_EDGE_ID;

// Pos2D should needn't 
struct Pos2D
{
	double d0, d1;

public:
	Pos2D():d0(0.0),d1(0.0){}
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
};

struct Pos2DEx
{
	int_32 d0, d1;

public:
	Pos2DEx():d0(0),d1(0){}
	Pos2DEx(int_32 x, int_32 y) : d0(x), d1(y){}
	Pos2DEx(const Pos2DEx &right) : d0(right.d0), d1(right.d1){}

	bool operator == (const Pos2DEx &right) const
	{	return d0 == right.d0 && d1 == right.d1;	}
	//this < does not mean 'less', it only provides some sortability which is useful for
	//associative containers such as set, map, etc.
	bool operator < (const Pos2DEx &right) const
	{	return d0 < right.d0 || (d0 == right.d0 && d1 < right.d1);	}
	bool operator != (const Pos2DEx &right) const
	{	return !(*this == right);	}

	Pos2DEx & operator = (const Pos2DEx &right)
	{
		d0 = right.d0; d1 = right.d1 ;
		return *this;
	}
	bool isZero()
	{
		return d0==0&&d1==0;
	}
};

#ifndef NULL
#define NULL 0
#endif

template<class T>
struct DeletePointerFunctor
{
	void operator()(const T * pT)const
	{
		delete pT,pT = NULL;
	}
};

template<class T>
struct PointerLess
	: public std::binary_function<const T *, const T *, bool>
{
public:
	bool operator()(const T* lval, const T* rval)const
	{
		return *lval < *rval;
	}
};
}// end GIS_Embedded
#endif