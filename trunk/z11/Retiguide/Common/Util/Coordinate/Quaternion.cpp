
#include "Quaternion.h"
#include "Matrix.h"
#include "Vector3.h"
#include "DataStruct.h"
#include <cmath>

const Quaternion Quaternion::ZERO(0.0, 0.0, 0.0, 0.0);
const Quaternion Quaternion::IDENTITY(0.0, 0.0, 0.0, 1.0);
const double Quaternion::EPS = 1e-06;

//--------------------------------------------------------------------
//Basic Operation
//--------------------------------------------------------------------
//operator =
Quaternion& Quaternion::operator = (const Quaternion& rkQ)
{
	if(this == &rkQ)
		return *this;
	else
	{
		w = rkQ.w;
		x = rkQ.x;
		y = rkQ.y;
		z = rkQ.z;
		return *this;
	}
}

//operator ==
bool Quaternion::operator== (const Quaternion& rhs) const
{
	return (rhs.x == x) && (rhs.y == y) &&
		(rhs.z == z) && (rhs.w == w);
}

//operator != 
bool Quaternion::operator!= (const Quaternion& rhs) const
{
	return (rhs.x != x) || (rhs.y != y) ||
		(rhs.z != z) || (rhs.w != w);
}

//operator +
Quaternion Quaternion::operator+ (const Quaternion& rkQ) const
{
	return Quaternion(x + rkQ.x, y + rkQ.y, z + rkQ.z, w + rkQ.w);
}

//operator -
Quaternion Quaternion::operator- (const Quaternion& rkQ) const
{
	return Quaternion(x - rkQ.x, y - rkQ.y, z - rkQ.z, w - rkQ.w);
}

//operator * (Q1 * Q2)

/*********************************************************** 
//Given two unit quaternions:
Q1=( x1, y1, z1, w1);
Q2=( x2, y2, z2, w2);

//A combined rotation of unit two quaternions is achieved by
Q1 * Q2 =(w1.v2 + w2.v1 + v1*v2 , w1.w2 - v1.v2)
where 
v1 = (x1, y1, z1)
v2 = (x2, y2, z2)
and both . and * are the standard vector dot and cross product.

//However an optimization can be made by rearranging the terms to produce
w = w1w2 - x1x2 - y1y2 - z1z2
x = w1x2 + x1w2 + y1z2 - z1y2
y = w1y2 + y1w2 + z1x2 - x1z2
z = w1z2 + z1w2 + x1y2 - y1x2 
************************************************************/
Quaternion Quaternion::operator* (const Quaternion& rkQ) const
{
	return Quaternion
		(
		w * rkQ.x + x * rkQ.w + y * rkQ.z - z * rkQ.y,
		w * rkQ.y + y * rkQ.w + z * rkQ.x - x * rkQ.z,
		w * rkQ.z + z * rkQ.w + x * rkQ.y - y * rkQ.x,
		w * rkQ.w - x * rkQ.x - y * rkQ.y - z * rkQ.z
		);
}

//Operator * (Q * scalar)
Quaternion Quaternion::operator* (double scalar) const
{
	return Quaternion(scalar * x, scalar * y, scalar * z, scalar * w);
}

//operator * ( Q * V)

// rotation of a vector by a quaternion
// old_v = [rkVector, 0]
// new_v = Q * old_v * Q.inverse()
// 上式是原始表达，以下代码是经过推导后优化的，效率更高
Vector3 Quaternion::operator* (const Vector3& rkVector) const
{
	Vector3 uv, uuv; 
	Vector3 q_vec(x, y, z);

	uv  = q_vec.crossProduct(rkVector); 
	uuv = q_vec.crossProduct(uv);
	uv *= (2.0 * w); 
	uuv *= 2.0; 

	return rkVector + uv + uuv;
}

//Operator * (scalar * Q)
Quaternion operator* (double scalar, const Quaternion &rkQ)
{
	return Quaternion(scalar * rkQ.x, scalar * rkQ.y, scalar * rkQ.z, scalar * rkQ.w);
}

//negative
Quaternion Quaternion::operator- () const
{
	return Quaternion(-x, -y, -z , -w);
}

//magnitude sqrt(Q * Q.inverse) 共轭
double  Quaternion::magnitude() const
{
	return sqrt(x*x + y*y + z*z + w*w);
}

//normalizeCopy
Quaternion Quaternion::normalizeCopy() const
{
	double len = magnitude();

	return Quaternion(x / len, y / len, z / len, w / len);
}

//normalize
void Quaternion::normalize()
{
	double len = magnitude();

	x /= len;
	y /= len;
	z /= len;
	w /= len;
}

//inverse Q
Quaternion Quaternion::inverse () const
{
	double len = magnitude();

	if(len > EPS)
		return Quaternion(-x / len , -y / len, -z / len, w / len);
	else
		return ZERO;
}

//Q->MATRIx4(注意转化前必须先单位化）
void Quaternion::toRotationMatrix(Matrix4& mat) const
{
	double xx, yy, zz;
	double xy, xz, xw;
	double yz, yw, zw;
	
	
	xx  = x * x;
	xy  = x * y;
	xz  = x * z;
	xw  = x * w;
	yy  = y * y;
	yz  = y * z;
	yw  = y * w;
	zz  = z * z;
	zw  = z * w;

	mat[0][0] = 1 - 2 * ( yy + zz );
	mat[1][0] =     2 * ( xy + zw );
	mat[2][0] =     2 * ( xz - yw );
	mat[0][1] =		2 * ( xy - zw );
	mat[1][1] =	1 - 2 * ( xx + zz );
	mat[2][1] =		2 * ( yz + xw );
	mat[0][2] =		2 * ( xz + yw );
	mat[1][2] =		2 * ( yz - xw );
	mat[2][2] = 1 - 2 * ( xx + yy );
	mat[0][3] = mat[1][3] = mat[2][3] = mat[3][0] =	mat[3][1] =	mat[3][2] = 0.0;
	mat[3][3] = 1.0;
}

//Quaternion->angle&axis
void Quaternion::toAngleAxis (double& angle, Vector3& axis) const
{
	double len = magnitude();

	if(len > EPS)
	{	
		angle = 2.0 * acos(w);
		axis.x = x / len;
		axis.y = y / len;
		axis.z = z / len;
	}
	else
	{
		angle = 0.0;
		axis.x = 1.0;
		axis.y = 0.0;
		axis.z = 0.0;
	}
}

//Angle&Axis->Quaternion(axis必须是单位化过的)
void Quaternion::fromAngleAxis(double angle, const Vector3& axis)
{
	double radian;
	Vector3 norm_axis;

	norm_axis = axis.normalizeCopy();
	radian = 3.141592654*(angle/180.0) / 2.0;
	w = cos(radian);
	x = sin(radian) * norm_axis.x;
	y = sin(radian) * norm_axis.y;
	z = sin(radian) * norm_axis.z;

}