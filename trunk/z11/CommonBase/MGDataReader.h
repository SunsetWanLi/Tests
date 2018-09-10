#pragma once
#include "MGData.h"
#include "MGWString.h"
#include <sstream>
namespace z11
{
	class MGDataReader
	{
	private:
		MGData *m_data;
		size_t current_offset;
		byte_8* m_end_pos;
		std::istringstream m_stream_reader;
	public:
		MGDataReader();
		MGDataReader(const std::string &path);
		
		~MGDataReader();
		void initWithData(MGData *data);
		void initWithContentsOfFile(const std::string &path);
		std::istringstream & getStringStream();
		bool is_open();
		size_t read(void* data,size_t  size);
		size_t available() const;
		//bool isOpen();
		//
		//bool isEnd();

		//int getInt();
		//unsigned int getUInt();
		//MGWString getString();
	};
}
