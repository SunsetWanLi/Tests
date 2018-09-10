
#include "CRT_NoChange.h"

namespace GIS_Embedded
{
;

bool CRT_NoChange::operator == (const CRT &crt) const
{
	const CRT *right = dynamic_cast<const CRT_NoChange*>(&crt);
	return right != NULL;
}

int_32 CRT_NoChange::trans(const Pos3D &src, Pos3D *dest) const
{
	*dest = src;
	return 0;
}

//int_32 CRT_NoChange::packSize(const void *how_to) const
//{
//	return sizeof(PackHead) + sizeof(int_32);
//}
//
//int_32 CRT_NoChange::pack(byte_8 **result, const void *how_to) const
//{
//	byte_8 *temp;
//	int_32 sz = packSize();
//	*result = new byte_8[sz];
//	return pack(*result, sz, &temp, how_to);
//}
//
//int_32 CRT_NoChange::pack(byte_8 *buffer, int_32 buffer_size, byte_8 **end_pos, const void *how_to)const
//{
//	int_32 sz = packSize();
//	if(buffer_size < sz)
//	{
//		*end_pos = buffer;
//		return 0;
//	}
//
//	PackHead *ph = (PackHead*)buffer;
//	ph->byte_order = SYS_ENDIAN_TAG;
//	ph->is_segment = false;
//	ph->size = sz;
//	ph->type = PTC_CRT_NOCHANGE;
//
//	*(int_32*)(buffer + sizeof(PackHead)) = sz;
//	// warning: how_to is not packed!
//
//	*end_pos = buffer + sz;
//	return sz;
//}
//
//int_32 CRT_NoChange::unpack(const byte_8 *buffer, int_32 buf_size)
//{
//	int_32 sz = packSize();
//	if(buf_size != PACK_IGNORE_BUF_SIZE && buf_size < sz)
//	{
//		return -1;
//	}
//
//	return sz;
//}

}//GIS_Embedded
