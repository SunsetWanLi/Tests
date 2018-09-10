#include "MGImage.h"
#include "BitmapStructDefination.h"
#include "../System/IO/MGBundle.h"
#include "../System/Shared/Utility.h"
#include "../CommonBase/CommonDef.h"
#include "../ExternalLib/2PassScale/2PassScale.h"
#include "../System/Shared/MGLog.h"
#include "stdio.h"
#include "stdlib.h"
#include "MGPNGCodec.h"
#include "MGJPEGCodec.h"
#include "MGBMPCodec.h"
#include "MGPPMCodec.h"
#include "../System/Thread/MGMutex.h"
#include "../System/Shared/MGTime.h"
#include "../System/Shared/MGLog.h"
#include "MGFTGLESFontContextManager.h"
#include "MGPersistenceImage.h"
namespace z11
{
    static MGImageType getImageType(uint_16 image_file_header_word)
    {
        MGImageType ret = IMAGE_INVALID;
		if (image_file_header_word	==	0x4D42)
		{
			//MGBMPCodec::decode(buffer,size,MGBMPCodec::FORMAT_RGB,&output,&w,&h);
			ret = IMAGE_BMP;
		}
		else if(image_file_header_word ==	0xD8FF)
		{
			ret = IMAGE_JPG;
			//MGJPEGCodec::decode(buffer,size,MGJPEGCodec::FORMAT_RGB,&output,&w,&h);
		}
		else if(image_file_header_word ==	0x5089)
		{
			ret = IMAGE_PNG;
			//MGPNGCodec::decode(buffer,size,MGPNGCodec::FORMAT_RGB,&output,&w,&h);
		}
		else if(image_file_header_word == 0x3650)
		{
			ret=IMAGE_PPM;
		}
		else
		{
			MGLogD("========================no match=========================");
		}
		return ret;
    }
	inline int size_of_image(const MGImageInfo &info)
	{
		return SIZEOFIMAGE(info.width_in_pixel,info.height_in_pixel,info.bit_count);
	}
	std::map<std::string ,MGTextureInfo*> MGImage::name2id;
	MGMutex MGImage::image_locker;
	MGImage::~MGImage()
	{
		//MGLogD("~MGImage");
		if (content_data != NULL)
		{
			delete_array_and_set_null(content_data);
		}
        image_locker.lock();
        MapIterator it=name2id.find(filename);
        if (it!=name2id.end())
        {
            if (--it->second->ref==0)
            {
                // 当RecoveryContext后，很可能没有执行过sendToContext的对象要销毁，但此时对象保存的Texture ID还是RecoveryContext之前的ID，就会误把其他Texture销毁，导致绘制异常 //
                context.texture_id = it->second->context.texture_id;
                delete_array_and_set_null(it->second->data);
                delete_and_set_null(it->second);
                name2id.erase(it);
                if (context.texture_id != 0) {
                    MGGraphicEngine::sharedGraphicEngine()->pushContext(context);
                }
                //MGLogD("~MGImage glDeleteTextures %d",context.texture_id);
            }
        }
        else
        {
            MGLogD("MGImage:~MGImage filename doesn't match context");
        }
        context.texture_id = 0;
        image_locker.unlock();
	}

	MGImage::MGImage()
	{
		content_data = NULL;
	}
	MGImage * MGImage::loadImageImpl( const std::string& filepath , const MGImageType& type)
	{
        
       // MGTimeTicks tick = MGTimeTicks::Now();
		MGImage *ret=NULL;
		if (type == IMAGE_JPG)
		{
			ret= MGJPEGCodec::decode(filepath);
		}else if (type == IMAGE_PNG)
		{
			ret= MGPNGCodec::decode(filepath);
		}else if (type == IMAGE_BMP)
		{
			ret=MGBMPCodec::decode(filepath);
		}else if (type == IMAGE_PPM)
		{
			ret=MGPPMCodec::decode(filepath);
		}
        //MGTimeTicks a_ticks = MGTimeTicks::Now();
        //int_64 delta = (a_ticks - tick).InMilliseconds();
        //MGLogD("+++++++++++++++++++++++++++++++++++++++++++");
        //MGLogD("load image %s in %lld ms.", filepath.c_str(), delta);
        //MGLogD("+++++++++++++++++++++++++++++++++++++++++++");
        
        if(ret==NULL)
        {
            //only for these image that can be read by fopen
            FILE *file = fopen(filepath.c_str(), "rb");
            if (file!=NULL) {
                uint_16 buffer=0;
                fread(&buffer, 2, 1, file);
                MGImageType type = getImageType(buffer);

				fclose(file);    
                
                if (type == IMAGE_JPG)
                {
                    ret= MGJPEGCodec::decode(filepath);
                }else if (type == IMAGE_PNG)
                {
                    ret= MGPNGCodec::decode(filepath);
                }else if (type == IMAGE_BMP)
                {
                    ret=MGBMPCodec::decode(filepath);
                }else if (type == IMAGE_PPM)
                {
                    ret=MGPPMCodec::decode(filepath);
                }
            }

        }
		return ret;
	}


	MGImage* MGImage::loadImageFromFile(const std::string& filepath, bool reload)
	{
		MGImage *ret = NULL;
		string filename = Utility::getFileFromPath(filepath);
        MGImageType type = IMAGE_INVALID;
        
        if (Utility::hasExtName(filename, "jpg"))
        {
            type = IMAGE_JPG;
        }else if(Utility::hasExtName(filename, "png"))
        {
            type = IMAGE_PNG;
        }else if(Utility::hasExtName(filename, "ppm"))
        {
            type = IMAGE_PPM;
        }else if(Utility::hasExtName(filename, "bmp"))
        {
            type = IMAGE_BMP;
        }else {
            return ret;
        }


		image_locker.lock();
		MapIterator it=name2id.find(filepath);
		if (it != name2id.end() && it->second->ref > 0)
		{
            if (reload) {
                if (it->second->data == NULL) {
                    ret = loadImageImpl(filepath, type);
                    it->second->data = ret->content_data;
                    ret->content_data = NULL;
                }else{
                    ret=new MGImage();
                }
            }else
                ret = new MGImage();
            
            ret->content_info=it->second->info;
            ret->context=it->second->context;
            ret->filename = filepath;
            it->second->ref++;
		}
		else
		{
			ret=loadImageImpl(filepath, type);
            if (ret!=NULL) {
                ret->filename = filepath;
                if (ret->content_data != NULL) //first time to add
                {
                    //ret->expand();
                    name2id[filepath]=new MGTextureInfo(ret->content_data,ret->content_info,ret->context,1);//引用???
                    name2id[filepath]->m_filepath=filepath;
                    ret->content_data=NULL;//this content_data has be shared
                }
            }
		}
		
		if (ret == NULL)
		{
			string log = "Load Image: ";
			log = log.append(filepath).append(" failed!");
			MGLogD(log.c_str());
			delete_and_set_null(ret);
		}
		image_locker.unlock();
		return ret;
	}

	void MGImage::sendDataToContext()
	{
		image_locker.lock();
		MapIterator it=name2id.find(filename);
		if (it!=name2id.end())
		{
			MGTextureInfo *id=it->second;
			if (id->data!=NULL)
			{
				id->context.initWithData(id->data,id->info.format,id->info.adapter_width,id->info.adapter_height);
			}
			context.texture_id=id->context.texture_id;
		}
		else
		{
			MGLogD("MGImage filename doesn't match context");
		}
		image_locker.unlock();
	}

	MGImage* MGImage::fromMem( int bit_count,int width,int height,byte_8 *bytes, PixelFormat pf)
	{
		MGImage *ret=MGPersistenceImage::fromBlank(bit_count,width,height,pf);
		if (ret == NULL)
		{
			MGLogD("Cache BMP file in memory failed!");
		}
		else
		{
            long adpter_byteperrow = ret->content_info.adapter_width * bit_count / 8;
            long byteperrow = ret->content_info.width_in_pixel * bit_count / 8;
            byte_8* pDest = ret->getContentData() + (ret->content_info.height_in_pixel - 1) * adpter_byteperrow;
            byte_8* pSrc = bytes;
            for (int i=0;i<ret->content_info.height_in_pixel;i++)
            {
                memcpy(pDest,pSrc,byteperrow);
                pDest-=adpter_byteperrow;
                pSrc+=byteperrow;
            }
		}
		return ret;
	}
    
    void MGImage::resetMem(byte_8 *bytes, int bit_count, int width, int height)
    {
        if (getContentData() == NULL)
            return;
        if (content_info.width_in_pixel != width || content_info.height_in_pixel != height || content_info.bit_count != bit_count)
            return;
        long adpter_byteperrow = content_info.adapter_width * content_info.bit_count / 8;
        long byteperrow = content_info.width_in_pixel * content_info.bit_count / 8;
        byte_8* pDest = getContentData() + (content_info.height_in_pixel - 1) * adpter_byteperrow;
        byte_8* pSrc = bytes;
        for (int i=0;i<content_info.height_in_pixel;i++)
        {
            memcpy(pDest,pSrc,byteperrow);
            pDest-=adpter_byteperrow;
            pSrc+=byteperrow;
        }
    }

	MGImage* MGImage::fromBlank( int bit_count,int width,int height, PixelFormat pf)
	{
		MGImage *ret=new MGImage;

		ret->fillContentInfo(bit_count,width,height, pf);
		ret->content_info.image_size=SIZEOFIMAGE(ret->content_info.adapter_width,ret->content_info.adapter_height,ret->content_info.bit_count);
		ret->content_data = new byte_8[ret->content_info.image_size];
		if (ret->content_data == NULL)
		{
			MGLogD("Cache BMP file in memory failed!");
			delete ret;
			ret=NULL;
		}
		else
		{
			memset(ret->content_data, 0, ret->content_info.image_size);
			char str[256];
			sprintf(str,"MGImage %p",ret->content_data);
			ret->filename=str;
		}
		return ret;
	}

	byte_8 * MGImage::getContentData() const
	{
		byte_8* ret=content_data;
		if (content_data==NULL)
		{
			image_locker.lock();
			MapIterator it=name2id.find(filename);
			if (it!=name2id.end())
			{
				MGTextureInfo *id=it->second;
				if (id->data!=NULL)
				{	
					ret= id->data;
					
				}
			}
			image_locker.unlock();
		}
		return ret;
	}


	bool MGImage::save(const string& filename)
	{
// 		z11::uint_32 iBits; 					//当前显示分辨率下每个像素所占bit???
// 		z11::uint_32 wBitCount; 			//位图中每个像素所占bit???
// 		z11::uint_32 dwPaletteSize	= 0, 	//定义调色板大小， 位图中像素字节大???，位图文件大???
// 			dwBitmapSize, dwDIBSize;
// 		struct UnionBitmapStruct	ub;
// 		z11::byte_8				 	*pbp;	//指向位图像素数据
// 		RGBQUAD 			rgbquad[256];
// 		RGBQUAD				*pquad;	//指向调色???
// 		int					i;
// 		int					widthbyte;
// 		char				*pDBI;
// 		MGImage *SourceImage=this;
// 		wBitCount=SourceImage->content_info.bit_count;
// 
// 		iBits=wBitCount;
// 
// 
// 		widthbyte = BYTEOFLINE(SourceImage->getWidth(),iBits);//wBitCount);
// 		dwBitmapSize = widthbyte * SourceImage->getHeight();
// 
// 		//设置位图信息头结???
// 		ub.bi.biSize 			= sizeof(BITMAPINFOHEADER);
// 		ub.bi.biWidth 			= (long)SourceImage->getWidth();
// 		ub.bi.biHeight 		= (long)SourceImage->getHeight();
// 		ub.bi.biPlanes 		= 1;
// 		ub.bi.biBitCount 		= iBits;//wBitCount;
// 		if (iBits==16)
// 		{
// 			ub.bi.biCompression 	=	BI_BITFIELDS;
// 		}
// 		else
// 			ub.bi.biCompression		=	BI_RGB;
// 		ub.bi.biSizeImage 		= dwBitmapSize;
// 		ub.bi.biXPelsPerMeter 	= 0;
// 		ub.bi.biYPelsPerMeter 	= 0;
// 		ub.bi.biClrUsed 		= 0;
// 		ub.bi.biClrImportant 	= 0;
// 
// 
// 		//计算调色板大???
// 		if (wBitCount <= 8)
// 		{
// 			//构造灰度图的调色版
// 			for (int i=0; i<(1 << wBitCount); i++)
// 			{
// 				rgbquad[i].rgbBlue		= i;
// 				rgbquad[i].rgbGreen		= i;
// 				rgbquad[i].rgbRed		= i;
// 				rgbquad[i].rgbReserved	= 0;
// 			}
// 			dwPaletteSize = (1 << wBitCount) * sizeof(RGBQUAD);
// 		}
// 		if (ub.bi.biCompression 	== BI_BITFIELDS)
// 		{
// 			dwPaletteSize=12;
// 		}
// 
// 		// 设置位图文件???
// 		ub.bf.bfType = 0x4D42; // "BM"
// 		ub.bf.bfOffBits = (z11::uint_32)sizeof(BITMAPFILEHEADER)
// 			+ (z11::uint_32)sizeof(BITMAPINFOHEADER)
// 			+ dwPaletteSize ;
// 		dwDIBSize =ub.bf.bfOffBits + dwBitmapSize;
// 		ub.bf.bfSize = dwDIBSize;
// 		ub.bf.bfReserved1 = 0;
// 		ub.bf.bfReserved2 = 0;
// 
// 
// 		//为位图内容分配内???
// 		//pDBI = (char *)malloc((dwDIBSize+3)/4*4);
// 		pDBI = new char[(dwDIBSize+3)/4*4];
// 		if (NULL == pDBI)
// 		{
// 			OutputDebugMessage("malloc failed!");
// 			return false;
// 		}
// 		memset(pDBI, 0, dwDIBSize);
// 
// 		memcpy(pDBI,&ub,sizeof(ub));
// 
// 		if (ub.bi.biCompression 	== BI_BITFIELDS)
// 		{
// 			//填充mask数据
// 			pquad = (RGBQUAD *)(pDBI + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER));
// 			COLORREF temp;
// 			temp	=	0xF800;
// 			memcpy(pquad,&temp,sizeof(COLORREF));
// 			temp	=	0x07E0;
// 			memcpy(pquad+1,&temp,sizeof(COLORREF));
// 			temp	=	0x001F;
// 			memcpy(pquad+2,&temp,sizeof(COLORREF));
// 		}
// 		//填充调色板内???
// 		if (wBitCount <= 8)
// 		{
// 			pquad = (RGBQUAD *)(pDBI + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER));
// 			for (i=0; i<(1 << wBitCount); i++)
// 			{
// 				pquad[i].rgbBlue = rgbquad[i].rgbBlue;
// 				pquad[i].rgbGreen = rgbquad[i].rgbGreen;
// 				pquad[i].rgbRed = rgbquad[i].rgbRed;
// 				pquad[i].rgbReserved = rgbquad[i].rgbReserved;
// 			}
// 		}
// 
// 		//填充位图像素数据
// 		pbp = (z11::byte_8 *)(pDBI + ub.bf.bfOffBits);
// 
// 		{
// 			for (i=0; (long)i<ub.bi.biHeight; i++)
// 			{
// 				memcpy(pbp + i*widthbyte, SourceImage->getContentData() + i*(long)SourceImage->getAdapterWidth()*wBitCount/8, ub.bi.biWidth*wBitCount/8);
// 			}
// 		}

		// 写入位图文件
		//delete[] pDBI;
		MGData *write_data;
		getBMPBytes(write_data);
		bool ret = write_data->writeToFile(filename);
        delete_and_set_null(write_data);
        return ret;
	}

	bool MGImage::getBMPBytes(MGData *&m_data)
	{
		z11::uint_32 iBits; 					//当前显示分辨率下每个像素所占bit???
		z11::uint_32 wBitCount; 			//位图中每个像素所占bit???
		z11::uint_32 dwPaletteSize	= 0, 	//定义调色板大小， 位图中像素字节大???，位图文件大???
			dwBitmapSize, dwDIBSize;
		struct UnionBitmapStruct	ub;
		z11::byte_8				 	*pbp;	//指向位图像素数据
		RGBQUAD 			rgbquad[256];
		RGBQUAD				*pquad;	//指向调色???
		int					i;
		int					widthbyte;
		char				*pDBI;
		MGImage *SourceImage=this;
		wBitCount=SourceImage->content_info.bit_count;

		iBits=wBitCount;


		widthbyte = BYTEOFLINE(SourceImage->getWidth(),iBits);//wBitCount);
		dwBitmapSize = widthbyte * SourceImage->getHeight();

		//设置位图信息头结???
		ub.bi.biSize 			= sizeof(BITMAPINFOHEADER);
		ub.bi.biWidth 			= (long)SourceImage->getWidth();
		ub.bi.biHeight 		= (long)SourceImage->getHeight();
		ub.bi.biPlanes 		= 1;
		ub.bi.biBitCount 		= iBits;//wBitCount;
		if (iBits==16)
		{
			ub.bi.biCompression 	=	BI_BITFIELDS;
		}
		else
			ub.bi.biCompression		=	BI_RGB;
		ub.bi.biSizeImage 		= dwBitmapSize;
		ub.bi.biXPelsPerMeter 	= 0;
		ub.bi.biYPelsPerMeter 	= 0;
		ub.bi.biClrUsed 		= 0;
		ub.bi.biClrImportant 	= 0;


		//计算调色板大???
		if (wBitCount <= 8)
		{
			//构造灰度图的调色版
			for (int i=0; i<(1 << wBitCount); i++)
			{
				rgbquad[i].rgbBlue		= i;
				rgbquad[i].rgbGreen		= i;
				rgbquad[i].rgbRed		= i;
				rgbquad[i].rgbReserved	= 0;
			}
			dwPaletteSize = (1 << wBitCount) * sizeof(RGBQUAD);
		}
		if (ub.bi.biCompression 	== BI_BITFIELDS)
		{
			dwPaletteSize=12;
		}

		// 设置位图文件???
		ub.bf.bfType = 0x4D42; // "BM"
		ub.bf.bfOffBits = (z11::uint_32)sizeof(BITMAPFILEHEADER)
			+ (z11::uint_32)sizeof(BITMAPINFOHEADER)
			+ dwPaletteSize ;
		dwDIBSize =ub.bf.bfOffBits + dwBitmapSize;
		ub.bf.bfSize = dwDIBSize;
		ub.bf.bfReserved1 = 0;
		ub.bf.bfReserved2 = 0;


		//为位图内容分配内???
		//pDBI = (char *)malloc((dwDIBSize+3)/4*4);
		pDBI = new char[(dwDIBSize+3)/4*4];
		if (NULL == pDBI)
		{
			OutputDebugMessage("malloc failed!");
			return false;
		}
		memset(pDBI, 0, dwDIBSize);

		memcpy(pDBI,&ub,sizeof(ub));

		if (ub.bi.biCompression 	== BI_BITFIELDS)
		{
			//填充mask数据
			pquad = (RGBQUAD *)(pDBI + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER));
			COLORREF temp;
			temp	=	0xF800;
			memcpy(pquad,&temp,sizeof(COLORREF));
			temp	=	0x07E0;
			memcpy(pquad+1,&temp,sizeof(COLORREF));
			temp	=	0x001F;
			memcpy(pquad+2,&temp,sizeof(COLORREF));
		}
		//填充调色板内???
		if (wBitCount <= 8)
		{
			pquad = (RGBQUAD *)(pDBI + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER));
			for (i=0; i<(1 << wBitCount); i++)
			{
				pquad[i].rgbBlue = rgbquad[i].rgbBlue;
				pquad[i].rgbGreen = rgbquad[i].rgbGreen;
				pquad[i].rgbRed = rgbquad[i].rgbRed;
				pquad[i].rgbReserved = rgbquad[i].rgbReserved;
			}
		}

		//填充位图像素数据
		pbp = (z11::byte_8 *)(pDBI + ub.bf.bfOffBits);

		//一个rgb元素占wBitCount/8个字节，这里只处理长度为3字节的rgb数据,因为目前只发现这种情况会出现rgb与gbr的顺序问题
		if (3==(wBitCount/8))
		{
			unsigned char *convert_buffer = new unsigned char[ub.bi.biWidth*wBitCount/8];
			if (NULL == convert_buffer)
			{
				OutputDebugMessage("malloc failed!");
				return false;
			}

			for (i=0; (long)i<ub.bi.biHeight; i++)
			{
				memcpy(convert_buffer,SourceImage->getContentData() + i*(long)SourceImage->getAdapterWidth()*wBitCount/8,ub.bi.biWidth*wBitCount/8);

				for (int j=0;j<ub.bi.biWidth;j++)
				{
					size_t mem_start = 3*j;
					unsigned char tmp = convert_buffer[mem_start];
					convert_buffer[mem_start] = convert_buffer[mem_start+2];
					convert_buffer[mem_start+2] = tmp;
				}

				memcpy(pbp + i*widthbyte, convert_buffer, ub.bi.biWidth*wBitCount/8);
			}

			delete convert_buffer;
		}
		else
		{
			for (i=0; (long)i<ub.bi.biHeight; i++)
			{
				memcpy(pbp + i*widthbyte, SourceImage->getContentData() + i*(long)SourceImage->getAdapterWidth()*wBitCount/8, ub.bi.biWidth*wBitCount/8);
			}
		}


		
		m_data=MGData::dataWithBytesNoCopy(pDBI,dwDIBSize);

		bool ret=false;
		if (m_data->length()>0)
			ret=true;

		return ret;
	}

	z11::PixelInt MGImage::getWidth() const
	{
		return content_info.width_in_pixel;
	}

	z11::PixelInt MGImage::getHeight() const
	{
		return content_info.height_in_pixel;
	}

	void MGImage::fillContentInfo( int bit_count,int width,int height, PixelFormat pf)
	{
		content_info.bit_count = bit_count;
		content_info.width_in_pixel	= width;
		content_info.height_in_pixel = height;
		content_info.image_size=sizeof(z11::byte_8) * (content_info.width_in_pixel* content_info.height_in_pixel* content_info.bit_count/8);
		//content_info.image_size = sizeof(z11::byte_8) * SIZEOFIMAGE(content_info.width_in_pixel, content_info.height_in_pixel, content_info.bit_count);
		content_info.adapter_width=expand_to_2_power(width);
		content_info.adapter_height=expand_to_2_power(height);

        switch (pf) {
            case PF_DEFAULT:
            {
                if (bit_count==16)
                {
                    content_info.format = PF_RGBA5650;
                }
                else if (bit_count==24)
                {
                    content_info.format = PF_RGBA8880;
                }
                else if (bit_count==32)
                {
                    content_info.format = PF_RGBA8888;
                }
                else if(bit_count==8)
                {
                    content_info.format = PF_RGBA0008;
                }
                else
                {
                    MGLogD("MGImage: unknown bits size (bits=%d).",bit_count);
                    assert(0);
                }
            }
                break;
            default:
                content_info.format = pf;
                break;
        }
		
	}

	z11::PixelInt MGImage::getAdapterWidth() const
	{
		return content_info.adapter_width;//expand_to_2_power(content_info.width_in_pixel);
	}

	z11::PixelInt MGImage::getAdapterHeight() const
	{
		return content_info.adapter_height;//expand_to_2_power(content_info.height_in_pixel);
	}

	void MGImage::expand()
	{
		content_info.adapter_width	=	expand_to_2_power(content_info.width_in_pixel);
		content_info.adapter_height	=	expand_to_2_power(content_info.height_in_pixel);
		if (content_info.adapter_width != content_info.width_in_pixel
			||content_info.adapter_height != content_info.height_in_pixel)
		{
			byte_8 *content_data_=new byte_8[SIZEOFIMAGE(content_info.adapter_width,content_info.adapter_height,content_info.bit_count)];
			byte_8 *source=content_data;
			byte_8 *dest=content_data_;
			int soucebytes=content_info.width_in_pixel*content_info.bit_count/8;
			int destbytes=BYTEOFLINE(content_info.adapter_width,content_info.bit_count);
			for (int i=0 ; i<content_info.height_in_pixel ; i++)
			{
				memcpy(dest,source,soucebytes);//拷贝已有的像素点
				memset(dest+soucebytes,0,destbytes-soucebytes);//填充剩余像素???
				source+=soucebytes;
				dest+=destbytes;
			}
			int delta_height=(content_info.adapter_height-content_info.height_in_pixel);
			memset(dest,0,destbytes*delta_height);
			dest+=destbytes*delta_height;
			delete_array_and_set_null(content_data);
			content_data=content_data_;
		}
	}

	void MGImage::recoveryContext()
	{
		image_locker.lock();
        MapIterator it=name2id.begin();
        while (it != name2id.end())
		{
			it->second->m_filepath=it->first;
            it->second->context.texture_id = 0;
			it->second->reload();
            ++it;
		}
		image_locker.unlock();
		MGFTGLESFontContextManager::getInstance()->clear();
	}
	static void convert_(byte_8 *pBits,int num)
	{
		for (int i=0;i<num;i+=3,pBits+=3)
		{
			byte_8 temp=pBits[0];
			pBits[0]=pBits[2];
			pBits[2]= temp;
		}
	}
	void MGImage::convertPixelFormat( PixelFormat format )
	{
		if (format == content_info.format)
		{
			return ;
		}
		switch(content_info.format)
		{
		case PF_BGRA8880:
			switch(format)
			{
			case PF_RGBA8880:
				convert_(content_data,content_info.image_size);
				content_info.format=PF_RGBA8880;
				break;
			}
			break;

		}
	}

	MGImage* MGImage::imageWithData( const MGData* data )
	{
		MGImage *ret=NULL;
		do 
		{
			if (data == NULL || data->length()==0 || data->bytes()==NULL)
			{
				break;
			}
			image_locker.lock();
			stringstream ss;
			ss<<"Memory "<<data;
			string memory_id=ss.str();
			MapIterator it=name2id.find(memory_id);
			if (it!=name2id.end()&&it->second->ref>0)
			{
				ret=new MGPersistenceImage();
				ret->content_info=it->second->info;
				ret->context=it->second->context;
				it->second->ref++;
			}
			else
			{
				ret=imageWithDataImpl(data);
			}
			if (ret!=NULL) {
				//already load in MGPersistenceImage::fromBlank
				//ret->filename=memory_id;
				//if (ret->content_data != NULL) //第一次加入的时???
				//{
				//	//ret->expand();
				//	name2id[ret->filename]=new MGMGPersistenceTextureInfo(ret->content_data,ret->content_info,ret->context,1);//引用???
				//	name2id[ret->filename]->m_filepath=ret->filename;
				//	ret->content_data=NULL;//内存内容已经不属于加载它的文???
				//}
			}
			else
			{
				MGLogD("Load Memory Image %p failed",data->bytes());
				delete_and_set_null(ret);
			}
			image_locker.unlock();
		} while (0);
		return ret;
		
	}

	MGImage* MGImage::imageWithDataImpl(const MGData* data )
	{
		MGImage* ret = NULL;
		z11::uint_16 image_file_header_word=* ((z11::uint_16 *)data->bytes());
		if (image_file_header_word	==	0x4D42)
		{
			//MGBMPCodec::decode(buffer,size,MGBMPCodec::FORMAT_RGB,&output,&w,&h);
			ret = MGBMPCodec::decode((byte_8 *)data->bytes(),data->length(),MGBMPCodec::FORMAT_RGB);
		}
		else if(image_file_header_word ==	0xD8FF)
		{
			ret = MGJPEGCodec::decode((byte_8 *)data->bytes(),data->length(),MGJPEGCodec::FORMAT_DEFAULT);
			//MGJPEGCodec::decode(buffer,size,MGJPEGCodec::FORMAT_RGB,&output,&w,&h);
		}
		else if(image_file_header_word ==	0x5089)
		{
			ret = MGPNGCodec::decode((byte_8 *)data->bytes(),data->length(), MGPNGCodec::FORMAT_DEFAULT);
			//MGPNGCodec::decode(buffer,size,MGPNGCodec::FORMAT_RGB,&output,&w,&h);
		}
		else if(image_file_header_word == 0x3650)
		{
			ret=MGPPMCodec::decode((byte_8 *)data->bytes(),data->length());
		}
		else
		{
			MGLogD("========================no match=========================");
			ret = NULL;
		}
		return ret;
		//delete_array_and_set_null(buffer);
		
	}

	MGImage* MGImage::imageWithContentsOfFile( const std::string& path , bool reload)
	{
		return loadImageFromFile(path, reload);
	}

	int_32 MGImage::getImageSize() const
	{
		return  content_info.image_size;
	}

	MGImage * MGImage::imageWithMGImage( const MGImage *copy_from_image )
	{
		MGImage *ret=NULL;
		const MGPersistenceImage *persistence_image=dynamic_cast<const MGPersistenceImage *>(copy_from_image);
		if (copy_from_image!=NULL  )
		{
			if (persistence_image==NULL) //normal 
			{
				image_locker.lock();
				ret=new MGImage();
				ret->filename=copy_from_image->filename;
				MapIterator it=name2id.find(ret->filename);
				assert(it!=name2id.end()&&it->second->ref>0);
				ret->content_info=it->second->info;
				ret->context=it->second->context;
				it->second->ref++;
				image_locker.unlock();
			}
			else  
			{
				image_locker.lock();
				ret=new MGPersistenceImage();
				ret->filename=copy_from_image->filename;
				MapIterator it=name2id.find(ret->filename);
				assert(it!=name2id.end()&&it->second->ref>0);
				ret->content_info=it->second->info;
				ret->context=it->second->context;
				it->second->ref++;
				image_locker.unlock();

			}
		}
		return ret;
	}
	bool MGTextureInfo::reload()
	{
        bool return_value = false;
        string filename = Utility::getFileFromPath(m_filepath);
        MGImageType type = IMAGE_INVALID;
        
        if (Utility::hasExtName(filename, "jpg"))
        {
            type = IMAGE_JPG;
        }else if(Utility::hasExtName(filename, "png"))
        {
            type = IMAGE_PNG;
        }else if(Utility::hasExtName(filename, "ppm"))
        {
            type = IMAGE_PPM;
        }else if(Utility::hasExtName(filename, "bmp"))
        {
            type = IMAGE_BMP;
        }
        
		MGImage *ret=MGImage::loadImageImpl(m_filepath, type);
        
		if (ret!=NULL&&ret->content_data != NULL) //如果加载成功 //
		{
			//ret->expand();
//			MGImage::MapIterator it=MGImage::name2id.find(m_filepath);
//			MGTextureInfo *info=it->second;
            if (this->data != NULL) {
                delete[] this->data;
            }
			this->data=ret->content_data;
			this->info=ret->content_info;
			this->context=ret->context;
			//info->ref++;
			//MGImage::name2id[m_filepath]=MGTextureInfo(ret->content_data,ret->content_info,ret->context,it->second->ref+1);//引用???  //
			ret->content_data=NULL;//内存内容已经不属于加载它的文??? //
            return_value = true;
        }
        delete_and_set_null(ret);
        return return_value;
	}

} //namespace z11
