#pragma once
#include <string>
class StringUtils
{
public:
	StringUtils(void);
	~StringUtils(void);
	static std::string format(const char *szFormat, ...);
	static bool equalsIgnoreCase(const std::string &left,const std::string &right);
	static bool startsWith(const std::string &left,const std::string &right);
	static bool isDigit(char c);
	static int parseInt(const std::string &str);
	static bool contains(const std::string &left,const std::string &right);
};

