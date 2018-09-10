#ifndef GIS_Embeded_CRS_COMPOUND_H_
#define GIS_Embeded_CRS_COMPOUND_H_

#include "CRS.h"

namespace GIS_Embedded
{
;
class CRT;

class CRS_Compound : public CRS
{
public:
	virtual ~CRS_Compound(){}
	virtual CRS *getRefCRS() const = 0;
	virtual CRT *getRefCRT() const = 0;
protected:
//	CRS *m_ref_crs;
//	CRS *m_ref_crt;
};

} // namespace

#endif //GIS_Embeded_CRS_COMPOUND_H_