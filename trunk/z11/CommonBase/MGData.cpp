#include "MGData.h"
#include "../System/Shared/MGLog.h"
#include <stdio.h>
#include <string.h>
#include "../Platform/MGFileUtils.h"
#include "../System/Shared/Utility.h"
namespace z11
{
	MGData * MGData::dataWithBytes( const void *bytes,size_t length )
	{
		MGData *ret=new MGData();
		ret->m_capacity=length;
		if(length!=0)
		{
			ret->m_bytes = new byte_8[length];
			if (bytes != NULL)
				memcpy(ret->m_bytes,bytes,length);
			else
				memset(ret->m_bytes,0,length);
		}

		return ret;
	}

	MGData::MGData() :m_bytes(NULL),m_capacity(0),m_freeWhenDone(true)
	{

	}

	MGData::~MGData()
	{
		if (m_freeWhenDone)
		{
			byte_8*m_bytes_impl=(byte_8*)m_bytes;
			delete_array_and_set_null(m_bytes_impl);
			m_bytes=NULL;
		}
		
	}

	MGData * MGData::dataWithBytesNoCopy( void *bytes,size_t length )
	{
		if (bytes==NULL)
		{
			return NULL;
		}
		
		MGData *ret=new MGData();
		ret->m_bytes=bytes;
		ret->m_capacity=length;
		return ret;
	}

	MGData * MGData::dataWithBytesNoCopy( void *bytes,size_t length,bool freeWhenDone )
	{
		MGData *ret=dataWithBytesNoCopy(bytes,length);
		ret->m_freeWhenDone=freeWhenDone;
		return ret;
	}

	MGData * MGData::dataWithContentsOfFile( const std::string &path )
	{
		//MGLogD("File:%s ready to loading.....",path.c_str());
		MGData *ret=MGFileUtils::sharedFileUtils()->getFileData(path);
		if (ret==NULL)
		{
			MGLogD("MGData: File:%s not exist!",path.c_str());
		}
		else
		{
			//MGLogD("File:%s exist!",path.c_str());
		}
		
		return ret;
	}

	const void * MGData::bytes() const
	{
		return m_bytes;
	}

	size_t MGData::length() const
	{
		return m_capacity;
	}

	bool MGData::writeToFile( const std::string& path )
	{
		FILE *file=fopen(path.c_str(),"wb");
		if (file!=NULL)
		{
			if(bytes()!=NULL&&length()!=0)
			{
				fwrite(bytes(),length(),1,file);
			}
			fclose(file);
			return true;
		}
		else
		{
			MGLogD("MGData::writeToFile %s failed",path.c_str());
		}
		return false;
	}

	MGData * MGData::initWithBytesNoCopy( void *bytes,size_t length )
	{
		this->m_bytes=bytes;
		this->m_capacity=length;
		return this;
	}

	MGData * MGData::dataWithData( const MGData* data )
	{
		return dataWithBytes(data->bytes(),data->length());
	}
}