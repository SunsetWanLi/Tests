
#include "GuiderGeoPointNoTrigger.h"
namespace GIS_Embedded
{
	//unsigned int GuiderGeoPointNoTrigger::index_languages = 0;
	//unsigned int GuiderGeoPointNoTrigger::total_languages = 0;

	const z11::MGWString GuiderGeoPointNoTrigger::getName() const
	{
		if (m_bHasName)
			return m_names[index_languages];
		else
			return WS(L"");
	}

	const z11::MGWString GuiderGeoPointNoTrigger::getDescription() const
	{
		if (m_bHasDescription)
			return m_descriptions[index_languages];
		else
			return WS(L"");
	}

	void GuiderGeoPointNoTrigger::setKey(const z11::MGWString &key )
	{
		m_key = key;
	}

	void GuiderGeoPointNoTrigger::addDescription(const z11::MGWString& description )
	{
		m_descriptions.push_back(description);
		m_bHasDescription = true;
	}

	void GuiderGeoPointNoTrigger::addName(const z11::MGWString &name )
	{
		m_names.push_back(name);  
		m_bHasName =true;
	}
}
