
#include "CRT_Affine.h"

namespace GIS_Embedded
{
;
CRT_Affine::CRT_Affine()
{
	// x' = x, y' = y
	a = e = 1;
	b = c = d = f = 0;
}

CRT_Affine::CRT_Affine(const CRT_Affine &right)
{
	a = right.a, b = right.b, c = right.c, d = right.d, e = right.e, f = right.f;
}

bool CRT_Affine::operator == (const CRT &crt) const
{
	const CRT_Affine *right = dynamic_cast<const CRT_Affine *>(&crt);
	return right && 
		a == right->a && b == right->b && c == right->c && 
		d == right->d && e == right->e && f == right->f;
}

CRT *CRT_Affine::copy() const
{
	return new CRT_Affine(*this);
}

bool CRT_Affine::isInversable() const
{
	return a * e != b * d;
}

CRT *CRT_Affine::createInverseCRT() const
{
	double temp = b * d - a * e;
	double aa, bb, cc, dd, ee, ff;

	aa = - e/temp, bb = b/temp, cc = (c * e - b * f)/temp;
	dd = d/temp, ee = - a/temp, ff = (a * f - c * d)/temp;

	CRT_Affine *ret = new CRT_Affine();
	ret->a = aa, ret->b = bb, ret->c = cc, ret->d = dd, ret->e = ee, ret->f = ff;

	return ret;
}

int_32 CRT_Affine::trans(const Pos2D &src, Pos2D *dest)
{
	dest->d0 = a * src.d0 + b * src.d1 + c;
	dest->d1 = d * src.d0 + e * src.d1 + f;
	return 0;
}
int_32 CRT_Affine::trans(const Pos3D &src, Pos3D *dest) const
{
	dest->d0 = a * src.d0 + b * src.d1 + c;
	dest->d1 = d * src.d0 + e * src.d1 + f;
	dest->d2 = src.d2;

	return 0;
}

//int_32 CRT_Affine::packSize(const void *how_to) const
//{
//	return
//		sizeof(PackHead) + 
//		sizeof(int_32) * 2 +
//		sizeof(double) * 6;
//}
//
//int_32 CRT_Affine::pack(byte_8 **result, const void *how_to) const
//{
//	byte_8 *temp;
//	int_32 sz = packSize(how_to);
//
//	*result = new byte_8[sz];
//	return pack(*result, sz, &temp, how_to);
//}
//
//int_32 CRT_Affine::pack(byte_8 *buffer, int_32 buffer_size, byte_8 **end_pos, const void *how_to) const
//{
//	int_32 sz = packSize();
//
//	if(sz > buffer_size)
//	{
//		*end_pos = buffer;
//		return 0;
//	}
//
//	byte_8 *p = buffer;
//	PackHead *ph = (PackHead*)buffer;
//	ph->byte_order = SYS_ENDIAN_TAG;
//	ph->is_segment = false;
//	ph->size = sz;
//	ph->type = PTC_CRT_AFFINE;
//
//	p += sizeof(PackHead);
//	*(int_32*)p = 0;
//	p += sizeof(int_32);
//
//	*(double*)p = a;
//	p += sizeof(double);
//
//	*(double*)p = b;
//	p += sizeof(double);
//
//	*(double*)p = c;
//	p += sizeof(double);
//
//	*(double*)p = d;
//	p += sizeof(double);
//
//	*(double*)p = e;
//	p += sizeof(double);
//
//	*(double*)p = f;
//	p += sizeof(double);
//
//	*(int_32*)p = sz;
//	p += sizeof(int_32);
//
//	*end_pos = p;
//	return sz;
//}
//
//int_32 CRT_Affine::unpack(const byte_8 *buffer, int_32 buf_size)
//{
//	int_32 sz = packSize();
//	if(buf_size != PACK_IGNORE_BUF_SIZE || sz > buf_size)
//		return -1;
//
//	const byte_8 *p = buffer + sizeof(PackHead) + sizeof(int_32);
//
//	a = *(double*)p;
//	p += sizeof(double);
//
//	b = *(double*)p;
//	p += sizeof(double);
//
//	c = *(double*)p;
//	p += sizeof(double);
//
//	d = *(double*)p;
//	p += sizeof(double);
//
//	e = *(double*)p;
//	p += sizeof(double);
//
//	f = *(double*)p;
//	p += sizeof(double);
//
//	return sz;
//}
//
}