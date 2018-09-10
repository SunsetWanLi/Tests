#include <cstring>
#include "MGDataReader.h"
namespace z11
{
	//z11::MGDataReader::MGDataReader( const std::string & path ) :m_curr_pos(NULL)
	//{
	//	m_data=MGData::dataWithContentsOfFile(path);
	//	m_curr_pos=(byte_8 *)m_data->bytes();
	//	m_end_pos=m_curr_pos+m_data->length();
	//}

	z11::MGDataReader::~MGDataReader()
	{
		release_and_set_null(m_data);
	}

	//bool z11::MGDataReader::isOpen()
	//{
	//	return m_data!=NULL;
	//}

	//bool z11::MGDataReader::isEnd()
	//{
	//	return m_curr_pos >= m_end_pos;
	//}

	//int z11::MGDataReader::getInt()
	//{
	//	if(isEnd())
	//		return 0;
	//	int ret;
	//	memcpy(&ret,m_curr_pos,sizeof(int));
	//	m_curr_pos+=sizeof(int);
	//	return ret;
	//}

	//unsigned int z11::MGDataReader::getUInt()
	//{
	//	if(isEnd())
	//		return 0;
	//	unsigned int ret;
	//	memcpy(&ret,m_curr_pos,sizeof(int));
	//	m_curr_pos+=sizeof(unsigned int);
	//	return ret;
	//}

	//MGWString z11::MGDataReader::getString()
	//{
	//	MGWString ret;
	//	widechar i=0;
	//	while ((i=getUInt()) != 0)
	//	{
	//		ret.push_back(i);
	//	}
	//	return ret;
	//}

	MGDataReader::MGDataReader() :m_data(NULL),m_end_pos(NULL)
	{
		current_offset=0;
	}

	MGDataReader::MGDataReader( const std::string &path )
	{
		initWithContentsOfFile(path);
	}
	void MGDataReader::initWithContentsOfFile( const std::string &path )
	{
		m_data=MGData::dataWithContentsOfFile(path);
		if (m_data!=NULL)
		{
			//m_stream_reader.read((char *)m_data->bytes(),m_data->length());
			m_stream_reader.rdbuf()->sputn((char *)m_data->bytes(),m_data->length());
			current_offset=0;
		}
	}

	std::istringstream& MGDataReader::getStringStream()
	{
		return m_stream_reader;
	}

	bool MGDataReader::is_open()
	{
		return m_data!=NULL;
	}

	void MGDataReader::initWithData( MGData *data )
	{
		release_and_set_null(m_data);
		m_data=data;
		if (m_data!=NULL)
		{
			data->retain();
			m_stream_reader.rdbuf()->sputn((char *)m_data->bytes(),m_data->length());
			current_offset=0;
		}
	}

	size_t MGDataReader::read( void* data,size_t size )
	{
		if (data==NULL)
		{
			return 0;
		}
		size_t read_size=MINVALUE((m_data->length()-current_offset),size);
		if (read_size>0)
		{
			memcpy(data,(byte_8 *)m_data->bytes()+current_offset,read_size);
			current_offset+=read_size;
			return read_size;
		}
		return 0;
	}

	size_t MGDataReader::available() const
	{
		return m_data->length();
	}

}
