//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (MGPersistenceImage.h)  
/// @brief (MGPersistenceImage，conent_data不会提交给opengl管理)  
///  
///(MGPersistenceImage，conent_data不会提交给opengl管理)  
///  
/// @version 0.1   (版本声明)  
/// @author        (Gu RongFang)  
/// @date          (11-07-22)  
///  
///  
///    修订说明：最初版本  
////////////////////////////////////////////////////////////////////////// 
#ifndef _MGDURABLEIMAGE_H_
#define _MGDURABLEIMAGE_H_
#pragma once
#include "../CommonBase/CommonDef.h"
#include "MGImage.h"
namespace z11
{
	class MGMGPersistenceTextureInfo:public MGTextureInfo
	{
	public:
		MGMGPersistenceTextureInfo(byte_8 *_data,const MGImageInfo _info,MGGraphicEngineContext _context,int _ref)
			:MGTextureInfo(_data,_info,_context,_ref)
		{
		}
		virtual bool reload();
	};
	class MGPersistenceImage:public MGImage
	{
	public:
		MGPersistenceImage(void);
		virtual ~MGPersistenceImage(void);
	public:
		static MGImage* fromBlank(byte_8 bit_count,int width,int height, PixelFormat pf = PF_DEFAULT);
	public :
		//将像素数据发送到opengl es 的texture 
		virtual void sendDataToContext();
		void update();
		void dirty();
	private:

		void updateWithDataWithoutDelete(uint_32& texture_id, byte_8*& data, PixelFormat pixel_format, uint_16 pixel_width, uint_16 pixel_height );
		bool m_dirty;
	};

}


#endif //_MGDURABLEIMAGE_H_
