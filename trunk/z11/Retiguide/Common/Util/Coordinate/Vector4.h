#ifndef VECTOR4_H
#define VECTOR4_H

class Vector3;

class Vector4
{
public:
	double x, y, z, w;

public:
	Vector4() {}

	Vector4(double a, double b, double c, double d)
		: x(a), y(b), z(c), w(d) {}

	Vector4(double a[4]) : x(a[0]), y(a[1]), z(a[2]), w(a[3]) {}

	Vector4(const Vector4& r_vec) 
		: x(r_vec.x), y(r_vec.y), z(r_vec.z), w(r_vec.w) {}

	//-------------------------------------------------------------------
	//Basic Operation
	//-------------------------------------------------------------------
	//operator =
	Vector4& operator = (const Vector4& r_vec);
	Vector4& operator = (const Vector3& r_vec);
	//operator [] 
	double operator [] (uint_32 i);
	//operator [] (const)
	double operator [] (uint_32 i) const;
	//operator * 标量 (V * S)
	Vector4 operator * (double scalar) const;
	//operator * (r_vec各分量表示缩放的比例）
	Vector4 operator * (const Vector4& r_vec) const;
	//operator / 标量 （V / s)
	Vector4 operator / (double scalar) const;
	//operator / (r_vec各分量表示缩放的比例）
	Vector4 operator / (const Vector4& r_vec) const;
	//operator *= 标量
	Vector4& operator *= (double scalar);
	//operator *= (r_vec各分量表示缩放的比例）
	Vector4& operator *= (const Vector4& r_vec);
	//operator /= 标量
	Vector4& operator /= (double scalar);
	//operator /= (r_vec各分量表示缩放的比例）
	Vector4& operator /= (const Vector4& r_vec);


};

#endif//