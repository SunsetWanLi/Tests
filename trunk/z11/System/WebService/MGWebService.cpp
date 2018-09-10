#include "MGWebService.h"
#include "../../CommonBase/MGString.h"
#include "../../GUI/GuiContainer/GuiContainer.h"
#include "../../GUI/Events/MIEvent.h"
namespace z11
{
	MGWebServiceSystemDelegate *MGWebService::m_system_delegate=NULL;
	void MGWebService::requestWebService( const std::string& method, const vector<std::string>* keys, const vector<std::string> *values )
	{
		MIEvent *event=new MIEvent();
		event->_event_type = WEB_SERVICE_BEGIN_EVENT;
		event->_responder = NULL;
		MGRequestSoapObject *request_soap_object = new MGRequestSoapObject(m_url,m_namespace,method);
		request_soap_object->autoClean();
		if (keys!=NULL && values !=NULL)
		{
			size_t min_size= MINVALUE(keys->size(),values->size());
			for (size_t i=0;i<min_size;i++)
			{
				MGString *string_object=MGString::initWithSTLString(values->at(i));
				request_soap_object->addProperty(keys->at(i),string_object);
			}
		}
		event->_content=request_soap_object;
		request_soap_object->delegate=delegate;
		GuiContainer::container->preserveEvent(event);
	}

	bool MGWebService::handle()
	{
		return true;
	}


	void MGWebService::setDelegate( MGWebServiceDelegate *_delegate )
	{
		delegate=_delegate;
	}

	MGWebService::MGWebService()
	{
		delegate = NULL;
	}

	void MGWebService::setSystemDelegate( MGWebServiceSystemDelegate *_delegate )
	{
		m_system_delegate=_delegate;
	}

	MGWebServiceSystemDelegate* MGWebService::getSystemDelegate()
	{
		return m_system_delegate;
	}


	MGWebServiceDelegate::~MGWebServiceDelegate()
	{
		m_quit=true;
		locker.lock();
		locker.unlock();
	}

	MGWebServiceDelegate::MGWebServiceDelegate() :m_quit(false)
	{

	}

}