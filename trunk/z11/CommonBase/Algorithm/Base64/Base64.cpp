
#include "Base64.h"

#include "modp_b64.h"

namespace z11
{

	bool Base64Encode(const std::string& input, std::string* output)
	{
		std::string temp;
		temp.resize(modp_b64_encode_len(input.size())); // 预留空间分配空字节.

		// null结尾因为结果是base64的文本!
		int input_size  = static_cast<int>(input.size());
		int output_size = modp_b64_encode(&(temp[0]), input.c_str(), input_size);
		if(output_size < 0)
		{
			return false;
		}

		temp.resize(output_size); // 去除多余空字节
		output->swap(temp);
		return true;
	}

	bool Base64Decode(const std::string& input, std::string* output)
	{
		std::string temp;
		temp.resize(modp_b64_decode_len(input.size()));

		// 不以null结尾因为结果是二进制数据!
		int input_size = static_cast<int>(input.size());
		int output_size = modp_b64_decode(&(temp[0]), input.c_str(), input_size);
		if(output_size < 0)
		{
			return false;
		}

		temp.resize(output_size);
		output->swap(temp);
		return true;
	}

} //namespace base