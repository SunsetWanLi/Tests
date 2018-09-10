#include "MGPNGCodec.h"
#include "MGImage.h"
#include "MGPersistenceImage.h"
#include <vector>
#include "../CommonBase/CommonDef.h"
#include "../System/Shared/MGLog.h"
#include "../CommonBase/MGDataReader.h"
#include "../ExternalLib/png/png.h"
#include "../ExternalLib/png/pngconf.h"
#include "../ExternalLib/zlib/zlib.h"
#include "../ExternalLib/zlib/zconf.h"
#define READ_PNG_FROM_FILE 1

/** return the alpha uint_8 from a SkColor value */
#define SkColorGetA(color)      (((color) >> 24) & 0xFF)
/** return the red uint_8 from a SkColor value */
#define SkColorGetR(color)      (((color) >> 16) & 0xFF)
/** return the green uint_8 from a SkColor value */
#define SkColorGetG(color)      (((color) >>  8) & 0xFF)
/** return the blue uint_8 from a SkColor value */
#define SkColorGetB(color)      (((color) >>  0) & 0xFF)

// All little-endian Chrome platforms agree:  BGRA is the optimal pixel layout.
#define SK_A32_SHIFT    24
#define SK_R32_SHIFT    16
#define SK_G32_SHIFT    8
#define SK_B32_SHIFT    0

#define SkGetPackedA32(packed)      ((uint_32)((packed) << (24 - SK_A32_SHIFT)) >> 24)
#define SkGetPackedR32(packed)      ((uint_32)((packed) << (24 - SK_R32_SHIFT)) >> 24)
#define SkGetPackedG32(packed)      ((uint_32)((packed) << (24 - SK_G32_SHIFT)) >> 24)
#define SkGetPackedB32(packed)      ((uint_32)((packed) << (24 - SK_B32_SHIFT)) >> 24)

namespace z11
{

	MGPNGCodec::MGPNGCodec(void)
	{
	}

	MGPNGCodec::~MGPNGCodec(void)
	{
	}

	namespace
	{

		// Converts BGRA->RGBA and RGBA->BGRA.
		void ConvertBetweenBGRAandRGBA(const unsigned char* input, int pixel_width,
			unsigned char* output, bool* is_opaque)
		{
			for(int x=0; x<pixel_width; ++x)
			{
				const unsigned char* pixel_in = &input[x * 4];
				unsigned char* pixel_out = &output[x * 4];
				pixel_out[0] = pixel_in[2];
				pixel_out[1] = pixel_in[1];
				pixel_out[2] = pixel_in[0];
				pixel_out[3] = pixel_in[3];
			}
		}

		void ConvertRGBAtoRGB(const unsigned char* rgba, int pixel_width,
			unsigned char* rgb, bool* is_opaque)
		{
			for(int x=0; x<pixel_width; ++x)
			{
				const unsigned char* pixel_in = &rgba[x * 4];
				unsigned char* pixel_out = &rgb[x * 3];
				pixel_out[0] = pixel_in[0];
				pixel_out[1] = pixel_in[1];
				pixel_out[2] = pixel_in[2];
			}
		}

		//void ConvertRGBtoSkia(const unsigned char* rgb, int pixel_width,
		//	unsigned char* rgba, bool* is_opaque)
		//{
		//	for(int x=0; x<pixel_width; ++x)
		//	{
		//		const unsigned char* pixel_in = &rgb[x * 3];
		//		uint_32* pixel_out = reinterpret_cast<uint_32*>(&rgba[x * 4]);
		//		*pixel_out = SkPackARGB32(0xFF, pixel_in[0], pixel_in[1], pixel_in[2]);
		//	}
		//}

		//void ConvertRGBAtoSkia(const unsigned char* rgb, int pixel_width,
		//	unsigned char* rgba, bool* is_opaque)
		//{
		//	int total_length = pixel_width * 4;
		//	for(int x=0; x<total_length; x+=4)
		//	{
		//		const unsigned char* pixel_in = &rgb[x];
		//		uint_32* pixel_out = reinterpret_cast<uint_32*>(&rgba[x]);

		//		unsigned char alpha = pixel_in[3];
		//		if (alpha != 255)
		//		{
		//			*is_opaque = false;
		//			*pixel_out = SkPreMultiplyARGB(alpha,
		//				pixel_in[0], pixel_in[1], pixel_in[2]);
		//		}
		//		else
		//		{
		//			*pixel_out = SkPackARGB32(alpha,
		//				pixel_in[0], pixel_in[1], pixel_in[2]);
		//		}
		//	}
		//}

		void ConvertSkiatoRGB(const unsigned char* skia, int pixel_width,
			unsigned char* rgb, bool* is_opaque)
		{
			for(int x=0; x<pixel_width; ++x)
			{
				const uint_32 pixel_in = *reinterpret_cast<const uint_32*>(&skia[x * 4]);
				unsigned char* pixel_out = &rgb[x * 3];

				int alpha = SkGetPackedA32(pixel_in);
				if(alpha!=0 && alpha!=255)
				{
					//SkColor unmultiplied = SkUnPreMultiply::PMColorToColor(pixel_in);
					const uint_32 unmultiplied=pixel_in;
					pixel_out[0] = SkColorGetR(unmultiplied);
					pixel_out[1] = SkColorGetG(unmultiplied);
					pixel_out[2] = SkColorGetB(unmultiplied);
				}
				else
				{
					pixel_out[0] = SkGetPackedR32(pixel_in);
					pixel_out[1] = SkGetPackedG32(pixel_in);
					pixel_out[2] = SkGetPackedB32(pixel_in);
				}
			}
		}

		void ConvertSkiatoRGBA(const unsigned char* skia, int pixel_width,
			unsigned char* rgba, bool* is_opaque)
		{
			int total_length = pixel_width * 4;
			for(int i=0; i<total_length; i+=4)
			{
				const uint_32 pixel_in = *reinterpret_cast<const uint_32*>(&skia[i]);

				// Pack the components here.
				int alpha = SkGetPackedA32(pixel_in);
				if(alpha!=0 && alpha!=255)
				{
					//SkColor unmultiplied = SkUnPreMultiply::PMColorToColor(pixel_in);
					const uint_32 unmultiplied=pixel_in; 
					rgba[i + 0] = SkColorGetR(unmultiplied);
					rgba[i + 1] = SkColorGetG(unmultiplied);
					rgba[i + 2] = SkColorGetB(unmultiplied);
					rgba[i + 3] = alpha;
				}
				else
				{
					rgba[i + 0] = SkGetPackedR32(pixel_in);
					rgba[i + 1] = SkGetPackedG32(pixel_in);
					rgba[i + 2] = SkGetPackedB32(pixel_in);
					rgba[i + 3] = alpha;
				}
			}
		}

	}

	// Decoder --------------------------------------------------------------------
	//
	// This code is based on WebKit libpng interface (PNGImageDecoder), which is
	// in turn based on the Mozilla png decoder.

	namespace
	{

		// Gamma constants: We assume we're on Windows which uses a gamma of 2.2.
		const double kMaxGamma = 21474.83;  // Maximum gamma accepted by png library.
		const double kDefaultGamma = 2.2;
		const double kInverseGamma = 1.0 / kDefaultGamma;

		class PngDecoderState
		{
		public:
			// Output is a vector<unsigned char>.
			PngDecoderState(MGPNGCodec::ColorFormat ofmt, std::vector<unsigned char>* o)
				: output_format(ofmt),
				output_channels(0),
				bitmap(NULL),
				is_opaque(true),
				output(o),
				row_converter(NULL),
				width(0),
				height(0),
				done(false) {}

			//// Output is an SkBitmap.
			//explicit PngDecoderState(SkBitmap* skbitmap)
			//	: output_format(MGPNGCodec::FORMAT_SkBitmap),
			//	output_channels(0),
			//	bitmap(skbitmap),
			//	is_opaque(true),
			//	output(NULL),
			//	row_converter(NULL),
			//	width(0),
			//	height(0),
			//	done(false) {}

			MGPNGCodec::ColorFormat output_format;
			int output_channels;

			// An incoming SkBitmap to write to. If NULL, we write to output instead.
			MGImage* bitmap;

			// Used during the reading of an SkBitmap. Defaults to true until we see a
			// pixel with anything other than an alpha of 255.
			bool is_opaque;

			// The other way to decode output, where we write into an intermediary buffer
			// instead of directly to an SkBitmap.
			std::vector<unsigned char>* output;

			// Called to convert a row from the library to the correct output format.
			// When NULL, no conversion is necessary.
			void (*row_converter)(const unsigned char* in, int w, unsigned char* out,
				bool* is_opaque);

			// Size of the image, set in the info callback.
			int width;
			int height;

			// Set to true when we've found the end of the data.
			bool done;

		private:
//			DISALLOW_COPY_AND_ASSIGN(PngDecoderState);
		};

		void ConvertRGBtoRGBA(const unsigned char* rgb, int pixel_width,
			unsigned char* rgba, bool* is_opaque)
		{
			for(int x=0; x<pixel_width; ++x)
			{
				const unsigned char* pixel_in = &rgb[x * 3];
				unsigned char* pixel_out = &rgba[x * 4];
				pixel_out[0] = pixel_in[0];
				pixel_out[1] = pixel_in[1];
				pixel_out[2] = pixel_in[2];
				pixel_out[3] = 0xff;
			}
		}

		void ConvertRGBtoBGRA(const unsigned char* rgb, int pixel_width,
			unsigned char* bgra, bool* is_opaque)
		{
			for(int x=0; x<pixel_width; ++x)
			{
				const unsigned char* pixel_in = &rgb[x * 3];
				unsigned char* pixel_out = &bgra[x * 4];
				pixel_out[0] = pixel_in[2];
				pixel_out[1] = pixel_in[1];
				pixel_out[2] = pixel_in[0];
				pixel_out[3] = 0xff;
			}
		}

		// Called when the png header has been read. This code is based on the WebKit
		// PNGImageDecoder
		void DecodeInfoCallback(png_struct* png_ptr, png_info* info_ptr)
		{
			PngDecoderState* state = static_cast<PngDecoderState*>(
				png_get_progressive_ptr(png_ptr));

			int bit_depth, color_type, interlace_type, compression_type;
			int filter_type, channels;
			png_uint_32 w, h;
			png_get_IHDR(png_ptr, info_ptr, &w, &h, &bit_depth, &color_type,
				&interlace_type, &compression_type, &filter_type);

			// Bounds check. When the image is unreasonably big, we'll error out and
			// end up back at the setjmp call when we set up decoding.  "Unreasonably big"
			// means "big enough that w * h * 32bpp might overflow an int"; we choose this
			// threshold to match WebKit and because a number of places in code assume
			// that an image's size (in bytes) fits in a (signed) int.
			unsigned long long total_size =
				static_cast<unsigned long long>(w) * static_cast<unsigned long long>(h);
			if(total_size > ((1 << 29) - 1))
			{
				longjmp(png_jmpbuf(png_ptr), 1);
			}
			state->width = static_cast<int>(w);
			state->height = static_cast<int>(h);

			// Expand to ensure we use 24-bit for RGB and 32-bit for RGBA.
			if(color_type==PNG_COLOR_TYPE_PALETTE ||
				(color_type==PNG_COLOR_TYPE_GRAY && bit_depth<8))
			{
				png_set_expand(png_ptr);
			}

			// Transparency for paletted images.
			if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
			{
				png_set_expand(png_ptr);
			}

			// Convert 16-bit to 8-bit.
			if(bit_depth == 16)
			{
				png_set_strip_16(png_ptr);
			}

			// Expand grayscale to RGB.
			if(color_type==PNG_COLOR_TYPE_GRAY ||
				color_type==PNG_COLOR_TYPE_GRAY_ALPHA)
			{
				png_set_gray_to_rgb(png_ptr);
			}

			// Deal with gamma and keep it under our control.
			double gamma;
			if(png_get_gAMA(png_ptr, info_ptr, &gamma))
			{
				if(gamma<=0.0 || gamma>kMaxGamma)
				{
					gamma = kInverseGamma;
					png_set_gAMA(png_ptr, info_ptr, gamma);
				}
				png_set_gamma(png_ptr, kDefaultGamma, gamma);
			}
			else
			{
				png_set_gamma(png_ptr, kDefaultGamma, kInverseGamma);
			}

			// Tell libpng to send us rows for interlaced pngs.
			if(interlace_type == PNG_INTERLACE_ADAM7)
			{
				png_set_interlace_handling(png_ptr);
			}

			// Update our info now
			png_read_update_info(png_ptr, info_ptr);
			channels = png_get_channels(png_ptr, info_ptr);

			// Pick our row format converter necessary for this data.
			if(channels == 3)
			{
				switch(state->output_format)
				{
				case MGPNGCodec::FORMAT_DEFAULT:
				case MGPNGCodec::FORMAT_RGB:
					state->row_converter = NULL; // no conversion necessary
					state->output_channels = 3;
					break;
				case MGPNGCodec::FORMAT_RGBA:
					state->row_converter = &ConvertRGBtoRGBA;
					state->output_channels = 4;
					break;
				default:
					MGLogD("Unknown output format") ;
					break;
				}
			}
			else if(channels == 4)
			{
				switch(state->output_format)
				{
				case MGPNGCodec::FORMAT_RGB:
					state->row_converter = &ConvertRGBAtoRGB;
					state->output_channels = 3;
					break;
				case MGPNGCodec::FORMAT_DEFAULT:
				case MGPNGCodec::FORMAT_RGBA:
					state->row_converter = NULL; // no conversion necessary
					state->output_channels = 4;
					break;
				default:
					MGLogD("Unknown output format") ;
					break;
				}
			}
			else
			{
				MGLogD("Unknown  input channels") ;
				longjmp(png_jmpbuf(png_ptr), 1);
			}

			if(state->bitmap)
			{
				//state->bitmap->setConfig(SkBitmap::kARGB_8888_Config,
				//	state->width, state->height);
				//state->bitmap->allocPixels();
			}
			else if(state->output)
			{
				state->output->resize(
					state->width * state->output_channels * state->height);
			}
		}

		void DecodeRowCallback(png_struct* png_ptr, png_byte* new_row,
			png_uint_32 row_num, int pass)
		{
			PngDecoderState* state = static_cast<PngDecoderState*>(
				png_get_progressive_ptr(png_ptr));

			//DCHECK(pass == 0) << "We didn't turn on interlace handling, "
			//	"but libpng is giving us interlaced data.";
			if(static_cast<int>(row_num) > state->height)
			{
				MGLogD( "Invalid row") ;
				return;
			}

			unsigned char* base = NULL;
			if(state->bitmap)
			{

				base = const_cast<byte_8*>(state->bitmap->getContentData());
				//base = reinterpret_cast<unsigned char*>(state->bitmap->getAddr32(0, 0));
			}
			else if(state->output)
			{
				base = &state->output->front();
			}

			unsigned char* dest = &base[state->width * state->output_channels * row_num];
			if(state->row_converter)
			{
				state->row_converter(new_row, state->width, dest, &state->is_opaque);
			}
			else
			{
				memcpy(dest, new_row, state->width * state->output_channels);
			}
		}

		void DecodeEndCallback(png_struct* png_ptr, png_info* info)
		{
			PngDecoderState* state = static_cast<PngDecoderState*>(
				png_get_progressive_ptr(png_ptr));

			// Mark the image as complete, this will tell the Decode function that we
			// have successfully found the end of the data.
			state->done = true;
		}

		// Automatically destroys the given read structs on destruction to make
		// cleanup and error handling code cleaner.
		class PngReadStructDestroyer
		{
		public:
			PngReadStructDestroyer(png_struct** ps, png_info** pi)
				: ps_(ps), pi_(pi) {}
			~PngReadStructDestroyer()
			{
				png_destroy_read_struct(ps_, pi_, NULL);
			}
		private:
			png_struct** ps_;
			png_info** pi_;
		};

		bool BuildPNGStruct(const unsigned char* input, size_t input_size,
			png_struct** png_ptr, png_info** info_ptr)
		{
			if(input_size < 8)
			{
				return false; // Input data too small to be a png
			}

			// Have libpng check the signature, it likes the first 8 bytes.
			if(png_sig_cmp(const_cast<unsigned char*>(input), 0, 8) != 0)
			{
				MGLogD("It's Not a PNG file...");
				return false;
			}

			*png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
			if(!*png_ptr)
			{
				return false;
			}

			*info_ptr = png_create_info_struct(*png_ptr);
			if(!*info_ptr)
			{
				png_destroy_read_struct(png_ptr, NULL, NULL);
				MGLogD("ReadPngFile: Failed to read the PNG file");
				return false;
			}

			return true;
		}

	}
	static void sk_read_fn(png_structp png_ptr, png_bytep data, png_size_t length) {
		MGDataReader* sk_stream = (MGDataReader*)png_get_io_ptr(png_ptr);
		size_t bytes = sk_stream->read(data, length);
		if (bytes != length) {
			png_error(png_ptr, "Read Error!");
		}
	}

	MGImage* MGPNGCodec::decode(const std::string& input_file )
	{
		//FILE *file2=NULL;
		//if (file2=fopen(input_file.c_str(),"rb"))
		//{
		//	MGLogD("File:%s exists",input_file.c_str());
		//	fclose(file2);
		//}
		//else
		//{

		//	MGLogD("File:%s not exists",input_file.c_str());
		//}
		
#if READ_PNG_FROM_FILE
		png_struct* png_ptr = NULL;
		png_info* info_ptr = NULL;
		MGDataReader reader(input_file);
		/* open file and test for it being a png */
		//FILE *file = fopen(input_file.c_str(), "rb");

		if (!reader.is_open())
		{
			printf("decode PNG...can't open sourcefile!\n");
			return NULL;
		}

		unsigned char input[8];
		size_t input_size=reader.read(input,8);//fread(input, 1, 8, file);

		if(!BuildPNGStruct(input, input_size, &png_ptr, &info_ptr))
		{
			//fclose(file);
			return NULL;
		}

		PngReadStructDestroyer destroyer(&png_ptr, &info_ptr);
		if(setjmp(png_jmpbuf(png_ptr)))
		{
			// The destroyer will ensure that the structures are cleaned up in this
			// case, even though we may get here as a jump from random parts of the
			// PNG library called below.
			//fclose(file);
			return NULL;
		}
		
		//I/O initialisation methods
		//png_init_io(png_ptr, file);
		/* If you are using replacement read functions, instead of calling
		* png_init_io() here you would call:
		*/
		png_set_read_fn(png_ptr, (void *)&reader, sk_read_fn);

		png_set_sig_bytes(png_ptr, 8);  //Required!!!

		png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);
		//fclose(file);

		png_bytepp row_pointers = (png_bytepp)png_get_rows(png_ptr, info_ptr);
        
        PixelFormat pf = PF_DEFAULT;
        switch (info_ptr->color_type) {
            case PNG_COLOR_TYPE_GRAY:
                pf = PF_GRAY8;
                break;
            case PNG_COLOR_TYPE_PALETTE:
                pf = PF_RGBA0008;
                break;
            case PNG_COLOR_TYPE_RGB:
                pf = PF_RGBA8880;
                break;
            case PNG_COLOR_TYPE_RGB_ALPHA:
                pf = PF_RGBA8888;
                break;
            case PNG_COLOR_TYPE_GRAY_ALPHA:
                pf = PF_GRAYA88;
                break;
            default:
                break;
        }
        
		MGImage *ret=MGImage::fromBlank(info_ptr->rowbytes/info_ptr->width*8,info_ptr->width,info_ptr->height, pf);
		png_bytep pRowData	=	NULL;
		byte_8 *pWriteData=const_cast<byte_8 *>(ret->getContentData());
		int widthbyte = info_ptr->rowbytes;
		const int content_data_bytes=ret->content_info.adapter_width*ret->content_info.bit_count/8;
		for (int i=info_ptr->height-1; i>=0; i--)
		{
			pRowData=row_pointers[i];
			memcpy(pWriteData,pRowData,widthbyte);
			pWriteData+=content_data_bytes;
		}
#else
		MGImage *ret=NULL;
		
		{
			MGData *input_data=MGData::dataWithContentsOfFile(input_file);
			if (input_data==NULL||input_data->length()==0)
			{
				return NULL;
			}
			std::vector<unsigned char> output;
			int w;
			int h;
			if(MGPNGCodec::decode((byte_8 *)input_data->bytes(),input_data->length(),MGPNGCodec::FORMAT_DEFAULT,&output,&w,&h)) 
			{
				MGImageInfo info;
				info.bit_count=output.size()/w/h*8;
				info.width_in_pixel=w;
				info.height_in_pixel=h;
				ret=MGImage::fromBlank(info.bit_count,info.width_in_pixel,info.height_in_pixel);
				const byte_8 *pSrcData=static_cast<const byte_8 *>(&output[0]);
				byte_8 *pWriteData=const_cast<byte_8 *>(ret->getContentData());
				int widthbyte = BYTEOFLINE(info.width_in_pixel,info.bit_count);
				pSrcData+=widthbyte*(info.height_in_pixel-1);
				const int content_data_bytes=ret->content_info.adapter_width*ret->content_info.bit_count/8;
				for (int i=info.height_in_pixel-1; i>=0; i--)
				{
					memcpy(pWriteData,pSrcData,widthbyte);
					pWriteData+=content_data_bytes;
					pSrcData-=widthbyte;
				}
			}
		}
#endif
		return ret;
	}

	bool MGPNGCodec::decode( const unsigned char* input, size_t input_size, ColorFormat& format, std::vector<unsigned char>* output, int* w, int* h )
	{
		png_struct* png_ptr = NULL;
		png_info* info_ptr = NULL;
		if(!BuildPNGStruct(input, input_size, &png_ptr, &info_ptr))
		{
			return false;
		}

		PngReadStructDestroyer destroyer(&png_ptr, &info_ptr);
		if(setjmp(png_jmpbuf(png_ptr)))
		{
			// The destroyer will ensure that the structures are cleaned up in this
			// case, even though we may get here as a jump from random parts of the
			// PNG library called below.
			return false;
		}

		PngDecoderState state(format, output);

		png_set_progressive_read_fn(png_ptr, &state, &DecodeInfoCallback,
			&DecodeRowCallback, &DecodeEndCallback);
		png_process_data(png_ptr,
			info_ptr,
			const_cast<unsigned char*>(input),
			input_size);

		if(!state.done)
		{
			// Fed it all the data but the library didn't think we got all the data, so
			// this file must be truncated.
			output->clear();
			return false;
		}
        
        switch (info_ptr->color_type) {
            case PNG_COLOR_TYPE_GRAY:
                format = FORMAT_GRAY;
                break;
            case PNG_COLOR_TYPE_PALETTE:
                format = FORMAT_ALPHA;
                break;
            case PNG_COLOR_TYPE_RGB:
                format = FORMAT_RGB;
                break;
            case PNG_COLOR_TYPE_RGB_ALPHA:
                format = FORMAT_RGBA;
                break;
            case PNG_COLOR_TYPE_GRAY_ALPHA:
                format = FORMAT_GRAYA;
                break;
            default:
                break;
        }

		*w = state.width;
		*h = state.height;
		return true;
	}

#ifndef MG_BREAK_IF
	#define MG_BREAK_IF(cond) if(cond) break;
#endif
#define CC_RGB_PREMULTIPLY_APLHA(vr, vg, vb, va) \
	(unsigned)(((unsigned)((unsigned char)(vr) | \
	((unsigned)((unsigned char)(vg)  << 8) | \
	((unsigned)((unsigned char)(vb) << 16) | \
	((unsigned)(unsigned char)(va) << 24))

	typedef struct 
	{
		unsigned char* data;
		int size;
		int offset;
	}tImageSource;

	static void pngReadCallback(png_structp png_ptr, png_bytep data, png_size_t length)
	{
		tImageSource* isource = (tImageSource*)png_get_io_ptr(png_ptr);

		if((int)(isource->offset + length) <= isource->size)
		{
			memcpy(data, isource->data+isource->offset, length);
			isource->offset += length;
		}
		else
		{
			png_error(png_ptr, "pngReaderCallback failed");
		}
	}

	bool MGPNGCodec::decode( const std::string & input_file,MGData *&output,MGImageInfo *info )
	{

		MGData *data=MGData::dataWithContentsOfFile(input_file);
		if(data==NULL||data->length()==0)
		{
			return false;
		}
		// length of bytes to check if it is a valid png file
#define PNGSIGSIZE  8
		bool bRet = false;
		png_byte        header[PNGSIGSIZE]   = {0}; 
		png_structp     png_ptr     =   0;
		png_infop       info_ptr    = 0;

		do 
		{
			// png header len is 8 bytes
			MG_BREAK_IF(data->length() < PNGSIGSIZE);

			// check the data is png or not
			memcpy(header, data->bytes(), PNGSIGSIZE);
			MG_BREAK_IF(png_sig_cmp(header, 0, PNGSIGSIZE));

			// init png_struct
			png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
			MG_BREAK_IF(! png_ptr);

			// init png_info
			info_ptr = png_create_info_struct(png_ptr);
			MG_BREAK_IF(!info_ptr);

			MG_BREAK_IF(setjmp(png_jmpbuf(png_ptr)));

			// set the read call back function
			tImageSource imageSource;
			imageSource.data    = (unsigned char*)data->bytes();
			imageSource.size    = data->length();
			imageSource.offset  = 0;
			png_set_read_fn(png_ptr, &imageSource, pngReadCallback);

			// read png header info

			// read png file info
			png_read_info(png_ptr, info_ptr);

			int m_nWidth = png_get_image_width(png_ptr, info_ptr);
			int m_nHeight = png_get_image_height(png_ptr, info_ptr);
			int m_nBitsPerComponent = png_get_bit_depth(png_ptr, info_ptr);
			png_uint_32 channels = png_get_channels(png_ptr, info_ptr);
			png_uint_32 color_type = png_get_color_type(png_ptr, info_ptr);

			//MGLogD("color type %u", color_type);
			// only support color type: PNG_COLOR_TYPE_RGB, PNG_COLOR_TYPE_RGB_ALPHA PNG_COLOR_TYPE_PALETTE
			// and expand bit depth to 8
			switch (color_type) {
			case PNG_COLOR_TYPE_RGB:
			case PNG_COLOR_TYPE_RGB_ALPHA:
				// do nothing

				break;
			case PNG_COLOR_TYPE_PALETTE:
				png_set_palette_to_rgb(png_ptr);
				channels = 3;

				break;
			case PNG_COLOR_TYPE_GRAY:
			case PNG_COLOR_TYPE_GRAY_ALPHA:
				if (m_nBitsPerComponent < 8)
				{
					png_set_expand_gray_1_2_4_to_8(png_ptr);
				}
				png_set_gray_to_rgb(png_ptr);
				channels = 3;

				break;

			default:
				MGLogD("unsopprted color type %u", color_type);
				goto out;
			}
			if (m_nBitsPerComponent == 16)
			{
				png_set_strip_16(png_ptr);
				m_nBitsPerComponent = 8;
			} 

			bool m_bHasAlpha = (color_type & PNG_COLOR_MASK_ALPHA) ? true : false;
			if (m_bHasAlpha)
			{
				channels = 4;
			}

			// read png data
			// m_nBitsPerComponent will always be 8
			unsigned char *m_pData = new unsigned char[m_nWidth * m_nHeight * channels];
			png_bytep* row_pointers = (png_bytep*)malloc(sizeof(png_bytep)*m_nHeight);
			if (row_pointers)
			{
				const unsigned int stride = m_nWidth * channels;
				for (unsigned short i = 0; i < m_nHeight; ++i)
				{
					png_uint_32 q = i * stride;
					row_pointers[i] = (png_bytep)m_pData + q;
				}
				png_read_image(png_ptr, row_pointers);

				//if (m_bHasAlpha)
				//{
				//	unsigned int *tmp = (unsigned int *)m_pData;
				//	for(unsigned short i = 0; i < m_nHeight; i++)
				//	{
				//		for(unsigned int j = 0; j < m_nWidth * channels; j += 4)
				//		{
				//			*tmp++ = CC_RGB_PREMULTIPLY_APLHA( row_pointers[i][j], row_pointers[i][j + 1], 
				//				row_pointers[i][j + 2], row_pointers[i][j + 3] );
				//		}
				//	}

				//	bool m_bPreMulti = true;
				//}

				free(row_pointers);
				bRet = true;
			}

			int adapter_width=expand_to_2_power(m_nWidth);
			int adapter_height=expand_to_2_power(m_nHeight);
			if (info!=NULL)
			{
				info->width_in_pixel=m_nWidth;
				info->height_in_pixel=m_nHeight;
				info->bit_count=channels*8;
				info->adapter_width=adapter_width;
				info->adapter_height=adapter_height;
				//assert (info->bit_count==24);
				if (info->bit_count==32)
				{
					info->format=PF_RGBA8888;
				}
				else  if (info->bit_count==24)
				{
					info->format=PF_RGBA8880;
				}


			}

			assert(output==NULL);
			output=MGData::dataWithBytesNoCopy(m_pData,m_nWidth * m_nHeight * channels);
		} while (0);

out:
		if (png_ptr)
		{
			png_destroy_read_struct(&png_ptr, (info_ptr) ? &info_ptr : 0, 0);
		}
	
		
		return bRet;
	}

	MGImage * MGPNGCodec::decode( const unsigned char* input, size_t input_size,ColorFormat format )
	{
		std::vector<unsigned char> output;
		int w(0),h(0);
		if(MGPNGCodec::decode(input,input_size, format, &output,&w,&h))
		{
            PixelFormat pf = PF_DEFAULT;
            switch (format) {
                case MGPNGCodec::FORMAT_GRAY:
                    pf = PF_GRAY8;
                    break;
                case MGPNGCodec::FORMAT_GRAYA:
                    pf = PF_GRAYA88;
                    break;
                case MGPNGCodec::FORMAT_RGB:
                    pf = PF_RGBA8880;
                    break;
                case MGPNGCodec::FORMAT_RGBA:
                    pf = PF_RGBA8888;
                    break;
                default:
                    break;
            }
			//MGLogD("sizeof output =%d,width=%d,height=%d",output.size(),w,h);
			int bit_count=output.size()*8/w/h;
			MGImage *ret=MGPersistenceImage::fromBlank(bit_count,w,h,pf);
			if (ret!=NULL)
			{
				byte_8 *pWriteData=const_cast<byte_8 *>(ret->getContentData());
				const int content_data_bytes=ret->content_info.adapter_width*ret->content_info.bit_count/8;
				int widthbyte = output.size()/h;
				for (int i=ret->content_info.height_in_pixel-1; i>=0; i--)
				{
					memcpy(pWriteData,&output[i*widthbyte],widthbyte);
					pWriteData+=content_data_bytes;
				}
			}
			return ret;
		}
		return NULL;
	}


	bool MGPNGCodec::encode( const MGImage *image, std::vector<unsigned char>& output,int quality )
	{
		return encode(image->getContentData(),image->content_info,output,quality);
	}
	static void error_fn(png_structp png_ptr, png_const_charp msg) {
	#if 0
	   MGLogD ("------ png error %s\n", msg);
	#endif
		longjmp(png_jmpbuf(png_ptr), 1);
	}
	static void write_fn(png_structp png_ptr, png_bytep data, png_size_t len) {
		vector<unsigned char>* sk_stream = (vector<unsigned char>*)png_get_io_ptr(png_ptr);
		sk_stream->insert(sk_stream->end(),data,data+len);
		//fwrite(data,len,1,sk_stream);
		//if (!sk_stream->write(data, len)) {
		//	png_error(png_ptr, "sk_write_fn Error!");
		//}
	}

	typedef void (*transform_scanline_proc)(const unsigned char*  src,
		int width, unsigned  char*  dst);

	static void transform_scanline_565(const unsigned char*  src, int width,
		unsigned char*  dst) {
			const uint_16*  srcP = (const uint_16*)src;    
			for (int i = 0; i < width; i++) {
				unsigned c = *srcP++;
				*dst++ =(unsigned char)(c>>11<<3);
				*dst++ =(unsigned char)(c>>5<<2);
				*dst++ =(unsigned char)(c<<3);
			}
	}

	static void transform_scanline_888(const unsigned char*  src, int width,
		unsigned char*  dst) {  
			for (int i = 0; i < width; i++) {
				memcpy(dst,src,3);
				dst+=3;
				src+=3;
			}
	}

	static void transform_scanline_8888(const unsigned char*  src, int width,
		unsigned char*  dst) {
			for (int i = 0; i < width; i++) {
				memcpy(dst,src,4);
				dst+=4;
				src+=4;
			}
	}

	static transform_scanline_proc choose_proc(PixelFormat config,
		bool hasAlpha) {

			static const struct {
				PixelFormat        fConfig;
				bool                    fHasAlpha;
				transform_scanline_proc fProc;
			} gMap[] = {
				{ PF_RGBA5650,    false,  transform_scanline_565 },
				{ PF_RGBA8880,  false,  transform_scanline_888 },
				{ PF_RGBA8888,  true,   transform_scanline_8888 },
			};

			for (int i = 3 - 1; i >= 0; --i) {
				if (gMap[i].fConfig == config && gMap[i].fHasAlpha == hasAlpha) {
					return gMap[i].fProc;
				}
			}
			return NULL;
	}

	bool MGPNGCodec::encode( const byte_8*input,const MGImageInfo &info,std::vector<unsigned char>& output,int quality,bool reverse)
	{
		const bool hasAlpha =( info.bit_count>=32);
		int colorType= PNG_COLOR_MASK_COLOR;
		int bitDepth= 8;
		png_color_8 sig_bit;

		switch (info.format)
		{
		case PF_RGBA8888:
			sig_bit.red = 8;
			sig_bit.green = 8;
			sig_bit.blue = 8;
			sig_bit.alpha = 8;
			break;
		case PF_RGBA5650:
			sig_bit.red = 5;
			sig_bit.green = 6;
			sig_bit.blue = 5;
			sig_bit.alpha = 0;
			break;
		case PF_RGBA8880:
			sig_bit.red = 8;
			sig_bit.green = 8;
			sig_bit.blue = 8;
			sig_bit.alpha = 0;
			break;
		default:
			assert((info.format==PF_RGBA8888)||(info.format==PF_RGBA5650));
			break;
		}
		if (hasAlpha)
		{
			colorType|=PNG_COLOR_MASK_ALPHA;
		}
		else
		{
			sig_bit.alpha=0;
		}

		if (info.bit_count!=0)
		{
			//bitDepth=info.bit_count;
		}
		
		
		{

			png_structp png_ptr;
			png_infop info_ptr;

			png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, error_fn,
				NULL);
			if (NULL == png_ptr) {
				return false;
			}

			info_ptr = png_create_info_struct(png_ptr);
			if (NULL == info_ptr) {
				png_destroy_write_struct(&png_ptr,  NULL);
				return false;
			}

			/* Set error handling.  REQUIRED if you aren't supplying your own
			* error handling functions in the png_create_write_struct() call.
			*/
			if (setjmp(png_jmpbuf(png_ptr))) {
				png_destroy_write_struct(&png_ptr, &info_ptr);
				return false;
			}

			png_set_write_fn(png_ptr, (void*)&output, write_fn, NULL);

			/* Set the image information here.  Width and height are up to 2^31,
			* bit_depth is one of 1, 2, 4, 8, or 16, but valid values also depend on
			* the color_type selected. color_type is one of PNG_COLOR_TYPE_GRAY,
			* PNG_COLOR_TYPE_GRAY_ALPHA, PNG_COLOR_TYPE_PALETTE, PNG_COLOR_TYPE_RGB,
			* or PNG_COLOR_TYPE_RGB_ALPHA.  interlace is either PNG_INTERLACE_NONE or
			* PNG_INTERLACE_ADAM7, and the compression_type and filter_type MUST
			* currently be PNG_COMPRESSION_TYPE_BASE and PNG_FILTER_TYPE_BASE. REQUIRED
			*/

			png_set_IHDR(png_ptr, info_ptr, info.width_in_pixel,info.height_in_pixel,
				bitDepth, colorType,
				PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
				PNG_FILTER_TYPE_BASE);

			// set our colortable/trans arrays if needed
			//png_color paletteColors[256];
			//png_byte trans[256];
			//if (SkBitmap::kIndex8_Config == config) {
			//	SkColorTable* ct = bitmap.getColorTable();
			//	int numTrans = pack_palette(ct, paletteColors, trans, hasAlpha);
			//	png_set_PLTE(png_ptr, info_ptr, paletteColors, ct->count());
			//	if (numTrans > 0) {
			//		png_set_tRNS(png_ptr, info_ptr, trans, numTrans, NULL);
			//	}
			//}

			png_set_sBIT(png_ptr, info_ptr, &sig_bit);
			png_write_info(png_ptr, info_ptr);

			const unsigned char* srcImage = input;
			//SkAutoSMalloc<1024> rowStorage(bitmap.width() << 2);
			//char* storage = (char*)rowStorage.get();
			unsigned char* storage = new unsigned char[info.width_in_pixel<<2];
			transform_scanline_proc proc = choose_proc(info.format, hasAlpha);

			if (reverse)
			{
				for (int y = 0; y < info.height_in_pixel; y++) {
					srcImage =input+ y*info.width_in_pixel*info.bit_count/8;//要考虑字节对齐 ?  //
					png_bytep row_ptr = (png_bytep)storage;
					proc(srcImage, info.width_in_pixel, storage);
					png_write_rows(png_ptr, &row_ptr, 1);
				}
			}
			else
			{
				for (int y = 1; y <= info.height_in_pixel; y++) {
					srcImage =input+(info.height_in_pixel - y)* info.width_in_pixel*info.bit_count/8;//要考虑字节对齐 ?  //
					png_bytep row_ptr = (png_bytep)storage;
					proc(srcImage, info.width_in_pixel, storage);
					png_write_rows(png_ptr, &row_ptr, 1);
				}
			}
			

			png_write_end(png_ptr, info_ptr);

			/* clean up after the write, and free any memory allocated */
			png_destroy_write_struct(&png_ptr, &info_ptr);
			delete_array_and_set_null(storage);
			return true;
		}
		return false;
	}



}














































////////////////////////////////////////////////////////////////////////////
//using namespace  z11;
//
//class SkImageDecoder
//{
//
//};
//enum Format
//{
//	kPNG_Format
//};
//typedef MGData SkStream ;
//typedef MGImage SkBitmap;
//class SkPNGImageDecoder : public SkImageDecoder {
//public:
//	virtual Format getFormat() const {
//		return kPNG_Format;
//	}
//
//protected:
//	virtual bool onDecode(SkStream* stream, SkBitmap* bm);
//};
//
//#ifndef png_jmpbuf
//#  define png_jmpbuf(png_ptr) ((png_ptr)->jmpbuf)
//#endif
//
//#define PNG_BYTES_TO_CHECK 4
//
///* Automatically clean up after throwing an exception */
//struct PNGAutoClean {
//	PNGAutoClean(png_structp p, png_infop i): png_ptr(p), info_ptr(i) {}
//	~PNGAutoClean() {
//		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
//	}
//private:
//	png_structp png_ptr;
//	png_infop info_ptr;
//};
//
//static void sk_read_fn(png_structp png_ptr, png_bytep data, png_size_t length) {
//	SkStream* sk_stream = (SkStream*)png_get_io_ptr(png_ptr);
//	size_t bytes = sk_stream->read(data, length);
//	if (bytes != length) {
//		png_error(png_ptr, "Read Error!");
//	}
//}
//
////static int sk_read_user_chunk(png_structp png_ptr, png_unknown_chunkp chunk) {
////	SkImageDecoder::Peeker* peeker =
////		(SkImageDecoder::Peeker*)png_get_user_chunk_ptr(png_ptr);
////	// peek() returning true means continue decoding
////	return peeker->peek((const char*)chunk->name, chunk->data, chunk->size) ?
////		1 : -1;
////}
//
//static void sk_error_fn(png_structp png_ptr, png_const_charp msg) {
//#if 0
//	SkDebugf("------ png error %s\n", msg);
//#endif
//	longjmp(png_jmpbuf(png_ptr), 1);
//}
//
//static void skip_src_rows(png_structp png_ptr, uint8_t storage[], int count) {
//	for (int i = 0; i < count; i++) {
//		uint8_t* tmp = storage;
//		png_read_rows(png_ptr, &tmp, NULL, 1);
//	}
//}
//
//static bool pos_le(int value, int max) {
//	return value > 0 && value <= max;
//}
//
//static bool substituteTranspColor(SkBitmap* bm, SkPMColor match) {
//	SkASSERT(bm->config() == SkBitmap::kARGB_8888_Config);
//
//	bool reallyHasAlpha = false;
//
//	for (int y = bm->height() - 1; y >= 0; --y) {
//		SkPMColor* p = bm->getAddr32(0, y);
//		for (int x = bm->width() - 1; x >= 0; --x) {
//			if (match == *p) {
//				*p = 0;
//				reallyHasAlpha = true;
//			}
//			p += 1;
//		}
//	}
//	return reallyHasAlpha;
//}
//
//static bool canUpscalePaletteToConfig(SkBitmap::Config dstConfig,
//									  bool srcHasAlpha) {
//										  switch (dstConfig) {
//		case SkBitmap::kARGB_8888_Config:
//		case SkBitmap::kARGB_4444_Config:
//			return true;
//		case SkBitmap::kRGB_565_Config:
//			// only return true if the src is opaque (since 565 is opaque)
//			return !srcHasAlpha;
//		default:
//			return false;
//										  }
//}
//
//// call only if color_type is PALETTE. Returns true if the ctable has alpha
//static bool hasTransparencyInPalette(png_structp png_ptr, png_infop info_ptr) {
//	png_bytep trans;
//	int num_trans;
//
//	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
//		png_get_tRNS(png_ptr, info_ptr, &trans, &num_trans, NULL);
//		return num_trans > 0;
//	}
//	return false;
//}
//
//bool SkPNGImageDecoder::onDecode(SkStream* sk_stream, SkBitmap* decodedBitmap
//					) {
//									 //    SkAutoTrace    apr("SkPNGImageDecoder::onDecode");
//
//									 /* Create and initialize the png_struct with the desired error handler
//									 * functions.  If you want to use the default stderr and longjump method,
//									 * you can supply NULL for the last three parameters.  We also supply the
//									 * the compiler header file version, so that we know if the application
//									 * was compiled with a compatible version of the library.  */
//									 png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
//										 NULL, sk_error_fn, NULL);
//									 //   png_voidp user_error_ptr, user_error_fn, user_warning_fn);
//									 if (png_ptr == NULL) {
//										 return false;
//									 }
//
//									 /* Allocate/initialize the memory for image information. */
//									 png_infop info_ptr = png_create_info_struct(png_ptr);
//									 if (info_ptr == NULL) {
//										 png_destroy_read_struct(&png_ptr, NULL, NULL);
//										 return false;
//									 }
//
//									 PNGAutoClean autoClean(png_ptr, info_ptr);
//
//									 /* Set error handling if you are using the setjmp/longjmp method (this is
//									 * the normal method of doing things with libpng).  REQUIRED unless you
//									 * set up your own error handlers in the png_create_read_struct() earlier.
//									 */
//									 if (setjmp(png_jmpbuf(png_ptr))) {
//										 return false;
//									 }
//
//									 /* If you are using replacement read functions, instead of calling
//									 * png_init_io() here you would call:
//									 */
//									 png_set_read_fn(png_ptr, (void *)sk_stream, sk_read_fn);
//									 /* where user_io_ptr is a structure you want available to the callbacks */
//									 /* If we have already read some of the signature */
//									 //  png_set_sig_bytes(png_ptr, 0 /* sig_read */ );
//
//									 // hookup our peeker so we can see any user-chunks the caller may be interested in
//									 png_set_keep_unknown_chunks(png_ptr, PNG_HANDLE_CHUNK_ALWAYS, (png_byte*)"", 0);
//									 //unknown peeker by GRF
//									 //if (this->getPeeker()) {
//										// png_set_read_user_chunk_fn(png_ptr, (png_voidp)this->getPeeker(), sk_read_user_chunk);
//									 //}
//
//									 /* The call to png_read_info() gives us all of the information from the
//									 * PNG file before the first IDAT (image data chunk). */
//									 png_read_info(png_ptr, info_ptr);
//									 png_uint_32 origWidth, origHeight;
//									 int bit_depth, color_type, interlace_type;
//									 png_get_IHDR(png_ptr, info_ptr, &origWidth, &origHeight, &bit_depth, &color_type,
//										 &interlace_type, NULL, NULL);
//
//									 /* tell libpng to strip 16 bit/color files down to 8 bits/color */
//									 if (bit_depth == 16) {
//										 png_set_strip_16(png_ptr);
//									 }
//									 /* Extract multiple pixels with bit depths of 1, 2, and 4 from a single
//									 * byte into separate bytes (useful for paletted and grayscale images). */
//									 if (bit_depth < 8) {
//										 png_set_packing(png_ptr);
//									 }
//									 /* Expand grayscale images to the full 8 bits from 1, 2, or 4 bits/pixel */
//									 if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
//										 png_set_expand_gray_1_2_4_to_8(png_ptr);
//									 }
//
//									 /* Make a grayscale image into RGB. */
//									 if (color_type == PNG_COLOR_TYPE_GRAY ||
//										 color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
//											 png_set_gray_to_rgb(png_ptr);
//									 }
//
//									PixelFormat   config;
//									 bool                hasAlpha = false;
//									 bool                doDither = this->getDitherImage();
//									 SkPMColor           theTranspColor = 0; // 0 tells us not to try to match
//
//									 // check for sBIT chunk data, in case we should disable dithering because
//									 // our data is not truely 8bits per component
//									 if (doDither) {
//										 png_color_8p sig_bit = NULL;
//										 bool has_sbit = PNG_INFO_sBIT == png_get_sBIT(png_ptr, info_ptr,
//											 &sig_bit);
//#if 0
//										 if (has_sbit) {
//											 SkDebugf("----- sBIT %d %d %d %d\n", sig_bit->red, sig_bit->green,
//												 sig_bit->blue, sig_bit->alpha);
//										 }
//#endif
//										 // 0 seems to indicate no information available
//										 if (has_sbit && pos_le(sig_bit->red, SK_R16_BITS) &&
//											 pos_le(sig_bit->green, SK_G16_BITS) &&
//											 pos_le(sig_bit->blue, SK_B16_BITS)) {
//												 doDither = false;
//										 }
//									 }
//
//									 if (color_type == PNG_COLOR_TYPE_PALETTE) {
//										 bool paletteHasAlpha = hasTransparencyInPalette(png_ptr, info_ptr);
//										 config = this->getPrefConfig(kIndex_SrcDepth, paletteHasAlpha);
//										 // now see if we can upscale to their requested config
//										 if (!canUpscalePaletteToConfig(config, paletteHasAlpha)) {
//											 config = SkBitmap::kIndex8_Config;
//										 }
//									 } else {
//										 png_color_16p   transpColor = NULL;
//										 int             numTransp = 0;
//
//										 png_get_tRNS(png_ptr, info_ptr, NULL, &numTransp, &transpColor);
//
//										 bool valid = png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS);
//
//										 if (valid && numTransp == 1 && transpColor != NULL) {
//											 /*  Compute our transparent color, which we'll match against later.
//											 We don't really handle 16bit components properly here, since we
//											 do our compare *after* the values have been knocked down to 8bit
//											 which means we will find more matches than we should. The real
//											 fix seems to be to see the actual 16bit components, do the
//											 compare, and then knock it down to 8bits ourselves.
//											 */
//											 if (color_type & PNG_COLOR_MASK_COLOR) {
//												 if (16 == bit_depth) {
//													 theTranspColor = SkPackARGB32(0xFF, transpColor->red >> 8,
//														 transpColor->green >> 8, transpColor->blue >> 8);
//												 } else {
//													 theTranspColor = SkPackARGB32(0xFF, transpColor->red,
//														 transpColor->green, transpColor->blue);
//												 }
//											 } else {    // gray
//												 if (16 == bit_depth) {
//													 theTranspColor = SkPackARGB32(0xFF, transpColor->gray >> 8,
//														 transpColor->gray >> 8, transpColor->gray >> 8);
//												 } else {
//													 theTranspColor = SkPackARGB32(0xFF, transpColor->gray,
//														 transpColor->gray, transpColor->gray);
//												 }
//											 }
//										 }
//
//										 if (valid ||
//											 PNG_COLOR_TYPE_RGB_ALPHA == color_type ||
//											 PNG_COLOR_TYPE_GRAY_ALPHA == color_type) {
//												 hasAlpha = true;
//										 }
//										 config = this->getPrefConfig(k32Bit_SrcDepth, hasAlpha);
//										 // now match the request against our capabilities
//										 if (hasAlpha) {
//											 if (config != PF_RGBA4444) {
//												 config = PF_RGBA8888;
//											 }
//										 } else {
//											 if (config != PF_RGBA5650 &&
//												 config != PF_RGBA4444) {
//													 config =PF_RGBA8888;
//											 }
//										 }
//									 }
//
//									 // sanity check for size
//									 {
//										 Sk64 size;
//										 size.setMul(origWidth, origHeight);
//										 if (size.isNeg() || !size.is32()) {
//											 return false;
//										 }
//										 // now check that if we are 4-bytes per pixel, we also don't overflow
//										 if (size.get32() > (0x7FFFFFFF >> 2)) {
//											 return false;
//										 }
//									 }
//
//									 if (!this->chooseFromOneChoice(config, origWidth, origHeight)) {
//										 return false;
//									 }
//
//									 const int sampleSize = this->getSampleSize();
//									 SkScaledBitmapSampler sampler(origWidth, origHeight, sampleSize);
//
//									 decodedBitmap->setConfig(config, sampler.scaledWidth(),
//										 sampler.scaledHeight(), 0);
//									 //if (SkImageDecoder::kDecodeBounds_Mode == mode) {
//										// return true;
//									 //}
//
//									 // from here down we are concerned with colortables and pixels
//
//									 // we track if we actually see a non-opaque pixels, since sometimes a PNG sets its colortype
//									 // to |= PNG_COLOR_MASK_ALPHA, but all of its pixels are in fact opaque. We care, since we
//									 // draw lots faster if we can flag the bitmap has being opaque
//									 bool reallyHasAlpha = false;
//									 SkColorTable* colorTable = NULL;
//
//									 if (color_type == PNG_COLOR_TYPE_PALETTE) {
//										 int num_palette;
//										 png_colorp palette;
//										 png_bytep trans;
//										 int num_trans;
//
//										 png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);
//
//										 /*  BUGGY IMAGE WORKAROUND
//
//										 We hit some images (e.g. fruit_.png) who contain bytes that are == colortable_count
//										 which is a problem since we use the byte as an index. To work around this we grow
//										 the colortable by 1 (if its < 256) and duplicate the last color into that slot.
//										 */
//										 int colorCount = num_palette + (num_palette < 256);
//
//										 colorTable = SkNEW_ARGS(SkColorTable, (colorCount));
//
//										 SkPMColor* colorPtr = colorTable->lockColors();
//										 if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
//											 png_get_tRNS(png_ptr, info_ptr, &trans, &num_trans, NULL);
//											 hasAlpha = (num_trans > 0);
//										 } else {
//											 num_trans = 0;
//											 colorTable->setFlags(colorTable->getFlags() | SkColorTable::kColorsAreOpaque_Flag);
//										 }        
//										 // check for bad images that might make us crash
//										 if (num_trans > num_palette) {
//											 num_trans = num_palette;
//										 }
//
//										 int index = 0;
//										 int transLessThanFF = 0;
//
//										 for (; index < num_trans; index++) {
//											 transLessThanFF |= (int)*trans - 0xFF;
//											 *colorPtr++ = SkPreMultiplyARGB(*trans++, palette->red, palette->green, palette->blue);
//											 palette++;
//										 }
//										 reallyHasAlpha |= (transLessThanFF < 0);
//
//										 for (; index < num_palette; index++) {
//											 *colorPtr++ = SkPackARGB32(0xFF, palette->red, palette->green, palette->blue);
//											 palette++;
//										 }
//
//										 // see BUGGY IMAGE WORKAROUND comment above
//										 if (num_palette < 256) {
//											 *colorPtr = colorPtr[-1];
//										 }
//										 colorTable->unlockColors(true);
//									 }
//
//									 SkAutoUnref aur(colorTable);
//
//									 if (!this->allocPixelRef(decodedBitmap,
//										 SkBitmap::kIndex8_Config == config ?
//colorTable : NULL)) {
//									 return false;
//									 }
//
//									 SkAutoLockPixels alp(*decodedBitmap);
//
//									 /* swap the RGBA or GA data to ARGB or AG (or BGRA to ABGR) */
//									 //  if (color_type == PNG_COLOR_TYPE_RGB_ALPHA)
//									 //      ; // png_set_swap_alpha(png_ptr);
//
//									 /* swap bytes of 16 bit files to least significant byte first */
//									 //   png_set_swap(png_ptr);
//
//									 /* Add filler (or alpha) byte (before/after each RGB triplet) */
//									 if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY) {
//										 png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
//									 }
//
//									 /* Turn on interlace handling.  REQUIRED if you are not using
//									 * png_read_image().  To see how to handle interlacing passes,
//									 * see the png_read_row() method below:
//									 */
//									 const int number_passes = interlace_type != PNG_INTERLACE_NONE ? 
//										 png_set_interlace_handling(png_ptr) : 1;
//
//									 /* Optional call to gamma correct and add the background to the palette
//									 * and update info structure.  REQUIRED if you are expecting libpng to
//									 * update the palette for you (ie you selected such a transform above).
//									 */
//									 png_read_update_info(png_ptr, info_ptr);
//
//									 if (SkBitmap::kIndex8_Config == config && 1 == sampleSize) {
//										 for (int i = 0; i < number_passes; i++) {
//											 for (png_uint_32 y = 0; y < origHeight; y++) {
//												 uint8_t* bmRow = decodedBitmap->getAddr8(0, y);
//												 png_read_rows(png_ptr, &bmRow, NULL, 1);
//											 }
//										 }
//									 } else {
//										 SkScaledBitmapSampler::SrcConfig sc;
//										 int srcBytesPerPixel = 4;
//
//										 if (colorTable != NULL) {
//											 sc = SkScaledBitmapSampler::kIndex;
//											 srcBytesPerPixel = 1;
//										 } else if (hasAlpha) {
//											 sc = SkScaledBitmapSampler::kRGBA;
//										 } else {
//											 sc = SkScaledBitmapSampler::kRGBX;
//										 }
//
//										 /*  We have to pass the colortable explicitly, since we may have one
//										 even if our decodedBitmap doesn't, due to the request that we
//										 upscale png's palette to a direct model
//										 */
//										 SkAutoLockColors ctLock(colorTable);
//										 if (!sampler.begin(decodedBitmap, sc, doDither, ctLock.colors())) {
//											 return false;
//										 }
//										 const int height = decodedBitmap->height();
//
//										 if (number_passes > 1) {
//											 SkAutoMalloc storage(origWidth * origHeight * srcBytesPerPixel);
//											 uint8_t* base = (uint8_t*)storage.get();
//											 size_t rb = origWidth * srcBytesPerPixel;
//
//											 for (int i = 0; i < number_passes; i++) {
//												 uint8_t* row = base;
//												 for (png_uint_32 y = 0; y < origHeight; y++) {
//													 uint8_t* bmRow = row;
//													 png_read_rows(png_ptr, &bmRow, NULL, 1);
//													 row += rb;
//												 }
//											 }
//											 // now sample it
//											 base += sampler.srcY0() * rb;
//											 for (int y = 0; y < height; y++) {
//												 reallyHasAlpha |= sampler.next(base);
//												 base += sampler.srcDY() * rb;
//											 }
//										 } else {
//											 SkAutoMalloc storage(origWidth * srcBytesPerPixel);
//											 uint8_t* srcRow = (uint8_t*)storage.get();
//											 skip_src_rows(png_ptr, srcRow, sampler.srcY0());
//
//											 for (int y = 0; y < height; y++) {
//												 uint8_t* tmp = srcRow;
//												 png_read_rows(png_ptr, &tmp, NULL, 1);
//												 reallyHasAlpha |= sampler.next(srcRow);
//												 if (y < height - 1) {
//													 skip_src_rows(png_ptr, srcRow, sampler.srcDY() - 1);
//												 }
//											 }
//
//											 // skip the rest of the rows (if any)
//											 png_uint_32 read = (height - 1) * sampler.srcDY() +
//												 sampler.srcY0() + 1;
//											 SkASSERT(read <= origHeight);
//											 skip_src_rows(png_ptr, srcRow, origHeight - read);
//										 }
//									 }
//
//									 /* read rest of file, and get additional chunks in info_ptr - REQUIRED */
//									 png_read_end(png_ptr, info_ptr);
//
//									 if (0 != theTranspColor) {
//										 reallyHasAlpha |= substituteTranspColor(decodedBitmap, theTranspColor);
//									 }
//									 decodedBitmap->setIsOpaque(!reallyHasAlpha);
//									 return true;
//}
//
/////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////