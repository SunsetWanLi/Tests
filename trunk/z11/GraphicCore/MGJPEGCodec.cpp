#include "MGJPEGCodec.h"
#include <stdio.h>
#include "../Platform/MGPlatformConfig.h"
#if MG_TARGET_PLATFORM == MG_PLATFORM_ANDROID
#include "../Platform/Android/prebuilt/libjpeg/include/jpeglib.h"
#else
#include "../ExternalLib/jpeg/jpeglib.h"
#endif
#include "../System/Shared/MGLog.h"
#include "MGPersistenceImage.h"
#include <utility>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>
#include "SkImageDecoder.h"
namespace z11
{

	// Encoder/decoder shared stuff ------------------------------------------------

	namespace
	{

		// used to pass error info through the JPEG library
		struct CoderErrorMgr
		{
			jpeg_error_mgr pub;
			jmp_buf setjmp_buffer;
		};

		void ErrorExit(jpeg_common_struct* cinfo)
		{
			CoderErrorMgr *err = reinterpret_cast<CoderErrorMgr*>(cinfo->err);

			// Return control to the setjmp point.
			longjmp(err->setjmp_buffer, false);
		}

	}

	// Encoder ---------------------------------------------------------------------
	//
	// This code is based on nsJPEGEncoder from Mozilla.
	// Copyright 2005 Google Inc. (Brett Wilson, contributor)

	namespace
	{

		// Initial size for the output buffer in the JpegEncoderState below.
		static const int initial_output_buffer_size = 8192;

		struct JpegEncoderState
		{
			explicit JpegEncoderState(std::vector<unsigned char>* o)
				: out(o), image_buffer_used(0) {}

			// Output buffer, of which 'image_buffer_used' bytes are actually used (this
			// will often be less than the actual size of the vector because we size it
			// so that libjpeg can write directly into it.
			std::vector<unsigned char>* out;

			// Number of bytes in the 'out' buffer that are actually used (see above).
			size_t image_buffer_used;
		};

		// Initializes the JpegEncoderState for encoding, and tells libjpeg about where
		// the output buffer is.
		//
		// From the JPEG library:
		//  "Initialize destination. This is called by jpeg_start_compress() before
		//   any data is actually written. It must initialize next_output_byte and
		//   free_in_buffer. free_in_buffer must be initialized to a positive value."
		void InitDestination(jpeg_compress_struct* cinfo)
		{
			JpegEncoderState* state = static_cast<JpegEncoderState*>(cinfo->client_data);
			//DCHECK(state->image_buffer_used == 0) << "initializing after use";

			state->out->resize(initial_output_buffer_size);
			state->image_buffer_used = 0;

			cinfo->dest->next_output_byte = &(*state->out)[0];
			cinfo->dest->free_in_buffer = initial_output_buffer_size;
		}

		// Resize the buffer that we give to libjpeg and update our and its state.
		//
		// From the JPEG library:
		//  "Callback used by libjpeg whenever the buffer has filled (free_in_buffer
		//   reaches zero). In typical applications, it should write out the *entire*
		//   buffer (use the saved start address and buffer length; ignore the current
		//   state of next_output_byte and free_in_buffer). Then reset the pointer &
		//   count to the start of the buffer, and return TRUE indicating that the
		//   buffer has been dumped. free_in_buffer must be set to a positive value
		//   when TRUE is returned. A FALSE return should only be used when I/O
		//   suspension is desired (this operating mode is discussed in the next
		//   section)."
		boolean EmptyOutputBuffer(jpeg_compress_struct* cinfo)
		{
			JpegEncoderState* state = static_cast<JpegEncoderState*>(cinfo->client_data);

			// note the new size, the buffer is full
			state->image_buffer_used = state->out->size();

			// expand buffer, just double size each time
			state->out->resize(state->out->size() * 2);

			// tell libjpeg where to write the next data
			cinfo->dest->next_output_byte = &(*state->out)[state->image_buffer_used];
			cinfo->dest->free_in_buffer = state->out->size() - state->image_buffer_used;
			return 1;
		}

		// Cleans up the JpegEncoderState to prepare for returning in the final form.
		//
		// From the JPEG library:
		//  "Terminate destination --- called by jpeg_finish_compress() after all data
		//   has been written. In most applications, this must flush any data
		//   remaining in the buffer. Use either next_output_byte or free_in_buffer to
		//   determine how much data is in the buffer."
		void TermDestination(jpeg_compress_struct* cinfo)
		{
			JpegEncoderState* state = static_cast<JpegEncoderState*>(cinfo->client_data);
//			DCHECK(state->out->size() >= state->image_buffer_used);

			// update the used uint_8 based on the next uint_8 libjpeg would write to
			state->image_buffer_used = cinfo->dest->next_output_byte - &(*state->out)[0];
//			DCHECK(state->image_buffer_used < state->out->size()) <<"JPEG library busted, got a bad image buffer size";

			// update our buffer so that it exactly encompases the desired data
			state->out->resize(state->image_buffer_used);
		}

		// Converts RGBA to RGB (removing the alpha values) to prepare to send data to
		// libjpeg. This converts one row of data in rgba with the given width in
		// pixels the the given rgb destination buffer (which should have enough space
		// reserved for the final data).
		void StripAlpha(const unsigned char* rgba, int pixel_width, unsigned char* rgb)
		{
			for(int x=0; x<pixel_width; ++x)
			{
				const unsigned char* pixel_in = &rgba[x*4];
				unsigned char* pixel_out = &rgb[x*3];
				pixel_out[0] = pixel_in[0];
				pixel_out[1] = pixel_in[1];
				pixel_out[2] = pixel_in[2];
			}
		}

		// Converts BGRA to RGB by reordering the color components and dropping the
		// alpha. This converts  one row of data in rgba with the given width in
		// pixels the the given rgb destination buffer (which should have enough space
		// reserved for the final data).
		void BGRAtoRGB(const unsigned char* bgra, int pixel_width, unsigned char* rgb)
		{
			for(int x=0; x<pixel_width; ++x)
			{
				const unsigned char* pixel_in = &bgra[x*4];
				unsigned char* pixel_out = &rgb[x*3];
				pixel_out[0] = pixel_in[2];
				pixel_out[1] = pixel_in[1];
				pixel_out[2] = pixel_in[0];
			}
		}

		void BGRtoRGB(unsigned char* bgra, int pixel_width)
		{
			for(int x=0; x<pixel_width/3; ++x)
			{
				//unsigned char* pixel_in = &bgra[x*3];
				//std::swap(pixel_in[0],pixel_in[2]);
			}
		}

		// This class destroys the given jpeg_compress object when it goes out of
		// scope. It simplifies the error handling in Encode (and even applies to the
		// success case).
		class CompressDestroyer
		{
		public:
			CompressDestroyer() : cinfo_(NULL) {}
			~CompressDestroyer()
			{
				DestroyManagedObject();
			}
			void SetManagedObject(jpeg_compress_struct* ci)
			{
				DestroyManagedObject();
				cinfo_ = ci;
			}
			void DestroyManagedObject()
			{
				if(cinfo_)
				{
					jpeg_destroy_compress(cinfo_);
					cinfo_ = NULL;
				}
			}

		private:
			jpeg_compress_struct* cinfo_;
		};

	}

	// Decoder --------------------------------------------------------------------

	namespace
	{

		struct JpegDecoderState
		{
			JpegDecoderState(const unsigned char* in, size_t len)
				: input_buffer(in), input_buffer_length(len) {}

			const unsigned char* input_buffer;
			size_t input_buffer_length;
		};

		// Callback to initialize the source.
		//
		// From the JPEG library:
		//  "Initialize source. This is called by jpeg_read_header() before any data is
		//   actually read. May leave bytes_in_buffer set to 0 (in which case a
		//   fill_input_buffer() call will occur immediately)."
		void InitSource(j_decompress_ptr cinfo)
		{
			JpegDecoderState* state = static_cast<JpegDecoderState*>(cinfo->client_data);
			cinfo->src->next_input_byte = state->input_buffer;
			cinfo->src->bytes_in_buffer = state->input_buffer_length;
		}

		// Callback to fill the buffer. Since our buffer already contains all the data,
		// we should never need to provide more data. If libjpeg thinks it needs more
		// data, our input is probably corrupt.
		//
		// From the JPEG library:
		//  "This is called whenever bytes_in_buffer has reached zero and more data is
		//   wanted. In typical applications, it should read fresh data into the buffer
		//   (ignoring the current state of next_input_byte and bytes_in_buffer), reset
		//   the pointer & count to the start of the buffer, and return TRUE indicating
		//   that the buffer has been reloaded. It is not necessary to fill the buffer
		//   entirely, only to obtain at least one more uint_8. bytes_in_buffer MUST be
		//   set to a positive value if TRUE is returned. A FALSE return should only
		//   be used when I/O suspension is desired."
		boolean FillInputBuffer(j_decompress_ptr cinfo)
		{
			return false;
		}

		// Skip data in the buffer. Since we have all the data at once, this operation
		// is easy. It is not clear if this ever gets called because the JPEG library
		// should be able to do the skip itself (it has all the data).
		//
		// From the JPEG library:
		//  "Skip num_bytes worth of data. The buffer pointer and count should be
		//   advanced over num_bytes input bytes, refilling the buffer as needed. This
		//   is used to skip over a potentially large amount of uninteresting data
		//   (such as an APPn marker). In some applications it may be possible to
		//   optimize away the reading of the skipped data, but it's not clear that
		//   being smart is worth much trouble; large skips are uncommon.
		//   bytes_in_buffer may be zero on return. A zero or negative skip count
		//   should be treated as a no-op."
		void SkipInputData(j_decompress_ptr cinfo, long num_bytes)
		{
			if(num_bytes > static_cast<long>(cinfo->src->bytes_in_buffer))
			{
				// Since all our data should be in the buffer, trying to skip beyond it
				// means that there is some kind of error or corrupt input data. A 0 for
				// bytes left means it will call FillInputBuffer which will then fail.
				cinfo->src->next_input_byte += cinfo->src->bytes_in_buffer;
				cinfo->src->bytes_in_buffer = 0;
			}
			else if(num_bytes > 0)
			{
				cinfo->src->bytes_in_buffer -= static_cast<size_t>(num_bytes);
				cinfo->src->next_input_byte += num_bytes;
			}
		}

		// Our source doesn't need any cleanup, so this is a NOP.
		//
		// From the JPEG library:
		//  "Terminate source --- called by jpeg_finish_decompress() after all data has
		//   been read to clean up JPEG source manager. NOT called by jpeg_abort() or
		//   jpeg_destroy()."
		void TermSource(j_decompress_ptr cinfo) {}

		// Converts one row of rgb data to rgba data by adding a fully-opaque alpha
		// value.
		void AddAlpha(const unsigned char* rgb, int pixel_width, unsigned char* rgba)
		{
			for(int x=0; x<pixel_width; ++x)
			{
				const unsigned char* pixel_in = &rgb[x*3];
				unsigned char* pixel_out = &rgba[x*4];
				pixel_out[0] = pixel_in[0];
				pixel_out[1] = pixel_in[1];
				pixel_out[2] = pixel_in[2];
				pixel_out[3] = 0xff;
			}
		}

		// Converts one row of RGB data to BGRA by reordering the color components and
		// adding alpha values of 0xff.
		void RGBtoBGRA(const unsigned char* bgra, int pixel_width, unsigned char* rgb)
		{
			for(int x=0; x<pixel_width; ++x)
			{
				const unsigned char* pixel_in = &bgra[x*3];
				unsigned char* pixel_out = &rgb[x*4];
				pixel_out[0] = pixel_in[2];
				pixel_out[1] = pixel_in[1];
				pixel_out[2] = pixel_in[0];
				pixel_out[3] = 0xff;
			}
		}

		// This class destroys the given jpeg_decompress object when it goes out of
		// scope. It simplifies the error handling in Decode (and even applies to the
		// success case).
		class DecompressDestroyer
		{
		public:
			DecompressDestroyer() : cinfo_(NULL) {}
			~DecompressDestroyer()
			{
				DestroyManagedObject();
			}
			void SetManagedObject(jpeg_decompress_struct* ci)
			{
				DestroyManagedObject();
				cinfo_ = ci;
			}
			void DestroyManagedObject()
			{
				if(cinfo_)
				{
					jpeg_destroy_decompress(cinfo_);
					cinfo_ = NULL;
				}
			}

		private:
			jpeg_decompress_struct* cinfo_;
		};

	}

	MGJPEGCodec::MGJPEGCodec(void)
	{
	}

	MGJPEGCodec::~MGJPEGCodec(void)
	{
	}

	bool MGJPEGCodec::decode( const unsigned char* input, size_t input_size, ColorFormat format, std::vector<unsigned char>* output, int* w, int* h )
	{
		SkBitmap bmp;
		bool ret=false;
		switch(format)
		{
        case FORMAT_INDEX_8:
            ret=SkImageDecoder::DecodeMemory(input,input_size,&bmp,SkBitmap::kIndex8_Config,
                SkImageDecoder::kDecodePixels_Mode);
                break;
		case FORMAT_RGB_565:
			ret=SkImageDecoder::DecodeMemory(input,input_size,&bmp,SkBitmap::kRGB_565_Config,
				SkImageDecoder::kDecodePixels_Mode);
			break;
		case FORMAT_RGBA:
			ret=SkImageDecoder::DecodeMemory(input,input_size,&bmp,SkBitmap::kARGB_8888_Config,
				SkImageDecoder::kDecodePixels_Mode);
			break;
		case FORMAT_BGRA:
			ret=SkImageDecoder::DecodeMemory(input,input_size,&bmp,SkBitmap::kARGB_8888_Config,
				SkImageDecoder::kDecodePixels_Mode);
			break;
        case FORMAT_DEFAULT:
        default:
            ret=SkImageDecoder::DecodeMemory(input,input_size,&bmp,SkBitmap::kRGB_565_Config,
                SkImageDecoder::kDecodePixels_Mode);
		}
		output->clear();
		if (ret)
		{
			unsigned char * pixels=(unsigned char *)(bmp.getPixels());
			output->insert(output->end(),pixels,pixels+bmp.getSize());
			*w=bmp.width();
			*h=bmp.height();
		}
		return ret;

		jpeg_decompress_struct cinfo;
		DecompressDestroyer destroyer;
		destroyer.SetManagedObject(&cinfo);
		output->clear();

		// We set up the normal JPEG error routines, then override error_exit.
		// This must be done before the call to create_decompress.
		CoderErrorMgr errmgr;
		cinfo.err = jpeg_std_error(&errmgr.pub);
		errmgr.pub.error_exit = ErrorExit;
		//// Establish the setjmp return context for ErrorExit to use.
		if(setjmp(errmgr.setjmp_buffer))
		{
			// If we get here, the JPEG code has signaled an error.
			// See note in JPEGCodec::Encode() for why we need to destroy the cinfo
			// manually here.
			MGLogD("failed to decode jpeg!");
			destroyer.DestroyManagedObject();
			return false;
		}

		//MGLogD("before jpeg_create_decompress(&cinfo)");
		// The destroyer will destroy() cinfo on exit.  We don't want to set the
		// destroyer's object until cinfo is initialized.
		jpeg_create_decompress(&cinfo);

		// set up the source manager
		jpeg_source_mgr srcmgr;
		srcmgr.init_source = InitSource;
		srcmgr.fill_input_buffer = FillInputBuffer;
		srcmgr.skip_input_data = SkipInputData;
		srcmgr.resync_to_restart = jpeg_resync_to_restart; // use default routine
		srcmgr.term_source = TermSource;
		cinfo.src = &srcmgr;

		JpegDecoderState state(input, input_size);
		cinfo.client_data = &state;

		//MGLogD("before jpeg_read_header(&cinfo, true)");

		// fill the file metadata into our buffer
		if(jpeg_read_header(&cinfo, true) != JPEG_HEADER_OK)
		{
			MGLogD("jpeg_read_header(&cinfo, true) != JPEG_HEADER_OK");
			return false;
		}
		//MGLogD("test middle");
		// we want to always get RGB data out
		switch(cinfo.jpeg_color_space)
		{
		case JCS_GRAYSCALE:
		case JCS_RGB:
		case JCS_YCbCr:
#ifdef JCS_EXTENSIONS
			// Choose an output colorspace and return if it is an unsupported one.
			// Same as JPEGCodec::Encode(), libjpeg-turbo supports all input formats
			// used by Chromium (i.e. RGB, RGBA, and BGRA) and we just map the input
			// parameters to a colorspace.
			if(format == FORMAT_RGB)
			{
				cinfo.out_color_space = JCS_RGB;
				cinfo.output_components = 3;
			}
			else if(format==FORMAT_RGBA || (format==FORMAT_SkBitmap && SK_R32_SHIFT==0))
			{
				cinfo.out_color_space = JCS_EXT_RGBX;
				cinfo.output_components = 4;
			}
			else if(format==FORMAT_BGRA || (format==FORMAT_SkBitmap && SK_B32_SHIFT==0))
			{
				cinfo.out_color_space = JCS_EXT_BGRX;
				cinfo.output_components = 4;
			}
			else
			{
				// We can exit this function without calling jpeg_destroy_decompress()
				// because DecompressDestroyer automaticaly calls it.
				NOTREACHED() << "Invalid pixel format";
				return false;
			}
#else
			cinfo.out_color_space = JCS_RGB;
#endif
			break;
		case JCS_CMYK:
		case JCS_YCCK:
		default:
			// Mozilla errors out on these color spaces, so I presume that the jpeg
			// library can't do automatic color space conversion for them. We don't
			// care about these anyway.
			return false;
		}
#ifndef JCS_EXTENSIONS
		cinfo.output_components = 3;
#endif

		jpeg_calc_output_dimensions(&cinfo);
		*w = cinfo.output_width;
		*h = cinfo.output_height;

		jpeg_start_decompress(&cinfo);

		// FIXME(brettw) we may want to allow the capability for callers to request
		// how to align row lengths as we do for the compressor.
		int row_read_stride = cinfo.output_width * cinfo.output_components;

#ifdef JCS_EXTENSIONS
		// Create memory for a decoded image and write decoded lines to the memory
		// without conversions same as JPEGCodec::Encode().
		int row_write_stride = row_read_stride;
		output->resize(row_write_stride * cinfo.output_height);

		for(int row=0; row<static_cast<int>(cinfo.output_height); ++row)
		{
			unsigned char* rowptr = &(*output)[row*row_write_stride];
			if(!jpeg_read_scanlines(&cinfo, &rowptr, 1))
			{
				return false;
			}
		}
#else
		if(format == FORMAT_RGB_565)
		{
			// easy case, row needs no conversion
			int row_write_stride = row_read_stride;
			output->resize(row_write_stride * cinfo.output_height);
			//MGLogD("format == FORMAT_RGB");

			for(int row=0; row<static_cast<int>(cinfo.output_height); ++row)
			{
				unsigned char* rowptr = &(*output)[row*row_write_stride];
				if(!jpeg_read_scanlines(&cinfo, &rowptr, 1))
				{
					return false;
				}
				//BGRtoRGB(rowptr,row_write_stride);
			}
		}
		else
		{
			// Rows need conversion to output format: read into a temporary buffer and
			// expand to the final one. Performance: we could avoid the extra
			// allocation by doing the expansion in-place.
			int row_write_stride;
			void (*converter)(const unsigned char* rgb, int w, unsigned char* out);
			if(format==FORMAT_RGBA )
			{
				row_write_stride = cinfo.output_width * 4;
				converter = AddAlpha;
			}
			else if(format==FORMAT_BGRA)
			{
				row_write_stride = cinfo.output_width * 4;
				converter = RGBtoBGRA;
			}
			else
			{
				MGLogD("Invalid pixel format") ;
				jpeg_destroy_decompress(&cinfo);
				return false;
			}

			output->resize(row_write_stride*cinfo.output_height);

			unsigned char* row_data=(new unsigned char[row_read_stride]);
			unsigned char* rowptr = row_data;
			for(int row=0; row<static_cast<int>(cinfo.output_height); ++row)
			{
				if(!jpeg_read_scanlines(&cinfo, &rowptr, 1))
				{
					return false;
				}
				converter(rowptr, *w, &(*output)[row*row_write_stride]);
			}
			delete_and_set_null(row_data);
		}
#endif

		jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);
		return true;
	}

	MGImage*  MGJPEGCodec::decode( const std::string &input, ColorFormat format )
	{
		MGData *input_data=MGData::dataWithContentsOfFile(input);
		if (input_data==NULL||(input_data->length()==0))
		{
			delete_and_set_null(input_data);
			MGLogD("can't find jpeg source file");
			return NULL;
		}

		std::vector<unsigned char> output;
		int w(0),h(0);
		//MGLogD("buffer id =%x,size=%d",buffer,size);
		//LogStart();
		if(!MGJPEGCodec::decode(static_cast<const unsigned char *>(input_data->bytes()),input_data->length(),format,&output,&w,&h))
		{
			delete_and_set_null(input_data);
			return NULL;
		}
		//LogEnd("MGJPEGCodec::Decode");
		delete_and_set_null(input_data);

		//MGLogD("sizeof output =%d,width=%d,height=%d",output.size(),w,h);
        int bit_count=output.size()*8/w/h;
		MGImage *ret=MGImage::fromBlank(bit_count,w,h);
		if (ret!=NULL)
		{
			byte_8 *pWriteData=const_cast<byte_8 *>(ret->getContentData());
			int widthbyte = output.size()/h;
			const int content_data_bytes=ret->content_info.adapter_width*ret->content_info.bit_count/8;
			for (int i=ret->content_info.height_in_pixel-1; i>=0; i--)
			{
				memcpy(pWriteData,&output[i*widthbyte],widthbyte);
				pWriteData+=content_data_bytes;
			}
		}
		return ret;
	}

	MGImage * MGJPEGCodec::decode( const unsigned char* input, size_t input_size,ColorFormat format )
	{

		std::vector<unsigned char> output;
		int w(0),h(0);
		if(MGJPEGCodec::decode(input,input_size,format,&output,&w,&h))
		{
			//MGLogD("sizeof output =%d,width=%d,height=%d",output.size(),w,h);
			int bit_count=output.size()*8/w/h;
			MGImage *ret=MGPersistenceImage::fromBlank(bit_count,w,h);
			if (ret!=NULL)
			{
				byte_8 *pWriteData=const_cast<byte_8 *>(ret->getContentData());
				const int content_data_bytes=ret->content_info.adapter_width*ret->content_info.bit_count/8;
				int widthbyte =output.size()/h;
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

	bool MGJPEGCodec::decode( const std::string & filepath,MGData *&output,MGImageInfo *info )
	{
		MGData *data=MGData::dataWithContentsOfFile(filepath);
		assert(data!=NULL&&data->bytes()!=NULL);
		/* these are standard libjpeg structures for reading(decompression) */
		struct jpeg_decompress_struct cinfo;
		struct jpeg_error_mgr jerr;
		/* libjpeg data structure for storing one row, that is, scanline of an image */
		JSAMPROW row_pointer[1] = {0};
		unsigned long location = 0;
		unsigned int i = 0;

		bool bRet = false;
		do 
		{
			/* here we set up the standard libjpeg error handler */
			cinfo.err = jpeg_std_error( &jerr );

			/* setup decompression process and source, then read JPEG header */
			jpeg_create_decompress( &cinfo );

			jpeg_mem_src( &cinfo, (unsigned char *) data->bytes(), data->length() );

			/* reading the image header which contains image information */
			jpeg_read_header( &cinfo, true );

			// we only support RGB or grayscale
			if (cinfo.jpeg_color_space != JCS_RGB)
			{
				if (cinfo.jpeg_color_space == JCS_GRAYSCALE || cinfo.jpeg_color_space == JCS_YCbCr)
				{
					cinfo.out_color_space = JCS_RGB;
				}
			}
			else
			{
				break;
			}

			/* Start decompression jpeg here */
			jpeg_start_decompress( &cinfo );

			/* init image info */
			if (info!=NULL)
			{
				info->width_in_pixel  = (cinfo.image_width);
				info->height_in_pixel = (cinfo.image_height);
				info->format=PF_RGBA8880;
				info->bit_count = 24;
				info->adapter_width=expand_to_2_power(info->width_in_pixel);
				info->adapter_height=expand_to_2_power(info->height_in_pixel);
			}
			
			row_pointer[0] = new unsigned char[cinfo.output_width*cinfo.output_components];
			if(! row_pointer[0])
				break;

			unsigned char *m_pData = new unsigned char[cinfo.output_width*cinfo.output_height*cinfo.output_components];
			if(! m_pData)
				break;
			assert(output==NULL);
			output=MGData::dataWithBytesNoCopy(m_pData,cinfo.output_width*cinfo.output_height*cinfo.output_components);

			/* now actually read the jpeg into the raw buffer */
			/* read one scan line at a time */
			while( cinfo.output_scanline < cinfo.image_height )
			{
				jpeg_read_scanlines( &cinfo, row_pointer, 1 );
				for( i=0; i<cinfo.image_width*cinfo.output_components;i++) 
				{
					m_pData[location++] = row_pointer[0][i];
				}
			}

			jpeg_finish_decompress( &cinfo );
			jpeg_destroy_decompress( &cinfo );
			/* wrap up decompression, destroy objects, free pointers and close open files */        
			bRet = true;
		} while (0);

		delete_array_and_set_null(row_pointer[0]);
		return bRet;
	}
}
