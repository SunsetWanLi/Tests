#ifndef GIS_Embeded_CRT_FACTORY_H_
#define GIS_Embeded_CRT_FACTORY_H_

#include "extern.h"
#include "DatumType.h"
#include "CRS.h"
#include "CRT.h"

namespace GIS_Embedded
{
;

class CRS;
class CRT;
//enum 

class CRT_Factory
{
protected:
	struct CRSChainNode
	{
		CRS *m_from;
		CRT *m_how;
	};
	typedef vector<CRSChainNode> CRSChain;

public:
	virtual ~CRT_Factory(){}
	//int_32 createCRT(const CRS &src, const CRS &dest, CRT **result, CRT_Type type = CRT_UNKNOWN); // mem alloc
	//int_32 createCRT(CRT_Type type, CRT **result, bool inverse = false);
	static int_32 createCRT(const CRS &src, const CRS &dest, CRT **result) ;
protected:
	//bool checkCRTFitCRS(const CRS &src, const CRS &dest, CRT_Type type) const;
	static int_32 getCRSChain(const CRS &crs, CRSChain *result) ;
	static int_32 findFromCRSNode(const CRS &crs, CRSChainNode *result) ;
	static int_32 findInCRSChain(const CRS &crs, const CRSChain &chain, int_32 *pos) ;
	static int_32 buildCRT(const CRSChain &from_chain, const CRSChain &to_chain, int_32 dest_chain_pos, CRT **crt)  ;
	static void releaseChain(CRSChain *chain) ;
};

}

#endif //GIS_Embeded_CRT_FACTORY_H_