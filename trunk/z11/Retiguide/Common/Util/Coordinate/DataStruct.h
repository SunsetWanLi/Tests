#ifndef DATASTRUCT_H
#define DATASTRUCT_H

#include "../../Types/basic_types.h"

struct Vertex3;

struct VertexShort
{
	short x, y ;

	VertexShort(short v0 = 0, short v1 = 0): x(v0), y(v1) {}
} ;

struct Vertex2
{
	float x, y ;

	Vertex2(float v0 = 0.0, float v1 = 0.0): x(v0), y(v1) {}
} ;

struct Vertex3
{
	float x, y, z ;

	Vertex3()/*:x(0), y(0), z(0)*/
	{};
	Vertex3(float v0 , float v1 , float v2  )
		: x(v0), y(v1), z(v2) { }

	Vertex3(const Vertex2& v):x(v.x), y(v.y), z(0){}

	bool operator == (const Vertex3& p) const
	{
		return ((x == p.x) && (y == p.y) && (z == p.z));
	}

	Vertex3 & operator = (const Vertex3& p)
	{
		x = p.x , y = p.y , z = p.z ;
		return *this ;
	}
	// can able to insert to stl container
	bool operator < ( const Vertex3 & p ) const
	{
		return x < p.x || y < p.y || z < p.z ;
	}

	operator Vertex2() const
	{
		return Vertex2(x, y);
	}
} ;



#endif//