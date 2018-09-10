/////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2011 Zhejiang University.
///    All rights reserved.
///
/// @file    (MGImage.h)
/// @brief (Image�࣬�����ṩImage)
///
///(Image�࣬���ڶ�ȡ����Image�ļ�������֧��jpg,png,bmp)
///
/// @version 0.1   (�汾����)
/// @author        (Gu RongFang)
/// @date          (11-07-22)
///
///
///    �޶�˵��������汾
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
		PixelInt width_in_pixel;			//���  //
		PixelInt height_in_pixel;			//�߶�  //
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
		* @brief imageWithContentsOfFile ����ָ���ļ�������MGImage
		*
		* @param path ָ���ļ���·��
		*
		* @return ���ع����MGImage
		*/
		static MGImage* imageWithContentsOfFile(const std::string& path, bool reload = false);

		/**
		* @brief imageWithData ����MGData������MGImage��MGData��������Ҫ�����ļ�ͷ��Ϣ
		*
		* @param data ָ����MGData
		*
		* @return �¹����MGImage
		*/
		static MGImage* imageWithData(const MGData* data);			

		static MGImage * imageWithMGImage( const MGImage *copy_from_image );
		/**
		* @brief loadImageFromFile ��imageWithContentsOfFile��������ͬ,������ʹ������������������������Ҫ��Ϊ�˼�����ǰ�Ĵ���
		*
		* @param filename ָ���ļ���·��
		*
		* @return ���ع����MGImage
		*/
		static MGImage* loadImageFromFile(const std::string& filename, bool reload = false);

		/**
		* @brief fromBlank ����bit_count,width,height������һ���յ�MGImage,����Ĭ��ʹ��0���
		*
		* @param bit_count ��ɫ���
		* @param width		Image�Ŀ��
		* @param height	Image�ĸ߶�
		*
		* @return ���ع���Ŀ�MGImage
		*/
		static MGImage* fromBlank(int bit_count,int width,int height, PixelFormat pf = PF_DEFAULT);

		/**
		* @brief fromMem ����bit_count,width,height,bytes������һ��MGImage,������ʹ���������
		*
		* @param bit_count
		* @param width
		* @param height
		* @param bytes		RGBA����
		*
		* @return 
		*/
		static MGImage* fromMem(int bit_count,int width,int height,byte_8 *bytes, PixelFormat pf = PF_DEFAULT);

        void resetMem(byte_8 *bytes, int bit_count, int width, int height);
        
		/**
		* @brief recoveryContext ������е�MGImage��Context��Ϣ
		*/
		static void recoveryContext();
	public :
		/**
		* @brief sendDataToContext ���������ݷ��͵�opengl es ��texture
		*/
		virtual void sendDataToContext();

		/**
		* @brief convertPixelFormat ת��ɫ�ʿռ� 
		*
		* @param format Ҫת�������ظ�ʽ
		*/
		virtual void convertPixelFormat(PixelFormat format);
	public:
		/**
		* @brief getWidth ��ȡImage���
		*
		* @return 
		*/
		PixelInt getWidth() const;

		/**
		* @brief getHeight ��ȡImage�߶�
		*
		* @return 
		*/
		PixelInt getHeight() const;

		/**
		* @brief getContentData ��ȡ��������,���������sendDataToContext������ܷ���Ϊ��
		*
		* @return 
		*/
		byte_8 *getContentData() const;

		/**
		* @brief getAdapterWidth �����ܱ�opengl texture���ܵĿ��
		*
		* @return ����texture�Ŀ��
		*/
		PixelInt getAdapterWidth() const;

		/**
		* @brief getAdapterHeight �����ܱ�opengl texture���ܵĸ߶�
		*
		* @return ����texture�ĸ߶�
		*/
		PixelInt getAdapterHeight() const;

		int_32 getImageSize() const;
        
        bool save(const string& filename); //need to test, Only Can Save to BMP
		bool getBMPBytes(MGData *&m_data); //��ͼƬתΪbmp֮�󣬸�ֵ��������m_data
        
        string getFilename(){return filename;}
	protected:
		byte_8*	content_data;		//����ԭͼ  //
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
