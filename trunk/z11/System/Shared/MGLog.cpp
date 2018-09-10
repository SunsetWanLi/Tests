#include "Utility.h"
#include "MGLog.h"
#include "assert.h"
#include "stdio.h"
#include "MGTime.h"
using namespace z11;

#define DEBUG_PRINT_MGLOG

//////////////////////////////////////////////////////////////////////////
//define LOG function
//////////////////////////////////////////////////////////////////////////
#ifdef ANDROID_NDK

//Log function
#define LOG_TAG "z11" //自定义的变量，相当于logcat函数中的tag
#undef LOG
#include <android/log.h>
//#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
void LOGD(int priority, const char* tag, const char *str)
{
#ifdef DEBUG_PRINT_MGLOG
    if ((priority & MGLog::MGDEBUG) != 0) {
        __android_log_print(ANDROID_LOG_DEBUG,tag,"%s",str) ;
    }
    if ((priority & MGLog::MGERROR) != 0) {
        __android_log_print(ANDROID_LOG_ERROR,tag,"%s",str) ;
    }
    if ((priority & MGLog::MGINFO) != 0) {
        __android_log_print(ANDROID_LOG_INFO,tag,"%s",str) ;
    }
    if ((priority & MGLog::MGVERBOSE) != 0) {
        __android_log_print(ANDROID_LOG_VERBOSE,tag,"%s",str) ;
    }
    if ((priority & MGLog::MGWARN) != 0) {
        __android_log_print(ANDROID_LOG_WARN,tag,"%s",str) ;
    }
    if (priority == 0) {
        __android_log_print(ANDROID_LOG_DEFAULT,tag,"%s",str) ;
    }
#endif
}


//ms level for get Time function
#include <sys/time.h>
static MGTimeTicks _getTime(void)
{
	//struct timeval  now;
	//gettimeofday(&now, NULL);
	//return (long)(now.tv_sec*1000 + now.tv_usec/1000);
	return MGTimeTicks::Now();
}


#elif defined( WIN32 )
#include <winsock2.h>
#include <windows.h>
#include <iostream>
using namespace std;
//Log function
void LOGD(int priority, const char* tag, const char *str)
{
	if (tag ==  NULL || str == NULL)
		return;
#ifdef DEBUG_PRINT_MGLOG
	if (strlen(str)>4080)
	{
		return ;
	}
	wchar_t tempstr2[4096];
    memset(tempstr2,0,sizeof(tempstr2));
    MGWString tagstr = z11::Utility::ustr2wstr(tag);
	if(str!=NULL)
	{
		MGWString tempstr = z11::Utility::ustr2wstr(str);
        if ((priority & MGLog::MGDEBUG) != 0) {
            wsprintf(tempstr2,L"%s, MGDebug : %s\n", tagstr.c_str() ,tempstr.c_str());
        }
        if ((priority & MGLog::MGERROR) != 0) {
            wsprintf(tempstr2,L"%s, MGError : %s\n", tagstr.c_str() ,tempstr.c_str());
        }
        if ((priority & MGLog::MGINFO) != 0) {
            wsprintf(tempstr2,L"%s, MGInfo : %s\n", tagstr.c_str() ,tempstr.c_str());
        }
        if ((priority & MGLog::MGVERBOSE) != 0) {
            wsprintf(tempstr2,L"%s, MGVerbose : %s\n", tagstr.c_str() ,tempstr.c_str());
        }
        if ((priority & MGLog::MGWARN) != 0) {
            wsprintf(tempstr2,L"%s, MGWarn : %s\n", tagstr.c_str() ,tempstr.c_str());
        }
        if (priority == 0) {
            wsprintf(tempstr2,L"%s, MGLog : %s\n", tagstr.c_str() ,tempstr.c_str());
        }
	}
	else
	{
        if ((priority & MGLog::MGDEBUG) != 0) {
            wsprintf(tempstr2,L"%s, MGDebug : null\n", tagstr.c_str());
        }
        if ((priority & MGLog::MGERROR) != 0) {
            wsprintf(tempstr2,L"%s, MGError : null\n", tagstr.c_str());
        }
        if ((priority & MGLog::MGINFO) != 0) {
            wsprintf(tempstr2,L"%s, MGInfo : null\n", tagstr.c_str());
        }
        if ((priority & MGLog::MGVERBOSE) != 0) {
            wsprintf(tempstr2,L"%s, MGVerbose : null\n", tagstr.c_str());
        }
        if ((priority & MGLog::MGWARN) != 0) {
            wsprintf(tempstr2,L"%s, MGWarn : null\n", tagstr.c_str());
        }
        if (priority == 0) {
            wsprintf(tempstr2,L"%s, MGLog : null\n", tagstr.c_str());
        }
	}
	OutputDebugString(tempstr2);
#endif
}

//ms level for get Time function
static MGTimeTicks  _getTime(void)
{
	return MGTimeTicks::Now();
}

#else//#elif defined(__IPHONE_2_0)
#include <stdio.h>
#include <stdarg.h>
using namespace z11;
void LOGD(int priority, const char* tag, const char *str)
{
#ifdef DEBUG_PRINT_MGLOG
    if ((priority & MGLog::MGDEBUG) != 0) {
        printf("%s, MGDebug : %s\n", tag ,str);
    }
    if ((priority & MGLog::MGERROR) != 0) {
        printf("%s, MGError : %s\n", tag ,str);
    }
    if ((priority & MGLog::MGINFO) != 0) {
        printf("%s, MGInfo : %s\n", tag ,str);
    }
    if ((priority & MGLog::MGVERBOSE) != 0) {
        printf("%s, MGVerbose : %s\n", tag ,str);
    }
    if ((priority & MGLog::MGWARN) != 0) {
        printf("%s, MGWarn : %s\n", tag ,str);
    }
    if (priority == 0) {
        printf("%s, MGLog : %s\n", tag ,str);
    }
#endif
}

//ms level for get Time function
#include <sys/time.h>
static MGTimeTicks _getTime(void)
{
	//struct timeval  now;
	//gettimeofday(&now, NULL);
	//return (long)(now.tv_sec*1000 + now.tv_usec/1000);
	return MGTimeTicks::Now();
}

#endif

static MGTimeTicks curTime;
void LogStart()
{
#ifdef DEBUG_PRINT_MGLOG
	curTime=_getTime();
#endif
}
void LogEnd(const char *str)
{
#ifdef DEBUG_PRINT_MGLOG
	char buffer[256];
    memset(buffer,0,sizeof(buffer));
	sprintf(buffer,"after %s ,log time = %.3lf",str,(_getTime()-curTime).InMillisecondsF());
	LOGD(0, "z11", buffer);
#endif
}
long getCurrentTime()
{
#ifdef DEBUG_PRINT_MGLOG
	return (long)(_getTime().ToInternalValue()/MGTime::kMicrosecondsPerMillisecond);
#else 
    return 0;
#endif
}

void OutputDebugMessage(const char *str)
{
	LOGD(0, "z11", str);
}
namespace z11
{
	class MGLogImpl:public MGObject
	{
	public:
		static void i(const std::string& msg);
		static void d(const std::string& msg);
		static void e(const std::string& msg);
		static void v(const std::string& msg);
		static void w(const std::string& msg);
        static void o(const std::string& msg);
		static void setTag(const std::string& _tag);
		static void println(int priority,const std::string&_tag,const std::string&_msg);
	private:
		MGLogImpl();
		~MGLogImpl();
		static MGLogImpl *_instance;
		static MGLogImpl *GetInstance();
		std::string tag;

	};

	void MGLogImpl::setTag( const string& _tag )
	{
		GetInstance()->tag=_tag;
	}

	MGLogImpl *MGLogImpl::_instance=NULL;
    
    void MGLogImpl::o(const string& msg)
    {
        println(MGLog::MGOutput, GetInstance()->tag, msg);
    }

	void MGLogImpl::w( const string& msg )
	{
		println(MGLog::MGWARN,GetInstance()->tag,msg);
	}

	void MGLogImpl::v( const string& msg )
	{
		println(MGLog::MGVERBOSE,GetInstance()->tag,msg);

	}

	void MGLogImpl::e( const string& msg )
	{
		println(MGLog::MGERROR,GetInstance()->tag,msg);

	}

	void MGLogImpl::d( const string& msg )
	{
		//println(0,GetInstance()->tag,msg);
		println(MGLog::MGDEBUG,GetInstance()->tag,msg);
	}

	void MGLogImpl::i( const string& msg )
	{
		println(MGLog::MGINFO,GetInstance()->tag,msg);
	}

	MGLogImpl::MGLogImpl()
	{
		tag="z11";
	}

	MGLogImpl::~MGLogImpl()
	{

	}

	MGLogImpl * MGLogImpl::GetInstance()
	{
		if (_instance==NULL)
		{
			_instance=new MGLogImpl();

		}
		return _instance;
	}
    
	void MGLogImpl::println( int priority,const string&_tag,const string&_msg )
	{
		LOGD(priority, _tag.c_str(), _msg.c_str());
	}

	void MGLogPrint(int priority,const char *szFormat)
	{
        if ((priority & MGLog::MGDEBUG) != 0) {
            MGLogImpl::d(szFormat);
        }
        if ((priority & MGLog::MGERROR) != 0) {
            MGLogImpl::e(szFormat);
        }
        if ((priority & MGLog::MGINFO) != 0) {
            MGLogImpl::i(szFormat);
        }
        if ((priority & MGLog::MGVERBOSE) != 0) {
            MGLogImpl::v(szFormat);
        }
        if ((priority & MGLog::MGWARN) != 0) {
            MGLogImpl::w(szFormat);
        }
        if (priority == 0) {
            MGLogImpl::o(szFormat);
        }
	}

	MGLogD::MGLogD( const char *szFormat, ... )
	{
#ifdef DEBUG_PRINT_MGLOG
		char szBuffer[4096];
        memset(szBuffer,0,sizeof(szBuffer));
		va_list pArgs ;
		va_start (pArgs, szFormat) ;
		vsprintf(szBuffer,szFormat,pArgs);
		va_end (pArgs) ;
		MGLogPrint(MGLog::MGDEBUG,szBuffer);
#endif
	}

	MGLog::MGLog( int priority,const char *szFormat, ... )
	{
#ifdef DEBUG_PRINT_MGLOG
		char szBuffer[1024];
        memset(szBuffer,0,sizeof(szBuffer));
		va_list pArgs ;
		va_start (pArgs, szFormat) ;
		vsprintf(szBuffer,szFormat,pArgs);
		va_end (pArgs) ;
		//MGLogImpl::d(szBuffer);
        MGLogPrint(priority, szBuffer);
#endif
	}

	MGTimeLog::MGTimeLog( const char *szFormat, ... )
	{
#ifdef DEBUG_PRINT_MGLOG
		va_list pArgs ;
		va_start (pArgs, szFormat) ;
		vsprintf(szBuffer,szFormat,pArgs);
		va_end (pArgs) ;
		ticks=_getTime().ToInternalValue();
#endif
	}

	MGTimeLog::~MGTimeLog()
	{
#ifdef DEBUG_PRINT_MGLOG
		char print_buffer[1024];
        memset(print_buffer,0,sizeof(print_buffer));
		MGTimeDelta oldTime=MGTimeDelta::FromMicroseconds(_getTime().ToInternalValue()-ticks);
		sprintf(print_buffer,"after %s , log time=%lf ms",szBuffer,oldTime.InMillisecondsF());
		MGLogPrint(MGLog::MGDEBUG,print_buffer);
#endif
	}
}
