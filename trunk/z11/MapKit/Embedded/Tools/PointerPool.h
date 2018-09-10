#ifndef _EMBEDDED_POINTER_POOL_H_
#define _EMBEDDED_POINTER_POOL_H_
#include "PoolPointer.h"

using z11::uint_32;

namespace Embedded
{
	template <class X> class PointerPool
	{
	public:
		explicit PointerPool(uint_32 capacity) : m_capacity(capacity)
		{
#ifndef NDEBUG
			m_hits = 0;
			m_misses = 0;
			m_pointerCount = 0;
#endif
		}

		~PointerPool()
		{
			assert(m_pool.size() <= m_capacity);

			while (! m_pool.empty())
			{
				X* x = m_pool.top(); m_pool.pop();
#ifndef NDEBUG
				m_pointerCount--;
#endif
				delete x;
			}

#ifndef NDEBUG
			if(m_pointerCount != 0)
				std::cerr << "Lost pointers: " << m_pointerCount << std::endl;
#endif
		}

		PoolPointer<X> acquire()
		{
			X* p = 0;

			if (! m_pool.empty())
			{
				p = m_pool.top(); m_pool.pop();
#ifndef NDEBUG
				m_hits++;
#endif
			}
			else
			{
				p = new X();
#ifndef NDEBUG
				m_pointerCount++;
				m_misses++;
#endif
			}

			return PoolPointer<X>(p, this);
		}

		void release(X* p)
		{
			if (m_pool.size() < m_capacity)
			{
				m_pool.push(p);
			}
			else
			{
#ifndef NDEBUG
				m_pointerCount--;
#endif
				delete p;
			}

			assert(m_pool.size() <= m_capacity);
		}

		uint_32 getCapacity() const { return m_capacity; }
		void setCapacity(uint_32 c)
		{
			assert (c >= 0);
			m_capacity = c;
		}

	private:
		uint_32 m_capacity;
		std::stack<X*> m_pool;

#ifndef NDEBUG
	public:
		uint_32 m_hits;
		uint_32 m_misses;
		uint_32 m_pointerCount;
#endif
	};
}

#endif // _EMBEDDED_POINTER_POOL_H_