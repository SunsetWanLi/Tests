
#include "CRT_Transformation.h"
#include "ComputeEquation.h"
#include <assert.h>

namespace GIS_Embedded
{
;

CRT_Transformation CRT_Transformation::inverse() const
{
	CRT_Transformation crt_Trans;

	//crt_Trans._crtType = this->_crtType;
	//crt_Trans._crsSourceName = this->_crsTargetName;
	//crt_Trans._crsTargetName = this->_crsSourceName;
	crt_Trans._blThreePara = this->_blThreePara;
	
	crt_Trans._dShiftX = -(this->_dShiftX);
	crt_Trans._dShiftY = -(this->_dShiftY);
	crt_Trans._dShiftZ = -(this->_dShiftZ);

	crt_Trans._dRotateX = -(this->_dRotateX);
	crt_Trans._dRotateY = -(this->_dRotateY);
	crt_Trans._dRotateZ = -(this->_dRotateZ);
	
	crt_Trans._dScale = 1 - 1/(1 - (this->_dScale)); 

	return crt_Trans;
}

bool CRT_Transformation::operator == (const CRT &right) const
{
	const CRT_Transformation * _right = dynamic_cast<const CRT_Transformation *>(&right);
	if(!_right)
		return false;
	return /*_crtType == _right->_crtType 
		   &&*/ _dShiftX == _right->_dShiftX && _dShiftY == _right->_dShiftY && _dShiftZ == _right->_dShiftZ
		   && _dRotateX == _right->_dRotateX && _dRotateY == _right->_dRotateY && _dRotateZ == _right->_dRotateZ
		   && _dScale == _right->_dScale 
		   && _blThreePara == _right->_blThreePara;
}

GIS_Embedded::Pos3D CRT_Transformation::transDirectionPosion(const GIS_Embedded::Pos3D& posSource ) const
{
	//本计算有如下假设
	//一般认为三个旋转角均为小角度，可以认为：sin(旋转角) = 0 ,cos(旋转角) = 1 
	//则有
	//                                [  1   rZ  -rY]
	//    R[3*3] = R(rX)*R(rY)*R(rZ) =[ -rZ  1    rX]
	//                                [  rY  -rX  1 ]
	//
	// PosN[3] = Shift[3] + (1 + m) * R[3*3] * PosO[3]

	GIS_Embedded::Pos3D posTarget;

	if(_blThreePara == false)
	{
		posTarget.d0 = posSource.d0 + _dShiftX - posSource.d2*_dRotateY + posSource.d1*_dRotateZ + posSource.d0*_dScale;
		posTarget.d1 = posSource.d1 + _dShiftY + posSource.d2*_dRotateX - posSource.d0*_dRotateZ + posSource.d1*_dScale;
		posTarget.d2 = posSource.d2 + _dShiftZ - posSource.d1*_dRotateX + posSource.d0*_dRotateY + posSource.d2*_dScale;
	}
	else
	{
		posTarget.d0 = _dShiftX + posSource.d0;
		posTarget.d1 = _dShiftY + posSource.d1;
		posTarget.d2 = _dShiftZ + posSource.d2;
	}
	return posTarget;
}
void CRT_Transformation::setParameters( bool blThreePara , double dShiftX , double dShiftY , double dShiftZ ,
				   double dRotateX , double dRotateY , double dRotateZ , double dScale )
{
	_blThreePara = blThreePara;
	_dShiftX = dShiftX;
	_dShiftY = dShiftY;
	_dShiftZ = dShiftZ;
	_dRotateX = dRotateX;
	_dRotateY = dRotateY;
	_dRotateZ = dRotateZ;
	_dScale = dScale;
	return;
}

void CRT_Transformation::setSevenPara(vector<GIS_Embedded::Pos3D>& posSourceArray, vector<GIS_Embedded::Pos3D>& posTargetArray)
{
	_blThreePara = false;
	assert(posSourceArray.size() == posTargetArray.size() );
	//首先把该方程系数调整到同一个数量级上，以免构成病态方程
	for(uint_32 i = 0; i < posSourceArray.size(); i++)
	{
		posSourceArray[i].d0 /= 1e+6;	posSourceArray[i].d1 /= 1e+6;	posSourceArray[i].d2 /= 1e+6;
		posTargetArray[i].d0 /= 1e+6;	posTargetArray[i].d1 /= 1e+6;	posTargetArray[i].d2 /= 1e+6;
	}

	myMatrix t_matrix;
	MATRIX matrix, A, B;	//matrix用来存储中间变量， A，B矩阵构成方程A*X = B
	vector<double> temp;

	for(uint_32 i = 0; i < posSourceArray.size(); i++)
	{
		temp.push_back(0);	temp.push_back(-posTargetArray[i].d2);	
		temp.push_back(posTargetArray[i].d1);	temp.push_back(posTargetArray[i].d0);
		temp.push_back(1);	temp.push_back(0);	temp.push_back(0);	matrix.push_back(temp);	temp.clear();
		temp.push_back(posTargetArray[i].d2);	temp.push_back(0);
		temp.push_back(-posTargetArray[i].d0);	temp.push_back(posTargetArray[i].d1);
		temp.push_back(0);	temp.push_back(1);	temp.push_back(0);	matrix.push_back(temp);	temp.clear();
		temp.push_back(-posTargetArray[i].d1);	temp.push_back(posTargetArray[i].d0);
		temp.push_back(0);	temp.push_back(posTargetArray[i].d2);
		temp.push_back(0);	temp.push_back(0);	temp.push_back(1);	matrix.push_back(temp);	temp.clear();
	}
	//计算A
	t_matrix.set_Matrix(matrix);
	A = t_matrix.get_TransposeMatrix();  //得到转置矩阵	
	t_matrix.set_Matrix(A);	
	A = t_matrix.get_multiplyMatrix(matrix); //最小二乘法原来系数的转置*原来系数矩阵 = 新的有解方程得系数矩阵A

	//计算B
	matrix.clear();
	for(uint_32 i = 0; i < posTargetArray.size(); i++)
	{
		temp.push_back(posTargetArray[i].d0-posSourceArray[i].d0);	matrix.push_back(temp);	temp.clear();
		temp.push_back(posTargetArray[i].d1-posSourceArray[i].d1);	matrix.push_back(temp);	temp.clear();
		temp.push_back(posTargetArray[i].d2-posSourceArray[i].d2);	matrix.push_back(temp);	temp.clear();
	}

	B = t_matrix.get_multiplyMatrix(matrix);//原来系数矩阵的转置*原来的后面值矩阵 = 新的值矩阵（构成增广矩阵的最后一列）

	int_32 N = 7;	//设置解方程要用到的矩阵
	double** data = new double*[N];
	for(int_32 i = 0; i < N; i++ )
	{
		data[i] = new double [N+1];
	}

	for(int m = 0; m < N; m++)
	{   
		double item;
		for(int j = 0; j < N; j++)
		{
			item = A[m][j];
			data[m][j] = item;
		}

		item = B[m][0];
		data[m][N] = item;
	}

	//解该方程
	ComputeEquation myComputer;
	myComputer.set_data(data);
	//int_32 err = myComputer.colm_computeEquation( 0, N );	//列主元素法求方程解
	int_32 err = myComputer.sqrt_computeEquation(0, N );//平方根算法求解
	data = myComputer.get_data();	//解在data的最后一列

	_dRotateX = data[0][7];	_dRotateY = data[1][7];	_dRotateZ = data[2][7];	_dScale	= data[3][7];
	_dShiftX  = data[4][7]*1e+6;	_dShiftY  = data[5][7]*1e+6;	_dShiftZ  = data[6][7]*1e+6;

	for(int_32 i = 0; i < N; i++ )
	{
		delete data[i];
	}
	delete data;	data = NULL;
	return;
}

void CRT_Transformation::setThreePara(GIS_Embedded::Pos3D& posSource,GIS_Embedded::Pos3D& posTarget)
{
	_blThreePara = true;
	_dRotateX = 0; _dRotateY = 0; _dRotateZ = 0; _dScale = 0;
	_dShiftX = posTarget.d0 - posSource.d0;
	_dShiftY = posTarget.d1 - posSource.d1;
	_dShiftZ = posTarget.d2 - posSource.d2;
	return;
}

int_32 CRT_Transformation::trans(const Pos3D &src, Pos3D *dest) const
{
	*dest = transDirectionPosion(src);

	return 0;
}

CRT *CRT_Transformation::copy() const
{
	return new CRT_Transformation(*this);
}

} // GIS_Embedded
