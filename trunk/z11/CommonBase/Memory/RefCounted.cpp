#include "RefCounted.h"
#ifndef NDEBUG
#include "../System/Shared/MGLog.h"
#endif
namespace z11
{

	RefCountedBase::RefCountedBase():ref_count_(0)
#ifndef NDEBUG
		, in_dtor_(false)
#endif
	{

	}

	RefCountedBase::~RefCountedBase()
	{
#ifndef NDEBUG
		MGCheckD(in_dtor_,"RefCounted object deleted without calling Release()");
#endif
	}

	void RefCountedBase::AddRef() const
	{
#ifndef NDEBUG
		MGCheckD(!in_dtor_,"RefCounted object has deleted when AddRef");
#endif
		++ref_count_;
	}

	bool RefCountedBase::Release() const
	{
#ifndef NDEBUG
		MGCheckD(!in_dtor_,"RefCounted object has deleted when Release");
#endif
		if (--ref_count_==0)
		{
#ifndef NDEBUG
			in_dtor_=true;
#endif
			return true;
		}
		return false;
	}

}
