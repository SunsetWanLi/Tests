#include "DataReader.h"
namespace GIS_Embedded
{

	//DataReader::DataReader(){

	//}

	//void DataReader::setStartPointer(byte_8* data_start)
	//{
	//	m_curr_pos = data_start;

	//}

	//byte_8 DataReader::getByte()
	//{
	//	byte_8 ret = *m_curr_pos;
	//	++m_curr_pos;
	//	return ret;
	//}

	//int DataReader::getInt()
	//{
	//	int ret;
	//	if(m_curr_pos == NULL)
	//		return -1;
	//	memcpy((void *)&ret, (void *)m_curr_pos, sizeof(int));
	//	m_curr_pos += sizeof(int);
	//	return ret;

	//	//unsigned char pBuff[sizeof(int)];
	//	//for(int i = 0; i < sizeof(int); i++)
	//	//{
	//	//	pBuff[i] = *(byte_8 *)(m_curr_pos + i);
	//	//}
	//	//int ret = *(int*)pBuff;
	//	////ret = *(reinterpret_cast<int*>(m_curr_pos));
	//	////ret = *(int*)m_curr_pos;
	//	//m_curr_pos += sizeof(int);

	//	//return ret;

	//}

	//float DataReader::getFloat()
	//{
	//	float ret = 0;
	//	memcpy((void*)&ret, (void*)m_curr_pos, sizeof(float));
	//	m_curr_pos += sizeof(float);
	//	return ret;
	//}

	//double DataReader::getDouble()
	//{
	//	double ret;
	//	memcpy((void *)&ret, (void *)m_curr_pos, sizeof(double));
	//	m_curr_pos += sizeof(double);
	//	return ret;

	//	//unsigned char pBuff[sizeof(double)];
	//	//for(int i = 0; i < sizeof(double); i++)
	//	//{
	//	//	pBuff[i] = *(byte_8 *)(m_curr_pos + i);
	//	//}

	//	//double ret = *(double*)pBuff;
	//	//m_curr_pos += sizeof(double);

	//	//return ret;

	//}

	//int_64 DataReader::getInt64()
	//{
	//	int_64 ret;
	//	memcpy((void *)&ret, (void *)m_curr_pos, sizeof(int_64));
	//	m_curr_pos += sizeof(int_64);
	//	return ret;


	//	//DWORD cBytes;
	//	//unsigned char pBuff[sizeof(int_64)];
	//	//for(int i = 0; i < sizeof(int_64); i++)
	//	//{
	//	//	pBuff[i] = *(byte_8 *)(m_curr_pos + i);
	//	//}

	//	//int_64 ret = *(int_64*)pBuff;
	//	//m_curr_pos += sizeof(int_64);

	//	//return ret;

	//}

	//short DataReader::getShort()
	//{
	//	short ret;
	//	memcpy((void *)&ret, (void *)m_curr_pos, sizeof(short));
	//	m_curr_pos += sizeof(short);
	//	return ret;


	//	//DWORD cBytes;
	//	//unsigned char pBuff[sizeof(short)];
	//	//for(int i = 0; i < sizeof(short); i++)
	//	//{
	//	//	pBuff[i] = *(byte_8 *)(m_curr_pos + i);
	//	//}

	//	//short ret = *(short*)pBuff;
	//	//m_curr_pos += sizeof(short);

	//	//return ret;

	//}

	//z11::MGWString DataReader::getString()
	//{
	//	int sum = getInt();
	//	z11::MGWString ret = z11::MGWString((wchar_t *)m_curr_pos, sum);
	//	m_curr_pos += sum * sizeof(wchar_t);

	//	return ret;

	//	//byte_8 pBuff[sizeof(wchar_t)];

	//	//int sum = getInt();

	//	//wchar_t* wCh = new wchar_t[sum];

	//	//for(int i=0;i<sum;i++)
	//	//{
	//	//	for(int j = 0; j < sizeof(wchar_t); j ++)
	//	//	{
	//	//		pBuff[j] = *(byte_8 *)(m_curr_pos + j/*sizeof(wchar_t) - 1 - j*/);
	//	//	}
	//	//	m_curr_pos += sizeof(wchar_t);
	//	//	wCh[i] = *(wchar_t *)(pBuff);
	//	//}
	//	//z11::MGWString ret = z11::MGWString(wCh,sum);

	//	//delete []wCh;

	//	//return ret;

	//}


z11::MGWString DataReader::GetWchars( int sum )
{
#if defined(ANDROID_NDK)|| defined(__IPHONE_2_0)
    z11::uint_32 *ret=new z11::uint_32[sum+1];
	z11::uint_16 *pos=(z11::uint_16 *)m_curr_pos;
	for (int i=0;i<sum;i++)
	{
		ret[i]=pos[i];
	}
	ret[sum]=0;
	z11::MGWString ret2((widechar*)ret); 
	delete[] ret;
    
	m_curr_pos += sum * sizeof(z11::uint_16);
    
	return ret2;
#else
    z11::MGWString ret((wchar_t *)m_curr_pos, sum);
	m_curr_pos += sum * sizeof(wchar_t);
	return ret;
#endif
}

DataReader::DataReader() :m_curr_pos(NULL)
{

}
}