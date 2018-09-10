
#include "CRT.h"
#include <assert.h>
#include <iostream>
namespace GIS_Embedded{

//CRT::CRT() : _crtType(CRT_TRANSFORMATION)
//{}

//CRT::CRT(CRT_Type crtType ) : _crtType(crtType)
//{}

////////////////////////////////////////////////////////////////////////////
// safe when dest and src is the same vector
int_32 CRT::batchTrans(const vector<Pos2D> &src, vector<Pos2D> *dest) const
{
	dest->reserve(src.size() + 1); // 面不成环的话可能要再加一个点，优化
	dest->resize(src.size());
	Pos3D src3d, dest3d;

	src3d.d2 = dest3d.d2 = 0;

	for(uint_32 i = 0; i < src.size(); i ++)
	{
		*(Pos2D*)&src3d = src[i];
		src3d.d2 = 0;
		trans(src3d, &dest3d);
		dest->at(i) = *(Pos2D*)&dest3d;
	}

	return 0;
}

// safe when dest and src is the same vector
int_32 CRT::batchTrans(const vector<Pos3D> &src, vector<Pos3D> *dest) const
{
	dest->reserve(src.size() + 1); // 面不成环的话可能要再加一个点，优化
	dest->resize(src.size());

	for(uint_32 i = 0; i < src.size(); i ++)
	{
		trans(src[i], &dest->at(i));
	}

	return 0;
}

int_32 CRT::trans(const Pos2D &src, Pos2D *dest) const
{
	Pos3D s, d;
	int_32 ret = 0;

	*(Pos2D*)&s = src;
	s.d2 = 0;
	ret = trans(s, &d);

	*dest = *(Pos2D*)&d;

	return ret;
}

}//namespace GIS_Embedded
