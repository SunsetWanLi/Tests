#pragma once
#ifndef _MI_INSTANCE_H_
#define _MI_INSTANCE_H_
#include "../../CommonBase/MGStructure.h"
#include "../../System/Device/MGDevice.h"
#include "../Controllers/MIViewController.h"
namespace z11
{
	/*********************************************************************************************
		由于View由Controller管理，所以外界要对view进行操作，必须靠Controller来间接完成，这样实现了控制和界面分离，
		外界通过MIControllerInstance的getInstance()来获取Controller实例，必须通过exist先查询状态        
	********************************************************************************************/

	//适用于页面销毁  
	template <class Type>
	class MIViewFactory
	{
	public:
		static Type *create(MIViewController*controller,const z11::MGRect &rect)
		{
			Type * t = new Type;
			t->delegate=controller;
			t->initWithFrame(rect);
			return t;
		}
		static Type *create(MIViewController*controller )
		{
			return create(controller,z11::MGRectMake(0.0f,0.0f,MGDevice::getScreenWidth()*1.0f,MGDevice::getScreenHeight()*1.0f));
		}

	};

	//MIControllerInstance适用于Controller只有一个实例情形       
	template <class Type>
	class MIControllerInstance
	{
	public:
        // 如果设置了不自动初始化，那么必须手动调用initManually()以确保正确初始化 //
		static Type *getInstance(bool auto_init = true)
		{
			if (!m_created) //if m_created is true ,other thread has created 
			{
				m_created=true;
 				m_instance = new Type;
                if (auto_init && !m_initiallized) {
                    m_instance->initManually();
                    m_initiallized = true;
                }
			}
			while (m_instance==NULL) //waiting for other thread creates this object
			{
			}
			return m_instance;
		}
		static void releaseInstance()
		{
			Type * saved=m_instance;
			clear();
			delete_and_set_null(saved);
		}
		static void clear()
		{
			if (m_instance!=NULL&&m_instance->view!=NULL)
			{
				m_instance->view->detachFromSuperview();
				delete_and_set_null(m_instance->view);
			}
			m_instance=NULL;
			m_created=false;
            m_initiallized = false;
		}
		static bool exists()
		{
			return m_created;
		}
        static bool initiallized()
        {
            return m_initiallized;
        }
	protected:
		static Type *m_instance;
		static bool m_created;
        static bool m_initiallized;
	};
	template <class Type>
	Type* MIControllerInstance<Type>::m_instance=NULL;
	template <class Type>
	bool MIControllerInstance<Type>::m_created=false;
    template <class Type>
	bool MIControllerInstance<Type>::m_initiallized=false;

}

#endif
