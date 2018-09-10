#ifndef _EMBEDDED_SMART_POINTER_H_
#define _EMBEDDED_SMART_POINTER_H_

namespace Embedded
{
	template <class X> class SmartPointer
	{
	public:
		explicit SmartPointer(X* p = 0) throw() : m_pointer(p) { m_prev = m_next = this; }
		~SmartPointer()	{ release(); }
		SmartPointer(const SmartPointer& p) throw() { acquire(p); }
		SmartPointer& operator=(const SmartPointer& p)
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

	private:
		X* m_pointer;
		mutable const SmartPointer* m_prev;
		mutable const SmartPointer* m_next;

		void acquire(const SmartPointer& p) throw()
		{
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
			if (unique()) delete m_pointer;
			else
			{
				m_prev->m_next = m_next;
				m_next->m_prev = m_prev;
				m_prev = m_next = 0;
			}
			m_pointer = 0;
		}
	};
}

#endif //EMBEDDED
