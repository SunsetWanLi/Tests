#ifndef QUATERNION_H
#define QUATERNION_H

class Vector3;
class Matrix4;

class Quaternion
{
public:
	double x, y, z, w;

public:
	Quaternion(double a = 0.0, double b = 0.0, 
		double c = 0.0, double d = 1.0) : x(a), y(b), z(c), w(d) {}
	
	Quaternion (const Quaternion& rkQ)
	{
		w = rkQ.w;
		x = rkQ.x;
		y = rkQ.y;
		z = rkQ.z;
	}

	~Quaternion(){}
	
	static const double EPS;
	static const Quaternion ZERO;
	static const Quaternion IDENTITY;
	//--------------------------------------------------------------------
	//Basic Operation
	//--------------------------------------------------------------------
	//operator =
	Quaternion& operator = (const Quaternion& rkQ);
	//operator ==
	bool operator == (const Quaternion& rhs) const;
	//operator != 
	bool operator != (const Quaternion& rhs) const;
	//operator +
	Quaternion operator+ (const Quaternion& rkQ) const;
	//operator -
	Quaternion operator- (const Quaternion& rkQ) const;
	//operator * (Q1 * Q2)
	Quaternion operator* (const Quaternion& rkQ) const;
	//operator * (Q * scalar)
	Quaternion operator* (double scalar) const;
	//operator * ( Q * V) rotate vector
	Vector3 operator* (const Vector3& rkVector) const;
	//operator * (scalar * Q)
	friend Quaternion operator* (double scalar, const Quaternion &rkQ);
	//negative
	Quaternion operator- () const;
	//magnitude (Q * Q.inverse)共轭
	double  magnitude() const;
	//normalizeCopy
	Quaternion normalizeCopy() const;
	//normalizeSelf
	void normalize();
	//inverse Q
	Quaternion inverse () const;
	
	//-------------------------------------------------------------
	//Rotation transformation
	//-------------------------------------------------------------
	
	//Quaternion->RotationMatrix
	void toRotationMatrix(Matrix4& mat) const ;
	//Quaternion->Angle&Axis
	void toAngleAxis (double& angle, Vector3& axis) const;
	////RotationMatrix->Quaternion
	//Quaternion fromRotationMatrix(const Matrix4& mat);
	//Angle&Axis->Quaternion
	void fromAngleAxis(double angle, const Vector3& axis);
	
};

#endif//
