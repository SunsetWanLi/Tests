#include "Attribute.h"
#include "../Tools/StringTools.h"

namespace Embedded
{
	void Attribute::loadFromBuf(const unsigned char *data)
	{
		/*uint_32 len = *data;
		++data;
		len -= sizeof(char);
		m_name.resize(len/sizeof(wchar_t));
		memcpy(&m_name[0], data, len);*/
		StringTools::readWString(data, m_name);
	}

}