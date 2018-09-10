
#include "Vector3.h"
#include "Vector4.h"

//-------------------------------------------------------------------
//Basic Operation
//-------------------------------------------------------------------
Vector4& Vector4::operator = (const Vector3& r_vec)
{
	this->x = r_vec.x;
	this->y = r_vec.y;
	this->z = r_vec.z;
	this->w = 1.0;

	return *this;
}
//operator =
Vector4& Vector4::operator = (const Vector4& r_vec)
{
	if(this == &r_vec)
		return *this;
	else
	{
		this->x = r_vec.x;
		this->y = r_vec.y;
		this->z = r_vec.z;
		this->w = r_vec.w;

		return *this;
	}
}

//operator [] 
double Vector4::operator [] (uint_32 i)
{
	//assert( i < 4 );
	return *(&x+i);
}

//operator [] (const)
double Vector4::operator [] (uint_32 i) const
{
	//assert( i < 4 );
	return *(&x+i);
}

//operator *
Vector4 Vector4::operator * (double scalar) const
{
	return Vector4(x * scalar, y * scalar, z * scalar, w * scalar);
}

//operator *
Vector4 Vector4::operator * (const Vector4& r_vec) const
{
	return Vector4(x * r_vec.x, y * r_vec.y, z * r_vec.z, w *r_vec.w);
}

//operator /
Vector4 Vector4::operator / (double scalar) const
{
	return Vector4(x / scalar, y / scalar, z / scalar, w / scalar);
}

//operator /
Vector4 Vector4::operator / (const Vector4& r_vec) const
{
	return Vector4(x / r_vec.x, y / r_vec.y, z / r_vec.z, w / r_vec.w);
}

//operator *=
Vector4& Vector4::operator *= (double scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	w *= scalar;

	return *this;
}

//operator *= （缩放比例不一致）
Vector4& Vector4::operator *= (const Vector4& r_vec)
{
	x *= r_vec.x;
	y *= r_vec.y;
	z *= r_vec.z;
	w *= r_vec.w;

	return *this;
}

//operator /=
Vector4& Vector4::operator /= (double scalar)
{
	x /= scalar;
	y /= scalar;
	z /= scalar;
	w /= scalar;

	return *this;
}

//operator /=  （缩放比例不一致）
Vector4& Vector4::operator /= (const Vector4& r_vec)
{
	x /= r_vec.x;
	y /= r_vec.y;
	z /= r_vec.z;
	w /= r_vec.w;

	return *this;
}