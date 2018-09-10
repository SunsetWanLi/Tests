#ifndef GIS_Embeded_CRS_PROJECT_H_
#define GIS_Embeded_CRS_PROJECT_H_

#include "CRS_Compound.h"
 

namespace GIS_Embedded
{
;
class CRS_Geography;
class CRT_Projection;

class CRS_Project : public CRS_Compound/*, public Packable*/ // public CRS_RealWorld, public Packable
{
public:
	CRS_Project();
	CRS_Project(const CRS_Project &right);

	virtual ~CRS_Project();

	virtual CRS *getRefCRS() const;
	virtual CRT *getRefCRT() const;

	virtual CRS_Type getCrsType() const
	{
		return CRS_PROJECT;
	}
	virtual CRS * copy() const;
	virtual bool operator == ( const CRS& crs) const;
	virtual bool isCompound() const
	{
		return m_ref_crs != NULL;
	}

	void setRefCRS(const CRS_Geography *ref_crs);	// mem alloc
	void setRefCRT(const CRT_Projection *ref_crt);	// mem alloc
	//
	//virtual int_32 packSize(const void *how_to = NULL) const;
	//virtual int_32 pack(byte_8 *buffer, int_32 buffer_size, byte_8 **end_pos, const void *how_to = NULL) const;
	//virtual int_32 pack(byte_8 **result, const void *how_to = NULL)const;
	//virtual int_32 unpack(const byte_8 *buffer, int_32 buf_size = PACK_IGNORE_BUF_SIZE);
public:
	//virtual CRS_SingleType getSingleCrsType() const
	//{	return CRSS_PROJECTED;	}
	//virtual CRS * copy() const;
protected:
	CRS_Geography *m_ref_crs;
	CRT_Projection *m_ref_crt;
};

}

#endif //GIS_Embeded_CRS_PROJECT_H_