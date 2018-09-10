//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2011 Zhejiang University.
///    All rights reserved.
///
/// @file    (MGLog.h)
/// @brief (Log Print)
///
///()
///
/// @version 0.1   (0.1)
/// @author        (Gu RongFang)
/// @date          (11-07-22)
///
///
///    
//////////////////////////////////////////////////////////////////////////
#ifndef _MGLOG_H_
#define _MGLOG_H_


extern void LOGD(int priority, const char* tag, const char *str);
extern void LogStart();
extern void LogEnd(const char *str);
extern long getCurrentTime();
extern void OutputDebugMessage(const char *str);

#include "../../CommonBase/MGObject.h"
#include <string>
#include <assert.h>
namespace z11
{
	class MGLog
	{
	public:
		enum
		{
            MGOutput=0x0,
			MGDEBUG=0x1,
			MGERROR=0x2,
			MGINFO=0x4,
			MGVERBOSE=0x8,
			MGWARN=0x10,
			MGLOGTYPESCOUNT=6
		};
		MGLog(int priority,const char *szFormat, ...);
	};
	class MGLogD
	{
	public:
		MGLogD(const char *szFormat, ...);

	};
	class MGTimeLog
	{
	public:
		MGTimeLog(const char *szFormat, ...);
		~MGTimeLog();
	private:
		long long ticks;
		char szBuffer[1024];
	};
#define MGTimeLogD(str) MGTimeLog MGTIME_LOG_##__LINE__("%s",str)
#define MGAssert(exp) assert(exp)
}
#define DCHECK(exp) MGAssert(exp)
//inline void DCHECK(bool condition)
//{
//	z11::MGAssert(condition,"test");
//}

#endif //_MGLOG_H_
