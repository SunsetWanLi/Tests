#include "MGPersistenceImage.h"
#include "../System/Shared/MGLog.h"
#include <string.h>
#include <stdio.h>
extern void TestMem(wchar_t *str);
namespace z11
{
	MGPersistenceImage::MGPersistenceImage(void)
		:m_dirty(true)
	{
	}

	MGPersistenceImage::~MGPersistenceImage(void)
	{

		image_locker.lock();
		MapIterator it=name2id.find(filename);
		if (it!=name2id.end())
		{
            if(it->second->ref<=1)
                delete_array_and_set_null(it->second->data);
		}
		else
			delete_array_and_set_null(content_data);

		image_locker.unlock();
		
	}

	MGImage* MGPersistenceImage::fromBlank( byte_8 bit_count,int width,int height, PixelFormat pf)
	{
		MGImage *ret=new MGPersistenceImage;
		ret->fillContentInfo(bit_count,width,height, pf);
		if (ret->getAdapterWidth()>GL_MAX_TEXTURE_SIZE||ret->getAdapterHeight()>GL_MAX_TEXTURE_SIZE)
		{
			MGLogD("Alert!!! Texture size greater than GL_MAX_TEXTURE_SIZE");
		}
		//修正image_size大小
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
			sprintf(str,"MGPersistenceImage %p",ret->content_data);
			ret->filename=str;

		}

		image_locker.lock();
		if (ret != NULL && ret->content_data!=NULL) //第一次加入  //
		{
			name2id[ret->filename]=new MGMGPersistenceTextureInfo(ret->content_data,ret->content_info,ret->context,1);
			name2id[ret->filename]->m_filepath=ret->filename;
			name2id[ret->filename]->data=ret->content_data;
			ret->content_data=NULL;
		}
		image_locker.unlock();
		return ret;
	}

	void MGPersistenceImage::sendDataToContext()
	{
        image_locker.lock();
        MapIterator it=name2id.find(filename);
        if (it!=name2id.end())
        {
            MGTextureInfo *id=it->second;
            if (m_dirty)
            {
                update();
                m_dirty = false;
            }
            context.texture_id=id->context.texture_id;
        }
        else
        {
            if (content_data == NULL) {
                return;
            }
            context.initWithDataWithoutDelete(content_data, content_info.format, content_info.adapter_width, content_info.adapter_height);
        }
        image_locker.unlock();
	}

	void MGPersistenceImage::dirty()
	{
		m_dirty =true;
	}

	void MGPersistenceImage::update()
	{
		image_locker.lock();
		MapIterator it=name2id.find(filename);
		if (it!=name2id.end())
		{
			MGTextureInfo *id=it->second;
			if (id->data!=NULL)
			{
				id->context.initWithDataWithoutDelete(id->data,id->info.format,id->info.adapter_width,id->info.adapter_height);
			}
			context.texture_id=id->context.texture_id;
		}
		else
		{
			if (content_data == NULL) {
				return;
			}
			context.initWithDataWithoutDelete(content_data, content_info.format, content_info.adapter_width, content_info.adapter_height);
		}
		image_locker.unlock();
	}

	void MGPersistenceImage::updateWithDataWithoutDelete(uint_32 &texture_id, byte_8*& data, PixelFormat pixel_format, uint_16 pixel_width, uint_16 pixel_height )
	{
		assert(texture_id!=0);
		glBindTexture(GL_TEXTURE_2D, texture_id);


		// Specify OpenGL texture image

		switch(pixel_format)
		{
		case PF_RGBA8888:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pixel_width, pixel_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			break;
		case PF_RGBA8880:
			glTexSubImage2D(GL_TEXTURE_2D, 0, GL_RGB ,0, pixel_width, pixel_height, GL_RGB, GL_UNSIGNED_BYTE, data);
			break;
		case PF_RGBA4444:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pixel_width, pixel_height, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, data);
			break;
		case PF_RGBA5551:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pixel_width, pixel_height, 0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, data);
			break;
		case PF_RGBA5650:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pixel_width, pixel_height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, data);
			break;
		case PF_RGBA0008:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, pixel_width, pixel_height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, data);
			break;
        case PF_GRAYA88:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, pixel_width, pixel_height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data);
            break;
        case PF_GRAY8:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, pixel_width, pixel_height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
            break;
		default:
			break;
		}
		GLenum err=glGetError();
		if(err!=0)
		{
			char err_msg[3][32]={"GL_INVALID_ENUM","GL_INVALID_VALUE","GL_INVALID_OPERATION"};
			err-=GL_INVALID_ENUM;
			if (err<=2)
			{
				MGLogD("glTexImage2D failed(%s) while MGImage::sendDataToContext",err_msg[err]);
			}


		}
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	bool MGMGPersistenceTextureInfo::reload()
	{
		if (data == NULL) {
			return false;
		}
		context.initWithDataWithoutDelete(data, info.format, info.adapter_width, info.adapter_height);
        return true;
	}

}
