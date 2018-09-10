#ifndef GIS_Embeded_CRT_PROJECTION_H_
#define GIS_Embeded_CRT_PROJECTION_H_

#include "CRT.h"
#include "DatumType.h"

namespace GIS_Embedded
{
;

class CRT_Projection : public CRT
{
public:
	CRT_Projection(){}
	virtual ~CRT_Projection(){}

	//virtual CRT* copy() const = 0;

	virtual PrjType prjType() const  = 0;
};

}

#endif //GIS_Embeded_CRT_PROJECTION_H_