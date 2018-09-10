#ifndef TEST_GPS_COM_READ_THREAD_H_
#define TEST_GPS_COM_READ_THREAD_H_
#ifdef WINCE
//#include "../SRC_Communication/System/Thread.h"
//#include "../SRC_Communication/System/Thread.h"
//#include "Thread.h"
#include "../../../System/Thread/MGThread.h"
#include <windows.h>
#include "GPS_Info.h"
#include <vector>
#include <set>
#include "../../../System/Shared/Syncable.h"

using namespace std;

namespace z11
{
class GPS;
class ComReadThread : public z11::MGThread
{
	struct GPS_Sentence
	{
		vector<string> m_words;
		size_t wordCount() const
		{	return m_words.size();	}
		string & operator[](size_t index)
		{	return m_words[index];	}
		void clear()
		{	m_words.clear();	}
	};
	
	bool m_overwrite_saInfo;
	//bool m_exit_tag;
	HANDLE m_com_hdl;
	GPS_Info m_info;
	Syncable m_lock;
	//loujt,09-04-10
	set<int> m_valid_sa_number;
	Satellites_Info sa_info;
	GPS *m_parent_gps;
//原来线程while前面的变量，因为现在线程循环已经在父类写了，子类只能有while里面的部分。所以移到这里
	OVERLAPPED m_o;
	char* m_buffer;
	DWORD read_sum;
	DWORD last_sentence_l;
	//bool isMarkedStop();
	//return the length of last sentence
	int parse(char *buf, int sz);
private:
	virtual bool threadLoop();
	int parse(char *buf, int sz, vector<GPS_Sentence> *result);
	int readSentence(char *buf, int sz, GPS_Sentence *result);
	void updateGPSInfo(GPS_Sentence &sentence);
	void updateGPGLL(GPS_Sentence &sentence);
	void updateGPRMC(GPS_Sentence &sentence);
	void updateGPGSV(GPS_Sentence &sentence);

	void updateGPGGA(GPS_Sentence &sentence); //---Added by yangchuan for getting SeaLevel. 2010-8-30---
	int writeCOMM(char* write_data);

	double str2Lat(const string &word);
	double str2Lon(const string &word);
	double str2Speed(const string &word);
	double str2Heading(const string &word);
	int  str2Number(const string &word);
	GPS_Info::UTC_Time str2Time(const string &word);
	GPS_Info::UTC_Date str2Date(const string &word);
public:
	ComReadThread();
	~ComReadThread();
	void setWriteTarget(GPS *gps)
	{	m_parent_gps = gps;	}
	void setCOMPort(HANDLE com_hdl)
	{	m_com_hdl = com_hdl;	}
	//void markStop();
	//int run();
DWORD StartWAAS_EGNOS(bool b);
	DWORD stopWAAS_EGNOS();
	DWORD startWAAS_EGNOS(); //added by yangchuan, 本函数用以设置是否启用WAAS/EGNOS
								//通过向GPS Receiver发送数据完成.
//private:
//	bool m_bWAAS_EGNOS;
};
}
#endif
#endif //TEST_GPS_COM_READ_THREAD_H_