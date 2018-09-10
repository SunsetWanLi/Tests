#ifndef GIS_Embeded_CRS_GEOCENTRIC_H_
#define GIS_Embeded_CRS_GEOCENTRIC_H_

#include "CRS_Single.h"
 

namespace GIS_Embedded
{
;

class CRS_Geocentric : public CRS_Single/*, public Packable*/
{
public:
	CRS_Geocentric();
	CRS_Geocentric(const CRS_Geocentric &right);
	virtual ~CRS_Geocentric(){}

	virtual CRS_Type getCrsType() const
	{
		return CRS_GEOCENTRIC;
	}
	virtual CRS * copy() const;
	virtual bool operator == ( const CRS& crs) const;

	//virtual int_32 packSize(const void *how_to = NULL) const;
	//virtual int_32 pack(byte_8 *buffer, int_32 buffer_size, byte_8 **end_pos, const void *how_to = NULL) const;
	//virtual int_32 pack(byte_8 **result, const void *how_to = NULL)const;
	//virtual int_32 unpack(const byte_8 *buffer, int_32 buf_size = PACK_IGNORE_BUF_SIZE);
public:
	CD_GeodesicDatum m_datum;
	//GCSType m_geo_crs_type;
};

} // namespace

#endif //GIS_Embeded_CRS_GEOCENTRIC_H_