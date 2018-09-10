#ifndef VECTOR3_H
#define VECTOR3_H

#include "DataStruct.h"

using z11::uint_32;

struct Vertex3;

class Vector3
{
public:
	double x, y, z;
	
public:

	static const Vector3 ZERO;			//零向量
	static const Vector3 UNIT_X;		//X正向单位向量
	static const Vector3 UNIT_Y;		//Y正向单位向量
	static const Vector3 UNIT_Z;		//Z正向单位向量
	static const Vector3 NEGATIVE_UNIT_X;	//X负向单位向量
	static const Vector3 NEGATIVE_UNIT_Y;	//Y负向单位向量
	static const Vector3 NEGATIVE_UNIT_Z;	//Z负向单位向量

	Vector3() {}
	Vector3(double a, double b, double c) : x(a), y(b), z(c) {}

	Vector3(const Vertex3& pos);

	//-------------------------------------------------------------------
	//Basic Operation
	//-------------------------------------------------------------------
	//operator =
	Vector3& operator = (const Vector3& r_vec);

	Vector3& operator = (const Vertex3& pos);
	//operator [] 
	double operator [] (uint_32 i);
	//operator [] (const)
	double operator [] (uint_32 i) const;
	//operator ==
	bool operator == (const Vector3& r_vec) const;
	//operator <
	bool operator < (const Vector3& r_vec) const;
	//operator >
	bool operator > (const Vector3& r_vec) const;
	//operator <=
	bool operator <= (const Vector3& r_vec) const;
	//operator >=
	bool operator >= (const Vector3& r_vec) const;
	//operator !=
	bool operator != (const Vector3& r_vec) const;
	//operator () (for cast)
	operator Vertex3 () const;
	//negativeCopy
	Vector3 operator - (void) const;
	//operator +
	Vector3 operator + (const Vector3& r_vec) const;
	//vertex+vector
	friend Vertex3 operator + (const Vertex3& pos, const Vector3& vec);
	//vertex-vector
	friend Vertex3 operator - (const Vertex3& pos, const Vector3& vec);
	//operator -
	Vector3 operator - (const Vector3& r_vec) const;
	//operator * 标量 (V * S)
	Vector3 operator * (double scalar) const;
	//operator * (r_vec各分量表示缩放的比例）
	Vector3 operator * (const Vector3& r_vec) const;
	//operator / 标量 （V / s)
	Vector3 operator / (double scalar) const;
	//operator / (r_vec各分量表示缩放的比例）
	Vector3 operator / (const Vector3& r_vec) const;
	//operator +=
	Vector3& operator += (const Vector3& r_vec);
	//operator -=
	Vector3& operator -= (const Vector3& r_vec);
	//operator *= 标量
	Vector3& operator *= (double scalar);
	//operator *= (r_vec各分量表示缩放的比例）
	Vector3& operator *= (const Vector3& r_vec);
    //operator /= 标量
	Vector3& operator /= (double scalar);
	//operator /= (r_vec各分量表示缩放的比例）
	Vector3& operator /= (const Vector3& r_vec);

	//点积
	double  dotProduct(const Vector3& r_vec) const;
	//叉积
	Vector3 crossProduct(const Vector3& r_vec) const;
	//向量长度
	double  length() const;
	//向量长度的平方
	double  squareLength() const;
	//向量长度是否为零
	bool  isZeroLength() const;
	//自身单位化
	void  normalize();
	//返回单位化的一个copy
	Vector3 normalizeCopy() const;
	//自身取负数
	void negative();
	//获得正交的向量(任意的）
	Vector3 perpendicularVector3() const;

};

struct DERay
{
	Vertex3 start ;
	Vector3 direction ;
};

struct DEPlane
{
	//ax + by + cz + d = 0;
	Vector3 normal;	    //平面法向矢量  normal(a, b, c)
	double d ;			//原点垂直法向距离 d

	DEPlane()
	{
		normal = Vector3::UNIT_Y;
		d = 0;
	}
} ;

struct DEViewFrustrum
{
	DEPlane planes[6] ;	//front, back, left, right, top ,bottom;
	Vertex3 corners[8];	//八个顶点(先近平面4个，后远平面4个）
						//x方向增加1，y方向增加2，z方向增加4
						//      6-------7
						//     /|	   /|
						//    / |	 /  |
						//   2----3/
						//   |  4-|-----5
						//   | /  |    /
						//   |/   |  /
						//   0----1/
						
	Vertex3 cone_vertex;	//四个平面的交点（锥体的顶点）
	double height;

	Vertex3 getFarLeftUp() const
	{
		return corners[6] ;
	};
	Vertex3 getFarRightUp() const
	{
		return corners[7] ;
	};

	Vertex3 getFarLeftDown() const
	{
		return corners[4] ;
	}

	Vertex3 getFarRightDown() const
	{
		return corners[5] ;
	};
} ;

#endif//