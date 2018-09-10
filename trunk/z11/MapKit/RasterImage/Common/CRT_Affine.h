#ifndef __RASTERIMAGE_CRT_AFFINE_H__
#define __RASTERIMAGE_CRT_AFFINE_H__
#include "basic_gm_type.h"

namespace Raster_GIS
{
;

class CRT_Affine
{
public:
	CRT_Affine();
	int_32 trans(const Pos2D &src, Pos2D *dest);
public:
	double a, b, c, d, e, f;
};

}//IS_GIS

#endif //IS_GIS_CRT_AFFINE_H_