#include "CRT_Affine.h"

namespace Raster_GIS
{
;
CRT_Affine::CRT_Affine()
{
	// x' = x, y' = y
	/////////////////////////////////////////////////////////////////////
	//Hu DanYuan
	//a----width scale
	//e----height scale
	//c----the negative value of left in dest coordinate
	//f----the negative value of bottom in dest coordinate
	//b----0
	//d----0
	/////////////////////////////////////////////////////////////////////
	a = e = 1;
	b = c = d = f = 0;
}

int_32 CRT_Affine::trans(const Pos2D &src, Pos2D *dest)
{
	dest->d0 = a * src.d0 + b * src.d1 + c;
	dest->d1 = d * src.d0 + e * src.d1 + f;

	return 0;
}
}