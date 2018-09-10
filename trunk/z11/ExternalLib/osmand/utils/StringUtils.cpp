#include "StringUtils.h"
#include <stdio.h>
#include <stdarg.h>


StringUtils::StringUtils(void)
{
}


StringUtils::~StringUtils(void)
{
}

std::string StringUtils::format( const char *szFormat, ... )
{
	char szBuffer[1024];
	va_list pArgs ;
	va_start (pArgs, szFormat) ;
	vsprintf(szBuffer,szFormat,pArgs);
	va_end (pArgs) ;
	return szBuffer;
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