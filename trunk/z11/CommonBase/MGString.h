//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2012 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (MGString.h)  
/// @brief (本文件实现的功能的简述)  
///  
///(本文件实现的功能的详述)  
///  
/// @version 0.1   (版本声明)  
/// @author        (Gu RongFang)  
/// @date          (12-01-12)  
///  
///  
///    修订说明：最初版本  
////////////////////////////////////////////////////////////////////////// 
#ifndef _MGSTRING_H_
#define _MGSTRING_H_
#include <string>
#pragma once

#include "MGObject.h"
namespace z11
{
	class MGString:public MGObject
	{
    public:
        MGString(){}
        MGString(const std::string &str);
        virtual ~MGString(){}
	public:
		MGString &operator = (const std::string &str);
        MGString operator + (const MGString &str);
			/**
			 * @brief initWithSTLString 通过std::string 来初始化MGString
			 *
			 * @param str 指定的std::string
			 *
			 * @return 返回构造的MGString
			 */
		static MGString *initWithSTLString(const std::string &str);
		
		/**
		 * @brief toSTLString 将MGString 转换为std::string
		 *
		 * @return 返回转换后的std::string
		 */
		const std::string &toSTLString() const;

		   /**
			* @brief isEqual 重载MGObject的isEqual函数
			*
			* @param 要比较的MGObject
			*
			* @return 如果相等返回true ，否则返回false
			*/
		virtual bool isEqual(MGObject *);
	protected:
		std::string m_string;
	};

    class MGUTF8String : public MGString
    {
    public:
        MGUTF8String(){}
        MGUTF8String(const std::string &str):MGString(str){};
        virtual ~MGUTF8String(){}
    public:
        MGUTF8String &operator = (const std::string &str);
        MGUTF8String operator + (const MGUTF8String &str);
        // 返回指定text的字符数，不是字节数，一个多字节编码的字符算一个字符 //
        void clear();
        bool empty() const;
        uint_32 length() const;
        uint_32 size() const;
        MGUTF8String substr(const uint_32& location);
        MGUTF8String substr(const uint_32& b_location, const uint_32& f_length);
        MGUTF8String& replace(const uint_32& b_location, const uint_32& e_location, const MGUTF8String& text);
        void cleanSpecialChar(const char& special_char);
    };
}

#endif //_MGSTRING_H_
