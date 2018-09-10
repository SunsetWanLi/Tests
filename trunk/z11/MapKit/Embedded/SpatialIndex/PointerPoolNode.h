#ifndef _EMBEDDED_SPATIALINDEX_POINTER_POOL_NODE_H_
#define _EMBEDDED_SPATIALINDEX_POINTER_POOL_NODE_H_
#include "Node.h"

namespace Embedded
{
	template<> class PointerPool<SpatialIndex::Node>
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
				SpatialIndex::Node* x = m_pool.top(); m_pool.pop();
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

		PoolPointer<SpatialIndex::Node> acquire()
		{
			if (! m_pool.empty())
			{
				SpatialIndex::Node* p = m_pool.top(); m_pool.pop();
#ifndef NDEBUG
				m_hits++;
#endif

				return PoolPointer<SpatialIndex::Node>(p, this);
			}
#ifndef NDEBUG
			else
			{
				// fixme: well sort of...
				m_pointerCount++;
				m_misses++;
			}
#endif

			return PoolPointer<SpatialIndex::Node>();
		}

		void release(SpatialIndex::Node* p)
		{
			if (p != 0)
			{
				if (m_pool.size() < m_capacity)
				{
					if (p->m_pData != 0)
					{
						for (uint_32 cChild = 0; cChild < p->m_children; cChild++)
						{
							// there is no need to set the pointer to zero, after deleting it,
							// since it will be redeleted only if it is actually initialized again,
							// a fact that will be depicted by variable m_children.
							if (p->m_pData[cChild] != 0) delete[] p->m_pData[cChild];
						}
					}

					p->m_identifier = -1;
					p->m_children = 0;

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
		}

		uint_32 getCapacity() const { return m_capacity; }
		void setCapacity(uint_32 c)
		{
			assert (c >= 0);
			m_capacity = c;
		}

	protected:
		uint_32 m_capacity;
		std::stack<SpatialIndex::Node*> m_pool;

#ifndef NDEBUG
	public:
		uint_32 m_hits;
		uint_32 m_misses;
		uint_32 m_pointerCount;
#endif
	};
}

#endif // _EMBEDDED_SPATIALINDEX_POINTER_POOL_NODE_H_
