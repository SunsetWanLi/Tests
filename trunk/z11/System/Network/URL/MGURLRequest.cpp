#include "MGURLRequest.h"
#include "../../../CommonBase/CommonDef.h"
using namespace std;
using namespace z11;

namespace z11
{


	MGURLRequest * MGURLRequest::requestWithURL( MGURL *theUrl )
	{
		return requestWithURL(theUrl,30);
	}

	MGURLRequest * MGURLRequest::requestWithURL( MGURL *theUrl,MGTimeInterval _timeoutInterval )
	{
		MGURLRequest *ret=new MGURLRequest(theUrl);
		ret->m_timeoutInterval=_timeoutInterval;
		return ret;
	}

	MGURL * MGURLRequest::URL() const
	{
		return m_url;
	}

	MGURLRequest::MGURLRequest( MGURL *theUrl )
	{
		m_url=theUrl;
		m_url->retain();
		m_http_body=NULL;
	}

	MGURLRequest::~MGURLRequest()
	{
		release_and_set_null(m_url);
		release_and_set_null(m_http_body);
	}

	z11::MGTimeInterval MGURLRequest::timeoutInterval() const
	{
		return m_timeoutInterval;
	}

	void MGURLRequest::setValueForHTTPHeaderField( const std::string &value,const std::string &field )
	{
		http_header_field.setObjectForKey(value,field);
	}

	MGURLDictionary* MGURLRequest::allHTTPHeaderFields()
	{
		return &http_header_field;
	}
	MGURLRequest * MGURLRequest::requestWithRequest(const MGURLRequest *request )
	{
		MGURL *url=MGURL::URLWithString(request->URL()->absoluteString());
		MGURLRequest *ret=new MGURLRequest(url);
		ret->http_header_field=request->http_header_field;
		ret->m_timeoutInterval=request->m_timeoutInterval;
		ret->m_http_body=request->m_http_body;
		return ret;
	}

	void MGURLRequest::setHTTPBody( MGData *data )
	{
		release_and_set_null(m_http_body);

		data->retain();
		m_http_body=data;
	}

	void MGURLRequest::setHTTPMethod( const string &method )
	{
		setValueForHTTPHeaderField(method,"RequestMethod");
	}

	MGData* MGURLRequest::HTTPBody()
	{
		return m_http_body;
	}

	std::string MGURLRequest::valueForHTTPHeaderField( const std::string &field )
	{
		return http_header_field.objectForKey(field);
	}
}
