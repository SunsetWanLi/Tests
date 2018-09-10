#ifndef __z11_base64_h__
#define __z11_base64_h__

#pragma once

#include <string>

namespace z11
{

    
	/**
	 * \brief 对源字符串进行 base64编码. 
	 * \param input 需要编码的字符串
	 * \param output 编码之后的字符串
	 * \return 成功返回true失败返回false. output只在成功时被修改.
	 */
    bool Base64Encode(const std::string& input, std::string* output);

    
	/**
	 * \brief 对源字符串进行 base64解码
	 * \param input 需要解码的字符串
	 * \param output 解码之后的字符串
	 * \return 成功返回true,失败返回false. output只在成功时被修改.
	 */
    bool Base64Decode(const std::string& input, std::string* output);

} //namespace base

#endif //__z11_base64_h__