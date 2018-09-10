#pragma once
#include "../NonCopyable.h"
namespace z11
{
	// never throws
	template<class T> class ScopedPtr : NonCopyable {

	public:
		typedef T element_type;

		explicit ScopedPtr(T * p = 0)
		{
			pointer=p;
		}
		~ScopedPtr()
		{
			if (pointer!=NULL)
			{
				delete pointer;
				pointer=NULL;
			}
		}

		void reset(T * p = 0)
		{
			if (pointer!=NULL)
			{
				delete pointer;
				pointer=NULL;
			}
			pointer=p;
		}

		T & operator*() const
		{
			return *pointer;
		}
		T * operator->() const
		{
			return pointer;
		}
		T * get() const
		{
			return pointer;
		}

		operator bool() const
		{
			return pointer!=NULL;
		}

		void swap(ScopedPtr & b)
		{
			if (&b!=this)
			{
				T *tmp=NULL;
				tmp=this->pointer;
				this->pointer=b.pointer;
				b.pointer=tmp;
			}
		}
	private:
		T* pointer;
	};

	template<class T> void swap(ScopedPtr<T> & a, ScopedPtr<T> & b)
	{
		a.swap(b);
	}

}