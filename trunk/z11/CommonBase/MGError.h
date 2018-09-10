#pragma  once
#include <string>
#include "Memory/MGRefObject.h"
namespace z11
{
	class MGError:public MGRefObject
	{
	public:
		MGError( const string & localized_description ,int code=0)
			:m_localizedDescription(localized_description),m_code(code)
		{
		}

		std::string localizedDescription() const
		{
			return m_localizedDescription;
		}

		int code()
		{
			return m_code;
		}
	private:
		int m_code;
		std::string m_localizedDescription;
	};
}