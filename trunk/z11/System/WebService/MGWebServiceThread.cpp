#include "MGWebServiceThread.h"
#include "../Shared/MGLog.h"
#include "../../GUI/GuiContainer/GuiContainer.h"
#include "../../GUI/Events/MIEvent.h"
namespace z11
{
	//MGMutex MGWebServiceThread::pool_locker;
	//std::set<MGWebServiceThread *> MGWebServiceThread::m_pool;
	//std::set<MGWebServiceThread *> MGWebServiceThread::m_delete_pool;

	z11::MGWebServiceThread::MGWebServiceThread( MGRequestSoapObject *service )
	{
		m_request_soap_object = service;
	}

	z11::MGWebServiceThread::~MGWebServiceThread()
	{
		delete_and_set_null(m_request_soap_object);
	}

	int MGWebServiceThread::readyToRun()
	{

		MGLogD("MGWebServiceThreadBase::readyToRun");
		
		//pool_locker.lock();
		//m_pool.insert(this);
		//while(!m_delete_pool.empty())
		//{
		//	MGThread *thread=(*m_delete_pool.begin());
		//	m_delete_pool.erase(m_delete_pool.begin());
		//	delete_and_set_null(thread);
		//}
		//pool_locker.unlock();

		return 0;
	}

	bool z11::MGWebServiceThread::threadLoop()
	{
		//pool_locker.lock();
		//m_pool.erase(this);
		//m_delete_pool.insert(this);
		//pool_locker.unlock();
		
		MGWebServiceSystemDelegate *delegate=MGWebService::getSystemDelegate();
		MGSoapObject *object=NULL;
        MGWebServiceDelegate* req_delegate = m_request_soap_object->delegate;
		if(req_delegate!=NULL)
		{
			req_delegate->locker.lock();
		}
		if (delegate != NULL)
		{
			object=delegate->handle(m_request_soap_object);
		}

		if(!req_delegate->m_quit&&req_delegate!=NULL && object!=NULL)
		{
			req_delegate->didGetResponse(m_request_soap_object,object);
		}
        if(req_delegate!=NULL)
		{
			req_delegate->locker.unlock();
		}
		delete_and_set_null(object);
		MGLogD("MGWebServiceThreadBase::thread run over");
        MIEvent *_event = new MIEvent;
        _event->_event_type = WEB_SERVICE_END_EVENT;
        _event->_responder = NULL;
        _event->_content = this;
        GuiContainer::container->preserveEvent(_event);
		return false;
	}
}
