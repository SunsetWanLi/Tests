#pragma once
#ifndef _MG_SINGLETON_H_
#define _MG_SINGLETON_H_
#include "CommonDef.h"
namespace z11
{
	template <class Type>
	class MGSingleton
	{
	public:
		static Type *get()
		{
			if (!m_created) //if m_created is true ,other thread has created
			{
				m_created=true;
				m_instance = new Type();
			}
			while (m_instance==NULL) //other thread is creating this object
			{
			}
			return m_instance;
		}
		static void release()
		{
			delete m_instance;
			m_instance=NULL;
			m_created=false;
		}
		static bool isExist()
		{
			return m_instance!=NULL;
		}
	private:
		static bool m_created;
		static Type *m_instance;
	};

	template <class Type>
	bool MGSingleton<Type>::m_created=false;
	template <class Type>
	Type* MGSingleton<Type>::m_instance=NULL;
}

#endif
