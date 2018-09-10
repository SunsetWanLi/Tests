#include "StringTools.h"
#include "string.h"

//#include <windows.h>

using namespace Embedded;

//z11::MGWString StringTools::stringToWString(const std::string& str)
//{
//	wchar_t* wname;
//	int len = MultiByteToWideChar(/*0 CP_UTF8*/CP_ACP, 0 , str.c_str() , -1 , NULL, 0);
//	wname = new WCHAR[len];
//	MultiByteToWideChar(/*0 CP_UTF8*/CP_ACP, 0 , str.c_str(), -1, wname , len);
//	z11::MGWString temp_name(wname);
//
//	delete []wname;
//	return temp_name;
//}
//
//std::string StringTools::wstringToString(const z11::MGWString& wstr)
//{
//	int i = WideCharToMultiByte(/*0 CP_UTF8*/CP_ACP,
//		0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
//	char *strD = new char[i];
//	WideCharToMultiByte(/*0 CP_UTF8*/CP_ACP,
//		0, wstr.c_str(), -1, strD, i, NULL, NULL) ;
//	std::string str(strD );
//
//	delete []strD;
//	return str;
//}

//void StringTools::writeWString(const z11::MGWString& s, uint_8** data)
//{
//	uint_32 length = s.size();
//	assert(length < 256);
//
//	**data = (uint_8)length;
//	*data += sizeof(uint_8);
//
//	for (uint_32 i = 0; i < length; ++i)
//	{
//		memcpy(*data, &s[i], sizeof(wchar_t));
//		*data += sizeof(wchar_t);
//	}
//}

z11::MGWString StringTools::readWString(const uint_8* data, uint_32& len)
{

	z11::MGWString ret;
	len = readWString(data, ret);
	return ret;
}

uint_32 StringTools::readWString(const uint_8* data, z11::MGWString &ret)
{
	uint_32 length = *data;
	data += sizeof(uint_8);

	if(length>0)
	{
		ret.resize(length);
		if( sizeof(wchar_t) > 2)
		{
			uint_16 wchar;
			for (uint_32 i = 0; i < length; ++i)
			{
				memcpy(&wchar, data, 2);
				data += 2;
				ret[i] = wchar;
			}
		}
		else
		{
			memcpy(&ret[0], data, length << 1);
			data += (length << 1);
		}
	}

	return sizeof(uint_8) + (length <<1);

}
std::string StringTools::stringTrim(std::string& sIn)
{
	int i = 0;
	uint_32 size;
	std::string str_out;

	size = sIn.size();
	if ( i == size )
		return std::string("");
	while(sIn[i] ==' '|| sIn[i] == '\t')
		i++;

	if (i == size)
		return std::string("");
	else
	{
		str_out = sIn.substr(i);
		size = str_out.size();

		while (str_out[size-1] == ' '|| str_out[size-1] == '\t')
			size--;

		str_out = str_out.substr(0, size);
		return str_out;
	}
}
