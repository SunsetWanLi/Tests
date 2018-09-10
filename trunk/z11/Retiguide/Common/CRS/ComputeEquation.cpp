
////////////Matrix class method///////////////////////
#include "ComputeEquation.h"
#include <iostream>
#include <math.h>

using namespace std; 
///////////////////////////////////////////////////////////
myMatrix::myMatrix()
{
	m_matrix.clear();
}

myMatrix::myMatrix(MATRIX matrix)
{
		uint_32 temp = matrix[0].size();
	for(uint_32 i = 1; i < matrix.size(); i++)
	{
		if(matrix[i].size() == temp)
			temp = matrix[i].size();
		else
		{
			m_matrix.clear();
			return;
		}
	}
	m_matrix = matrix;
}

///////////////////////////////////////////////////////////////
int_32 myMatrix::set_Matrix(MATRIX matrix)
{
	uint_32 temp = matrix[0].size();
	for(uint_32 i = 1; i < matrix.size(); i++)
	{
		if(matrix[i].size() == temp)
			temp = matrix[i].size();
		else
			return -1;
	}
	m_matrix = matrix;

	return 0;
}

MATRIX myMatrix::get_Matrix()
{
	return m_matrix;
}

MATRIX myMatrix::get_TransposeMatrix()
{
	MATRIX t_matrix;
	
	for(uint_32 i = 0; i < m_matrix[0].size(); i++)
	{
		vector<double> row;
		for(uint_32 j = 0; j < m_matrix.size(); j++)
			row.push_back(m_matrix[j][i]);

		t_matrix.push_back(row);
	}

	return t_matrix;
}

MATRIX myMatrix::get_multiplyMatrix(MATRIX matrix)
{
	MATRIX t_matrix;

	if(m_matrix[0].size() != matrix.size() )
		t_matrix.clear();

	else
	{
		for(uint_32 i = 0; i < m_matrix.size(); i++)
		{
			vector<double> vec;
			for(uint_32 j = 0; j < matrix[0].size(); j++)
			{
				double temp = 0;
				for(uint_32 col = 0; col < matrix.size(); col++)
				{
					temp += m_matrix[i][col]*matrix[col][j];
				}
				vec.push_back(temp);
			}
			t_matrix.push_back(vec);
		}
	}

	return t_matrix;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
ComputeEquation::ComputeEquation()
{
	m_data = NULL;
}

ComputeEquation::~ComputeEquation()
{
}

int_32 ComputeEquation::set_data(double ** data)
{
	m_data = data;
	return 0;
}

double** ComputeEquation::get_data()
{
	return m_data;
}


int_32 ComputeEquation::sweapLine(int_32 startLine, int_32 N )
{//找主元素，并将主元素所在的行与startPos指定的行对调
	int_32 M_Line = startLine;//记录列主元素所在的行（肯定是第一列）
	int_32 i = startLine;
	for( ; i < N; i++ )
	{//找主元素
		if( m_data[i][startLine] > m_data[M_Line][startLine] )
		{
			M_Line = i;
		}
	}

	if( m_data[M_Line][startLine] < DOUBLE_ZERO && m_data[M_Line][startLine] > -DOUBLE_ZERO )
	{//主元素为0，方程无解
		return -1;
	}

	if( M_Line == startLine ) return 0;//主元素在第一行，什么都不用做

    double temp = 0.0;
	for( i = startLine; i <= N ; i++ )
	{//startLine以前的“列”肯定都是0，所以不必整行交换
		temp = m_data[startLine][i];
		m_data[startLine][i] = m_data[M_Line][i];
		m_data[M_Line][i] = temp;
	}

	return 0;
}

int_32 ComputeEquation::elimLine( int_32 startLine, int_32 N )
{//完成对一列的消元
	//data是方程系数矩阵（增广矩阵），startLine是开始行号，仅计算右下角的部分方程，
	//N是整个方程组的阶数
	if( sweapLine(startLine, N ) != 0 ) return -1;//找主元素并换行
	
	const double ai1 = m_data[startLine][startLine];//用局部变量应该比访问数组快
	double mi1 = 0.0;//可以利用零元素位置存储，但速度更重要
	int_32 i = startLine + 1, j = startLine + 1;
	for( ; i < N; i++ )
	{
		mi1 = m_data[i][startLine]/ai1;
		m_data[i][startLine] = 0.0;//第一列肯定是0
		for( j = startLine + 1; j <= N ; j++ )
		{
			m_data[i][j] -= m_data[startLine][j] * mi1;
		}
	}

	return 0;
}

int_32 ComputeEquation::elimination(int_32 startLine, int_32 N )
{//完成全部消元
	int_32 i = startLine;
	for( ; i < N - 1; i++ )
	{
		if( elimLine(i, N ) != 0 ) return -1;
	}

	return 0;
}
int_32 ComputeEquation::colm_computeEquation(int_32 startLine, int_32 N)
{
	int_32 err = elimination(startLine, N);
	err = topSubstituteBack(0, N);
	
	return err;
}

int_32 ComputeEquation::sqrtElimination(int_32 startLine, int_32 N )
{
	for(int j = startLine; j <= N-1; j++ ) //列
	{
		for(int i = startLine; i <= N-1; i++) //行
		{
			if(i < j)
			{
				m_data[i][j] = 0;
			}
			else if(i == j) //对角线元素
			{
				double tempSum = 0;
				for(int k = 0; k < j; k++)
					tempSum += pow(m_data[j][k], 2);

				m_data[i][j] = sqrt(m_data[i][j] - tempSum);
			}

			else//下三角部分
			{
				double tempFenZi = 0;
				for(int k = 0; k < j; k++)
				{
					tempFenZi += m_data[i][k]*m_data[j][k];
				}

				tempFenZi = m_data[i][j] - tempFenZi;
				m_data[i][j] = tempFenZi/m_data[j][j];

			}
		}
	}
	return 0;
}

int_32 ComputeEquation::topSubstituteBack(int_32 startLine, int_32 N )
{//上三角回代
	int_32 i = N - 1;
	int_32 j;
	//for( ; i >= 0; i-- )
	while( i >= int_32(startLine) )
	{
		for( j = N - 1; j > i; j-- )
		{
			m_data[i][N] -= m_data[i][j] * m_data[j][N];
		}
		m_data[i][N] = m_data[i][N]/m_data[i][i];
		i--;
	}
	return 0;
}

int_32 ComputeEquation::downSubstituteBack(int_32 startLine, int_32 N )
{
	int_32 i = startLine;
	int_32 j;
	while( i <= int_32(N-1) )
	{
		for( j = 0; j < i; j++ )
		{
			m_data[i][N] -= m_data[i][j] * m_data[j][N];
		}
		m_data[i][N] = m_data[i][N]/m_data[i][i];
		i++;
	}
	return 0;
}

int_32 ComputeEquation::sqrt_computeEquation(int_32 startLine, int_32 N)
{
	int_32 err = sqrtElimination(0, N);	//平方根法得到下三角矩阵
	err = downSubstituteBack( 0, N );	//下三角矩阵回带求解
	
	//得到转置矩阵求最终解
	MATRIX matrix;
	matrix.clear();
	for(int i = 0; i < N; i++)
	{
		vector<double> temp;
		for(int j = 0; j < N; j++)
			temp.push_back(m_data[i][j]);
		matrix.push_back(temp);
	}
	
	myMatrix t_matrix;
	t_matrix.set_Matrix(matrix);
	matrix = t_matrix.get_TransposeMatrix();

	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < N; j++)
			m_data[i][j] = matrix[i][j];
	}

	err = topSubstituteBack(0, N);

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
