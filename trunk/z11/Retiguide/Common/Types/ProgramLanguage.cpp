#include "ProgramLanguage.h"
//#include "../../DataFetcher/DataFetcher.h"

namespace GIS_Embedded
{

	int ProgramLanguage::changeLanguage(Settings& setting)
	{
		for(int i=0;i<m_languages.size();i++)
		{
			if(setting.language_units.program_language == m_languages[i].INDEX)
			{
				m_current_language = &m_languages[i];
				return 0;
			}
		}

		m_current_language = &m_languages[0];

		return -1;
	}

	int ProgramLanguage::getLanguageID(int index)
	{
		if(m_languages.size() <= index)
			return 0;
		
		return m_languages[index].INDEX;
	}

	int ProgramLanguage::getLanguageIndex(int id)
	{
		for(int i=0;i<m_languages.size();i++)
		{
			if(id == m_languages[i].INDEX)
			{
				return i;
			}
		}

		return 0;
	}
}