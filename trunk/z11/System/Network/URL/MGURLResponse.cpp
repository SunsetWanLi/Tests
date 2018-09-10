#include "MGURLResponse.h"
#include "../../../CommonBase/CommonDef.h"
using namespace std;
using namespace z11;

namespace z11
{



	MGURLResponse::~MGURLResponse()
	{
		release_and_set_null(m_url);
	}

	MGURLResponse::MGURLResponse( const std::string &theUrl,const std::string &MIMEType, int expectedContentLength,const std::string &textEncodingName )
	{
		m_url=MGURL::URLWithString(theUrl);
		m_MIMEType=MIMEType;
		m_expectedContentLength=expectedContentLength;
		m_textEncodingName=textEncodingName;
	}

	MGURLResponse::MGURLResponse()
	{
		m_url=NULL;
	}
	MGURLResponse * MGURLResponse::initWithURL(const MGURL *theUrl,const std::string &MIMEType, int expectedContentLength,const std::string &textEncodingName )
	{
		return new MGURLResponse(theUrl->absoluteString(),MIMEType,expectedContentLength,textEncodingName);
	}

	MGURL * MGURLResponse::URL() const 
	{
		return m_url;
	}

	std::string MGURLResponse::MIMEType() const 
	{
		return m_MIMEType;
	}

	std::string MGURLResponse::textEncodingName()  const 
	{
		return m_textEncodingName;
	}

	int MGURLResponse::expectedContentLength()  const 
	{
		return m_expectedContentLength;
	}

	std::string MGURLResponse::suggestedFilename()  const 
	{
		return m_suggestedFilename;
	}

	MGURLDictionary * MGURLResponse::allHTTPHeaderFields()
	{
		return &m_allHTTPHeaderFields;
	}

	MGURLResponse * MGURLResponse::responseWithResponse( const MGURLResponse* response )
	{
		MGURLResponse *ret=new MGURLResponse();
		ret->m_allHTTPHeaderFields=response->m_allHTTPHeaderFields;
		ret->m_expectedContentLength=response->m_expectedContentLength;
		ret->m_MIMEType=response->m_MIMEType;
		ret->m_suggestedFilename=response->m_suggestedFilename;
		ret->m_textEncodingName=response->m_textEncodingName;
		ret->m_url=MGURL::URLWithString(response->URL()->absoluteString());
		return ret;
	}
}
