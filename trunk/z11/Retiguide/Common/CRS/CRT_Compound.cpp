
#include "CRT_Compound.h"

namespace GIS_Embedded
{
;

CRT_Compound::CRT_Compound()
{}

CRT_Compound::CRT_Compound(const CRT_Compound &right)
{
	ConstIterType r_it;
	IterType l_it;

	m_crt_list.resize(right.m_crt_list.size());

	for(r_it = right.m_crt_list.begin(), l_it = m_crt_list.begin(); 
		r_it != right.m_crt_list.end(); ++ r_it, ++ l_it)
	{
		*l_it = (*r_it)->copy();
	}
}

CRT_Compound::~CRT_Compound()
{
	for(IterType it = m_crt_list.begin(); it != m_crt_list.end(); ++ it)
	{
		delete *it;
	}

	m_crt_list.clear();
}

bool CRT_Compound::operator ==(const CRT &crt) const
{
	const CRT_Compound *right = dynamic_cast<const CRT_Compound*>(&crt);
	if(!right)
		return false;

	if(m_crt_list.size() != right->m_crt_list.size())
		return false;

	ConstIterType l_it, r_it;
	for(l_it = m_crt_list.begin(), r_it = right->m_crt_list.begin();
		l_it != m_crt_list.end(); ++ l_it, ++ r_it)
	{
		if(**l_it == **r_it)
			continue;
		return false;
	}

	return true;
}

CRT *CRT_Compound::copy() const
{
	return new CRT_Compound(*this);
}

bool CRT_Compound::isInversable() const
{
	if(m_crt_list.size() < 1)
		return false;

	for(ConstIterType it = m_crt_list.begin(); it != m_crt_list.end(); ++ it)
	{
		if((*it)->isInversable() == false)
			return false;
	}

	return true;
}

CRT *CRT_Compound::createInverseCRT() const
{
	if(!isInversable())
		return NULL;

	CRT_Compound *ret = new CRT_Compound();
	ret->m_crt_list.resize(m_crt_list.size());

	ConstIterType o_it;
	IterType n_it;

	for(o_it = m_crt_list.begin(), n_it = ret->m_crt_list.begin();
		o_it != m_crt_list.end(); ++ o_it, ++ n_it)
	{
		*n_it = (*o_it)->createInverseCRT();
	}

	return ret;
}

int_32 CRT_Compound::trans(const Pos3D &src, Pos3D *dest) const
{
	Pos3D work1 = src, work2;
	int_32 ret = 0;

	if(m_crt_list.size() < 1)
		return -1;	//TODO: err-code

	for(ConstIterType it = m_crt_list.begin(); it != m_crt_list.end(); ++ it)
	{
		ret = (*it)->trans(work1, &work2);
		if(ret < 0)
			return ret;

		work1 = work2;
	}

	*dest = work1;

	return ret;
}

}// GIS_Embedded
