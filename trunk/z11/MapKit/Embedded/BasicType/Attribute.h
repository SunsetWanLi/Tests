#ifndef _EMBEDDED_ATTRIBUTE_H_
#define _EMBEDDED_ATTRIBUTE_H_

#include "../../../CommonBase/MGWString.h"
using namespace std;

namespace Embedded
{
	class Line;
	class Attribute
	{
	public:
		const z11::MGWString& getName() const { return m_name; };
		void loadFromBuf(const unsigned char* data);
		friend class Line;
	private:
		z11::MGWString m_name;
	};
}
#endif