#include "MGRequestSoapObject.h"
namespace z11
{
	z11::MGRequestSoapObject::MGRequestSoapObject( const std::string &url, const std::string &name_space,const std::string &method_name ) :MGSoapObject(name_space,method_name),m_url(url),delegate(NULL)
	{

	}
    
    MGRequestSoapObject::~MGRequestSoapObject()
    {
        m_url.clear();
    }

}
