#include "String_Tools.h"
#include <algorithm>
#include <string>
#include "System/Shared/Utility.h"
#include <cstring>
using namespace std;
using namespace z11;

namespace GIS_Embedded
{

	/*z11::MGWString String_Tools::strToWStr( const string & str )
	{
 		z11::MGWString wstr;
		wchar_t *text=new wchar_t[str.size()+1] ;
		memset(text,0,(str.size()+1)*sizeof(wchar_t));
		z11::Utility::utf8_to_unicode(text,str.c_str());
 		wstr = WS(text);
		delete []text;
		return wstr;
	}
	std::string String_Tools::wstrToStr( const z11::MGWString & wstr)
	{
		char chars[1024];
		std::string str = z11::Utility::unicode_to_utf8(chars,RWS(wstr.c_str()));
		return str;
	}*/

}
