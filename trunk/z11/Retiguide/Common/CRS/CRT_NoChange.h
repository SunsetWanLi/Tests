#ifndef GIS_Embeded_CRT_NO_CHANGE_H_
#define GIS_Embeded_CRT_NO_CHANGE_H_

#include "CRT.h"
 

namespace GIS_Embedded
{
;

class CRT_NoChange : public CRT/*, public Packable*/
{
public:
	CRT_NoChange() {}
	CRT_NoChange(const CRT_NoChange &right){}
	virtual ~CRT_NoChange() {}
	virtual bool operator == ( const CRT &crt) const;
	virtual CRT *copy() const	{	return new CRT_NoChange;	}
	virtual bool isInversable() const {	return true;	}
	virtual CRT * createInverseCRT() const {	return copy();	}

	virtual int_32 trans(const Pos3D &src, Pos3D *dest) const;

	//virtual int_32 packSize(const void *how_to = NULL) const;
	//virtual int_32 pack(byte_8 *buffer, int_32 buffer_size, byte_8 **end_pos, const void *how_to = NULL) const;
	//virtual int_32 pack(byte_8 **result, const void *how_to = NULL)const;
	//virtual int_32 unpack(const byte_8 *buffer, int_32 buf_size = PACK_IGNORE_BUF_SIZE);
};

}// GIS_Embedded

#endif //GIS_Embeded_CRT_NO_CHANGE_H_