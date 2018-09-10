#include "DataWriter.h"

namespace GIS_Embedded
{

	DataWriter::DataWriter()
	{
		m_curr_pos = NULL;
	}

	void DataWriter::setStartPointer(byte_8* data_start)
	{
		m_curr_pos = data_start;
	}

	void DataWriter::writeByte(byte_8 byteVal)
	{
		*m_curr_pos = byteVal;
		++ m_curr_pos ;
	}

	void DataWriter::writeInt(int intVal)
	{
		byte_8 * pBuff = (byte_8*)&intVal;
		
		for(int i = 0; i < sizeof(int); i ++)
		{
			*((byte_8 *)m_curr_pos) = *(pBuff + i);
			m_curr_pos ++;
		}
	}

	void DataWriter::writeFloat(float floatVal)
	{
		byte_8 * pBuff = (byte_8*)&floatVal;
		
		for(int i = 0; i < sizeof(float); ++ i)
		{
			*((byte_8 *)m_curr_pos) = *(pBuff + i);
			m_curr_pos ++;
		}
	}

	void DataWriter::writeDouble(double doubleVal)
	{
		byte_8 * pBuff = (byte_8*)&doubleVal;
		
		for(int i = 0; i < sizeof(double); i ++)
		{
			*((byte_8 *)m_curr_pos) = *(pBuff + i);
			m_curr_pos ++;
		}
	}

	void DataWriter::writeInt64(int_64 int64Val)
	{
		byte_8 * pBuff = (byte_8*)&int64Val;
		
		for(int i = 0; i < sizeof(int_64); i ++)
		{
			*((byte_8 *)m_curr_pos) = *(pBuff + i);
			m_curr_pos ++;
		}
	}

	void DataWriter::writeShort(short shortVal)
	{
		byte_8 * pBuff = (byte_8*)&shortVal;
		
		for(int i = 0; i < sizeof(short); i ++)
		{
			*((byte_8 *)m_curr_pos) = *(pBuff + i);
			m_curr_pos ++;
		}
	}

	void DataWriter::writeString(z11::MGWString &wstrVal)
	{
		int size = wstrVal.size();
		writeInt(size);

		for(int i = 0;i < size; i ++)
		{
			wchar_t tmpVal = wstrVal[i];
			byte_8 * pBuff = (byte_8*)&tmpVal;
			
			for(int j = 0; j < sizeof(wchar_t); j ++)
			{
				*((byte_8 *)m_curr_pos) = *(pBuff + j/*sizeof(wchar_t) -1 - j*/);
				m_curr_pos ++;
			}
		}

	}

	void DataWriter::writeString_2(z11::MGWString &wstrVal)
	{
		int size = wstrVal.size();
		writeByte((byte_8)size);

		for(int i = 0;i < size; i ++)
		{
			wchar_t tmpVal = wstrVal[i];
			byte_8 * pBuff = (byte_8*)&tmpVal;
			
			for(int j = 0; j < sizeof(wchar_t); j ++)
			{
				*((byte_8 *)m_curr_pos) = *(pBuff + j/*sizeof(wchar_t) -1 - j*/);
				m_curr_pos ++;
			}
		}

	}
}