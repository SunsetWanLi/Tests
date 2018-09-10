#include "MGSoapObject.h"
namespace z11
{
	MGSoapObject * z11::MGSoapObject::addProperty( const std::string &name,MGObject *value )
	{
        delete_and_set_null(m_property[name]);
		m_property[name]=value;
		return this;
	}

	MGObject * z11::MGSoapObject::getProperty( const std::string &name )
	{
		return m_property[name];
	}

	void z11::MGSoapObject::setProperty( const std::string &name, MGObject *value )
	{
		m_property[name] = value;
	}

	z11::MGSoapObject::MGSoapObject( const std::string &name_space,const std::string &method_name ) 
		:m_namespace(name_space),
		m_method_name(method_name),
		m_auto_clean(false)
	{

	}



	void MGSoapObject::autoClean()
	{
		m_auto_clean=true;
	}

	void MGSoapObject::manualClean()
	{
		m_auto_clean=false;
	}

	MGSoapObject::~MGSoapObject()
	{
        if (m_auto_clean)
		{
			std::map<std::string,MGObject *>::iterator it;
			for (it=m_property.begin();it!=m_property.end();it++)
			{
				delete_and_set_null(it->second);
			}
		}
	}

	size_t MGSoapObject::getPropertyCount()
	{
		return m_property.size();
	}

	MGObject * MGSoapObject::getProperty( size_t i )
	{
		std::map<std::string,MGObject *>::iterator it=m_property.begin();
		while (i--)it++;
		return it->second;
	}

	const std::string & MGSoapObject::getNamespace()
	{
		return m_namespace;
	}

	const std::string & MGSoapObject::getMethodName()
	{
		return m_method_name;
	}

	void MGSoapObject::setNamespace( const std::string &_namespace )
	{
		m_namespace=_namespace;
	}

	void MGSoapObject::setMethodName( const std::string &method )
	{
		m_method_name=method;
	}

}