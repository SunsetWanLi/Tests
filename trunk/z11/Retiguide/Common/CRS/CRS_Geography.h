#ifndef GIS_Embeded_CRS_GEOGRAPHY_H_
#define GIS_Embeded_CRS_GEOGRAPHY_H_

#include "CRS_Single.h"
 

namespace GIS_Embedded
{

class CRS_Geography : /* public CRS_RealWorld,*/ public CRS_Single/*, public Packable*/
{
public:
	CRS_Geography();
	CRS_Geography(const CRS_Geography &right);
	virtual ~CRS_Geography(){}

	virtual CRS_Type getCrsType() const
	{	return CRS_GEOGRAPHY;	}
	virtual CRS * copy() const;
	virtual bool operator == ( const CRS& crs) const;

	//
	//virtual int_32 packSize(const void *how_to = NULL) const;
	//virtual int_32 pack(byte_8 *buffer, int_32 buffer_size, byte_8 **end_pos, const void *how_to = NULL) const;
	//virtual int_32 pack(byte_8 **result, const void *how_to = NULL)const;
	//virtual int_32 unpack(const byte_8 *buffer, int_32 buf_size = PACK_IGNORE_BUF_SIZE);
public:
	CD_GeodesicDatum m_datum;
};

}

#endif //GIS_Embeded_CRS_GEOGRAPHY_H_