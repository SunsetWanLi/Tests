#ifndef GIS_Embeded_CRS_GENIRIC_H_
#define GIS_Embeded_CRS_GENIRIC_H_

#include "CRS_Compound.h"
 

namespace GIS_Embedded
{
;

class CRT;

class CRS_Generic : public CRS_Compound/*, public Packable*/
{
public:
	CRS_Generic();
	CRS_Generic(const CRS_Generic &right);

	virtual ~CRS_Generic();

	virtual CRS *getRefCRS() const;
	virtual CRT *getRefCRT() const;

	virtual CRS_Type getCrsType() const
	{	return CRS_GENERIC;	}

	virtual CRS * copy() const;
	virtual bool operator == ( const CRS& crs) const;
	virtual bool isCompound() const;

	void setRefCRS(const CRS *ref_crs);	// mem alloc
	void setRefCRT(const CRT *ref_crt);	// mem alloc

	//virtual int_32 packSize(const void *how_to = NULL) const;
	//virtual int_32 pack(byte_8 *buffer, int_32 buffer_size, byte_8 **end_pos, const void *how_to = NULL) const;
	//virtual int_32 pack(byte_8 **result, const void *how_to = NULL)const;
	//virtual int_32 unpack(const byte_8 *buffer, int_32 buf_size = PACK_IGNORE_BUF_SIZE);
public:
	CRS *m_ref_crs;
	CRT *m_ref_crt;
};

} //GIS_Embedded

#endif //GIS_Embeded_CRS_GENIRIC_H_