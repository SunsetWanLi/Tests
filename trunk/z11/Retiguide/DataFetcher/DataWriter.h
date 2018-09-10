#ifndef _DATA_WRITER_H_
#define _DATA_WRITER_H_

#include <string>

#include "../Common/Types/basic_types.h"
#include "CommonBase/MGWString.h"

using namespace std;
using namespace z11;

namespace GIS_Embedded
{

	class DataWriter
	{
	public:
		DataWriter();

		//设置起始地址 应保证此地址已指向有效地址
		void setStartPointer(byte_8* data_start);
	
		void writeByte(byte_8 byteVal);

		void writeInt(int intVal);

		void writeFloat(float floatVal);

		void writeDouble(double doubleVal);

		void writeInt64(int_64 int64Val);

		void writeShort(short shortVal);

		void writeString(z11::MGWString &wstrVal);

		void writeString_2(z11::MGWString &wstrVal);

	private:
		byte_8* m_curr_pos;
	};

}

#endif