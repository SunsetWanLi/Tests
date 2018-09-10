/*
 * =====================================================================================
 *
 *       Filename:  MGPPMCodec.cpp
 *
 *    Description:  PPM codec
 *
 *        Version:  1.0
 *        Created:  2012年02月20日 13时42分50秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */

#include "MGPPMCodec.h"
#include <cstdio>
#include <cstring>
#include "../System/Shared/MGLog.h"
#include "../GraphicCore/MGPersistenceImage.h"
namespace z11
{
	bool MGPPMCodec::decode(const std::string & filepath,MGData *&data,MGImageInfo *info)
	{
		FILE *file=fopen(filepath.c_str(),"rb");
		if (file == NULL)
		{
			return false;
		}

		char buffer[256];
		int width=0;
		int height=0;
		fgets(buffer,256,file);//gets "P6"
		fgets(buffer,256,file);//get width & height
		sscanf(buffer,"%d %d",&width,&height);
		fgets(buffer,256,file);//get "255"
		
		
		size_t linewidth=width *3;
		int adapter_width=expand_to_2_power(width);
		int adapter_height=expand_to_2_power(height);
		if (data==NULL)
		{
			assert(0);
			//width=expand_to_2_power(width);
			//height=expand_to_2_power(height);
			//char *buffer_content=
			//data=
		}
		else
		{
			const int content_data_bytes=adapter_width*3;
			assert(data->length()>=(size_t)height*content_data_bytes);
			byte_8 *data_src=(byte_8 *)(data->bytes());
			//byte_8 *p=data_src+(height-1)*content_data_bytes;
			//for(int i=0; i<height; ++i) {
			//	fread(p,linewidth,1,file);
			//	p-=content_data_bytes;
			//}
			fread(data_src,linewidth*height,1,file);
		}
		if (info!=NULL)
		{
			info->width_in_pixel=width;
			info->height_in_pixel=height;
			info->bit_count=24;
			info->format=PF_RGBA8880;
			info->adapter_height=adapter_height;
			info->adapter_width=adapter_width;
			info->image_size=linewidth*height;
		}
		
		fclose(file);
		return true;
	}
	MGImage* z11::MGPPMCodec::decode(const std::string& filepath)
	{
		//MGData *data=MGData::dataWithContentsOfFile(filepath);

		FILE *file=fopen(filepath.c_str(),"rb");
		if (file == NULL)
		{
			return NULL;
		}
		
		char buffer[256];
		int width=0;
		int height=0;
		fgets(buffer,256,file);//gets "P6"
		fgets(buffer,256,file);//get width & height
		sscanf(buffer,"%d %d",&width,&height);
		fgets(buffer,256,file);//get "255"
		size_t linewidth=width *3;


		MGImage *ret=MGImage::fromBlank(24,width,height);
		const int content_data_bytes=ret->content_info.adapter_width*ret->content_info.bit_count/8;
		byte_8 *p=ret->getContentData()+(height-1)*content_data_bytes;
		for(int i=0; i<height; ++i) {
			fread(p,linewidth,1,file);
			p-=content_data_bytes;
		}
		fclose (file);
		return ret;
	}

	MGImage* z11::MGPPMCodec::decode(const unsigned char* input, size_t input_size)
	{
		int width;
		int height;
		//char buffer[256];
		const char *pt=NULL;
		pt=strstr((const char *)input,"\n")+1;//gets "P6","+1"for skip '\n'
		sscanf(pt,"%d %d",&width,&height);
		pt=strstr(pt,"\n")+1;//get width & height 
		pt=strstr(pt,"\n")+1;//get "255"

		size_t linewidth=width *3;


		MGImage *ret=MGPersistenceImage::fromBlank(24,width,height);
		const int content_data_bytes=ret->content_info.adapter_width*ret->content_info.bit_count/8;
		byte_8 *p=ret->getContentData()+(height-1)*content_data_bytes;
		for(int i=0; i<height; ++i) {
			memcpy(p,pt,linewidth);
			p-=content_data_bytes;
			pt+=linewidth;
		}
		return ret;
	}

	bool z11::MGPPMCodec::encode( const MGImage *image, std::vector<unsigned char>& output )
	{
		MGData *data= encode(image);
		if (data!=NULL)
		{
			const byte_8 *bytes=reinterpret_cast<const byte_8 *>(data->bytes());
			output.assign(bytes,bytes+data->length());
			delete_and_set_null(data);
			return true;
		}
		else
		{
			return false;
		}
	}

	bool MGPPMCodec::encode( const byte_8*input,const MGImageInfo &info,std::vector<unsigned char>& output )
	{
		
		//FILE *file=fopen(MGBundle::mainBundle()->pathWithFileName("image.ppm").c_str(),"wb");
		//fwrite(buffer,buffer_size,1,file);
		//fclose(file);
		//vector<unsigned char> tmp(buffer,buffer+buffer_size);
		//output.swap(tmp);

		//delete_array_and_set_null(buffer);
		return false;
	}

	MGData * MGPPMCodec::encode( const MGImage *image )
	{
		MGData *input=MGData::dataWithBytesNoCopy(image->getContentData(),image->getImageSize(),false);
		const MGImageInfo &info=image->content_info;
		MGData *ret=encode(input,info);
		delete_and_set_null(input);
		return ret;
	}

	MGData * MGPPMCodec::encode( MGData *input,const MGImageInfo &info )
	{
		if (input == NULL)
		{
			return NULL;
		}


		int width=info.width_in_pixel;
		int height=info.height_in_pixel;
		int line_width=info.width_in_pixel*info.bit_count/8;
		const unsigned char *data=(const unsigned char *)input->bytes();

		char header[256];
		sprintf(header, "P6\n%d %d\n255\n", width, height);
		int header_size=strlen(header);
		int output_line_size=width*3;
		unsigned int buffer_size=header_size+output_line_size * height;
		unsigned char *buffer=new unsigned char[buffer_size];
		memcpy(buffer,header,header_size);
		unsigned char *pt=buffer+header_size;
		const unsigned char *line_src=data+line_width*(height-1);
		for (int y = 0; y < height; y++) {
			const unsigned char *src=line_src ;
			if (info.format==PF_RGBA8880)
			{
				memcpy(pt,src ,3*width);
				pt+=width*3;
				src+=width*3;
			}
			else
			{
				for(int x = 0; x<width;x++)
				{
					switch (info.format)
					{
					case PF_RGBA8888:
						{
							for (int i=0;i<3;i++)
							{
								pt[i]=src[i]*src[3]/255;
							}

							//memcpy(pt,src ,3);
							pt+=3;
							src+=4;
						}
						break;
						//case PF_RGBA8880:
						//	{
						//		memcpy(pt,src ,3);
						//		pt+=3;
						//		src+=3;
						//	}
						//	break;
					case PF_BGRA8880:
						{
							memcpy(pt,src ,3);
							pt+=3;
							src+=3;
						}
						break;
					case PF_RGBA5650:
						{
							unsigned short line=*((unsigned short *)src);
							pt[0]=(unsigned char)(line>>11<<3);
							pt[1]=(unsigned char)(line>>5<<2);
							pt[2]=(unsigned char)(line<<3);
							pt+=3;
							src+=2;
						}
						break;
					default:
						{
							assert((info.format==PF_RGBA8888)||(info.format==PF_RGBA8880)||(info.format==PF_BGRA8880));
						}
					}

				}

			}
			line_src-=line_width;
		}
		return MGData::dataWithBytesNoCopy(buffer,buffer_size);
	}

}
