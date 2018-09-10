#include "MGRefObject.h"
#include "../CommonDef.h"
//#include "../../System/Thread/MGMutex.h"
#include <assert.h>
namespace z11
{
	MGRefObject::MGRefObject( void )
	{
		static unsigned int uObjectCount = 0;
		m_uID = ++uObjectCount;
		m_uReference = 1;
		m_bManaged = false;
		//locker=new MGMutex();
	}

	MGRefObject::~MGRefObject( void )
	{
		if (m_bManaged)
		{
			//do nothing 
		}
		//delete_and_set_null(locker);
	}

	void MGRefObject::release( void )
	{
		//locker->lock();
		-- m_uReference;
		if (m_uReference==0)
		{
			//locker->unlock();
			delete this;
		}
	}

	void MGRefObject::retain( void )
	{
		//locker->lock();
		++ m_uReference;
		//locker->unlock();
	}

	//MGRefObject* MGRefObject::autorelease( void )
	//{
	//	assert(0);
	//	return this;
	//}

	//MGRefObject* MGRefObject::copy( void )
	//{
	//	return NULL;
	//}

	bool MGRefObject::isSingleRefrence( void )
	{
		return m_uReference==1;
	}

	unsigned int MGRefObject::retainCount( void )
	{
		return m_uReference;
	}

	bool MGRefObject::isEqual( const MGRefObject* pObject ) const
	{
		return this == pObject;
	}

	void MGRefObject::encodeWithCoder( MGCoder *encoder )
	{

	}

	MGRefObject* MGRefObject::initWithCoder( const MGCoder *decoder )
	{
		return this;
	}

	Ref<MGRefObject> MGRefObject::mutableCopy()
	{
		return MGRefObjectRef(this);
	}

	Ref<MGRefObject> MGRefObject::copy()
	{
		return MGRefObjectRef(this);
	}
}