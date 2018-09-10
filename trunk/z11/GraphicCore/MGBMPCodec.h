#pragma once
#include <string>
#include <vector>
namespace z11
{
class MGImage;
class MGBMPCodec
{
public:

	enum ColorFormat
	{
		// 3 bytes per pixel (packed), in RGB order regardless of endianness.
		// This is the native JPEG format.
		FORMAT_RGB,

		// 4 bytes per pixel, in RGBA order in memory regardless of endianness.
		FORMAT_RGBA,

		// 4 bytes per pixel, in BGRA order in mem regardless of endianness.
		// This is the default Windows DIB order.
		FORMAT_BGRA,
	};

	MGBMPCodec(void);
	~MGBMPCodec(void);

	/**
	 * @brief decode 根据指定的文件构造MGImage
	 *
	 * @param filepath 指定的文件路径
	 *
	 * @return 构造的MGImage
	 */
	static MGImage* decode(const std::string& filepath);

	/**
	 * @brief decode 
	 *
	 * @param input
	 * @param input_size
	 * @param format
	 * @param output
	 * @param w
	 * @param h
	 *
	 * @return 
	 */
	static bool decode(const unsigned char* input, size_t input_size,
		ColorFormat format, std::vector<unsigned char>* output,
		int* w, int* h);

	static MGImage *decode(const unsigned char* input, size_t input_size,ColorFormat format );

private:
	static bool decode(const unsigned char* input, size_t input_size,
		ColorFormat format, unsigned char*& output,size_t &output_size,
		int& w, int& h );
};
}
