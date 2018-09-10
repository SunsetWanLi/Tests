#ifndef _EMBEDDED_STRING_TOOLS_H_
#define _EMBEDDED_STRING_TOOLS_H_
#include "Tools.h"


namespace Embedded
{
	class StringTools
	{
	public:
		//static z11::MGWString stringToWString(const std::string& str);
		//static std::string wstringToString(const z11::MGWString& wstr);

		// should less than 256
		//static void writeWString(const z11::MGWString& s, uint_8** data);
		static z11::MGWString readWString(const uint_8* data, uint_32& len);
		static uint_32 readWString(const uint_8* data, z11::MGWString &ret);

		// erase ' ' and '\t' at endpoing of sin
		static std::string stringTrim(std::string& sIn);

		template<class T>
		static T substrFromBack(const T& src, const char start, const char end);

	};	// class StringTools

	template<class T>
	inline T StringTools::substrFromBack(const T& src, const char start, const char end)
	{
		typename T::size_type s = src.find_last_of(start),
				  e = src.find_last_of(end);
		return src.substr(++s, e - s);
	}
}
#endif // _EMBEDDED_STRING_TOOLS_H_