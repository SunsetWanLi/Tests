#include "StringUtils.h"
#include <stdio.h>
#include <stdarg.h>
#if defined(_MSC_VER)
#define VSNPRINTF _vsnprintf
#else
#ifdef LOG4CPP_HAVE_SNPRINTF
#define VSNPRINTF vsnprintf
#else
/* use alternative snprintf() from http://www.ijs.si/software/snprintf/ */

#define HAVE_SNPRINTF
#define PREFER_PORTABLE_SNPRINTF

#include <stdlib.h>
#include <stdarg.h>

extern "C" {
#include "snprintf.c"
}

#define VSNPRINTF portable_vsnprintf

#endif // LOG4CPP_HAVE_SNPRINTF
#endif // _MSC_VER
using namespace std;
namespace z11
{
	StringUtils::StringUtils(void)
	{
	}


	StringUtils::~StringUtils(void)
	{
	}

	std::string StringUtils::format( const char *szFormat, ... )
	{
		va_list pArgs ;
		va_start (pArgs, szFormat) ;
		string ret=vform(szFormat,pArgs);
		va_end (pArgs) ;
		return ret;
	}

	char upper(char c)
	{
		if (c>='a'&&c<='z')
		{
			c-=32;
		}
		return c;
	}
	bool StringUtils::equalsIgnoreCase( const std::string &left,const std::string &right )
	{
		if (left.size()!=right.size())
		{
			return false;
		}
		if (left.empty()&&right.empty())
		{
			return true;
		}
		std::string left_upper;
		std::string right_upper;
		for (size_t i=0;i<left.size();i++)
		{
			left_upper.push_back(upper(left[i]));
		}
		for (size_t i=0;i<right.size();i++)
		{
			right_upper.push_back(upper(right[i]));
		}
		return left_upper==right_upper;
	}

	bool StringUtils::startsWith( const std::string &left,const std::string &right )
	{
		if (left.size()<right.size())
		{
			return false;
		}
		for (size_t i=0;i<right.size();i++)
		{
			if (left[i]!=right[i])
			{
				return false;
			}
		}
		return true;
	}

	bool StringUtils::isDigit( char c )
	{
		if (c>='0'&&c<='9')
		{
			return true;
		}
		return false;
	}

	int StringUtils::parseInt( const std::string &str )
	{
		int i=0;
		if (str.empty())
		{
			return 0;
		}
		sscanf(str.c_str(),"%d",&i);
		return i;
	}

	bool StringUtils::contains( const std::string &left,const std::string &right )
	{
		return left.find(right)!=std::string::npos;
	}

	std::string StringUtils::vform( const char* format, va_list args )
	{
		size_t size = 1024;
		char* buffer = new char[size];

		while (1) {
			va_list args_copy;

#if defined(_MSC_VER) || defined(__BORLANDC__)
			args_copy = args;
#else
			va_copy(args_copy, args);
#endif

			int n = VSNPRINTF(buffer, size, format, args_copy);

			va_end(args_copy);

			// If that worked, return a string.
			if ((n > -1) && (static_cast<size_t>(n) < size)) {
				std::string s(buffer);
				delete [] buffer;
				return s;
			}

			// Else try again with more space.
			size = (n > -1) ?
				n + 1 :   // ISO/IEC 9899:1999
			size * 2; // twice the old size

			delete [] buffer;
			buffer = new char[size];
		}
	}
}