////Matrix.h  myMatrix  Class

#ifndef __COUNTSEVENPARAM_H__
#define __COUNTSEVENPARAM_H__

#include <vector>
#include <utility>

//#define int_32 int
//#define uint_32 unsigned int
//typedef int 
#include "../Types/basic_types.h"
using z11::int_32;
using z11::uint_32;

#ifdef DOUBLE_ZERO
#undef DOUBLE_ZERO
#endif
#define DOUBLE_ZERO 0.000000001 //TODO:统一定义0的浮点精度

typedef std::vector<std::vector<double> >  MATRIX;

///////////////////////////////////////////////////////////////////////////////////
//int_32 sweapLine( double ** data, uint_32 startLine, uint_32 N );
//int_32 elimLine( double ** data, uint_32 startLine, uint_32 N );
//int_32 elimination( double ** data, uint_32 startLine, uint_32 N );
//int_32 substituteBack( double ** data, uint_32 startLine, uint_32 N );


//////////////////////////////////////////////////////////////////////////////////
class myMatrix
{
public:
	myMatrix();
	myMatrix(MATRIX matrix);
	virtual ~myMatrix(){}

	int_32 set_Matrix(MATRIX matrix);
	MATRIX get_Matrix();

	//得到m_matrix的转置矩阵
	MATRIX get_TransposeMatrix();
	//得到m_matrix * matrix的乘积矩阵
    MATRIX get_multiplyMatrix(MATRIX matrix);

private:
	MATRIX m_matrix;
};

//实现解方程的类
class ComputeEquation
{
public:
	ComputeEquation();
	virtual ~ComputeEquation();

	int_32 set_data(double** m_data);
	double** get_data();
	
	//平方根法求方程
	int_32 sqrt_computeEquation(int_32 startLine, int_32 N);
	//列主元素法求方程
	int_32 colm_computeEquation(int_32 startLine, int_32 N);

private:
	int_32 sweapLine(int_32 startLine, int_32 N );
	int_32 elimLine(int_32 startLine, int_32 N );

	int_32 elimination( int_32 startLine, int_32 N );//按照高斯消元法把它化为上三角矩阵
	int_32 sqrtElimination( int_32 startLine, int_32 N );	//平方根消元法得到下三角矩阵
	int_32 topSubstituteBack( int_32 startLine, int_32 N );	//上三角回代求方程
	int_32 downSubstituteBack( int_32 startLine, int_32 N );	//下三角回代求方程
private:
	double** m_data;
};

////////////////////////////////////////////////////////////////////////////////
#endif //__COUNTSEVENPARAM_H__