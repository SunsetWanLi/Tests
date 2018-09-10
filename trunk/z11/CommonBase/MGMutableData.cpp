#include "MGMutableData.h"
#include "../System/Shared/MGLog.h"
namespace z11
{


	MGMutableData * MGMutableData::dataWithCapacity( size_t aNumItems )
	{
		MGMutableData *ret=new MGMutableData();
		ret->m_bytes=NULL;
		ret->m_length=0;
		ret->m_capacity=aNumItems;
		return ret;
	}

	void * MGMutableData::mutableBytes()
	{
		if (m_capacity!=0&&m_bytes==NULL)
		{
			m_bytes=alloc(m_capacity);
		}
		return m_bytes;
	}

	void MGMutableData::increaseLengthBy( size_t extraLength )
	{
		appendBytes(NULL,extraLength);
	}

	void MGMutableData::setLength( size_t length )
	{
		if (length<=m_capacity)
		{
			if (m_bytes==NULL)
			{
				m_bytes=alloc(m_capacity);
			}
			if (length<=m_length)
			{
				m_length=length;
			}
			else
			{
				memset((byte_8*)m_bytes+m_length,0,length-m_length);
			}
		}
		else
		{
			increaseLengthBy(length-m_length);
		}
	}

	MGMutableData::MGMutableData()
	{

	}

	void MGMutableData::appendBytes( const void *_bytes,size_t length )
	{
		size_t new_length=m_length+length;
		if (new_length>m_capacity)
		{
			m_capacity=expand_to_2_power(new_length);
			byte_8 *new_bytes=new byte_8[m_capacity];
			memcpy(new_bytes,m_bytes,m_length);//copy original data
			delete_array_and_set_null(m_bytes);
			m_bytes=new_bytes;
		}
		if (m_bytes==NULL)
		{
			m_bytes=alloc(m_capacity);
		}
		if (length!=0)
		{
			if (_bytes==NULL)
			{
				memset((byte_8*)m_bytes+m_length,0,length);// append data zero
			}
			else
				memcpy((byte_8*)m_bytes+m_length,_bytes,length);//copy append data
		}
		m_length=new_length;
	}

	void MGMutableData::appendData( const MGData *otherData )
	{
		if (otherData!=NULL)
		{
			appendBytes(otherData->bytes(),otherData->length());
		}
	}

	size_t MGMutableData::length() const
	{
		return m_length;
	}

	byte_8 * MGMutableData::alloc( size_t size )
	{
		byte_8 *ret=new byte_8[size];
		memset(ret,0,size);
		return ret;
	}

	void MGMutableData::replaceBytesInRangeWithBytes( const MGRange &range,const void *replace_bytes )
	{
		if (range.location+range.length<m_capacity)
		{
			memcpy((byte_8*)m_bytes+range.location,replace_bytes,range.length);
		}
		else
		{
			MGLogD(" MGMutableData::replaceBytesInRangeWithBytes error");
			assert(0);
		}
	}
}