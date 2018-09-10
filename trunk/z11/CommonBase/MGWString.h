#pragma once

#include <string>
#include <iosfwd>
namespace z11
{
		typedef wchar_t widechar;
		typedef std::wstring MGWString;
		typedef std::wifstream MGWifstream;
		#define WS(x) (x)
		#define RWS(x) (x)

	//zj: rename std::string for different usages.
		typedef std::string MGOString; //O short for Only, Only store ASCI characters
		typedef std::string MGAString; //A short for ASCI, Store ASCI and GB2312
		typedef std::string MGUString; //U short for utf-8, Store utf-8 format;

}