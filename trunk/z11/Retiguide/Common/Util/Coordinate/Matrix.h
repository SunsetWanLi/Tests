#ifndef MATRIX_H
#define MATRIX_H

#include "DataStruct.h"
#include "Vector3.h"
#include "Vector4.h"
#include "memory.h"
#include "Quaternion.h"
#include <iostream>
using namespace std;

class Matrix4
{
private:
	double m[4][4];

public:
	//常用数据
	static const Matrix4 ZERO;		//零矩阵
	static const Matrix4 IDENTITY;	//单位矩阵
	static const double  DET_EPSILON ;	//精度控制 
	
	Matrix4() 
	{
		*this = Matrix4::IDENTITY ;
	} //default
	Matrix4 (double arr[4][4])
	{
		memcpy(m, arr, 16 * sizeof(double));
	}
	Matrix4 (const Matrix4& r_mat)
	{
		memcpy(m, r_mat.m, 16 * sizeof(double));
	}
	Matrix4 (double arr[16])
	{
		memcpy(m, arr, 16 * sizeof(double));
	}
    Matrix4(
		double m00, double m01, double m02, double m03,
		double m10, double m11, double m12, double m13,
		double m20, double m21, double m22, double m23,
		double m30, double m31, double m32, double m33 )
	{
		m[0][0] = m00;
		m[0][1] = m01;
		m[0][2] = m02;
		m[0][3] = m03;
		m[1][0] = m10;
		m[1][1] = m11;
		m[1][2] = m12;
		m[1][3] = m13;
		m[2][0] = m20;
		m[2][1] = m21;
		m[2][2] = m22;
		m[2][3] = m23;
		m[3][0] = m30;
		m[3][1] = m31;
		m[3][2] = m32;
		m[3][3] = m33;
	}

	~Matrix4() { }

	//-----------------------------------------------------------------------
	//Basic Opertion
	//----------------------------------------------------------------------
	//Matrix4_2 = Matrix4_1
	Matrix4& operator = (const Matrix4& m2);
	//Matrix4_1 == Matrix4_2
	bool operator == ( const Matrix4& m2 ) const;
	//Matrxi4_1 != Matrix4_2
	bool operator != ( const Matrix4& m2 ) const;
	//Matrix4 + Matrix4
	Matrix4 operator + ( const Matrix4 &m2 ) const;
	//operator +=
	Matrix4& operator += (const Matrix4& m2);
	//Matrix4 - Matrix4
	Matrix4 operator - ( const Matrix4 &m2 ) const;
	//operator -=
	Matrix4& operator -= (const Matrix4& m2);
	//return -Matrix4
	Matrix4 operator - (void) const;
	//Matrix * scalar...
	Matrix4 operator * (double scalar) const;
	//operator *= scalar
	Matrix4& operator *= (double scalar);
	//Matrix4 * Matrix4...
	Matrix4 operator * ( const Matrix4 &m2 ) const;
	//operator *= Matrix4
	Matrix4& operator *= (const Matrix4& m2);
	//Matrix4 * Vector3...
	Vector3 operator * ( const Vector3& v ) const;
	//Matrix4 * Vector4...		
	Vector4 operator * (const Vector4& v) const;
	//Matrix4 * DEPlane...
	DEPlane operator * (const DEPlane& p) const;

	//Matrix4[][] 
	double* operator [] ( uint_32 iRow );
	//Matrix4[][] (const)
	const double *const operator [] ( uint_32 iRow ) const;
	//Matrix4 Transpose...
	Matrix4 transposeMatrix(void) const;
	//-Matrix4
	void negative(void);
	//获得代数余子式的值
	double COFACTOR(const uint_32 r0, const uint_32 r1, const uint_32 r2, 
					const uint_32 c0, const uint_32 c1, const uint_32 c2) const;
	//计算行列式的值
	double determinant() const;
	//计算矩阵的伴随矩阵
	Matrix4 adjointMatrix() const;
	//计算矩阵的逆矩阵
	Matrix4 inverseMatrix() const;


	//-----------------------------------------------------------------------
	//Translation Transformation
	//-----------------------------------------------------------------------
	// set a translation matrix by Vector3... * current matrix
	void setTransMatrix( const Vector3& v );
	// set a translation matrix by x, y , z...  * current matrix
	void setTransMatrix( double x, double y, double z );

	//-----------------------------------------------------------------------
	//Scale Transformation
	//-----------------------------------------------------------------------
	// Sets the scale matrix by Vector3... * current matrix
	void setScaleMatrix( const Vector3& v );
	// Sets the scale matrix by x, y, z... * current matrix
	void setScaleMatrix( double x, double y, double z );


	//---------------------------------------------------------------------
	//Rotate Transformation
	//---------------------------------------------------------------------
	// Sets the Rotate matrix by Angle & Vector3... * current matrix
	void setRotateMatrix(double angle, const Vector3& v);
	// Sets the Rotate matrix by Angle & x, y , z... * current matrix
	void setRotateMatrix(double angle, double x, double y, double z);

	//---------------------------------------------------------------------
	//Project Transformation
	//---------------------------------------------------------------------
	//Gets the OpenGL Ortho Project Matrix
	static Matrix4 getGLOrthoMatrix(double left,   double right,
									double bottom, double top,
									double zNear,  double zFar);
	
	//Gets the OpenGL Persperctive Matrix
	static Matrix4 getGLPerspectiveMatrix(double angle, double aspect, 
										  double zNear,  double zFar);
	

	//--------------------------------------------------------------------------
	//View Transformation
	//--------------------------------------------------------------------------
	//Gets View Matrix 
	static Matrix4 getViewMatrix(	double eye_x,  double eye_y,  double eye_z,
									double look_x, double look_y, double look_z,
									double up_x,   double up_y,   double up_z );
	//Gets View Matrix
	static Matrix4 getViewMatrix(	const Vector3& eye,
									const Vector3& look,
									const Vector3& up);


	//src_point ---Transformation--->dst_point
	Vertex3 getDestPoint(const Vertex3& src_pos) const;
	//dst_point ---Transformation(INVERSE)--->src_point
	Vertex3 getSrcPoint(const Vertex3& dst_pos) const;
	//src_plane--->dst_plane
	DEPlane getDestPlane(const DEPlane& plane) const;
	//src_box---->dst_box
	//DEBox3D getDestBox(const DEBox3D& box) const;
	//src triangle--->dst triangle
	//Triangle getDestTriangle(const Triangle& src_tri) const;
	//src DERay--->dst DERay
	DERay getDestRay( const DERay &ray ) const;
	//src Frustrum ---> dst Frustrum
	DEViewFrustrum getDestFrustrum(const DEViewFrustrum& frust) const;
	static double getDistance(const Vertex3& pos, const DEPlane& plane);
	//src Normal ---> dst Normal
	Vector3 getDestNormal( const Vector3& src_normal) const;

	//void createGLMatrix(float *glmat, const Matrix4 & mat)
	//{
	//	for( int i = 0 ; i < 4; ++ i )
	//		for( int j = 0 ; j < 4 ; ++ j )
	//			glmat[4*i+j] = (float)mat[j][i] ;	//openGL's matrix colum first...
	//}
	void print()
	{
		for(int l = 0; l < 4; l++)
		{
			for(int ll = 0; ll < 4; ll++)
			{
				cout<<m[ll][l]<<"   ";
			}
			cout<<endl;
		}
		cout<<endl<<endl;
	}

};

#endif//