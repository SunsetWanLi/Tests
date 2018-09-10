#ifndef _DATA_READER_H_
#define _DATA_READER_H_

#include <string>
#include <cstring>

#include "../Common/Types/basic_types.h"
#include "CommonBase/MGWString.h"
#include "Retiguide/Common/String_Tools.h"
#include "System/Shared/MGLog.h"
#include "CommonBase/MGData.h"
using namespace std;
using namespace z11;

namespace GIS_Embedded{

	class DataReader
	{

	private:
		byte_8* m_curr_pos;
	public:
		DataReader();

		void setStartPointer(byte_8* data_start)
		{
			m_curr_pos = data_start;

		}

		byte_8 *getStartPointer() const
		{
			return m_curr_pos;
		}

		void setStartPointer(int byteCount)
		{
			m_curr_pos += byteCount;
		}

		byte_8 getByte()
		{
			byte_8 ret = *m_curr_pos;
			++m_curr_pos;
			return ret;
		}

		int getInt()
		{
			int ret;
			memcpy((void *)&ret, (void *)m_curr_pos, sizeof(int));
			m_curr_pos += sizeof(int);
			return ret;

			//unsigned char pBuff[sizeof(int)];
			//for(int i = 0; i < sizeof(int); i++)
			//{
			//	pBuff[i] = *(byte_8 *)(m_curr_pos + i);
			//}
			//int ret = *(int*)pBuff;
			////ret = *(reinterpret_cast<int*>(m_curr_pos));
			////ret = *(int*)m_curr_pos;
			//m_curr_pos += sizeof(int);

			//return ret;

		}

		unsigned int getUint()
		{
			unsigned int ret;
			memcpy((void *)&ret, (void *)m_curr_pos, sizeof(unsigned int));
			m_curr_pos += sizeof(unsigned int);
			return ret;
		}

		float getFloat()
		{
			float ret = 0;
			memcpy((void*)&ret, (void*)m_curr_pos, sizeof(float));
			m_curr_pos += sizeof(float);
			return ret;
		}

		double getDouble()
		{
			double ret;
			memcpy((void *)&ret, (void *)m_curr_pos, sizeof(double));
			m_curr_pos += sizeof(double);
			return ret;

			//unsigned char pBuff[sizeof(double)];
			//for(int i = 0; i < sizeof(double); i++)
			//{
			//	pBuff[i] = *(byte_8 *)(m_curr_pos + i);
			//}

			//double ret = *(double*)pBuff;
			//m_curr_pos += sizeof(double);

			//return ret;

		}

		int_64 getInt64()
		{
			int_64 ret;
			memcpy((void *)&ret, (void *)m_curr_pos, sizeof(int_64));
			m_curr_pos += sizeof(int_64);
			return ret;


			//DWORD cBytes;
			//unsigned char pBuff[sizeof(int_64)];
			//for(int i = 0; i < sizeof(int_64); i++)
			//{
			//	pBuff[i] = *(byte_8 *)(m_curr_pos + i);
			//}

			//int_64 ret = *(int_64*)pBuff;
			//m_curr_pos += sizeof(int_64);

			//return ret;

		}

		short getShort()
		{
			short ret;
			memcpy((void *)&ret, (void *)m_curr_pos, sizeof(short));
			m_curr_pos += sizeof(short);
			return ret;


			//DWORD cBytes;
			//unsigned char pBuff[sizeof(short)];
			//for(int i = 0; i < sizeof(short); i++)
			//{
			//	pBuff[i] = *(byte_8 *)(m_curr_pos + i);
			//}

			//short ret = *(short*)pBuff;
			//m_curr_pos += sizeof(short);

			//return ret;

		}

		z11::MGWString getString_2()
		{
			int sum = (int)getByte();
			//z11::MGWString ret = WS((wchar_t *)m_curr_pos, sum);
			//m_curr_pos += sum * sizeof(wchar_t);

			//return ret;
			return GetWchars(sum);
		}

		void getString_2(z11::MGWString &ret)
		{
			int sum = (int)getByte();
			//ret = z11::MGWString((wchar_t *)m_curr_pos, sum);

			////tmp
			//z11::MGWString tmp = ret;
			//while(!('a' <= tmp[tmp.size() - 1] && tmp[tmp.size() - 1] <= 'z' || 'A' <= tmp[tmp.size() - 1] && tmp[tmp.size() - 1] <= 'Z' ))
			//{
			//	tmp = tmp.substr(0, tmp.size() -1);
			//}
			//ret = tmp;

			//m_curr_pos += sum * sizeof(wchar_t);
			ret= GetWchars(sum);
		}

		//GRF
		z11::MGWString getString_1()
		{
			int sum = getShort();
			//z11::MGWString ret = WS((wchar_t *)m_curr_pos, sum);
			//m_curr_pos += sum * sizeof(wchar_t);

			//return ret;
			return GetWchars(sum);

		}
		z11::MGWString getString()
		{
			int sum = getInt();
			return GetWchars(sum);

		}

		void getString(z11::MGWString &ret)
		{
			ret=getString();

			//int sum = getInt();
			//ret = WS((wchar_t *)m_curr_pos, sum);
			//m_curr_pos += sum * sizeof(wchar_t);
		}

		z11::MGWString GetWchars( int sum );
	};




}

#endif
