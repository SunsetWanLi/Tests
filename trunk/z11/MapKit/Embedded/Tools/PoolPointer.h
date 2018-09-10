#ifndef _EMBEDDED_POOL_POINTER_H_
#define _EMBEDDED_POOL_POINTER_H_
#include "PointerPool.h"

namespace Embedded
{
	template <class X> class PointerPool;

	template <class X> class PoolPointer
	{
	public:
		explicit PoolPointer(X* p = 0) : m_pointer(p), m_pPool(0) { m_prev = m_next = this; }
		explicit PoolPointer(X* p, PointerPool<X>* pPool) throw() : m_pointer(p), m_pPool(pPool) { m_prev = m_next = this; }
		~PoolPointer() { release(); }
		PoolPointer(const PoolPointer& p) throw() { acquire(p); }
		PoolPointer& operator=(const PoolPointer& p)
		{
			if (this != &p)
			{
				release();
				acquire(p);
			}
			return *this;
		}

		X& operator*() const throw() { return *m_pointer; }
		X* operator->() const throw() { return m_pointer; }
		X* get() const throw() { return m_pointer; }
		bool unique() const throw() { return m_prev ? m_prev == this : true; }
		void relinquish() throw()
		{
			m_pPool = 0;
			m_pointer = 0;
			release();
		}

	private:
		X* m_pointer;
		mutable const PoolPointer* m_prev;
		mutable const PoolPointer* m_next;
		PointerPool<X>* m_pPool;

		void acquire(const PoolPointer& p) throw()
		{
			m_pPool = p.m_pPool;
			m_pointer = p.m_pointer;
			m_next = p.m_next;
			m_next->m_prev = this;
			m_prev = &p;
#ifndef mutable
			p.m_next = this;
#else
			(const_cast<linked_ptr<X>*>(&p))->m_next = this;
#endif
		}

		void release()
		{
			if (unique())
			{
				if (m_pPool != 0) m_pPool->release(m_pointer);
				else delete m_pointer;
			}
			else
			{
				m_prev->m_next = m_next;
				m_next->m_prev = m_prev;
				m_prev = m_next = 0;
			}
			m_pointer = 0;
			m_pPool = 0;
		}
	};
}

#endif // _EMBEDDED_POOL_POINTER_H_
