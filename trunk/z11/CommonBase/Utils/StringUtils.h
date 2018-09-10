#pragma once
#include <string>
namespace z11
{
	class StringUtils
	{
	public:
		StringUtils(void);
		~StringUtils(void);
		static std::string format(const char *szFormat, ...);
		static std::string vform(const char* format, va_list args);
		static bool equalsIgnoreCase(const std::string &left,const std::string &right);
		static bool startsWith(const std::string &left,const std::string &right);
		static bool isDigit(char c);
		static int parseInt(const std::string &str);
		static bool contains(const std::string &left,const std::string &right);
	};
}
