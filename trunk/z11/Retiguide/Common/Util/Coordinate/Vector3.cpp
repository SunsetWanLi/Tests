
#include "Vector3.h"
#include <cmath>

const Vector3 Vector3::ZERO(0.0, 0.0, 0.0);
const Vector3 Vector3::UNIT_X(1.0, 0.0, 0.0);
const Vector3 Vector3::UNIT_Y(0.0, 1.0, 0.0);
const Vector3 Vector3::UNIT_Z(0.0, 0.0, 1.0);
const Vector3 Vector3::NEGATIVE_UNIT_X(-1.0, 0.0, 0.0);
const Vector3 Vector3::NEGATIVE_UNIT_Y(0.0, -1.0, 0.0);
const Vector3 Vector3::NEGATIVE_UNIT_Z(0.0, 0.0, -1.0);

Vector3::Vector3(const Vertex3& pos) : x(pos.x), y(pos.y), z(pos.z) {}
//-------------------------------------------------------------------
//Basic Operation
//-------------------------------------------------------------------
//operator =
Vector3& Vector3::operator = (const Vector3& r_vec)
{
	if(this == &r_vec)
		return *this;
	else
	{
		this->x = r_vec.x;
		this->y = r_vec.y;
		this->z = r_vec.z;

		return *this;
	}
}

Vector3&  Vector3::operator = (const Vertex3& pos)
{
	this->x = pos.x;
	this->y = pos.y;
	this->z = pos.z;
	
	return *this;
}

//operator [] 
double Vector3::operator [] (uint_32 i)
{
	//assert( i < 3 );
	return *(&x+i);
}

//operator [] (const)
double Vector3::operator [] (uint_32 i) const
{
	//assert( i < 3 );
	return *(&x+i);
}

//operator () (for cast)
Vector3::operator Vertex3 () const
{
	return Vertex3(x, y, z);
}

//operator ==
bool Vector3::operator == (const Vector3& r_vec) const
{
	return (x == r_vec.x && y == r_vec.y && z == r_vec.z);
}

//operator <
bool Vector3::operator < ( const Vector3& rhs ) const
{
	if( x < rhs.x && y < rhs.y && z < rhs.z )
		return true;
	return false;
}

//operator >
bool Vector3::operator > ( const Vector3& rhs ) const
{
	if( x > rhs.x && y > rhs.y && z > rhs.z )
		return true;
	return false;
}

//operator <=
bool Vector3::operator <= ( const Vector3& rhs ) const
{
	if( x <= rhs.x && y <= rhs.y && z <= rhs.z )
		return true;
	return false;
}

//operator >=
bool Vector3::operator >= ( const Vector3& rhs ) const
{
	if( x >= rhs.x && y >= rhs.y && z >= rhs.z )
		return true;
	return false;
}

//operator !=
bool Vector3::operator != (const Vector3& r_vec) const
{
	return (x != r_vec.x || y != r_vec.y || z != r_vec.z);
}

//negativeCopy
Vector3 Vector3::operator - (void) const
{
	return Vector3(-x, -y, -z);
}

//operator +
Vector3 Vector3::operator + (const Vector3& r_vec) const
{
	return Vector3(x + r_vec.x, y + r_vec.y, z + r_vec.z);
}

//operator -
Vector3 Vector3::operator - (const Vector3& r_vec) const
{
	return Vector3(x - r_vec.x, y - r_vec.y, z - r_vec.z);
}

//operator *
Vector3 Vector3::operator * (double scalar) const
{
	return Vector3(x * scalar, y * scalar, z * scalar);
}

//operator *
Vector3 Vector3::operator * (const Vector3& r_vec) const
{
	return Vector3(x * r_vec.x, y * r_vec.y, z * r_vec.z);
}

//operator /
Vector3 Vector3::operator / (double scalar) const
{
	return Vector3(x / scalar, y / scalar, z / scalar);
}

//operator /
Vector3 Vector3::operator / (const Vector3& r_vec) const
{
	return Vector3(x / r_vec.x, y / r_vec.y, z / r_vec.z);
}

//operator +=
Vector3& Vector3::operator += (const Vector3& r_vec)
{
	x += r_vec.x;
	y += r_vec.y;
	z += r_vec.z;

	return *this;
}

//operator -=
Vector3& Vector3::operator -= (const Vector3& r_vec)
{
	x -= r_vec.x;
	y -= r_vec.y;
	z -= r_vec.z;

	return *this;
}

//operator *=
Vector3& Vector3::operator *= (double scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;

	return *this;
}

//operator *= （缩放比例不一致）
Vector3& Vector3::operator *= (const Vector3& r_vec)
{
	x *= r_vec.x;
	y *= r_vec.y;
	z *= r_vec.z;

	return *this;
}

//operator /=
Vector3& Vector3::operator /= (double scalar)
{
	x /= scalar;
	y /= scalar;
	z /= scalar;

	return *this;
}

//operator /=  （缩放比例不一致）
Vector3& Vector3::operator /= (const Vector3& r_vec)
{
	x /= r_vec.x;
	y /= r_vec.y;
	z /= r_vec.z;

	return *this;
}

//点积
double  Vector3::dotProduct(const Vector3& r_vec) const
{
	return (x * r_vec.x + y * r_vec.y + z * r_vec.z );
}

//叉积
Vector3 Vector3::crossProduct(const Vector3& r_vec) const
{
	return Vector3
				(
				y * r_vec.z - z * r_vec.y,
				z * r_vec.x - x * r_vec.z,
				x * r_vec.y - y * r_vec.x
				);
}

//向量长度
double  Vector3::length() const
{
	return sqrt(squareLength());
}

//向量长度的平方
double  Vector3::squareLength() const
{
	return (x * x + y * y + z * z);
}

//向量长度是否为零
bool  Vector3::isZeroLength() const
{
	return (squareLength() < (1e-16 * 1e-16));
}

//自身单位化
void  Vector3::normalize()
{
	double len = length();

	if(len > 1e-16)
		*this /= len;
}

//返回单位化的一个copy
Vector3 Vector3::normalizeCopy() const
{
	Vector3 norm = *this;
	norm.normalize();
	return norm;
}

//自身取负数
void Vector3::negative()
{
	x = -x;
	y = -y;
	z = -z;
}

//获得正交的向量 
Vector3 Vector3::perpendicularVector3() const
{
    static const double zero_tolerance = 1e-06 * 1e-06;
    Vector3 perp = this->crossProduct( Vector3::UNIT_X );

    if( perp.squareLength() < zero_tolerance )
      perp = this->crossProduct( Vector3::UNIT_Y );
   
	perp.normalize();

	return perp;
}

//vertex+vector
Vertex3 operator + (const Vertex3& pos, const Vector3& vec)
{
	return Vertex3(pos.x + vec.x, pos.y + vec.y , pos.z + vec.z);
}

//vertex-vector
Vertex3 operator - (const Vertex3& pos, const Vector3& vec)
{
	return Vertex3(pos.x - vec.x, pos.y - vec.y , pos.z - vec.z);
}