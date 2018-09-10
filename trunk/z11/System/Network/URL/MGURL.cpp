#include "MGURL.h"
using namespace std;
using namespace z11;

namespace z11
{

	MGURL * MGURL::URLWithString( const std::string &url )
	{
		return new MGURL(url);
	}

	MGURL::MGURL( const std::string &url/*=""*/ )
	{
		m_url_string=url;
	}

	std::string MGURL::absoluteString() const
	{
		return m_url_string;
	}

	MGURL::~MGURL()
	{

	}

	MGURLDictionary * MGURLDictionary::dictionaryWithObjectsForKeys( const std::vector<std::string > &objects,const std::vector<std::string>& keys )
	{
		MGURLDictionary *dict=new MGURLDictionary();
		for (size_t i=0;i<objects.size();i++)
		{
			dict->key_value_pair[keys[i]]=objects[i];
		}
		return dict;
	}

	const std::string & MGURLDictionary::objectForKey( const std::string &key )
	{
		return key_value_pair[key];
	}

	MGURLDictionary * MGURLDictionary::dictionaryWithDictionary( const MGURLDictionary *dict )
	{
		MGURLDictionary *ret=new MGURLDictionary();
		ret->key_value_pair=dict->key_value_pair;
		return ret;
	}

	MGURLDictionary * MGURLDictionary::initWithDictionary( const MGURLDictionary *dict )
	{
		this->key_value_pair=dict->key_value_pair;
		return  this;
	}

	vector<string> MGURLDictionary::allKeys()
	{
		vector<string>  ret;
		std::map<std::string,std::string>::iterator it=key_value_pair.begin();
		for (;it!=key_value_pair.end();it++)
		{
			ret.push_back(it->first);
		}	
		return ret;
	}

	void MGMutableURLDictionary::setObjectForKey( const std::string &object,const std::string &key )
	{
		key_value_pair[key]=object;
	}



}
