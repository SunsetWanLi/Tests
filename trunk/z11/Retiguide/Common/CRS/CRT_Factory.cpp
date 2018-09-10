
// CRT_Factory.cpp

#include "CRT_Factory.h"
#include "DatumType.h"
#include "CRS.h"
#include "CRS_Compound.h"
#include "CRS_Geography.h"
#include "CRT.h"
#include "CRT_Compound.h"
#include "CRT_NoChange.h"

namespace GIS_Embedded
{
;

//int_32 CRT_Factory::createCRT(const CRS &src, const CRS &dest, CRT **result, CRT_Type type)
//{
//	int_32 ret = 0;
//
//	switch(type)
//	{
//	case CRT_CONVERSION:
//		break;
//	case CRT_TRANSFORMATION:
//		break;
//	case CRT_PRJ_GAUSS:
//	case CRT_PRJ_GAUSS_INVERSE:
//	case CRT_PRJ_GAUSS_ZONED:
//	case CRT_PRJ_MERCATOR:
//	case CRT_PRJ_MERCATOR_TRANVERSE:
//	case CRT_PRJ_LAMBERT:
//	case CRT_PRJ_LAMBERT_CONFORMAL:
//		if(!checkCRTFitCRS(src, dest, type))
//		{
//			*result = NULL;
//			ret = -1;
//		}
//		else
//		{
//			*result = createCRT_Prj(type);
//		}
//		break;
//	case CRT_PROJECTION:
//		break;
//	case CRT_UNKNOWN:
//		break;
//	}
//
//	return ret;
//}
int_32 CRT_Factory::createCRT(const CRS &src, const CRS &dest, CRT **result) 
{
	int_32 ret = 0;
	*result = NULL;
	int_32 chain_pos = 0;

	if(src == dest)
	{
		*result = new CRT_NoChange();
		return 0;
	}

	CRSChain dest_chain;
	CRSChain work_chain;

	CRSChainNode work_node;
	CRS *last_crs = src.copy();

	getCRSChain(dest, &dest_chain);

	while(1)
	{
		findInCRSChain(*last_crs, dest_chain, &chain_pos);
		if(chain_pos >= 0) // found
		{
			buildCRT(work_chain, dest_chain, chain_pos, result);
			break;
		}
		else
		{
			if(last_crs->isCompound())
			{
				CRS_Compound *crs_compound = dynamic_cast<CRS_Compound*>(last_crs);
				if(crs_compound->getRefCRT()->isInversable())
				{
					work_node.m_from = last_crs;
					work_node.m_how = crs_compound->getRefCRT()->createInverseCRT();
					work_chain.push_back( work_node );
					last_crs = crs_compound->getRefCRS()->copy();
					continue;
				}
			}

			break;
		}
	}

	// TODO:
	// no direct relationship, try geography->geocentric->geocentric->geography

	delete last_crs;
	releaseChain(&dest_chain);
	releaseChain(&work_chain);

	if(!*result)
	{
		*result = new CRT_NoChange();
	}

	return ret;
}

int_32 CRT_Factory::findFromCRSNode(const CRS &crs, CRSChainNode *result) 
{
	if(crs.isCompound())
	{
		const CRS_Compound * crs_compound = dynamic_cast<const CRS_Compound*>(&crs);
		result->m_from = crs_compound->getRefCRS()->copy();
		result->m_how = crs_compound->getRefCRT()->copy();

		return 0;
	}
	else
	{
		result->m_from = NULL;
		result->m_how = NULL;

		return -1;
	}
	//else
	//{
	//	if(crs.getCrsType() == CRS_GEOGRAPHY)
	//	{
	//		// TO GEOCENTRIC
	//		const CRS_Geography *crs_g = dynamic_cast<const CRS_Geography*>(&crs);
	//		CRS_Geocentric *crs_c = new CRS_Geocentric();
	//		crs_c.m_datum = crs_g->m_datum;
	//		
	//		CRT_Conversion *crt = new CRT_Conversion();
	//		crt->setGeocentricEllipsoid(crs_c->m_datum._ellipsoid);
	//		crt->m_is_geocentric_to_geographic = true;

	//		result->m_from = crs_c;
	//		result->m_how = crt;

	//		return 0;
	//	}
	//	else
	//	{
	//		result->m_from = NULL;
	//		result->m_how = NULL;

	//		return 1;	//TODO: err_code
	//	}
	//}
}

int_32 CRT_Factory::getCRSChain(const CRS &crs, CRSChain *result) 
{
	CRSChainNode node, node1;

	node.m_from = crs.copy();
	node.m_how = NULL;

	result->clear();
	result->push_back(node);

	while(findFromCRSNode(*node.m_from, &node1) == 0)
	{
		node = node1;
		result->push_back(node);
	}

	return 0;
}

int_32 CRT_Factory::findInCRSChain(const CRS &crs, const CRSChain &chain, int_32 *pos) 
{
	for(uint_32 i = 0; i < chain.size(); i ++)
	{
		if(*chain[i].m_from == crs)
		{
			*pos = i;
			return i;
		}
	}

	*pos = -1;
	return -1;
}

void CRT_Factory::releaseChain(CRSChain *chain) 
{
	for(uint_32 i = 0; i < chain->size(); i ++)
	{
		delete chain->at(i).m_from;
		delete chain->at(i).m_how;
	}

	chain->clear();
}

int_32 CRT_Factory::buildCRT(const CRSChain &from_chain, const CRSChain &to_chain, int_32 dest_chain_pos, CRT **crt) 
{
	CRT_Compound crt_compound;
	for(uint_32 i = 0; i < from_chain.size(); i ++)
	{
		crt_compound.m_crt_list.push_back(from_chain[i].m_how->copy());
	}
	for(uint_32 i = dest_chain_pos; i > 0; i -- )
	{
		if(to_chain[i].m_how)
			crt_compound.m_crt_list.push_back(to_chain[i].m_how->copy());
	}

	if(crt_compound.m_crt_list.size() == 1)
	{
		*crt = crt_compound.m_crt_list[0];
		crt_compound.m_crt_list.clear();
	}
	else if(crt_compound.m_crt_list.size() == 0)
	{
		//TODO: build a 'no change' crt??
		*crt = NULL;
	}
	else
	{
		*crt = crt_compound.copy();
	}

	return 0;
}

} // GIS_Embedded
