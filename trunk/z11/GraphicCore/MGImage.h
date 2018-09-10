/////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2011 Zhejiang University.
///    All rights reserved.
///
/// @file    (MGImage.h)
/// @brief (Image类，用于提供Image)
///
///(Image类，用于读取各种Image文件，现在支持jpg,png,bmp)
///
/// @version 0.1   (版本声明)
/// @author        (Gu RongFang)
/// @date          (11-07-22)
///
///
///    修订说明：最初版本
//////////////////////////////////////////////////////////////////////////
#ifndef _MGIMAGE_H_
#define _MGIMAGE_H_
#pragma once
#include <map>
#include <string>
#include "MGGraphicEngine.h"
#include "../System/Thread/MGMutex.h"
#include "../CommonBase/MGData.h"
namespace z11
{
	typedef int PixelInt;
    
    typedef enum
    {
        IMAGE_INVALID,
        IMAGE_JPG,
        IMAGE_PNG,
        IMAGE_PPM,
        IMAGE_BMP
    }MGImageType;

	class MGImageInfo
	{
	public:
		MGImageInfo():width_in_pixel(0), height_in_pixel(0),
			bit_count(0), image_size(0), format(-1),adapter_width(0),adapter_height(0)
		{
		}
		PixelInt width_in_pixel;			//宽度  //
		PixelInt height_in_pixel;			//高度  //
		byte_8 bit_count;
		unsigned long image_size;
		PixelFormat format;
		PixelInt adapter_width;
		PixelInt adapter_height;
	};


	class MGTextureInfo
	{
	public:
		MGTextureInfo()
		{
			data =NULL;
			ref=0;
		}
		MGTextureInfo(byte_8 *_data,const MGImageInfo _info,MGGraphicEngineContext _context,int _ref)
			:info(_info),context(_context),data(_data)
		{
			ref=_ref;
		}
		MGImageInfo info;
		MGGraphicEngineContext context;
		byte_8 *data;
		int ref;
		std::string m_filepath;
	public:
		virtual bool reload();
	};

	class MGImage : public MGRefObject
	{
	public:
		MGImage();
		virtual ~MGImage();

	public:
		//Recommand
		/**
		* @brief imageWithContentsOfFile 根据指定文件来构造MGImage
		*
		* @param path 指定文件的路径
		*
		* @return 返回构造的MGImage
		*/
		static MGImage* imageWithContentsOfFile(const std::string& path, bool reload = false);

		/**
		* @brief imageWithData 根据MGData来构造MGImage，MGData的数据需要包含文件头信息
		*
		* @param data 指定的MGData
		*
		* @return 新构造的MGImage
		*/
		static MGImage* imageWithData(const MGData* data);			

		static MGImage * imageWithMGImage( const MGImage *copy_from_image );
		/**
		* @brief loadImageFromFile 与imageWithContentsOfFile的作用相同,不建议使用这个函数，保留这个函数主要是为了兼容以前的代码
		*
		* @param filename 指定文件的路径
		*
		* @return 返回构造的MGImage
		*/
		static MGImage* loadImageFromFile(const std::string& filename, bool reload = false);

		/**
		* @brief fromBlank 根据bit_count,width,height来构造一个空的MGImage,数据默认使用0填充
		*
		* @param bit_count 颜色深度
		* @param width		Image的宽度
		* @param height	Image的高度
		*
		* @return 返回构造的空MGImage
		*/
		static MGImage* fromBlank(int bit_count,int width,int height, PixelFormat pf = PF_DEFAULT);

		/**
		* @brief fromMem 根据bit_count,width,height,bytes来构造一个MGImage,不建议使用这个函数
		*
		* @param bit_count
		* @param width
		* @param height
		* @param bytes		RGBA数据
		*
		* @return 
		*/
		static MGImage* fromMem(int bit_count,int width,int height,byte_8 *bytes, PixelFormat pf = PF_DEFAULT);

        void resetMem(byte_8 *bytes, int bit_count, int width, int height);
        
		/**
		* @brief recoveryContext 清除所有的MGImage的Context信息
		*/
		static void recoveryContext();
	public :
		/**
		* @brief sendDataToContext 将像素数据发送到opengl es 的texture
		*/
		virtual void sendDataToContext();

		/**
		* @brief convertPixelFormat 转化色彩空间 
		*
		* @param format 要转换的像素格式
		*/
		virtual void convertPixelFormat(PixelFormat format);
	public:
		/**
		* @brief getWidth 获取Image宽度
		*
		* @return 
		*/
		PixelInt getWidth() const;

		/**
		* @brief getHeight 获取Image高度
		*
		* @return 
		*/
		PixelInt getHeight() const;

		/**
		* @brief getContentData 获取像素内容,如果调用了sendDataToContext，则可能返回为空
		*
		* @return 
		*/
		byte_8 *getContentData() const;

		/**
		* @brief getAdapterWidth 返回能被opengl texture接受的宽度
		*
		* @return 返回texture的宽度
		*/
		PixelInt getAdapterWidth() const;

		/**
		* @brief getAdapterHeight 返回能被opengl texture接受的高度
		*
		* @return 返回texture的高度
		*/
		PixelInt getAdapterHeight() const;

		int_32 getImageSize() const;
        
        bool save(const string& filename); //need to test, Only Can Save to BMP
		bool getBMPBytes(MGData *&m_data); //将图片转为bmp之后，赋值给参数的m_data
        
        string getFilename(){return filename;}
	protected:
		byte_8*	content_data;		//保存原图  //
		void fillContentInfo(int bit_count,int width,int height, PixelFormat pf);

	public:
		MGGraphicEngineContext context;
		MGImageInfo content_info;

		bool empty();
	private:
		friend class MGPersistenceImage;
		friend class MGTextureInfo;
        friend class MGGraphicEngine;
		void expand();
		static MGImage *loadImageImpl(const std::string& filepath, const MGImageType& type);
		static MGImage* imageWithDataImpl(const MGData* data);
	private:
		static std::map<std::string ,MGTextureInfo *> name2id;
		typedef std::map<std::string ,MGTextureInfo *>::iterator MapIterator;
		std::string filename;
    public:
        static MGMutex image_locker;
	};

#define BYTEOFLINE(lWidth,biBitCount) ((((lWidth) * (biBitCount)+ 31) & ~31) >> 3)
#define  SIZEOFIMAGE(lWidth,lHeight,biBitCount) (BYTEOFLINE(lWidth,biBitCount)*(lHeight))

} //namespace z11

#endif //_MGIMAGE_H_
