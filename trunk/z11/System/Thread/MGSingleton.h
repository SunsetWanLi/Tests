#pragma once
#ifndef _MG_SINGLETON_H_
#define _MG_SINGLETON_H_
#include "../../CommonBase/Memory/MGRefObject.h"
#include "MGMutex.h"
namespace z11
{
	template <class Type>
	class MGSingleton
	{
	public:
		static Ref<Type> get()
		{
			if (m_instance.empty())
			{
				MGAutoLock(&m_guard);
				if (m_instance.empty())
				{
					m_instance =  Ref<Type>(new Type());
				}
			}
			return m_instance;
		}
		static void release()
		{
			m_instance=NULL;
		}
		static bool exists()
		{
			return !m_instance.empty();
		}
	private:
		static MGMutex m_guard;
		static Ref<Type> m_instance;
	};

	template <class Type>
	MGMutex MGSingleton<Type>::m_guard;
	template <class Type>
	Ref<Type> MGSingleton<Type>::m_instance;
}

#endif
