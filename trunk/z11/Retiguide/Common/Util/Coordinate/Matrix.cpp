
#include "Matrix.h"
#include "DataStruct.h"
#include <cmath>

const Matrix4 Matrix4::ZERO(
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0 );

//单位矩阵
const Matrix4 Matrix4::IDENTITY(
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1 );	

//精度（行列式的值小于此，则看作零矩阵）
const double Matrix4::DET_EPSILON = 1e-16;

//---------------------------------------------------------------------
// Basic Operation
//---------------------------------------------------------------------
//operator =
Matrix4& Matrix4::operator = (const Matrix4& m2)
{
	if(this == &m2)
		return *this;
	else
	{
		::memcpy(this->m, m2.m, 16 * sizeof( double));
		return *this;
	}
}
//operator [] 
double* Matrix4::operator [] ( uint_32 iRow )
{
	//assert( iRow < 4 );
	return m[iRow];
}
//operator [] (const)
const double *const Matrix4::operator [] ( uint_32 iRow ) const
{
	//assert( iRow < 4 );
	return m[iRow];
}

//Matrix4 * scalar...
Matrix4 Matrix4::operator * (double scalar) const
{
	Matrix4 ret(*this);
	ret *= scalar;
	return ret;
}

//operator *=
Matrix4& Matrix4::operator *= (double scalar)
{
	for(uint_32 row = 0; row < 4; row++)
		for(uint_32 col = 0; col < 4; col++)
			m[row][col] *= scalar;
	
	return *this;
}

//operator *= 
Matrix4& Matrix4::operator *= (const Matrix4& m2)
{
	Matrix4 temp(*this);

	m[0][0] = temp.m[0][0] * m2.m[0][0] + temp.m[0][1] * m2.m[1][0] + temp.m[0][2] * m2.m[2][0] + temp.m[0][3] * m2.m[3][0];
	m[0][1] = temp.m[0][0] * m2.m[0][1] + temp.m[0][1] * m2.m[1][1] + temp.m[0][2] * m2.m[2][1] + temp.m[0][3] * m2.m[3][1];
	m[0][2] = temp.m[0][0] * m2.m[0][2] + temp.m[0][1] * m2.m[1][2] + temp.m[0][2] * m2.m[2][2] + temp.m[0][3] * m2.m[3][2];
	m[0][3] = temp.m[0][0] * m2.m[0][3] + temp.m[0][1] * m2.m[1][3] + temp.m[0][2] * m2.m[2][3] + temp.m[0][3] * m2.m[3][3];

	m[1][0] = temp.m[1][0] * m2.m[0][0] + temp.m[1][1] * m2.m[1][0] + temp.m[1][2] * m2.m[2][0] + temp.m[1][3] * m2.m[3][0];
	m[1][1] = temp.m[1][0] * m2.m[0][1] + temp.m[1][1] * m2.m[1][1] + temp.m[1][2] * m2.m[2][1] + temp.m[1][3] * m2.m[3][1];
	m[1][2] = temp.m[1][0] * m2.m[0][2] + temp.m[1][1] * m2.m[1][2] + temp.m[1][2] * m2.m[2][2] + temp.m[1][3] * m2.m[3][2];
	m[1][3] = temp.m[1][0] * m2.m[0][3] + temp.m[1][1] * m2.m[1][3] + temp.m[1][2] * m2.m[2][3] + temp.m[1][3] * m2.m[3][3];

	m[2][0] = temp.m[2][0] * m2.m[0][0] + temp.m[2][1] * m2.m[1][0] + temp.m[2][2] * m2.m[2][0] + temp.m[2][3] * m2.m[3][0];
	m[2][1] = temp.m[2][0] * m2.m[0][1] + temp.m[2][1] * m2.m[1][1] + temp.m[2][2] * m2.m[2][1] + temp.m[2][3] * m2.m[3][1];
	m[2][2] = temp.m[2][0] * m2.m[0][2] + temp.m[2][1] * m2.m[1][2] + temp.m[2][2] * m2.m[2][2] + temp.m[2][3] * m2.m[3][2];
	m[2][3] = temp.m[2][0] * m2.m[0][3] + temp.m[2][1] * m2.m[1][3] + temp.m[2][2] * m2.m[2][3] + temp.m[2][3] * m2.m[3][3];

	m[3][0] = temp.m[3][0] * m2.m[0][0] + temp.m[3][1] * m2.m[1][0] + temp.m[3][2] * m2.m[2][0] + temp.m[3][3] * m2.m[3][0];
	m[3][1] = temp.m[3][0] * m2.m[0][1] + temp.m[3][1] * m2.m[1][1] + temp.m[3][2] * m2.m[2][1] + temp.m[3][3] * m2.m[3][1];
	m[3][2] = temp.m[3][0] * m2.m[0][2] + temp.m[3][1] * m2.m[1][2] + temp.m[3][2] * m2.m[2][2] + temp.m[3][3] * m2.m[3][2];
	m[3][3] = temp.m[3][0] * m2.m[0][3] + temp.m[3][1] * m2.m[1][3] + temp.m[3][2] * m2.m[2][3] + temp.m[3][3] * m2.m[3][3];

	return *this;
}

//Matrix4 * Matrix4...
Matrix4 Matrix4::operator * ( const Matrix4 &m2 ) const
{
	Matrix4 ret_matrix(*this);
	ret_matrix *= m2;
	return ret_matrix;
}

//Matrix4 * Vector3...
//将Vector3转化为Vector4，相当于Vector4中的w为1.0，
//最后结果重新转化的时候要除以新的w
//[ m[0][0]  m[0][1]  m[0][2]  m[0][3] ]   {x}   {new_x}   {new_x/new_w}
//| m[1][0]  m[1][1]  m[1][2]  m[1][3] | * {y} = {new_y}=> {new_y/new_w}
//| m[2][0]  m[2][1]  m[2][2]  m[2][3] |   {z}   {new_z}   {new_z/new_w}
//[ m[3][0]  m[3][1]  m[3][2]  m[3][3] ]   {1}   {new_w}   {1.0}
Vector3 Matrix4::operator * ( const Vector3 &v ) const
{
	Vector3 ret_vertex;

	double new_w = m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3];

	ret_vertex.x = ( m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] ) / new_w;
	ret_vertex.y = ( m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] ) / new_w;
	ret_vertex.z = ( m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] ) / new_w;

	return ret_vertex;
}

//Matrix4 * Vector4...		
Vector4 Matrix4::operator * (const Vector4& v) const
{
	return Vector4(
		m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w, 
		m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
		m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
		m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w
		);
	//return Vector4(
	//	m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0] * v.w, 
	//	m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1] * v.w,
	//	m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2] * v.w,
	//	m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3] * v.w
	//	);
}

//Matrix4 * DEPlane...
DEPlane Matrix4::operator * (const DEPlane& p) const
{
	DEPlane ret;
	
	//ret.normal.x =
	//	m[0][0] * p.normal.x + m[0][1] * p.normal.y + m[0][2] * p.normal.z;
	//ret.normal.y = 
	//	m[1][0] * p.normal.x + m[1][1] * p.normal.y + m[1][2] * p.normal.z;
	//ret.normal.z = 
	//	m[2][0] * p.normal.x + m[2][1] * p.normal.y + m[2][2] * p.normal.z;
	
	Matrix4 inverse_transpose_mat = this->inverseMatrix().transposeMatrix();
	ret.normal.x =
		inverse_transpose_mat.m[0][0] * p.normal.x + inverse_transpose_mat.m[0][1] * p.normal.y + inverse_transpose_mat.m[0][2] * p.normal.z;
	ret.normal.y = 
		inverse_transpose_mat.m[1][0] * p.normal.x + inverse_transpose_mat.m[1][1] * p.normal.y + inverse_transpose_mat.m[1][2] * p.normal.z;
	ret.normal.z = 
		inverse_transpose_mat.m[2][0] * p.normal.x + inverse_transpose_mat.m[2][1] * p.normal.y + inverse_transpose_mat.m[2][2] * p.normal.z;

	Vector3 pt = p.normal * -p.d;
	pt = *this * pt;

	ret.normal.normalize();
	ret.d = - pt.dotProduct(ret.normal);

	return ret;

	//DEPlane ret;
	//vector< Vector3 > pos;

	//pos.resize(3);
	//if(abs(p.normal.x) > 1e-06)
	//{
	//	pos[0] = Vertex3( -p.d/p.normal.x,				0,		0);
	//	pos[1] = Vertex3((-p.d-p.normal.y)/p.normal.x,	1.0,	0);
	//	pos[2] = Vertex3((-p.d-p.normal.z)/p.normal.x,	0,		1.0);
	//}
	//else if(abs(p.normal.y) > 1e-06)
	//{
	//	pos[0] = Vertex3(0,		-p.d/p.normal.y,				0);
	//	pos[1] = Vertex3(1.0,	(-p.d-p.normal.x)/p.normal.y,	0);
	//	pos[2] = Vertex3(0,		(-p.d-p.normal.z)/p.normal.y,	1.0);
	//}
	//else
	//{
	//	pos[0] = Vertex3(0,		0,		-p.d/p.normal.z);
	//	pos[1] = Vertex3(1.0,	0,		(-p.d-p.normal.x)/p.normal.z);
	//	pos[2] = Vertex3(0,		1.0,	(-p.d-p.normal.y)/p.normal.z);
	//}

	//pos[0] = *this * pos[0];
	//pos[1] = *this * pos[1];
	//pos[2] = *this * pos[2];

	//Math::calcPlane(pos, &ret.normal.x, &ret.normal.y, &ret.normal.z, &ret.d);
	//
	//ret.d /= ret.normal.length();
	//ret.normal.normalize();

	//return ret;
}


//operator +=
Matrix4& Matrix4::operator += (const Matrix4& m2)
{
	m[0][0] += m2.m[0][0];
	m[0][1] += m2.m[0][1];
	m[0][2] += m2.m[0][2];
	m[0][3] += m2.m[0][3];

	m[1][0] += m2.m[1][0];
	m[1][1] += m2.m[1][1];
	m[1][2] += m2.m[1][2];
	m[1][3] += m2.m[1][3];

	m[2][0] += m2.m[2][0];
	m[2][1] += m2.m[2][1];
	m[2][2] += m2.m[2][2];
	m[2][3] += m2.m[2][3];

	m[3][0] += m2.m[3][0];
	m[3][1] += m2.m[3][1];
	m[3][2] += m2.m[3][2];
	m[3][3] += m2.m[3][3];
	
	return *this;
}

//Matrix4 + Matrix4
Matrix4 Matrix4::operator + ( const Matrix4 &m2 ) const
{
	Matrix4 r(*this);
	r += m2;
	return r;
}

//operator -=
Matrix4& Matrix4::operator -= (const Matrix4& m2)
{
	m[0][0] -= m2.m[0][0];
	m[0][1] -= m2.m[0][1];
	m[0][2] -= m2.m[0][2];
	m[0][3] -= m2.m[0][3];

	m[1][0] -= m2.m[1][0];
	m[1][1] -= m2.m[1][1];
	m[1][2] -= m2.m[1][2];
	m[1][3] -= m2.m[1][3];

	m[2][0] -= m2.m[2][0];
	m[2][1] -= m2.m[2][1];
	m[2][2] -= m2.m[2][2];
	m[2][3] -= m2.m[2][3];

	m[3][0] -= m2.m[3][0];
	m[3][1] -= m2.m[3][1];
	m[3][2] -= m2.m[3][2];
	m[3][3] -= m2.m[3][3];

	return *this;
}

//Matrix4 - Matrix4
Matrix4 Matrix4::operator - ( const Matrix4 &m2 ) const
{
	Matrix4 r(*this);
	r -= m2;
	return r;
}

//check if Matrix4_1 == Matrix4_2...
bool Matrix4::operator == ( const Matrix4& m2 ) const
{
	if( 
		m[0][0] != m2.m[0][0] || m[0][1] != m2.m[0][1] || m[0][2] != m2.m[0][2] || m[0][3] != m2.m[0][3] ||
		m[1][0] != m2.m[1][0] || m[1][1] != m2.m[1][1] || m[1][2] != m2.m[1][2] || m[1][3] != m2.m[1][3] ||
		m[2][0] != m2.m[2][0] || m[2][1] != m2.m[2][1] || m[2][2] != m2.m[2][2] || m[2][3] != m2.m[2][3] ||
		m[3][0] != m2.m[3][0] || m[3][1] != m2.m[3][1] || m[3][2] != m2.m[3][2] || m[3][3] != m2.m[3][3] )
		return false;
	return true;
}

//check if Matrix4_1 != Matrix4_2...
bool Matrix4::operator != (const Matrix4& m2 ) const
{
	if( 
		m[0][0] != m2.m[0][0] || m[0][1] != m2.m[0][1] || m[0][2] != m2.m[0][2] || m[0][3] != m2.m[0][3] ||
		m[1][0] != m2.m[1][0] || m[1][1] != m2.m[1][1] || m[1][2] != m2.m[1][2] || m[1][3] != m2.m[1][3] ||
		m[2][0] != m2.m[2][0] || m[2][1] != m2.m[2][1] || m[2][2] != m2.m[2][2] || m[2][3] != m2.m[2][3] ||
		m[3][0] != m2.m[3][0] || m[3][1] != m2.m[3][1] || m[3][2] != m2.m[3][2] || m[3][3] != m2.m[3][3] )
		return true;
	return false;
}

//Matrix4 transpose...
Matrix4 Matrix4::transposeMatrix(void) const
{
	return Matrix4(m[0][0], m[1][0], m[2][0], m[3][0],
		m[0][1], m[1][1], m[2][1], m[3][1],
		m[0][2], m[1][2], m[2][2], m[3][2],
		m[0][3], m[1][3], m[2][3], m[3][3]);
}

//return -Matrix4...
Matrix4 Matrix4::operator - (void) const
{
	Matrix4 r(*this);
	r.negative();
	return r;
}

//Modify itself -Matrix...
void Matrix4::negative()
{
	m[0][0] = -m[0][0];
	m[0][1] = -m[0][1];
	m[0][2] = -m[0][2];
	m[0][3] = -m[0][3];

	m[1][0] = -m[1][0];
	m[1][1] = -m[1][1];
	m[1][2] = -m[1][2];
	m[1][3] = -m[1][3];

	m[2][0] = -m[2][0];
	m[2][1] = -m[2][1];
	m[2][2] = -m[2][2];
	m[2][3] = -m[2][3];

	m[3][0] = -m[3][0];
	m[3][1] = -m[3][1];
	m[3][2] = -m[3][2];
	m[3][3] = -m[3][3];
}

//获得代数余子式的值
double Matrix4::COFACTOR(const uint_32 r0, const uint_32 r1, const uint_32 r2, 
							const uint_32 c0, const uint_32 c1, const uint_32 c2) const
{
	return (m[r0][c0] * (m[r1][c1] * m[r2][c2] - m[r2][c1] * m[r1][c2]) -
			m[r0][c1] * (m[r1][c0] * m[r2][c2] - m[r2][c0] * m[r1][c2]) +
			m[r0][c2] * (m[r1][c0] * m[r2][c1] - m[r2][c0] * m[r1][c1]));
}

//计算当前矩阵的伴随矩阵...
Matrix4 Matrix4::adjointMatrix() const
{
	return Matrix4( 
					COFACTOR( 1, 2, 3, 1, 2, 3),
				-COFACTOR( 0, 2, 3, 1, 2, 3),
					COFACTOR( 0, 1, 3, 1, 2, 3),
				-COFACTOR( 0, 1, 2, 1, 2, 3),

				-COFACTOR( 1, 2, 3, 0, 2, 3),
					COFACTOR( 0, 2, 3, 0, 2, 3),
				-COFACTOR( 0, 1, 3, 0, 2, 3),
					COFACTOR( 0, 1, 2, 0, 2, 3),

					COFACTOR( 1, 2, 3, 0, 1, 3),
				-COFACTOR( 0, 2, 3, 0, 1, 3),
					COFACTOR( 0, 1, 3, 0, 1, 3),
				-COFACTOR( 0, 1, 2, 0, 1, 3),

				-COFACTOR( 1, 2, 3, 0, 1, 2),
					COFACTOR( 0, 2, 3, 0, 1, 2),
				-COFACTOR( 0, 1, 3, 0, 1, 2),
					COFACTOR( 0, 1, 2, 0, 1, 2)
					);
}

//计算当前矩阵的行列式
double Matrix4::determinant() const
{
	return (m[0][0] * COFACTOR( 1, 2, 3, 1, 2, 3) -
			m[0][1] * COFACTOR( 1, 2, 3, 0, 2, 3) +
			m[0][2] * COFACTOR( 1, 2, 3, 0, 1, 3) -
			m[0][3] * COFACTOR( 1, 2, 3, 0, 1, 2));
}

//计算矩阵的逆矩阵
Matrix4 Matrix4::inverseMatrix() const
{
	if(determinant() > DET_EPSILON)
		return adjointMatrix() * (1.0f / determinant());
	else
		return ZERO;
}

//-----------------------------------------------------------------------
//Translation Transformation
//-----------------------------------------------------------------------
// set a translation matrix by Vector3...
void Matrix4::setTransMatrix( const Vector3& v )
{
	Matrix4 mat = *this;

	m[0][0] = 1.0; m[0][1] = 0.0; m[0][2] = 0.0; m[0][3] = v.x;
	m[1][0] = 0.0; m[1][1] = 1.0; m[1][2] = 0.0; m[1][3] = v.y;
	m[2][0] = 0.0; m[2][1] = 0.0; m[2][2] = 1.0; m[2][3] = v.z;
	m[3][0] = 0.0; m[3][1] = 0.0; m[3][2] = 0.0; m[3][3] = 1.0;

	*this = *this * mat;
}
// set a translation matrix by x, y , z... 
void Matrix4::setTransMatrix( double x, double y, double z )
{
	Matrix4 mat = *this;

	m[0][0] = 1.0; m[0][1] = 0.0; m[0][2] = 0.0; m[0][3] = x;
	m[1][0] = 0.0; m[1][1] = 1.0; m[1][2] = 0.0; m[1][3] = y;
	m[2][0] = 0.0; m[2][1] = 0.0; m[2][2] = 1.0; m[2][3] = z;
	m[3][0] = 0.0; m[3][1] = 0.0; m[3][2] = 0.0; m[3][3] = 1.0;

	*this = *this * mat;
}



//-----------------------------------------------------------------------
//Scale Transformation
//-----------------------------------------------------------------------
// Sets the scale matrix by Vector3...
void Matrix4::setScaleMatrix( const Vector3& v )
{
	Matrix4 mat = *this;

	m[0][0] = v.x; m[0][1] = 0.0; m[0][2] = 0.0; m[0][3] = 0.0;
	m[1][0] = 0.0; m[1][1] = v.y; m[1][2] = 0.0; m[1][3] = 0.0;
	m[2][0] = 0.0; m[2][1] = 0.0; m[2][2] = v.z; m[2][3] = 0.0;
	m[3][0] = 0.0; m[3][1] = 0.0; m[3][2] = 0.0; m[3][3] = 1.0;

	*this = *this * mat;
}
// Sets the scale matrix by x, y, z...
void Matrix4::setScaleMatrix( double x, double y, double z )
{
	Matrix4 mat = *this;

	m[0][0] =   x; m[0][1] = 0.0; m[0][2] = 0.0; m[0][3] = 0.0;
	m[1][0] = 0.0; m[1][1] =   y; m[1][2] = 0.0; m[1][3] = 0.0;
	m[2][0] = 0.0; m[2][1] = 0.0; m[2][2] =   z; m[2][3] = 0.0;
	m[3][0] = 0.0; m[3][1] = 0.0; m[3][2] = 0.0; m[3][3] = 1.0;
	
	*this = *this * mat;
}


//---------------------------------------------------------------------
//Rotate Transformation
//---------------------------------------------------------------------
// Sets the Rotate matrix by Angle & Vector3...
void Matrix4::setRotateMatrix(double angle, const Vector3& v)
{
	Matrix4 mat = *this;

	Quaternion q_rot;
	q_rot.fromAngleAxis(angle, v);
	q_rot.toRotationMatrix(*this);

	*this = *this * mat;
}

// Sets the Rotate matrix by Angle & x, y , z...
void Matrix4::setRotateMatrix(double angle, double x, double y, double z)
{
	Matrix4 mat = *this;

	Quaternion q_rot;
	q_rot.fromAngleAxis(angle, Vector3(x, y, z));
	q_rot.toRotationMatrix(*this);	

	*this = *this * mat;
}



//---------------------------------------------------------------------
//Project Transformation
//---------------------------------------------------------------------
//Gets the Ortho Project Matrix
Matrix4 Matrix4::getGLOrthoMatrix(double left,   double right,
								double bottom, double top,
								double zNear,  double zFar)
{
	Matrix4 mat;

	mat.m[0][0] = 2.0 / (right - left);
	mat.m[1][0] = 0.0;
	mat.m[2][0] = 0.0;
	mat.m[3][0] = 0.0;

	mat.m[0][1] = 0.0;
	mat.m[1][1] = 2.0 / (top - bottom);
	mat.m[2][1] = 0.0;
	mat.m[3][1] = 0.0;

	mat.m[0][2] = 0.0;
	mat.m[1][2] = 0.0;
	mat.m[2][2] = -2.0 / (zFar - zNear);
	mat.m[3][2] = 0.0;

	mat.m[0][3] = -(right + left) / (right - left);
	mat.m[1][3] = -(top + bottom) / (top - bottom);
	mat.m[2][3] = -(zFar + zNear) / (zFar - zNear);
	mat.m[3][3] = 1.0;
	
	return mat;
}

//Gets the OpenGL Persperctive Matrix(only for computation)
Matrix4 Matrix4::getGLPerspectiveMatrix(double angle, double aspect, 
										double zNear,  double zFar)
{
	Matrix4 mat;
	double randian = 3.141592654*(angle/180.0) / 2.0;
	double tan_val = tan(randian);

	mat.m[0][0] = 1.0 / tan_val / aspect;
	mat.m[1][0] = 0.0;
	mat.m[2][0] = 0.0;
	mat.m[3][0] = 0.0;

	mat.m[0][1] = 0.0;
	mat.m[1][1] = 1.0 / tan_val;
	mat.m[2][1] = 0.0;
	mat.m[3][1] = 0.0;

	mat.m[0][2] = 0.0;
	mat.m[1][2] = 0.0;
	//mat.m[2][2] = (zFar) / (zFar - zNear);
	//mat.m[3][2] = 1.0;
	mat.m[2][2] = -(zFar + zNear) / (zFar - zNear);
	mat.m[3][2] = -1.0;

	mat.m[0][3] = 0.0;
	mat.m[1][3] = 0.0;
	//mat.m[2][3] = zFar * zNear / (zFar - zNear);
	mat.m[2][3] = -2.0 * zFar * zNear / (zFar - zNear);
	mat.m[3][3] = 0.0;

	return mat;
}

//--------------------------------------------------------------------------
//View Transformation
//--------------------------------------------------------------------------
//Gets View Matrix
Matrix4 Matrix4::getViewMatrix(	double eye_x,  double eye_y,  double eye_z,
								double look_x, double look_y, double look_z,
								double up_x,   double up_y,   double up_z )
{
	Vector3 eye(eye_x, eye_y, eye_z);
	Vector3 look(look_x, look_y, look_z);
	Vector3 up(up_x, up_y, up_z);

	return getViewMatrix(eye, look, up);
}

//Gets View Matrix
Matrix4 Matrix4::getViewMatrix(	const Vector3& eye,
								const Vector3& look,
								const Vector3& up)
{
	Matrix4 mat;
	Vector3 norm_x, norm_y, norm_z;
	
	norm_z = (eye - look);
	norm_z.normalize();
	norm_x = up.crossProduct(norm_z);
	norm_x.normalize();
    norm_y = norm_z.crossProduct(norm_x);

	mat.m[0][0] = norm_x.x;
	mat.m[0][1] = norm_x.y;
	mat.m[0][2] = norm_x.z;
	mat.m[0][3] = -eye.dotProduct(norm_x);

	mat.m[1][0]	= norm_y.x;
	mat.m[1][1] = norm_y.y;
	mat.m[1][2] = norm_y.z;
	mat.m[1][3] = -eye.dotProduct(norm_y);

	mat.m[2][0] = norm_z.x;
	mat.m[2][1] = norm_z.y;
	mat.m[2][2] = norm_z.z;
	mat.m[2][3] = -eye.dotProduct(norm_z);

	mat.m[3][0] = 0.0;
	mat.m[3][1] = 0.0;
	mat.m[3][2] = 0.0;
	mat.m[3][3] = 1.0;

	return mat;
}

/**********************************************************************/
/*																	  */
/**********************************************************************/
//src_point ---Transformation--->dst_point
Vertex3 Matrix4::getDestPoint(const Vertex3& src_pos) const 
{
	Vector3 vec_dst;
	Vector3 vec_src;

	vec_src = src_pos;
	vec_dst = (*this) * vec_src;

	return Vertex3(vec_dst);
}

//dst_point ---(INVERSE)Transformation--->src_point
Vertex3 Matrix4::getSrcPoint(const Vertex3& dst_pos) const 
{
	Vector3 vec_dst;
	Vector3 vec_src;
	Matrix4 inverse_mat;

	vec_dst = dst_pos;
	inverse_mat = this->inverseMatrix();
	vec_src = inverse_mat * vec_dst;

	return Vertex3(vec_src);
}

//src_plane--->dst_plane
DEPlane Matrix4::getDestPlane(const DEPlane& src_plane) const
{
	DEPlane dst_plane;

	dst_plane = (*this) * src_plane;

	return dst_plane;
}

//src_box---->dst_box
//NOTE:转化后box必须仍然与坐标轴平行
//DEBox3D Matrix4::getDestBox(const DEBox3D& box) const
//{
//	DEBox3D dst_box;
//	Vertex3 box_min, box_max, temp;
//	vector < Vertex3 > corners;
//
//	corners = Math::getAllBoxCorners(box);
//	
//	box_min = box_max = corners[0];
//	for(uint_32 i = 1; i < 8; ++i )
//	{
//		temp = *this * corners[i];
//		if( temp.x > box_max.x )
//			box_max.x = temp.x;
//		if( temp.y > box_max.y )
//			box_max.y = temp.y;
//		if( temp.z > box_max.z )
//			box_max.z = temp.z;
//		if( temp.x < box_min.x )
//			box_min.x = temp.x;
//		if( temp.y < box_min.y )
//			box_min.y = temp.y;
//		if( temp.z < box_min.z )
//			box_min.z = temp.z;
//	}
//	dst_box.min = box_min;
//	dst_box.max = box_max;
//
//	return dst_box;
//}

//src triangle--->dst triangle
//Triangle Matrix4::getDestTriangle(const Triangle& src_tri) const
//{
//	Triangle dst_tri;
//
//	dst_tri.points[0] = *this * src_tri.points[0];
//	dst_tri.points[1] = *this * src_tri.points[1];
//	dst_tri.points[2] = *this * src_tri.points[2];
//
//	return dst_tri;
//}

//src DERay--->dst DERay
DERay Matrix4::getDestRay( const DERay &ray ) const
{
	DERay dst_ray;

	dst_ray.start = *this * ray.start;
	//dst_ray.direction = *this * ray.direction;
	Matrix4 temp_mat;

	temp_mat = this->inverseMatrix().transposeMatrix();
	dst_ray.direction.x =
		temp_mat.m[0][0] * ray.direction.x + temp_mat.m[0][1] * ray.direction.y + temp_mat.m[0][2] * ray.direction.z;
	dst_ray.direction.y = 
		temp_mat.m[1][0] * ray.direction.x + temp_mat.m[1][1] * ray.direction.y + temp_mat.m[1][2] * ray.direction.z;
	dst_ray.direction.z = 
		temp_mat.m[2][0] * ray.direction.x + temp_mat.m[2][1] * ray.direction.y + temp_mat.m[2][2] * ray.direction.z;

	dst_ray.direction.normalize();
	
	return dst_ray;
}

double Matrix4::getDistance(const Vertex3& pos, const DEPlane& plane)
{
	Vector3 v_pos = pos;
	
	return (v_pos.dotProduct(plane.normal) + plane.d)/plane.normal.length();

}

//src Frustrum ---> dst Frustrum
DEViewFrustrum Matrix4::getDestFrustrum(const DEViewFrustrum& frust) const
{
	DEViewFrustrum dst_frust;
	
	for(uint_32 idx = 0; idx < 6; ++idx)
		dst_frust.planes[idx] = getDestPlane(frust.planes[idx]);

	for(uint_32 idx = 0; idx < 8; ++idx)
		dst_frust.corners[idx] = getDestPoint(frust.corners[idx]);

	dst_frust.cone_vertex = getDestPoint(frust.cone_vertex);

	//取转化后的近平面上一点，并求到远平面的距离即可
	dst_frust.height = Matrix4::getDistance(dst_frust.corners[0], dst_frust.planes[1]);
	
	return dst_frust;
}

//src Normal ---> dst Normal
Vector3 Matrix4::getDestNormal( const Vector3& src_normal) const
{
	Vector3 ret_normal;
	Matrix4 inverse_transpose_mat = this->inverseMatrix().transposeMatrix();

	ret_normal.x =
		inverse_transpose_mat.m[0][0] * src_normal.x + inverse_transpose_mat.m[0][1] * src_normal.y + inverse_transpose_mat.m[0][2] * src_normal.z;
	ret_normal.y = 
		inverse_transpose_mat.m[1][0] * src_normal.x + inverse_transpose_mat.m[1][1] * src_normal.y + inverse_transpose_mat.m[1][2] * src_normal.z;
	ret_normal.z = 
		inverse_transpose_mat.m[2][0] * src_normal.x + inverse_transpose_mat.m[2][1] * src_normal.y + inverse_transpose_mat.m[2][2] * src_normal.z;

	return ret_normal;

}
